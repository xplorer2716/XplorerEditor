#include "MainComponent.hpp"

#include "BinaryData.h"
#include "Dialogs.hpp"
#include "SettingsDialog.hpp"
#include "midiapp/service/FileUtils.hpp"
#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"
#include "xplorer/model/XpanderConstants.hpp"

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
        _lookAndFeel = std::make_unique<XplorerLookAndFeel>(
            juce::Colour(static_cast<juce::uint32>(
                _settingsService->allUsersSettings().uiConfig.knobLedBorderColor))); // [RQ-GUI-031]
        // Global skin: covers fixed-block, page-family and matrix controls alike.
        juce::LookAndFeel::setDefaultLookAndFeel(_lookAndFeel.get());

        // Route controller parameter changes to the registry (UI refresh). [RQ-GUI-003]
        _controller->setAutomationParameterChangeHandler(
            [this](const std::string& name, int value)
            {
                _registry->onParameterChanged(name, value);
                _display.showParameter(name, value); // [RQ-GUI-020]
            });
        // Local panel edits refresh the VFD too, like the reference
        // MainForm.AnyValuedControl_ValueChanged. [RQ-GUI-020]
        _registry->setLocalEditHandler(
            [this](const std::string& name, int value) { _display.showParameter(name, value); });
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
        _controller->setAllDataDumpProgressionHandler(
            [this](const controller::AllDataDumpProgressionEvent& event)
            { onAllDataDumpProgression(event); }); // [RQ-GUI-026]

        setSize(LOGICAL_CANVAS_WIDTH, LOGICAL_CANVAS_HEIGHT);
        placeFixedBlockControls();
        placeStaticLabels();
        createPageFamilyBlocks();
        _matrixPanel = std::make_unique<ModMatrixPanel>(*this, *_controller);
        createShortcutButtonsAndDisplay();
        _registry->refreshAllFromModel(); // seed all controls with the current tone
        _display.showToneInfo(_controller->currentProgramNumber(), _controller->toneName());

        // Apply persisted MIDI device/channel/delay settings at startup. [RQ-GUI-025]
        applyMidiSettings(*_controller, *_settingsService, _backend);
    }

    MainComponent::~MainComponent()
    {
        juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    }

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
                    auto check = std::make_unique<BoundCheckBox>(*_registry, tag,
                                                                juce::String(spec.label)); // [RQ-GUI-032]
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

            // Skinned via the default LookAndFeel (set in the ctor). [RQ-GUI-031]
            component->setBounds(spec.x, spec.y, spec.width, spec.height);
            addAndMakeVisible(*component);
            if (bound != nullptr)
            {
                _registry->bind(tag, *bound);
            }
            _controls.push_back(std::move(component));
        }
    }

    void MainComponent::placeStaticLabels()
    {
        // Standalone Label controls carry captions in the resx (e.g. the
        // "VCO1 MOD =" and matrix column headers) that are not baked into the
        // background bitmap. [RQ-GUI-032]
        for (const auto& spec : controlTable())
        {
            if (spec.kind != ControlKind::Label || std::string(spec.label).empty())
            {
                continue;
            }
            auto label = std::make_unique<juce::Label>(juce::String(spec.id), juce::String(spec.label));
            label->setBounds(spec.x, spec.y, spec.width, spec.height);
            label->setJustificationType(juce::Justification::centredLeft);
            label->setColour(juce::Label::textColourId, juce::Colours::white);
            label->setInterceptsMouseClicks(false, false);
            addAndMakeVisible(*label);
            _controls.push_back(std::move(label));
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

        // 8 shortcut buttons using the reference GIF triples (normal/hover/down).
        // [RQ-GUI-021, RQ-GUI-031]
        _shortcutActions["btPatchMinus"] = [this] { _controller->decreaseCurrentProgramNumber(); };
        _shortcutActions["btPatchPlus"] = [this] { _controller->increaseCurrentProgramNumber(); };
        _shortcutActions["btPatchGoto"] = [this]
        {
            showStoreOrGotoDialog("Go to patch", _controller->currentProgramNumber(),
                                  [this](int program)
                                  { _controller->sendProgramChangeAndGetSinglePatchFromSynth(program); });
        };
        _shortcutActions["btPatchRandom"] = [this]
        { _controller->randomizeTone(midiapp::controller::RandomizeToneArguments{}); };
        _shortcutActions["btPatchLoad"] = [this]
        {
            _fileChooser = std::make_unique<juce::FileChooser>("Load patch", juce::File(), "*.syx");
            _fileChooser->launchAsync(
                juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& chooser)
                {
                    const auto file = chooser.getResult();
                    if (file.existsAsFile())
                    {
                        loadSysexFileByType(file.getFullPathName()); // classify like the reference
                    }
                });
        };
        _shortcutActions["btPatchSave"] = [this]
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
        };
        _shortcutActions["btPatchStore"] = [this]
        {
            showStoreOrGotoDialog("Store", _controller->currentProgramNumber(),
                                  [this](int program) { _controller->storeSinglePatchToSynth(program); });
        };
        _shortcutActions["btSettings"] = [this] { openSettingsDialog(); };

        // GIF base names (goto's "normal" image is gotopatch.gif in the assets).
        const std::map<std::string, std::string> gifBase = {
            {"btPatchMinus", "minus"}, {"btPatchPlus", "plus"},   {"btPatchGoto", "goto"},
            {"btPatchRandom", "random"}, {"btPatchLoad", "load"}, {"btPatchSave", "save"},
            {"btPatchStore", "store"},   {"btSettings", "settings"}};

        auto loadGif = [](const std::string& fileName) -> juce::Image
        {
            int size = 0;
            const std::string resource = fileName + "_gif";
            const auto* data = BinaryData::getNamedResource(resource.c_str(), size);
            return data != nullptr ? juce::ImageFileFormat::loadFrom(data, static_cast<std::size_t>(size))
                                   : juce::Image();
        };

        for (const auto& [id, base] : gifBase)
        {
            for (const auto& spec : controlTable())
            {
                if (std::string(spec.id) != id)
                {
                    continue;
                }
                const std::string normalName = (base == "goto") ? "gotopatch" : base;
                const auto normal = loadGif(normalName);
                const auto hover = loadGif(base + "hover");
                const auto down = loadGif(base + "down");
                auto button = std::make_unique<juce::ImageButton>(id);
                button->setImages(true, true, true, normal, 1.0F, {}, hover, 1.0F, {}, down, 1.0F, {});
                button->setBounds(spec.x, spec.y, spec.width, spec.height);
                button->onClick = _shortcutActions[id];
                addAndMakeVisible(*button);
                _shortcutButtons.push_back(std::move(button));
                break;
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
            {
                menu.addItem(20, "Settings...");
                menu.addItem(21, "Tune request");
                menu.addItem(22, "Piano keyboard");
                juce::PopupMenu singlePatches;
                singlePatches.addItem(40, "Get all single patches from synth");
                singlePatches.addItem(41, "Extract all single patches from file");
                menu.addSubMenu("Single patches...", singlePatches);
                juce::PopupMenu allDataDump;
                allDataDump.addItem(42, "Backup all data");
                allDataDump.addItem(43, "Restore all data");
                menu.addSubMenu("Backup/Restore...", allDataDump);
                break;
            }
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
            case 2: // Open — reuse the load action
                _shortcutActions["btPatchLoad"]();
                break;
            case 3: // Save
                _shortcutActions["btPatchSave"]();
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
                openSettingsDialog();
                break;
            case 21:
                _controller->sendTuneRequestToSynth();
                break;
            case 22: // [RQ-GUI-028]
                if (_pianoWindow == nullptr)
                {
                    _pianoWindow = std::make_unique<PianoWindow>(*_controller);
                }
                else
                {
                    _pianoWindow->setVisible(true);
                    _pianoWindow->toFront(true);
                }
                break;
            case 30:
                showAboutDialog("Xplorer 0.1.0");
                break;
            case 40:
                getAllSinglePatchesFromSynth();
                break;
            case 41:
                showExtractSingleTonesDialog(*_controller);
                break;
            case 42:
                backupAllData();
                break;
            case 43:
                restoreAllData();
                break;
            default:
                break;
        }
    }

    void MainComponent::openSettingsDialog()
    {
        showSettingsDialog(*_controller, *_settingsService, _backend,
                           [this](int argb) { updateLedColour(argb); }); // [RQ-GUI-025]
    }

    void MainComponent::updateLedColour(int argb)
    {
        // Rebuild the skin with the new LED colour and repaint the tree. [RQ-GUI-031]
        juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
        _lookAndFeel = std::make_unique<XplorerLookAndFeel>(juce::Colour(static_cast<juce::uint32>(argb)));
        juce::LookAndFeel::setDefaultLookAndFeel(_lookAndFeel.get());
        if (auto* top = getTopLevelComponent())
        {
            top->sendLookAndFeelChange();
        }
    }

    void MainComponent::backupAllData()
    {
        _fileChooser = std::make_unique<juce::FileChooser>("Backup all data", juce::File(), "*.syx");
        _fileChooser->launchAsync(
            juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser)
            {
                const auto file = chooser.getResult();
                if (file == juce::File())
                {
                    return;
                }
                _allDataDumpModeIsAll = true;
                try
                {
                    _controller->backupAllDataDumpToFile(file.getFullPathName().toStdString()); // [RQ-CTL-005]
                }
                catch (const std::exception& e)
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                          "All data dump backup", e.what());
                }
            });
    }

    void MainComponent::restoreAllData()
    {
        _fileChooser = std::make_unique<juce::FileChooser>("Restore all data", juce::File(), "*.syx");
        _fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser)
            {
                const auto file = chooser.getResult();
                if (file.existsAsFile())
                {
                    runRestoreAllDataWithProgress(*_controller, file.getFullPathName().toStdString());
                }
            });
    }

    void MainComponent::loadSysexFileByType(const juce::String& filePath)
    {
        const auto path = filePath.toStdString();
        switch (_controller->determineSysexFileType(path))
        {
            case model::SysexFileType::SingleTone:
                try
                {
                    _controller->loadTone(path); // [RQ-CTL-001]
                }
                catch (const std::exception& e)
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                          "Load patch", e.what());
                }
                break;

            case model::SysexFileType::AllDataDump:
                // A bank file overwrites every patch in the synth: confirm first,
                // as the reference does. [RQ-CTL-001, RQ-GUI-026]
                juce::AlertWindow::showOkCancelBox(
                    juce::MessageBoxIconType::WarningIcon, "Confirm All Data Dump Restore",
                    "The selected file is a bank file that may overwrite ALL patches "
                    "in the synth's memory. Proceed?",
                    "Restore", "Cancel", nullptr,
                    juce::ModalCallbackFunction::create(
                        [this, path](int result)
                        {
                            if (result == 1)
                            {
                                runRestoreAllDataWithProgress(*_controller, path);
                            }
                        }));
                break;

            case model::SysexFileType::Unknown:
            default:
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                      "Warning", "Unable to determine sysex file type.");
                break;
        }
    }

    void MainComponent::getAllSinglePatchesFromSynth()
    {
        _fileChooser = std::make_unique<juce::FileChooser>(
            "Destination folder for single patch sysex files", juce::File(), juce::String());
        _fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
            [this](const juce::FileChooser& chooser)
            {
                const auto folder = chooser.getResult();
                if (folder == juce::File())
                {
                    return;
                }
                _allDataDumpModeIsAll = false;
                try
                {
                    _controller->getSingleTonesFromSynth(folder.getFullPathName().toStdString()); // [RQ-CTL-004]
                }
                catch (const std::exception& e)
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                          "Single patches", e.what());
                }
            });
    }

    void MainComponent::onAllDataDumpProgression(const controller::AllDataDumpProgressionEvent& event)
    {
        // Event-driven progress for backup / get-all-single-patches (the
        // reception is fed by incoming MIDI). Port of MainForm's progression
        // handler. [RQ-GUI-026]
        const int received = event.singlePatchCount + event.multiPatchCount;
        const int maxValue = _allDataDumpModeIsAll
                                 ? model::constants::SINGLE_TONES_MAX_COUNT + model::constants::MULTI_PATCHES_MAX_COUNT
                                 : model::constants::SINGLE_TONES_MAX_COUNT;

        if (_progressWindow == nullptr)
        {
            _progressWindow = std::make_unique<ProgressWindow>();
            _progressWindow->begin(_allDataDumpModeIsAll ? "All data dump request in progress..."
                                                         : "Receiving single patches...",
                                   maxValue);
        }

        const bool finished = !event.isWaitingForAllDataDumpRequest || received >= maxValue;
        if (finished)
        {
            _progressWindow.reset();
        }
        else if (event.multiPatchCount == 0)
        {
            _progressWindow->setStatus("Receiving single patch [" + juce::String(event.singlePatchCount) + "/"
                                           + juce::String(model::constants::SINGLE_TONES_MAX_COUNT) + "]",
                                       received);
        }
        else
        {
            _progressWindow->setStatus("Receiving multi patch [" + juce::String(event.multiPatchCount) + "/"
                                           + juce::String(model::constants::MULTI_PATCHES_MAX_COUNT) + "]",
                                       received);
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

    bool ScaledCanvasComponent::isInterestedInFileDrag(const juce::StringArray& files)
    {
        // Reference OnDragEnter: accept a file drop; the drop handler keeps
        // only the first .syx. [reference MainForm AllowDrop]
        for (const auto& file : files)
        {
            if (file.endsWithIgnoreCase(midiapp::service::SYSEX_FILE_EXTENSION_WITH_DOT))
            {
                return true;
            }
        }
        return false;
    }

    void ScaledCanvasComponent::filesDropped(const juce::StringArray& files, int, int)
    {
        // Reference OnDragDrop: act on files[0] only, when it is an existing .syx.
        if (files.isEmpty())
        {
            return;
        }
        const auto& first = files[0];
        if (juce::File(first).existsAsFile()
            && first.endsWithIgnoreCase(midiapp::service::SYSEX_FILE_EXTENSION_WITH_DOT))
        {
            _canvas.loadSysexFileByType(first);
        }
    }
}
