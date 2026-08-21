#include "MainComponent.hpp"

#include "ShortcutIcons.hpp"

#include "BackgroundRenderer.hpp"
#include "BinaryData.h"
#include "DesignTokens.hpp"
#include "Dialogs.hpp"
#include "SettingsDialog.hpp"
#include "midiapp/service/FileUtils.hpp"
#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"
#include "xplorer/app/MenuIds.hpp"
#include "xplorer/app/ModulationHighlight.hpp"
#include "xplorer/model/XpanderConstants.hpp"

#include <juce_core/juce_core.h>

namespace xplorer::app
{
    namespace
    {
        /// A shortcut key. juce::Button already owns the hit testing, the
        /// keyboard handling and the hover/down state machine, so the vector
        /// treatment is exactly one overridden paint — nothing else about the
        /// eight buttons changes when they stop being bitmaps.
        /// [RQ-GUI-063, ADR-GUI-001 (DEC-GUI-001-A, DEC-GUI-001-B)]
        class ShortcutButton final : public juce::Button
        {
        public:
            ShortcutButton(const juce::String& name, ShortcutIcon icon)
                : juce::Button(name), _icon(icon)
            {
            }

            void paintButton(juce::Graphics& g, bool isHovered, bool isDown) override
            {
                // Read at paint time, not cached: the accent is user-themeable and
                // a live preview mutates the LookAndFeel in place. [ADR-JUC-020]
                const auto* lookAndFeel = dynamic_cast<XplorerLookAndFeel*>(&getLookAndFeel());
                const auto accent = lookAndFeel != nullptr ? lookAndFeel->ledColour()
                                                           : tokens::semantic::indicatorSynthIn;
                paintShortcutButton(g, getLocalBounds().toFloat(), _icon, accent, isHovered, isDown);
            }

        private:
            ShortcutIcon _icon;
        };
    }

    namespace
    {
        // Shortcut-button control ids, from the reference control table. Each
        // is a key into _shortcutActions AND the id matched against the control
        // table for geometry, so a typo at one site yields a
        // default-constructed empty action and a button that silently does
        // nothing. Declared once for all three uses. [RQ-QLT-004]
        constexpr const char* SHORTCUT_ID_PATCH_MINUS = "btPatchMinus";
        constexpr const char* SHORTCUT_ID_PATCH_PLUS = "btPatchPlus";
        constexpr const char* SHORTCUT_ID_PATCH_GOTO = "btPatchGoto";
        constexpr const char* SHORTCUT_ID_PATCH_RANDOM = "btPatchRandom";
        constexpr const char* SHORTCUT_ID_PATCH_LOAD = "btPatchLoad";
        constexpr const char* SHORTCUT_ID_PATCH_SAVE = "btPatchSave";
        constexpr const char* SHORTCUT_ID_PATCH_STORE = "btPatchStore";
        constexpr const char* SHORTCUT_ID_SETTINGS = "btSettings";

        // Titles of the two program-number dialogs, each shown from both a
        // shortcut button and a menu item. [RQ-QLT-005]
        constexpr const char* GOTO_PATCH_DIALOG_TITLE = "Go to patch";
        constexpr const char* STORE_PATCH_DIALOG_TITLE = "Store";

        // File-dialog wildcard for sysex files, derived from the extension
        // constant the same file already uses for matching, rather than spelled
        // out again at each of the four chooser sites. [RQ-QLT-006]
        const juce::String SYSEX_FILE_FILTER =
            juce::String("*") + midiapp::service::SYSEX_FILE_EXTENSION_WITH_DOT;

        // The settings directory is <data root>/Xplorer/Xplorer — the vendor
        // folder and the application folder happen to carry the same name, so
        // the nesting reads as a typo unless the component is named.
        // [RQ-SET-001, RQ-QLT-007]
        constexpr const char* SETTINGS_DIRECTORY_NAME = "Xplorer";

        // juce::AlertWindow::showOkCancelBox reports the first (OK) button as
        // 1 and the cancel button as 0. [RQ-QLT-007]
        constexpr int MODAL_RESULT_FIRST_BUTTON = 1;

