#include "MainComponent.hpp"

#include "BackgroundRenderer.hpp"
#include "BinaryData.h"
#include "DesignTokens.hpp"
#include "Dialogs.hpp"
#include "SettingsDialog.hpp"
#include "midiapp/service/FileUtils.hpp"
#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"
#include "xplorer/app/ModulationHighlight.hpp"
#include "xplorer/model/XpanderConstants.hpp"

#include <juce_core/juce_core.h>

namespace xplorer::app
{
    namespace
    {
        // Fixed (non page-family) blocks handled in TASK-JUC-063.
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
        _dispatcher = std::make_shared<JuceEventDispatcher>();
        _settingsService = std::make_unique<settings::XmlSettingsService>(
            settingsDirectory().toStdString());
        _controller = std::make_unique<controller::XpanderController>(
            _backend, *_settingsService, _dispatcher, "XPLORER");
        _registry = std::make_unique<ParameterBindingRegistry>(*_controller);
        _lookAndFeel = std::make_unique<XplorerLookAndFeel>(
            juce::Colour(static_cast<juce::uint32>(
                _settingsService->allUsersSettings().uiConfig.knobLedBorderColor))); // [RQ-GUI-031]
        // Block palette: defaults with the persisted per-block overrides
        // applied. [RQ-DSN-095, RQ-SET-007, ADR-JUC-020]
        _lookAndFeel->setBlockPalette(
            resolveBlockPalette(_settingsService->allUsersSettings().uiConfig));
        // Global skin: covers fixed-block, page-family and matrix controls alike.
        juce::LookAndFeel::setDefaultLookAndFeel(_lookAndFeel.get());
        _vfd = std::make_unique<VfdDisplayHelper>(_display, *_controller);

