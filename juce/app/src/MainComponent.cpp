#include "MainComponent.hpp"

#include "BinaryData.h"
#include "Dialogs.hpp"
#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"

#include <juce_core/juce_core.h>

namespace xplorer::app
{
    namespace
    {
        // Fixed (non page-family) blocks handled in TASK-JUCE-063.
        bool isFixedBlockTag(const std::string& tag)
        {
            if (tag.empty() || tag.find("_X") != std::string::npos)
            {
                return false;
            }
            for (const auto* prefix : {"VCO1", "VCO2", "FM", "LAG", "VCF"})
            {
                if (tag.rfind(prefix, 0) == 0)
                {
                    return true;
                }
            }
            return false;
        }

        juce::String settingsDirectory()
        {
            return juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory)
                .getChildFile("Xplorer")
                .getChildFile("Xplorer")
                .getFullPathName();
        }
    }

    MainComponent::MainComponent()
    {
        _background = juce::ImageCache::getFromMemory(BinaryData::mainbackground_jpg,
                                                      BinaryData::mainbackground_jpgSize);
        _dispatcher = std::make_shared<JuceEventDispatcher>();
        _settingsService = std::make_unique<settings::XmlSettingsService>(
            settingsDirectory().toStdString());
        _controller = std::make_unique<controller::XpanderController>(
            _backend, *_settingsService, _dispatcher, "XPLORER");
        _registry = std::make_unique<ParameterBindingRegistry>(*_controller);

        // Route controller parameter changes to the registry (UI refresh). [RQ-GUI-003]
        _controller->setAutomationParameterChangeHandler(
            [this](const std::string& name, int value)
            {
                _registry->onParameterChanged(name, value);
                _display.showParameter(name, value); // [RQ-GUI-020]
            });
        _controller->setMidiActivityHandler(
            [this](controller::EnumMidiDevice) { flashMidiActivity(); }); // [RQ-GUI-022]
        _controller->setFullToneChangeHandler(
            [this](const controller::FullToneChangeEvent&)
            {
                _registry->refreshAllFromModel();
                if (_matrixPanel != nullptr)
                {
                    _matrixPanel->refreshAll(); // [RQ-GUI-017]
                }
                _display.showToneInfo(_controller->currentProgramNumber(),
                                      _controller->toneName()); // [RQ-GUI-020]
            });
        _controller->setPageChangeHandler(
            [this](const controller::PageChangeEvent& event) { onSynthPageChanged(event); });
        _controller->setModulationEntryChangeHandler(
            [this](const controller::ModulationEntryChangeEvent& event)
            {
                if (_matrixPanel != nullptr)
                {
                    _matrixPanel->refreshRow(event.entryNumber); // [RQ-GUI-017]
                }
            });

        setSize(LOGICAL_CANVAS_WIDTH, LOGICAL_CANVAS_HEIGHT);
        placeFixedBlockControls();
        createPageFamilyBlocks();
        _matrixPanel = std::make_unique<ModMatrixPanel>(*this, *_controller);
        createShortcutButtonsAndDisplay();
        _registry->refreshAllFromModel(); // seed all controls with the current tone
        _display.showToneInfo(_controller->currentProgramNumber(), _controller->toneName());

        // Apply persisted MIDI device/channel/delay settings at startup. [RQ-GUI-025]
        applyMidiSettings(*_controller, *_settingsService, _backend);
    }

    MainComponent::~MainComponent() = default;

    void MainComponent::placeFixedBlockControls()
    {
        for (const auto& spec : controlTable())
        {
            const std::string tag = spec.tag;
            if (!isFixedBlockTag(tag))
            {
                continue;
            }
            const auto* parameter = _controller->getParameter(tag);
            std::unique_ptr<juce::Component> component;
            IBoundControl* bound = nullptr;

            switch (spec.kind)
            {
                case ControlKind::KnobControl:
                {
                    if (parameter == nullptr)
                    {
                        continue;
                    }
                    auto knob = std::make_unique<BoundKnob>(*_registry, tag, parameter->minValue(),
                                                            parameter->maxValue(), parameter->step());
                    bound = knob.get();
                    component = std::move(knob);
                    break;
                }
                case ControlKind::CheckBoxValuedControl:
                {
                    if (parameter == nullptr)
                    {
                        continue;
                    }
                    auto check = std::make_unique<BoundCheckBox>(*_registry, tag, juce::String());
                    bound = check.get();
                    component = std::move(check);
                    break;
                }
                case ControlKind::ComboBoxValuedControl:
                {
                    std::vector<std::pair<std::string, int>> options;
                    const auto labels = comboLabelsForControl(tag);
                    for (std::size_t i = 0; i < labels.size(); ++i)
                    {
                        options.emplace_back(labels[i], static_cast<int>(i));
                    }
                    if (options.empty())
                    {
                        continue;
                    }
                    auto combo = std::make_unique<BoundComboBox>(*_registry, tag, options);
                    bound = combo.get();
                    component = std::move(combo);
                    break;
                }
                case ControlKind::RadioButtonPanel:
                {
                    const auto options = radioPanelOptions(tag);
                    if (options.empty())
                    {
                        continue;
                    }
                    auto combo = std::make_unique<BoundComboBox>(*_registry, tag, options);
                    bound = combo.get();
                    component = std::move(combo);
                    break;
                }
                default:
                    continue;
            }

            component->setBounds(spec.x, spec.y, spec.width, spec.height);
            addAndMakeVisible(*component);
            if (bound != nullptr)
            {
                _registry->bind(tag, *bound);
            }
            _controls.push_back(std::move(component));
        }
    }

    void MainComponent::createPageFamilyBlocks()
    {
        int radioGroup = 100;
        for (const auto& family : pageFamilies())
        {
            std::vector<ControlSpec> controlSpecs;
            std::vector<ControlSpec> selectorSpecs;
            const std::string selectorPrefix =
                family.controlTagPrefix.substr(0, family.controlTagPrefix.size() - 1); // "ENV_"
            for (const auto& spec : controlTable())
            {
                const std::string tag = spec.tag;
                const std::string id = spec.id;
                if (tag.rfind(family.controlTagPrefix, 0) == 0)
                {
                    controlSpecs.push_back(spec); // "ENV_X_*"
                }
                else if (spec.kind == ControlKind::RadioButton
                         && id.rfind(selectorPrefix, 0) == 0
                         && id.size() == selectorPrefix.size() + 1
                         && id.back() >= '1' && id.back() <= '9')
                {
                    selectorSpecs.push_back(spec); // "ENV_1".."ENV_5"
                }
            }
            _familyBlocks.push_back(std::make_unique<PageFamilyBlock>(
                *this, *_registry, *_controller, family, controlSpecs, selectorSpecs, radioGroup++));
        }
    }

    void MainComponent::onSynthPageChanged(const controller::PageChangeEvent& event)
    {
        // Map the synth page to a family + instance and activate the selector. [RQ-GUI-012]
        struct Range { model::EnumPages first; model::EnumPages last; const char* prefix; };
        static const Range ranges[] = {
            {model::EnumPages::ENV_1, model::EnumPages::ENV_5, "ENV_X"},
            {model::EnumPages::LFO_1, model::EnumPages::LFO_5, "LFO_X"},
            {model::EnumPages::RAMP_1, model::EnumPages::RAMP_4, "RAMP_X"},
            {model::EnumPages::TRACK_1, model::EnumPages::TRACK_3, "TRACK_X"},
        };
        const int page = static_cast<int>(event.page);
        for (const auto& range : ranges)
        {
            if (page >= static_cast<int>(range.first) && page <= static_cast<int>(range.last))
            {
                const int instance = page - static_cast<int>(range.first) + 1;
                for (auto& block : _familyBlocks)
                {
                    if (block->familyPrefix() == range.prefix)
                    {
                        block->setActiveInstanceFromSynth(instance);
                    }
                }
                return;
            }
        }
    }

    void MainComponent::createShortcutButtonsAndDisplay()
    {
        // Display panel over the VFD area.
        if (const auto* spec = [&]() -> const ControlSpec*
            {
                for (const auto& s : controlTable())
                {
                    if (std::string(s.id) == "_vfdDisplay")
                    {
                        return &s;
                    }
                }
                return nullptr;
            }())
        {
            _display.setBounds(spec->x, spec->y, spec->width, spec->height);
            addAndMakeVisible(_display);
        }

        // MIDI activity LED.
        for (const auto& spec : controlTable())
        {
            if (spec.kind == ControlKind::LedPanelControl)
            {
                _midiLed.setBounds(spec.x, spec.y, spec.width, spec.height);
                addAndMakeVisible(_midiLed);
            }
        }

        // 8 shortcut buttons (functional-first labels; GIF skin in TASK-JUCE-069).
        // [RQ-GUI-021]
        struct Shortcut { const char* id; const char* label; std::function<void()> action; };
        const std::vector<Shortcut> shortcuts = {
            {"btPatchMinus", "-", [this] { _controller->decreaseCurrentProgramNumber(); }},
            {"btPatchPlus", "+", [this] { _controller->increaseCurrentProgramNumber(); }},
            {"btPatchGoto", "GO", [this]
             {
                 showStoreOrGotoDialog("Go to patch", _controller->currentProgramNumber(),
                                       [this](int program)
                                       { _controller->sendProgramChangeAndGetSinglePatchFromSynth(program); });
             }},
            {"btPatchRandom", "R", [this]
             { _controller->randomizeTone(midiapp::controller::RandomizeToneArguments{}); }},
            {"btPatchLoad", "LD", [this]
             {
                 _fileChooser = std::make_unique<juce::FileChooser>("Load patch", juce::File(), "*.syx");
                 _fileChooser->launchAsync(
                     juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                     [this](const juce::FileChooser& chooser)
                     {
                         const auto file = chooser.getResult();
                         if (file.existsAsFile())
                         {
                             _controller->loadTone(file.getFullPathName().toStdString());
                         }
                     });
             }},
            {"btPatchSave", "SV", [this]
             {
                 _fileChooser = std::make_unique<juce::FileChooser>("Save patch", juce::File(), "*.syx");
                 _fileChooser->launchAsync(
                     juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                     [this](const juce::FileChooser& chooser)
                     {
                         const auto file = chooser.getResult();
                         if (file != juce::File())
                         {
                             _controller->saveTone(file.getFullPathName().toStdString());
                         }
                     });
             }},
            {"btPatchStore", "ST", [this]
             {
                 showStoreOrGotoDialog("Store", _controller->currentProgramNumber(),
                                       [this](int program) { _controller->storeSinglePatchToSynth(program); });
             }},
            {"btSettings", "SET",
             [this] { showMidiSettingsDialog(*_controller, *_settingsService, _backend); }},
        };
        for (const auto& shortcut : shortcuts)
        {
            for (const auto& spec : controlTable())
            {
                if (std::string(spec.id) == shortcut.id)
                {
                    auto button = std::make_unique<juce::TextButton>(shortcut.label);
                    button->setBounds(spec.x, spec.y, spec.width, spec.height);
                    button->onClick = shortcut.action;
                    addAndMakeVisible(*button);
                    _shortcutButtons.push_back(std::move(button));
                    break;
                }
            }
        }
    }

    void MainComponent::flashMidiActivity()
    {
        _midiLed.flash();
    }

    void MainComponent::MidiActivityLed::flash()
    {
        _lit = true;
        repaint();
        startTimer(120);
    }

    void MainComponent::MidiActivityLed::timerCallback()
    {
        _lit = false;
        stopTimer();
        repaint();
    }

    void MainComponent::MidiActivityLed::paint(juce::Graphics& g)
    {
        g.setColour(_lit ? juce::Colour::fromRGB(255, 120, 40) : juce::Colour::fromRGB(60, 30, 10));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 2.0F);
    }

    void MainComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
        if (_background.isValid())
        {
            g.drawImage(_background, getLocalBounds().toFloat()); // [RQ-GUI-007]
        }
    }

    // --- menu bar [RQ-GUI-008] ---------------------------------------------

    juce::StringArray MainComponent::getMenuBarNames()
    {
        return {"File", "Patch", "Tools", "Help"};
    }

    juce::PopupMenu MainComponent::getMenuForIndex(int index, const juce::String&)
    {
        juce::PopupMenu menu;
        switch (index)
        {
            case 0: // File
                menu.addItem(1, "New");
                menu.addItem(2, "Open...");
                menu.addItem(3, "Save...");
                menu.addSeparator();
                menu.addItem(4, "Exit");
                break;
            case 1: // Patch
                menu.addItem(10, "Next");
                menu.addItem(11, "Previous");
                menu.addItem(12, "Go to...");
                menu.addItem(13, "Store...");
                menu.addSeparator();
                menu.addItem(14, "Rename...");
                menu.addItem(15, "Randomize");
                break;
            case 2: // Tools
                menu.addItem(20, "MIDI settings...");
                menu.addItem(21, "Tune request");
                break;
            case 3: // Help
                menu.addItem(30, "About");
                break;
            default:
                break;
        }
        return menu;
    }

    void MainComponent::menuItemSelected(int menuItemId, int)
    {
        switch (menuItemId)
        {
            case 3: // Save
            case 2: // Open — reuse the shortcut buttons' file choosers
                for (auto& button : _shortcutButtons)
                {
                    if (button->getButtonText() == (menuItemId == 3 ? "SV" : "LD"))
                    {
                        button->onClick();
                    }
                }
                break;
            case 4:
                juce::JUCEApplication::getInstance()->systemRequestedQuit();
                break;
            case 10:
                _controller->increaseCurrentProgramNumber();
                break;
            case 11:
                _controller->decreaseCurrentProgramNumber();
                break;
            case 12:
                showStoreOrGotoDialog("Go to patch", _controller->currentProgramNumber(),
                                      [this](int program)
                                      { _controller->sendProgramChangeAndGetSinglePatchFromSynth(program); });
                break;
            case 13:
                showStoreOrGotoDialog("Store", _controller->currentProgramNumber(),
                                      [this](int program) { _controller->storeSinglePatchToSynth(program); });
                break;
            case 14:
                showRenameDialog(_controller->toneName(),
                                 [this](const std::string& name)
                                 {
                                     _controller->setToneName(name);
                                     _display.showToneInfo(_controller->currentProgramNumber(), name);
                                 });
                break;
            case 15:
                _controller->randomizeTone(midiapp::controller::RandomizeToneArguments{});
                break;
            case 20:
                showMidiSettingsDialog(*_controller, *_settingsService, _backend);
                break;
            case 21:
                _controller->sendTuneRequestToSynth();
                break;
            case 30:
                showAboutDialog("Xplorer 0.1.0");
                break;
            default:
                break;
        }
    }

    ScaledCanvasComponent::ScaledCanvasComponent()
        : _menuBar(&_canvas)
    {
        addAndMakeVisible(_menuBar);
        addAndMakeVisible(_canvas);
        setSize(LOGICAL_CANVAS_WIDTH, LOGICAL_CANVAS_HEIGHT + 24);
    }

    ScaledCanvasComponent::~ScaledCanvasComponent()
    {
        _menuBar.setModel(nullptr);
    }

    void ScaledCanvasComponent::resized()
    {
        auto area = getLocalBounds();
        _menuBar.setBounds(area.removeFromTop(24));

        // Uniform scale, aspect ratio preserved, canvas centered below the menu. [RQ-GUI-005]
        const auto scale = juce::jmin(static_cast<float>(area.getWidth()) / LOGICAL_CANVAS_WIDTH,
                                      static_cast<float>(area.getHeight()) / LOGICAL_CANVAS_HEIGHT);
        const auto scaledWidth = LOGICAL_CANVAS_WIDTH * scale;
        const auto scaledHeight = LOGICAL_CANVAS_HEIGHT * scale;
        _canvas.setTransform(juce::AffineTransform::scale(scale).translated(
            (static_cast<float>(area.getWidth()) - scaledWidth) * 0.5F,
            static_cast<float>(area.getY()) + (static_cast<float>(area.getHeight()) - scaledHeight) * 0.5F));
    }

    void ScaledCanvasComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black); // letterbox bars when aspect differs
    }
}