        // Base of the JUCE radio-group ids handed to the page-family blocks,
        // one per family. Any value clear of the groups other components use
        // works; 100 is the one in service. [RQ-QLT-007]
        constexpr int PAGE_FAMILY_RADIO_GROUP_BASE = 100;

        // A page-family selector id is the family prefix plus one instance
        // digit ("ENV_1".."ENV_5"), so the suffix is bounded by these.
        // [RQ-GUI-012, RQ-QLT-007]
        constexpr char SELECTOR_FIRST_INSTANCE_DIGIT = '1';
        constexpr char SELECTOR_LAST_INSTANCE_DIGIT = '9';

        // Control-table id of the VFD area, whose spec supplies the display
        // panel's geometry. [RQ-GUI-050, RQ-QLT-007]
        constexpr const char* VFD_DISPLAY_CONTROL_ID = "_vfdDisplay";

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

        juce::String preferredSettingsDirectory()
        {
            return juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory)
                .getChildFile(SETTINGS_DIRECTORY_NAME)
                .getChildFile(SETTINGS_DIRECTORY_NAME)
                .getFullPathName();
        }

        // Used when preferredSettingsDirectory() cannot be created — Linux
        // (/opt) and macOS (/Library) are root-owned and this project ships
        // no installer to grant a standard user write access there.
        // [RQ-SET-001, ADR-SET-001 (DEC-SET-001)]
        juce::String fallbackSettingsDirectory()
        {
            return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                .getChildFile(SETTINGS_DIRECTORY_NAME)
                .getChildFile(SETTINGS_DIRECTORY_NAME)
                .getFullPathName();
        }