        // Route controller parameter changes to the registry (UI refresh). [RQ-GUI-003]
        // Automation from the synth moves the control, then the VFD shows it —
        // the reference updates the display on programmatic value changes too.
        _controller->setAutomationParameterChangeHandler(
            [this](const std::string& name, int value)
            {
                _registry->onParameterChanged(name, value);
                _vfd->showControlEdit(name, _registry->displayTextFor(name)); // [RQ-GUI-020]
            });
        // Local panel edits refresh the VFD too, like the reference
        // MainForm.AnyValuedControl_ValueChanged. [RQ-GUI-020]
        _registry->setLocalEditHandler(
            [this](const std::string& name)
            { _vfd->showControlEdit(name, _registry->displayTextFor(name)); });
        _controller->setMidiActivityHandler(
            [this](controller::EnumMidiDevice device) { _midiLed.flash(device); }); // [RQ-GUI-022]
        _controller->setFullToneChangeHandler(
            [this](const controller::FullToneChangeEvent&)
            {
                _registry->refreshAllFromModel();
                if (_matrixPanel != nullptr)
                {
                    _matrixPanel->refreshAll(); // [RQ-GUI-017]
                }
                _vfd->showToneInfo(); // [RQ-GUI-020]
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
        _vfd->showToneInfo();
        _matrixPanel->setEditHandler(
            [this](int entryNumber)
            {
                _vfd->showModulationEntry(_controller->getModulationEntryByNumber(entryNumber),
                                          false); // [RQ-GUI-020]
            });

        // Modulation-matrix hover highlight: the colour is derived from the
        // LookAndFeel (single source of truth, ADR-JUC-011); knobs and page-family
        // selectors forward their hover. [RQ-GUI-018]
        _hover.onEnter = [this](juce::Component* component) { onControlHovered(component); };
        _hover.onExit = [this]
        {
            if (_matrixPanel != nullptr)
            {
                _matrixPanel->clearHighlight();
            }
        };
        for (auto& block : _familyBlocks)
        {
            block->attachHoverListener(&_hover);
            for (const auto& selector : block->selectors())
            {
                _selectorSourceId[selector.button.get()] = selector.id;
            }
        }

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
                    // Fixed-block radio panels (FM destination, LAG timing) render
                    // as real two-way radio groups, matching the reference form
                    // (owner request). [RQ-GUI-038, RQ-GUI-040, ADR-JUC-016]
                    auto radios = std::make_unique<BoundRadioGroup>(*_registry, tag, options);
                    bound = radios.get();
                    component = std::move(radios);
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
            // Fixed knobs forward their hover to the matrix highlight. [RQ-GUI-018]
            if (spec.kind == ControlKind::KnobControl)
            {
                component->addMouseListener(&_hover, false);
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
            // Owner-arbitrated deviation (ADR-JUC-007): the reference bounds
            // (267x75) only fit 4 glyph rows; grow to 5 rows (82 px) upward —
            // the area above is free artwork — so the MIDI CC line is always
            // visible.
            const int glassHeight = 5 * DisplayPanel::GLYPH_HEIGHT + 2;
            const int glassY = spec->y - (glassHeight - spec->height);

            // The panel's bounds cover the bezel as well as the glass
            // (DEC-JUC-058), so they are the glass expanded by the band's
            // thickness. The band is token-driven, hence computed here rather
            // than baked into the control table: retuning a margin must not
            // require editing coordinates. The *lift* is a layout decision and
            // does live in the table (DEC-JUC-059) — this applies no offset of
            // its own. [RQ-GUI-050, RQ-DSN-098]
            _display.setBounds(juce::Rectangle<int>(spec->x, glassY, spec->width, glassHeight)
                                   .expanded(tokens::component::vfdBezelMarginH,
                                             tokens::component::vfdBezelMarginV));
            addAndMakeVisible(_display);
        }

        // MIDI activity LED. Bounds are the extracted spec expanded by the
        // glow margin (DEC-JUC-097) -- same idiom as the VFD bezel above,
        // applied here because the 8 px-tall panel has no room of its own
        // for a lit lamp's glow. [RQ-GUI-056, ADR-JUC-031]
        for (const auto& spec : controlTable())
        {
            if (spec.kind == ControlKind::LedPanelControl)
            {
                _midiLed.setBounds(juce::Rectangle<int>(spec.x, spec.y, spec.width, spec.height)
                                       .expanded(ledGlowMarginPx()));
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
                        _controller->saveXplorerTone(file.getFullPathName().toStdString());
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

    MainComponent::LedPanelComponent::LedPanelComponent()
    {
        setInterceptsMouseClicks(false, false);
        // Non-opaque: the background artwork shows through between the LEDs,
        // like the reference's transparent panel.
    }

    void MainComponent::LedPanelComponent::flash(controller::EnumMidiDevice device)
    {
        // Port of OnMidiDataSendReceive's device -> LED index mapping.
        const std::size_t index = device == controller::EnumMidiDevice::AutomationInputDevice ? 0
                                  : device == controller::EnumMidiDevice::SynthInputDevice    ? 1
                                                                                              : 2;
        _litUntil[index] = juce::Time::currentTimeMillis() + HOLD_MILLISECONDS;
        if (!isTimerRunning())
        {
            startTimer(TICK_MILLISECONDS); // decay tick, only while lit [ADR-JUC-008]
        }
        repaint();
    }

    void MainComponent::LedPanelComponent::timerCallback()
    {
        const auto now = juce::Time::currentTimeMillis();
        bool anyLit = false;
        for (const auto expiry : _litUntil)
        {
            anyLit = anyLit || expiry > now;
        }
        if (!anyLit)
        {
            stopTimer();
        }
        repaint();
    }

    void MainComponent::LedPanelComponent::paint(juce::Graphics& g)
    {
        // This component's bounds are the extracted _ledPanelControl rect
        // expanded by ledGlowMarginPx() (DEC-JUC-097); `area` below undoes
        // that inflation, so every lamp position is derived from the ORIGINAL
        // panel rect and never moves when the margin retunes.
        const auto area = getLocalBounds().reduced(ledGlowMarginPx());

        // Reference LedPanelControl: automation-in / synth-in / synth-out.
        // Round lamp, glow, then rim -- a dedicated indicator treatment, not
        // a control one (DEC-JUC-095): a check box/radio says "you may set
        // this", an LED reports state the user cannot set.
        // [RQ-GUI-056, ADR-JUC-031]
        static const std::array<juce::Colour, LED_COUNT> onColours = {
            tokens::semantic::indicatorAutomation, tokens::semantic::indicatorSynthIn,
            tokens::semantic::indicatorSynthOut};
        const auto offColour = tokens::semantic::indicatorOffFill;
        const auto borderColour = tokens::semantic::indicatorOffBorder;

        const int size = tokens::component::indicatorSize;
        const auto now = juce::Time::currentTimeMillis();
        const int horizontalSpace = (area.getWidth() - LED_COUNT * size) / (LED_COUNT + 1);
        const float y = static_cast<float>(area.getY() + (area.getHeight() - size) / 2);

        for (int i = 0; i < LED_COUNT; ++i)
        {
            const auto index = static_cast<std::size_t>(i);
            const bool lit = _litUntil[index] > now;
            const juce::Rectangle<float> lamp(
                static_cast<float>(area.getX() + horizontalSpace * (i + 1) + i * size), y,
                static_cast<float>(size), static_cast<float>(size));

            if (lit)
            {
                // Glow beneath the body, only while lit, so a lit LED reads
                // as emitting rather than as a colour swap. Radius is a
                // MULTIPLE OF THE LAMP'S OWN RADIUS, verified clear of the
                // VFD glass and the button row (ADR-JUC-031, DEC-JUC-095).
                const auto centre = lamp.getCentre();
                const float glowRadius =
                    static_cast<float>(size) * 0.5F * tokens::component::indicatorGlowRadius;
                const auto& onColour = onColours[index];
                juce::ColourGradient glow(
                    onColour.withAlpha(tokens::component::indicatorGlowAlpha), centre,
                    onColour.withAlpha(0.0F), centre.translated(glowRadius, 0.0F), true);
                g.setGradientFill(glow);
                g.fillEllipse(juce::Rectangle<float>(glowRadius * 2.0F, glowRadius * 2.0F)
                                  .withCentre(centre));
            }

            g.setColour(lit ? onColours[index] : offColour);
            g.fillEllipse(lamp);
            g.setColour(borderColour);
            g.drawEllipse(lamp, tokens::semantic::strokeBorder);
        }
    }

    void MainComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
        // Live palette from the single runtime authority. [RQ-GUI-037, RQ-DSN-095,
        // ADR-JUC-013, ADR-JUC-020 (DEC-JUC-036)]
        paintVectorBackground(g, _lookAndFeel->blockPalette());
    }

    // --- menu bar [RQ-GUI-008] ---------------------------------------------

    void applyWindowScale(juce::ResizableWindow& window, float scale)
    {
        const auto size = windowSizeForScale(scale);

        // If the preset does not fit the display the window is on, full
        // screen is the closest equivalent to "as large as this preset asks
        // for" — an oversized window that spills off-screen serves the user
        // worse than the largest size the screen actually has. [RQ-SCL-002]
        const auto* display = juce::Desktop::getInstance().getDisplays().getDisplayForRect(
            window.getScreenBounds());
        if (display != nullptr
            && (size.width > display->userArea.getWidth() || size.height > display->userArea.getHeight()))
        {
            window.setFullScreen(true);
            return;
        }

        if (window.isFullScreen())
        {
            window.setFullScreen(false);
        }
        window.centreWithSize(size.width, size.height);
    }

    namespace
    {
        // View-menu item ids. Kept clear of every existing range because
        // menuItemSelected dispatches by id, which is what lets "View" be
        // inserted mid-bar without disturbing Patch/Tools/Help. [DEC-JUC-065]
        constexpr int VIEW_SCALE_FIRST_ID = 50;
        constexpr int VIEW_FULL_SCREEN_ID = 60;

        // The three URLs the reference's Help menu opens, verbatim from its
        // XplorerConstants. Opened in the system browser, no networking of our
        // own -- same as the reference's OpenBrowserWithUrl.
        // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-101)]
        constexpr const char* USER_MANUAL_URL =
            "https://github.com/xplorer2716/XplorerEditor/blob/main/Xplorer/xdata/manual/"
            "XplorerUserManual.pdf";
        constexpr const char* RELEASES_URL = "https://github.com/xplorer2716/XplorerEditor/releases";
        constexpr const char* WEBSITE_URL = "https://xplorer2716.github.io/XplorerEditor.site/";

        // The default patch File > New loads, copied beside the executable by
        // the build (app/CMakeLists.txt). The reference resolves it the same
        // way -- executable directory + this name -- and "New" there means
        // "load this patch", not "blank the editor".
        // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-100)]
        constexpr const char* DEFAULT_TONE_FILENAME = "oberheim.syx";

        [[nodiscard]] juce::File defaultToneFile()
        {
            return juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                .getSiblingFile(DEFAULT_TONE_FILENAME);
        }

        juce::String scaleItemName(float scale)
        {
            // "1x", "1.25x", ... — trailing zeros trimmed so 1.5 does not read
            // as "1.50x".
            auto text = juce::String(scale, 2).trimCharactersAtEnd("0").trimCharactersAtEnd(".");
            return text + "x";
        }
    }

    juce::StringArray MainComponent::getMenuBarNames()
    {
        return {"File", "Patch", "View", "Tools", "Help"};
    }

    int MainComponent::currentWindowWidth() const
    {
        // The content component is what carries the size RQ-SCL-001/002 state;
        // the DocumentWindow's own bounds equal it only because the port uses a
        // native title bar, so ask the content rather than rely on that.
        if (const auto* canvas = findParentComponentOfClass<ScaledCanvasComponent>())
        {
            return canvas->getWidth();
        }
        return 0;
    }

    juce::ResizableWindow* MainComponent::topLevelWindow() const
    {
        return dynamic_cast<juce::ResizableWindow*>(getTopLevelComponent());
    }

    juce::PopupMenu MainComponent::getMenuForIndex(int index, const juce::String&)
    {
        juce::PopupMenu menu;
        switch (index)
        {
            // Order, wording and grouping are the reference's own, read from
            // MainForm.Designer.cs's DropDownItems.AddRange calls and the
            // matching .resx strings -- not an approximation.
            // [RQ-GUI-008, ADR-JUC-032]
            case 0: // File
                menu.addItem(1, "New");
                menu.addItem(2, "Open");
                menu.addSeparator();
                // The reference's "Save as" is deliberately absent: our Save
                // already always prompts for a destination (no current-file
                // path is tracked), so it IS the reference's Save as.
                // [RQ-GUI-008, owner-confirmed deviation]
                menu.addItem(3, "Save");
                menu.addSeparator();
                menu.addItem(4, "Exit");
                break;
            case 1: // Patch
                menu.addItem(11, "Previous");
                menu.addItem(10, "Next");
                menu.addItem(12, "Go to patch...");
                menu.addSeparator();
                menu.addItem(15, "Randomize");
                menu.addItem(14, "Rename");
                menu.addItem(13, "Store");
                menu.addItem(16, "Synchronize");
                break;
            case 2: // View [RQ-SCL-002, RQ-SCL-003]
            {
                // Ticked from the window's ACTUAL width, never from a memory of
                // the last click: a dragged or OS-clamped window then correctly
                // shows nothing ticked. [DEC-JUC-066]
                const auto width = currentWindowWidth();
                for (std::size_t i = 0; i < WINDOW_SCALE_PRESETS.size(); ++i)
                {
                    const auto scale = WINDOW_SCALE_PRESETS[i];
                    menu.addItem(VIEW_SCALE_FIRST_ID + static_cast<int>(i), scaleItemName(scale),
                                 true, windowSizeForScale(scale).width == width);
                }
                menu.addSeparator();
                const auto* window = topLevelWindow();
                menu.addItem(VIEW_FULL_SCREEN_ID, "Full screen", true,
                             window != nullptr && window->isFullScreen());
                break;
            }
            case 3: // Tools
            {
                menu.addItem(20, "Settings");
                menu.addItem(21, "Tune Request");
                // No reference counterpart -- the second sanctioned JUCE-only
                // item after the View menu, kept at the owner's decision.
                // [RQ-GUI-028, RQ-GUI-008]
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
            case 4: // Help
                menu.addItem(31, "Xplorer help");
                menu.addSeparator();
                menu.addItem(32, "Check for new releases");
                menu.addItem(33, "Go to website");
                menu.addItem(30, "About...");
                break;
            default:
                break;
        }
        return menu;
    }

    void MainComponent::menuItemSelected(int menuItemId, int)
    {
        // View menu, handled before the switch because its scale items are a
        // contiguous id range rather than individual cases. [RQ-SCL-002]
        const auto scaleIndex = menuItemId - VIEW_SCALE_FIRST_ID;
        if (scaleIndex >= 0 && scaleIndex < static_cast<int>(WINDOW_SCALE_PRESETS.size()))
        {
            if (auto* window = topLevelWindow())
            {
                applyWindowScale(*window, WINDOW_SCALE_PRESETS[static_cast<std::size_t>(scaleIndex)]);
            }
            return;
        }
        if (menuItemId == VIEW_FULL_SCREEN_ID) // [RQ-SCL-003, DEC-JUC-067]
        {
            if (auto* window = topLevelWindow())
            {
                // JUCE keeps the pre-fullscreen bounds itself and restores them
                // on the way back, so no geometry is remembered here.
                window->setFullScreen(!window->isFullScreen());
            }
            return;
        }

        switch (menuItemId)
        {
            case 1: // New — load the bundled default patch, like the reference's
                    // FileOperationsManager.NewPatch(). Not a blank editor: the
                    // program number comes from the file's own sysex data.
                    // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-100)]
            {
                const auto file = defaultToneFile();
                if (!file.existsAsFile())
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::MessageBoxIconType::WarningIcon, "New patch",
                        "Unable to create a new patch: " + file.getFullPathName() + " is missing.");
                    break;
                }
                try
                {
                    _controller->loadXplorerTone(file.getFullPathName().toStdString());
                }
                catch (const std::exception& e)
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                          "New patch",
                                                          juce::String("Unable to create a new patch: ")
                                                              + e.what());
                }
                break;
            }
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
                                     _vfd->showToneInfo();
                                 });
                break;
            case 15:
                _controller->randomizeTone(midiapp::controller::RandomizeToneArguments{});
                break;
            case 16: // Synchronize — re-fetch the current patch from the synth,
                     // the same call Go to / Store already make.
                     // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-101)]
                _controller->sendProgramChangeAndGetSinglePatchFromSynth(
                    _controller->currentProgramNumber());
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
            // The three Help URLs, opened in the system browser exactly as the
            // reference's OpenBrowserWithUrl does — no update check of our own.
            // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-101)]
            case 31:
                juce::URL(USER_MANUAL_URL).launchInDefaultBrowser();
                break;
            case 32:
                juce::URL(RELEASES_URL).launchInDefaultBrowser();
                break;
            case 33:
                juce::URL(WEBSITE_URL).launchInDefaultBrowser();
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
                           [this](int argb) { updateLedColour(argb); }, // [RQ-GUI-025]
                           [this](const BlockPalette& palette)
                           { updateBlockPalette(palette); }); // [RQ-GUI-046]
    }

    void MainComponent::updateBlockPalette(const BlockPalette& palette)
    {
        // Live preview / accept / cancel-restore all land here: mutate the
        // palette in place and repaint the tree — no LookAndFeel rebuild.
        // [RQ-DSN-095, ADR-JUC-020 (DEC-JUC-038)]
        _lookAndFeel->setBlockPalette(palette);
        if (auto* top = getTopLevelComponent())
        {
            top->sendLookAndFeelChange();
        }
    }

    void MainComponent::updateLedColour(int argb)
    {
        // Rebuild the skin with the new LED colour and repaint the tree; the
        // block palette is carried across the rebuild so a customised palette
        // survives an LED-colour change. [RQ-GUI-031, RQ-DSN-095, ADR-JUC-020]
        const auto palette = _lookAndFeel->blockPalette();
        juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
        _lookAndFeel = std::make_unique<XplorerLookAndFeel>(juce::Colour(static_cast<juce::uint32>(argb)));
        _lookAndFeel->setBlockPalette(palette);
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

    void MainComponent::onControlHovered(juce::Component* component)
    {
        if (_matrixPanel == nullptr)
        {
            return;
        }
        // A knob that is a modulation destination highlights matching dest rows.
        if (const auto* bound = dynamic_cast<BoundControl*>(component))
        {
            if (const auto destination = modulationDestinationForParameter(bound->parameterName()))
            {
                _matrixPanel->highlightDestinations(static_cast<int>(*destination));
            }
            return;
        }
        // A page-family selector that is a modulation source highlights matching
        // source rows.
        if (const auto found = _selectorSourceId.find(component); found != _selectorSourceId.end())
        {
            if (const auto source = modulationSourceForSelector(found->second))
            {
                _matrixPanel->highlightSources(static_cast<int>(*source));
            }
        }
    }

    void MainComponent::loadSysexFileByType(const juce::String& filePath)
    {
        const auto path = filePath.toStdString();
        switch (_controller->determineSysexFileType(path))
        {
            case model::SysexFileType::SingleTone:
                try
                {
                    _controller->loadXplorerTone(path); // [RQ-CTL-001]
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
        // No setSize here: the window states the size (windowSizeForScale) and
        // this component is laid out to it. Setting it here too would be a
        // second place declaring what the launch size is. [DEC-JUC-064]
    }

    ScaledCanvasComponent::~ScaledCanvasComponent()
    {
        _menuBar.setModel(nullptr);
    }

    void ScaledCanvasComponent::resized()
    {
        auto area = getLocalBounds();
        _menuBar.setBounds(area.removeFromTop(MENU_BAR_HEIGHT));

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