        // Name and version from the build, not from a literal: the same
        // string the About dialog shows (RQ-BLD-015, RQ-BLD-016), now also
        // the controller's own product-name-and-version (RQ-CTL-061) instead
        // of the constructor's placeholder default — a second call site with
        // its own literal would drift from the About box the moment either
        // changed. [RQ-CTL-061]
        std::string productNameAndVersion()
        {
            return std::string(JUCE_APPLICATION_NAME_STRING) + " " + XPL_VERSION_FULL_STRING;
        }
    }

    MainComponent::MainComponent()
    {
        _dispatcher = std::make_shared<JuceEventDispatcher>();
        _settingsService = std::make_unique<settings::XmlSettingsService>(
            preferredSettingsDirectory().toStdString(), fallbackSettingsDirectory().toStdString());
        _controller = std::make_unique<controller::XpanderController>(
            _backend, *_settingsService, _dispatcher, productNameAndVersion());
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
        _matrixPanel->setMaxSourceReachedHandler(
            [this](int entryNumber)
            {
                // showModulationEntry already carried this exact notice; only
                // the true-passing trigger was missing. [RQ-GUI-020,
                // ADR-JUC-036 (DEC-JUC-123)]
                _vfd->showModulationEntry(_controller->getModulationEntryByNumber(entryNumber), true);
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
        int radioGroup = PAGE_FAMILY_RADIO_GROUP_BASE;
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
                         && id.back() >= SELECTOR_FIRST_INSTANCE_DIGIT
                         && id.back() <= SELECTOR_LAST_INSTANCE_DIGIT)
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
        // Map the synth page to a family + instance and activate the selector,
        // via the same family/page table selectInstance() uses to send. [RQ-GUI-012, RQ-CTL-028]
        const auto familyPage = familyPageFor(static_cast<int>(event.page));
        if (!familyPage)
        {
            return;
        }
        for (auto& block : _familyBlocks)
        {
            if (block->familyPrefix() == familyPage->familyPrefix)
            {
                block->setActiveInstanceFromSynth(familyPage->instance);
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
                    if (std::string(s.id) == VFD_DISPLAY_CONTROL_ID)
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
        _shortcutActions[SHORTCUT_ID_PATCH_MINUS] = [this]
        { _controller->decreaseCurrentProgramNumber(); };
        _shortcutActions[SHORTCUT_ID_PATCH_PLUS] = [this]
        { _controller->increaseCurrentProgramNumber(); };
        // One implementation shared with the Patch menu item, like rename
        // already is. [RQ-QLT-005]
        _shortcutActions[SHORTCUT_ID_PATCH_GOTO] = [this] { showGotoPatchDialog(); };
        _shortcutActions[SHORTCUT_ID_PATCH_RANDOM] = [this]
        { _controller->randomizeTone(midiapp::controller::RandomizeToneArguments{}); };
        _shortcutActions[SHORTCUT_ID_PATCH_LOAD] = [this]
        {
            _fileChooser = std::make_unique<juce::FileChooser>("Load patch", juce::File(), SYSEX_FILE_FILTER);
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
        _shortcutActions[SHORTCUT_ID_PATCH_SAVE] = [this]
        {
            // Default the file name to the tone's own name, sanitized and
            // made unique the same way RQ-CTL-003's bank extraction already
            // does — one sanitizer, not a second implementation of the same
            // rule. [RQ-GUI-077]
            //
            // Trailing spaces are trimmed first: the model pads tone names to
            // a fixed width with spaces (XPANDER's own on-synth storage
            // format), which are meaningless trailing characters in a file
            // name and not caught by the sanitizer (space is a legal file-name
            // character everywhere else in a name).
            const auto trimmedToneName = juce::String(_controller->toneName()).trimEnd().toStdString();
            const auto documentsDirectory =
                juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
            const auto defaultFileName = midiapp::service::makeUniqueFilenameFromString(
                trimmedToneName, midiapp::service::SYSEX_FILE_EXTENSION_WITH_DOT,
                documentsDirectory.getFullPathName().toStdString());
            _fileChooser = std::make_unique<juce::FileChooser>(
                "Save patch", documentsDirectory.getChildFile(defaultFileName), SYSEX_FILE_FILTER);
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
        // One implementation shared with the Patch menu item. [RQ-QLT-005]
        _shortcutActions[SHORTCUT_ID_PATCH_STORE] = [this] { showStorePatchDialog(); };
        _shortcutActions[SHORTCUT_ID_SETTINGS] = [this] { openSettingsDialog(); };

        // Vector keys, replacing the eight .NET ImageButtons and their 24 GIFs.
        // The icon is chosen per control id, the geometry comes from the control
        // table, and the action is the one RQ-GUI-021 already assigned.
        // [RQ-GUI-063, RQ-GUI-065, ADR-GUI-001 (DEC-GUI-001-A)]
        // Size deduced from the initialiser rather than restated. [RQ-QLT-007]
        const std::array shortcutIcons{
            std::pair{SHORTCUT_ID_PATCH_MINUS, ShortcutIcon::PreviousProgram},
            std::pair{SHORTCUT_ID_PATCH_PLUS, ShortcutIcon::NextProgram},
            std::pair{SHORTCUT_ID_PATCH_GOTO, ShortcutIcon::GoToProgram},
            std::pair{SHORTCUT_ID_PATCH_RANDOM, ShortcutIcon::Randomise},
            std::pair{SHORTCUT_ID_PATCH_LOAD, ShortcutIcon::LoadFile},
            std::pair{SHORTCUT_ID_PATCH_SAVE, ShortcutIcon::SaveFile},
            std::pair{SHORTCUT_ID_PATCH_STORE, ShortcutIcon::StoreToSynth},
            std::pair{SHORTCUT_ID_SETTINGS, ShortcutIcon::MidiSettings}};

        for (const auto& [id, icon] : shortcutIcons)
        {
            for (const auto& spec : controlTable())
            {
                if (std::string(spec.id) != id)
                {
                    continue;
                }
                auto button = std::make_unique<ShortcutButton>(id, icon);
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
        // Port of OnMidiDataSendReceive's device -> LED index mapping. The
        // indices are named because paint()'s colour array is ordered to match
        // them: the two orderings must agree, and neither said so.
        // [RQ-QLT-007]
        const std::size_t index =
            device == controller::EnumMidiDevice::AutomationInputDevice ? LED_INDEX_AUTOMATION_IN
            : device == controller::EnumMidiDevice::SynthInputDevice    ? LED_INDEX_SYNTH_IN
                                                                        : LED_INDEX_SYNTH_OUT;
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
        // Indexed by the LED_INDEX_* constants, so this ordering and flash()'s
        // device mapping stay tied together. [RQ-QLT-007]
        static const std::array<juce::Colour, LED_COUNT> onColours = [] {
            std::array<juce::Colour, LED_COUNT> colours{};
            colours[LED_INDEX_AUTOMATION_IN] = tokens::semantic::indicatorAutomation;
            colours[LED_INDEX_SYNTH_IN] = tokens::semantic::indicatorSynthIn;
            colours[LED_INDEX_SYNTH_OUT] = tokens::semantic::indicatorSynthOut;
            return colours;
        }();
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
        // Menu item ids, the top-level order and the View id constants all come
        // from MenuIds.hpp: one declaration for the three sites below (the
        // shortcut table, getMenuForIndex and menuItemSelected) that used to
        // repeat the same raw integers with only a comment relating them.
        // [RQ-QLT-001, RQ-QLT-003, ADR-QLT-001 (DEC-QLT-001)]

        // The three URLs the reference's Help menu opens, verbatim from its
        // XplorerConstants. Opened in the system browser, no networking of our
        // own -- same as the reference's OpenBrowserWithUrl.
        // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-101)]
        constexpr const char* USER_MANUAL_URL =
            "https://github.com/xplorer2716/XplorerEditor/blob/main/xdata/manual/XplorerUserManual.pdf";
        constexpr const char* RELEASES_URL = "https://github.com/xplorer2716/XplorerEditor/releases/latest";
        constexpr const char* WEBSITE_URL = "https://xplorer2716.github.io/XplorerEditor.site/";

        // The default patch File > New loads, embedded in the executable as
        // BinaryData rather than shipped as a sibling file -- immune to being
        // edited or deleted between download and launch. "New" means "load
        // this patch", not "blank the editor", matching the reference.
        // [RQ-GUI-008, ADR-BLD-005 (DEC-BLD-027, DEC-BLD-028) -- supersedes
        // ADR-JUC-032 (DEC-JUC-100)]
        constexpr const char* DEFAULT_TONE_FILENAME = "oberheim.syx";

        // Rewritten from the embedded bytes on every call, never cached: the
        // existing tone-loading API (IToneReader::readTone) takes a filesystem
        // path with no in-memory overload, so the temp file is the bounce that
        // keeps that API untouched -- and always overwriting it, rather than
        // reusing a path written once, means a tampered temp copy cannot
        // survive to the next File > New. [ADR-BLD-005 (DEC-BLD-028)]
        [[nodiscard]] juce::File defaultToneFile()
        {
            auto file = juce::File::getSpecialLocation(juce::File::tempDirectory)
                            .getChildFile(DEFAULT_TONE_FILENAME);
            file.replaceWithData(BinaryData::oberheim_syx,
                                  static_cast<size_t>(BinaryData::oberheim_syxSize));
            return file;
        }

        // The reference's sixteen menu shortcuts (MainForm.resx
        // ToolStripMenuItem.ShortcutKeys -- NOT ShortcutKeyDisplayString, which
        // is blank everywhere: WinForms derives the shown text from ShortcutKeys
        // itself). ONE table feeds two independent JUCE mechanisms -- the item's
        // displayed shortcut text and MainComponent::keyPressed's dispatch -- so
        // pressing a key and clicking its item cannot drift apart.
        //
        // commandModifier, not ctrlModifier: Ctrl on Windows/Linux, Cmd on
        // macOS, matching PageSelectorButton::keyPressed (RQ-GUI-027) so a
        // future macOS build needs no revisit here.
        //
        // Items absent from this table have NO shortcut in the reference (Exit,
        // About, the Single-patches/Backup-Restore submenus) and get none here.
        // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-099)]
        // Keyed by MenuItem rather than by a raw id: the key and modifier
        // fields are juce:: types and cannot cross into the UI-framework-free
        // layer, so the binding stays here while the identity lives in
        // MenuIds.hpp. [RQ-QLT-001, ADR-QLT-001 (DEC-QLT-003)]
        struct MenuShortcut
        {
            MenuItem item;
            int keyCode;
            juce::ModifierKeys::Flags modifiers;
            const char* displayText;
        };

        constexpr auto NO_MODIFIER = juce::ModifierKeys::noModifiers;
        constexpr auto CMD = juce::ModifierKeys::commandModifier;
        constexpr auto CMD_SHIFT =
            static_cast<juce::ModifierKeys::Flags>(juce::ModifierKeys::commandModifier
                                                   | juce::ModifierKeys::shiftModifier);

        // `const`, not `constexpr`: JUCE's KeyPress::F*Key are `static const int`
        // defined in a translation unit, so they are not constant expressions.
        // The size is deduced, not restated, so adding a row cannot desync it.
        // [RQ-QLT-007]
        const std::array MENU_SHORTCUTS = {
            MenuShortcut{MenuItem::FileNew, 'n', CMD, "Ctrl+N"},
            MenuShortcut{MenuItem::FileOpen, 'o', CMD, "Ctrl+O"},
            MenuShortcut{MenuItem::FileSave, 's', CMD, "Ctrl+S"},
            MenuShortcut{MenuItem::PatchPrevious, juce::KeyPress::F5Key, NO_MODIFIER, "F5"},
            MenuShortcut{MenuItem::PatchNext, juce::KeyPress::F6Key, NO_MODIFIER, "F6"},
            MenuShortcut{MenuItem::PatchGoto, juce::KeyPress::F7Key, NO_MODIFIER, "F7"},
            MenuShortcut{MenuItem::PatchRandomize, juce::KeyPress::F8Key, NO_MODIFIER, "F8"},
            MenuShortcut{MenuItem::PatchRename, juce::KeyPress::F9Key, NO_MODIFIER, "F9"},
            MenuShortcut{MenuItem::PatchStore, juce::KeyPress::F10Key, NO_MODIFIER, "F10"},
            MenuShortcut{MenuItem::PatchSynchronize, juce::KeyPress::F12Key, NO_MODIFIER, "F12"},
            MenuShortcut{MenuItem::ToolsSettings, 'g', CMD, "Ctrl+G"},
            MenuShortcut{MenuItem::ToolsTuneRequest, juce::KeyPress::F4Key, NO_MODIFIER, "F4"},
            MenuShortcut{MenuItem::HelpUserManual, juce::KeyPress::F1Key, NO_MODIFIER, "F1"},
        };

        [[nodiscard]] const MenuShortcut* shortcutForItem(MenuItem item)
        {
            for (const auto& entry : MENU_SHORTCUTS)
            {
                if (entry.item == item)
                {
                    return &entry;
                }
            }
            return nullptr;
        }

        /// Adds a menu item carrying the reference's shortcut text and, for the
        /// three File items that have one, its reference icon. The explicit
        /// PopupMenu::Item form is required: addItem(id, text) cannot express
        /// either. [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-098, DEC-JUC-099)]
        void addReferenceItem(juce::PopupMenu& menu, MenuItem itemId, const juce::String& text,
                              const void* iconData = nullptr, int iconSize = 0)
        {
            juce::PopupMenu::Item item(text);
            item.itemID = menuItemId(itemId);
            if (const auto* shortcut = shortcutForItem(itemId))
            {
                item.shortcutKeyDescription = shortcut->displayText;
            }
            if (iconData != nullptr)
            {
                item.image = juce::Drawable::createFromImageData(iconData, static_cast<size_t>(iconSize));
            }
            menu.addItem(std::move(item));
        }

        juce::String scaleItemName(float scale)
        {
            // "1x", "1.25x", ... — trailing zeros trimmed so 1.5 does not read
            // as "1.50x".
            auto text = juce::String(scale, 2).trimCharactersAtEnd("0").trimCharactersAtEnd(".");
            return text + "x";
        }
    }

    namespace
    {
        /// The displayed name of a top-level menu. Built by iterating
        /// TOP_LEVEL_MENUS so the name list and getMenuForIndex's dispatch come
        /// from one declaration of the order, rather than being coupled by
        /// position alone. [RQ-QLT-003, ADR-QLT-001 (DEC-QLT-004)]
        [[nodiscard]] const char* topLevelMenuName(TopLevelMenu menu)
        {
            switch (menu)
            {
                case TopLevelMenu::File:
                    return "File";
                case TopLevelMenu::Patch:
                    return "Patch";
                case TopLevelMenu::View:
                    return "View";
                case TopLevelMenu::Tools:
                    return "Tools";
                case TopLevelMenu::Help:
                    return "Help";
            }
            return "";
        }
    }

    juce::StringArray MainComponent::getMenuBarNames()
    {
        juce::StringArray names;
        for (const auto menu : TOP_LEVEL_MENUS)
        {
            names.add(topLevelMenuName(menu));
        }
        return names;
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
        switch (static_cast<TopLevelMenu>(index))
        {
            // Order, wording and grouping are the reference's own, read from
            // MainForm.Designer.cs's DropDownItems.AddRange calls and the
            // matching .resx strings -- not an approximation.
            // [RQ-GUI-008, ADR-JUC-032]
            case TopLevelMenu::File: // the only three icons in the whole reference
                addReferenceItem(menu, MenuItem::FileNew, "New", BinaryData::menu_new_png,
                                 BinaryData::menu_new_pngSize);
                addReferenceItem(menu, MenuItem::FileOpen, "Open", BinaryData::menu_open_png,
                                 BinaryData::menu_open_pngSize);
                menu.addSeparator();
                // The reference's "Save as" is deliberately absent: our Save
                // already always prompts for a destination (no current-file
                // path is tracked), so it IS the reference's Save as.
                // [RQ-GUI-008, owner-confirmed deviation]
                addReferenceItem(menu, MenuItem::FileSave, "Save", BinaryData::menu_save_png,
                                 BinaryData::menu_save_pngSize);
                menu.addSeparator();
                addReferenceItem(menu, MenuItem::FileExit, "Exit"); // no shortcut in the reference
                break;
            case TopLevelMenu::Patch:
                addReferenceItem(menu, MenuItem::PatchPrevious, "Previous");
                addReferenceItem(menu, MenuItem::PatchNext, "Next");
                addReferenceItem(menu, MenuItem::PatchGoto, "Go to patch...");
                menu.addSeparator();
                addReferenceItem(menu, MenuItem::PatchRandomize, "Randomize");
                addReferenceItem(menu, MenuItem::PatchRename, "Rename");
                addReferenceItem(menu, MenuItem::PatchStore, "Store");
                addReferenceItem(menu, MenuItem::PatchSynchronize, "Synchronize");
                break;
            case TopLevelMenu::View: // [RQ-SCL-002, RQ-SCL-003]
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
            case TopLevelMenu::Tools:
            {
                addReferenceItem(menu, MenuItem::ToolsSettings, "Settings");
                addReferenceItem(menu, MenuItem::ToolsTuneRequest, "Tune Request");
                // No reference counterpart -- the second sanctioned JUCE-only
                // item after the View menu, kept at the owner's decision. No
                // shortcut, since the reference has none to match.
                // [RQ-GUI-028, RQ-GUI-008]
                menu.addItem(menuItemId(MenuItem::ToolsPianoKeyboard), "Piano keyboard");
                juce::PopupMenu singlePatches;
                singlePatches.addItem(menuItemId(MenuItem::ToolsGetAllSinglePatches),
                                      "Get all single patches from synth");
                singlePatches.addItem(menuItemId(MenuItem::ToolsExtractSinglePatches),
                                      "Extract all single patches from file");
                menu.addSubMenu("Single patches...", singlePatches);
                juce::PopupMenu allDataDump;
                allDataDump.addItem(menuItemId(MenuItem::ToolsBackupAllData), "Backup all data");
                allDataDump.addItem(menuItemId(MenuItem::ToolsRestoreAllData), "Restore all data");
                menu.addSubMenu("Backup/Restore...", allDataDump);
                break;
            }
            case TopLevelMenu::Help:
                addReferenceItem(menu, MenuItem::HelpUserManual, "Xplorer help");
                menu.addSeparator();
                addReferenceItem(menu, MenuItem::HelpReleases, "Check for new releases");
                addReferenceItem(menu, MenuItem::HelpWebsite, "Go to website");
                addReferenceItem(menu, MenuItem::HelpAbout, "About...");
                break;
        }
        return menu;
    }

    bool MainComponent::keyPressed(const juce::KeyPress& key)
    {
        // Same dispatcher a menu click uses, so the two cannot diverge. Keys
        // outside the table return false and keep travelling -- that is what
        // leaves PageSelectorButton's Ctrl+C/Ctrl+V (RQ-GUI-027) and any
        // focused editor's own keys working. [RQ-GUI-008, ADR-JUC-032]
        for (const auto& entry : MENU_SHORTCUTS)
        {
            if (key == juce::KeyPress(entry.keyCode, entry.modifiers, 0))
            {
                menuItemSelected(menuItemId(entry.item), 0);
                return true;
            }
        }
        return juce::Component::keyPressed(key);
    }

    void MainComponent::mouseDoubleClick(const juce::MouseEvent& event)
    {
        // DisplayPanel opts out of hit-testing (setInterceptsMouseClicks(false,
        // false)), so a double click over the VFD lands here; _display's
        // bounds cover bezel and glass alike. [RQ-GUI-025]
        if (_display.getBounds().contains(event.getPosition()))
        {
            showRenameDialogForCurrentTone();
        }
    }

    void MainComponent::showRenameDialogForCurrentTone()
    {
        showRenameDialog(_controller->toneName(),
                         [this](const std::string& name)
                         {
                             _controller->setToneName(name);
                             _vfd->showToneInfo();
                         });
    }

    void MainComponent::showGotoPatchDialog()
    {
        showStoreOrGotoDialog(GOTO_PATCH_DIALOG_TITLE, _controller->currentProgramNumber(),
                              [this](int program)
                              { _controller->sendProgramChangeAndGetSinglePatchFromSynth(program); });
    }

    void MainComponent::showStorePatchDialog()
    {
        showStoreOrGotoDialog(STORE_PATCH_DIALOG_TITLE, _controller->currentProgramNumber(),
                              [this](int program) { _controller->storeSinglePatchToSynth(program); });
    }

    // `selectedId`, not `menuItemId`: the latter is the free function that maps
    // a MenuItem to the int JUCE hands back here, and a parameter of that name
    // would shadow it throughout the dispatch below. [RQ-QLT-001]
    void MainComponent::menuItemSelected(int selectedId, int)
    {
        // View menu, handled before the switch because its scale items are a
        // contiguous id range rather than individual cases. [RQ-SCL-002]
        const auto scaleIndex = selectedId - VIEW_SCALE_FIRST_ID;
        if (scaleIndex >= 0 && scaleIndex < static_cast<int>(WINDOW_SCALE_PRESETS.size()))
        {
            if (auto* window = topLevelWindow())
            {
                applyWindowScale(*window, WINDOW_SCALE_PRESETS[static_cast<std::size_t>(scaleIndex)]);
            }
            return;
        }
        if (selectedId == VIEW_FULL_SCREEN_ID) // [RQ-SCL-003, DEC-JUC-067]
        {
            if (auto* window = topLevelWindow())
            {
                // JUCE keeps the pre-fullscreen bounds itself and restores them
                // on the way back, so no geometry is remembered here.
                window->setFullScreen(!window->isFullScreen());
            }
            return;
        }

        switch (static_cast<MenuItem>(selectedId))
        {
            case MenuItem::FileNew:
                // Load the bundled default patch, like the reference's
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
            case MenuItem::FileOpen: // reuse the load action
                _shortcutActions[SHORTCUT_ID_PATCH_LOAD]();
                break;
            case MenuItem::FileSave:
                _shortcutActions[SHORTCUT_ID_PATCH_SAVE]();
                break;
            case MenuItem::FileExit:
                juce::JUCEApplication::getInstance()->systemRequestedQuit();
                break;
            case MenuItem::PatchNext:
                _controller->increaseCurrentProgramNumber();
                break;
            case MenuItem::PatchPrevious:
                _controller->decreaseCurrentProgramNumber();
                break;
            case MenuItem::PatchGoto:
                showGotoPatchDialog();
                break;
            case MenuItem::PatchStore:
                showStorePatchDialog();
                break;
            case MenuItem::PatchRename:
                showRenameDialogForCurrentTone();
                break;
            case MenuItem::PatchRandomize:
                _controller->randomizeTone(midiapp::controller::RandomizeToneArguments{});
                break;
            case MenuItem::PatchSynchronize:
                // Re-fetch the current patch from the synth, the same call
                // Go to / Store already make.
                // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-101)]
                _controller->sendProgramChangeAndGetSinglePatchFromSynth(
                    _controller->currentProgramNumber());
                break;
            case MenuItem::ToolsSettings:
                openSettingsDialog();
                break;
            case MenuItem::ToolsTuneRequest:
                _controller->sendTuneRequestToSynth();
                break;
            case MenuItem::ToolsPianoKeyboard: // [RQ-GUI-028]
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
            case MenuItem::HelpAbout:
                // Name and version from the build, not from a literal: this
                // call site is the RQ-GUI-025 defect RQ-BLD-016 closes at its
                // root. [RQ-BLD-015, RQ-BLD-016]
                showAboutDialog(productNameAndVersion());
                // Reference AboutForm.OnLoad() greets the synth the moment the
                // About window is shown (XpanderController.cs:756, called from
                // AboutForm.cs:90); the JUCE port already carries a faithful
                // sendGreetingsToSynth() but nothing called it here — this was
                // the missing link. [RQ-GUI-076, RQ-CTL-061]
                _controller->sendGreetingsToSynth();
                break;
            // The three Help URLs, opened in the system browser exactly as the
            // reference's OpenBrowserWithUrl does — no update check of our own.
            // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-101)]
            case MenuItem::HelpUserManual:
                juce::URL(USER_MANUAL_URL).launchInDefaultBrowser();
                break;
            case MenuItem::HelpReleases:
                juce::URL(RELEASES_URL).launchInDefaultBrowser();
                break;
            case MenuItem::HelpWebsite:
                juce::URL(WEBSITE_URL).launchInDefaultBrowser();
                break;
            case MenuItem::ToolsGetAllSinglePatches:
                getAllSinglePatchesFromSynth();
                break;
            case MenuItem::ToolsExtractSinglePatches:
                showExtractSingleTonesDialog(*_controller);
                break;
            case MenuItem::ToolsBackupAllData:
                backupAllData();
                break;
            case MenuItem::ToolsRestoreAllData:
                restoreAllData();
                break;
            // Ids outside the enumeration reach here only if JUCE reports an
            // item this component never added; ignored, as before.
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
        // Live preview / accept / cancel-restore all land here, exactly as they
        // do for the block palette above: retune the colour in place and
        // repaint the tree — no LookAndFeel rebuild, so a customised block
        // palette survives with nothing to carry across.
        // [RQ-GUI-031, RQ-GUI-073, RQ-DSN-095, ADR-JUC-011, ADR-JUC-020 (DEC-JUC-113)]
        _lookAndFeel->setLedColour(juce::Colour(static_cast<juce::uint32>(argb)));
        if (auto* top = getTopLevelComponent())
        {
            top->sendLookAndFeelChange();
        }
    }

    void MainComponent::backupAllData()
    {
        _fileChooser = std::make_unique<juce::FileChooser>("Backup all data", juce::File(), SYSEX_FILE_FILTER);
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
        _fileChooser = std::make_unique<juce::FileChooser>("Restore all data", juce::File(), SYSEX_FILE_FILTER);
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
                            if (result == MODAL_RESULT_FIRST_BUTTON)
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
        // So the menu shortcuts still arrive when no control holds focus --
        // this is where an unfocused key event stops. [ADR-JUC-032 (DEC-JUC-099)]
        setWantsKeyboardFocus(true);
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

    bool ScaledCanvasComponent::keyPressed(const juce::KeyPress& key)
    {
        // Forward DOWN to the menu owner: bubbling only goes up, so a key
        // pressed with nothing focused would otherwise die here.
        // [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-099)]
        return _canvas.keyPressed(key);
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
