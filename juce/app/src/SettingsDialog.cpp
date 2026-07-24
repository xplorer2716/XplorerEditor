#include "SettingsDialog.hpp"

#include "DesignTokens.hpp"
#include "Dialogs.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/MidiAutomationTable.hpp"
#include "xplorer/model/XpanderTone.hpp"
#include "xplorer/settings/AllUsersSettings.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace xplorer::app
{
    namespace
    {
        // Layout grid: TASK-JUC-098, RQ-DSN-020 (audited from this file's own
        // pre-existing layout, value-preserving — every gap already divided
        // evenly into a 4px base unit).
        constexpr int LABEL_WIDTH = tokens::semantic::dialogLabelWidth;
        constexpr int ROW_HEIGHT = tokens::semantic::dialogRowHeight;
        constexpr int MARGIN = tokens::semantic::layoutMargin;

        // Editable CC automation table (reference MidiPage LvAutomation): one
        // row per parameter, CC picked from the reference CC-name list.
        // [RQ-GUI-036, ADR-JUC-012]
        class AutomationTableModel final : public juce::TableListBoxModel
        {
        public:
            struct Row
            {
                std::string tag;
                juce::String friendly;
                int cc;
            };
            std::vector<Row> rows;
            int hoveredRow = -1; // updated by TableHoverListener; -1 = none [RQ-GUI-041]

            int getNumRows() override { return static_cast<int>(rows.size()); }

            void paintRowBackground(juce::Graphics& g, int rowNumber, int, int, bool selected) override
            {
                // Selection takes visual precedence over hover (RQ-DSN-062 ordering);
                // an unselected hovered row brightens by the shared factor. [RQ-GUI-041, ADR-JUC-017]
                if (selected)
                {
                    g.fillAll(tokens::semantic::surfaceSelected);
                }
                else if (rowNumber == hoveredRow)
                {
                    g.fillAll(tokens::semantic::surfaceRecessed.brighter(tokens::semantic::hoverBrighten));
                }
                else
                {
                    g.fillAll(tokens::semantic::surfaceRecessed);
                }
            }

            void paintCell(juce::Graphics& g, int row, int column, int width, int height, bool) override
            {
                if (column == 1 && row < static_cast<int>(rows.size()))
                {
                    g.setColour(tokens::semantic::textPrimary);
                    g.setFont(tokens::semantic::textSubtitle);
                    g.drawText(rows[static_cast<std::size_t>(row)].friendly, 4, 0, width - 6, height,
                               juce::Justification::centredLeft, true);
                }
                g.setColour(tokens::semantic::borderDefault);
                g.fillRect(width - 1, 0, 1, height);
            }

            struct CcCombo final : juce::ComboBox
            {
                AutomationTableModel& model;
                int row = -1;
                explicit CcCombo(AutomationTableModel& owner) : model(owner)
                {
                    for (int i = 0; i < controlChangeNameCount(); ++i)
                    {
                        addItem(controlChangeName(i), i + 1);
                    }
                    onChange = [this]
                    {
                        if (row >= 0 && row < static_cast<int>(model.rows.size()))
                        {
                            model.rows[static_cast<std::size_t>(row)].cc = getSelectedId() - 1;
                        }
                    };
                }
                void setRow(int r)
                {
                    row = r;
                    setSelectedId(model.rows[static_cast<std::size_t>(r)].cc + 1, juce::dontSendNotification);
                }
            };

            juce::Component* refreshComponentForCell(int row, int column, bool,
                                                     juce::Component* existing) override
            {
                if (column != 2)
                {
                    delete existing;
                    return nullptr;
                }
                auto* combo = dynamic_cast<CcCombo*>(existing);
                if (combo == nullptr)
                {
                    combo = new CcCombo(*this);
                }
                combo->setRow(row);
                return combo;
            }

            void resetAll()
            {
                for (auto& r : rows)
                {
                    r.cc = unassignedControlChange();
                }
            }
        };

        // TableListBoxModel has no built-in per-row hover callback, so hover is
        // tracked manually: this listener (attached to the TableListBox, nested
        // children included so events over the CC combos still count) resolves
        // the row under the pointer and repaints only the rows whose hover state
        // changed. [RQ-GUI-041, ADR-JUC-017 (DEC-JUC-023)]
        struct TableHoverListener final : juce::MouseListener
        {
            juce::TableListBox& table;
            AutomationTableModel& model;
            TableHoverListener(juce::TableListBox& t, AutomationTableModel& m) : table(t), model(m) {}

            void mouseMove(const juce::MouseEvent& e) override { setRow(rowAt(e)); }
            void mouseEnter(const juce::MouseEvent& e) override { setRow(rowAt(e)); }
            void mouseExit(const juce::MouseEvent&) override { setRow(-1); }

            [[nodiscard]] int rowAt(const juce::MouseEvent& e) const
            {
                const auto p = e.getEventRelativeTo(&table).getPosition();
                return table.getRowContainingPosition(p.x, p.y);
            }
            void setRow(int row)
            {
                if (row == model.hoveredRow)
                {
                    return;
                }
                const int previous = model.hoveredRow;
                model.hoveredRow = row;
                if (previous >= 0)
                {
                    table.repaintRow(previous);
                }
                if (row >= 0)
                {
                    table.repaintRow(row);
                }
            }
        };

        /// Lays a "caption: control" row and returns the control's bounds.
        juce::Rectangle<int> rowBounds(juce::Rectangle<int>& area)
        {
            return area.removeFromTop(ROW_HEIGHT).reduced(0, tokens::semantic::layoutHairline);
        }

        // ---- MIDI page -----------------------------------------------------
        class MidiSettingsPage final : public juce::Component
        {
        public:
            MidiSettingsPage(settings::ISettingsService& settingsService, xpl::midi::MidiBackend& backend)
            {
                const auto& midi = settingsService.allUsersSettings().midiConfig;

                addDeviceCombo(_synthOut, _synthOutLabel, "Synth output", backend.outputDeviceNames(),
                               midi.synthOutputDeviceName);
                addDeviceCombo(_synthIn, _synthInLabel, "Synth input", backend.inputDeviceNames(),
                               midi.synthInputDeviceName);
                addDeviceCombo(_autoIn, _autoInLabel, "Automation input", backend.inputDeviceNames(),
                               midi.automationInputDeviceName);

                addCombo(_channel, _channelLabel, "MIDI channel");
                for (int ch = 1; ch <= 16; ++ch)
                {
                    _channel.addItem(juce::String(ch), ch);
                }
                _channel.setSelectedId(juce::jlimit(1, 16, midi.midiChannel), juce::dontSendNotification);

                addLabelledEditor(_delay, _delayLabel, "SysEx transmit delay (ms)",
                                  juce::String(midi.sysexTransmitDelay));

                addCombo(_synthType, _synthTypeLabel, "Synth type");
                _synthType.addItem("Xpander", 1);
                _synthType.addItem("Matrix-12", 2);
                _synthType.setSelectedId(midi.synthTypeIsMatrix12 ? 2 : 1, juce::dontSendNotification);

                _smartNotesOff.setButtonText("Smart all notes off");
                _smartNotesOff.setToggleState(midi.smartAllNotesOff, juce::dontSendNotification);
                addAndMakeVisible(_smartNotesOff);

                // CC automation table editor. [RQ-GUI-036]
                for (const auto& entry : midi.automationTable)
                {
                    if (const auto parsed = parseAutomationEntry(entry))
                    {
                        _automationModel.rows.push_back(
                            {parsed->first, juce::String(parameterDisplayName(parsed->first)), parsed->second});
                    }
                }
                _automationLabel.setText("MIDI automation table", juce::dontSendNotification);
                _automationLabel.setJustificationType(juce::Justification::centredLeft);
                addAndMakeVisible(_automationLabel);
                _automationTable.setModel(&_automationModel);
                _automationTable.getHeader().addColumn("Parameter", 1, LABEL_WIDTH);
                _automationTable.getHeader().addColumn("MIDI CC", 2, 240);
                _automationTable.setRowHeight(ROW_HEIGHT);
                _automationTable.addMouseListener(&_tableHover, true); // nested = CC combos too [RQ-GUI-041]
                addAndMakeVisible(_automationTable);
                _resetAutomation.setButtonText("Reset all to unassigned");
                _resetAutomation.onClick = [this]
                {
                    _automationModel.resetAll();
                    _automationTable.updateContent();
                    _automationTable.repaint();
                };
                addAndMakeVisible(_resetAutomation);

                _exportHtml.setButtonText("Export as HTML");
                _exportHtml.onClick = [this] { exportMappingAsHtml(); };
                addAndMakeVisible(_exportHtml);
            }

            void applyTo(settings::AllUsersSettings::MidiConfiguration& midi) const
            {
                auto text = [](const juce::ComboBox& c)
                {
                    return c.getText() == "(none)" ? juce::String() : c.getText();
                };
                midi.synthOutputDeviceName = text(_synthOut).toStdString();
                midi.synthInputDeviceName = text(_synthIn).toStdString();
                midi.automationInputDeviceName = text(_autoIn).toStdString();
                midi.midiChannel = juce::jlimit(1, 16, _channel.getSelectedId());
                midi.sysexTransmitDelay = juce::jmax(0, _delay.getText().getIntValue());
                midi.synthTypeIsMatrix12 = _synthType.getSelectedId() == 2;
                midi.smartAllNotesOff = _smartNotesOff.getToggleState();

                // Persist the automation rows as "NAME;CC" (reference format).
                midi.automationTable.clear();
                for (const auto& row : _automationModel.rows)
                {
                    midi.automationTable.push_back(row.tag + ";" + std::to_string(row.cc));
                }
            }

            void resized() override
            {
                auto area = getLocalBounds().reduced(MARGIN);
                for (auto* combo : {&_synthOut, &_synthIn, &_autoIn, &_channel, &_synthType})
                {
                    place(*combo, rowBounds(area));
                }
                place(_delay, rowBounds(area));
                _smartNotesOff.setBounds(area.removeFromTop(ROW_HEIGHT).withTrimmedLeft(LABEL_WIDTH));

                area.removeFromTop(MARGIN);
                _automationLabel.setBounds(area.removeFromTop(ROW_HEIGHT));
                auto buttonRow = area.removeFromBottom(ROW_HEIGHT).reduced(0, tokens::semantic::layoutHairline);
                _resetAutomation.setBounds(buttonRow.removeFromLeft(200));
                buttonRow.removeFromLeft(tokens::semantic::layoutButtonGap);
                _exportHtml.setBounds(buttonRow.removeFromLeft(140));
                area.removeFromBottom(tokens::semantic::layoutFieldGap);
                _automationTable.setBounds(area);
            }

        private:
            void exportMappingAsHtml()
            {
                std::vector<std::pair<std::string, std::string>> rows;
                rows.reserve(_automationModel.rows.size());
                for (const auto& row : _automationModel.rows)
                {
                    rows.emplace_back(row.friendly.toStdString(), controlChangeName(row.cc));
                }
                const auto generatedOn =
                    juce::Time::getCurrentTime().toString(true, true).toStdString();
                const auto html = buildMidiMappingHtml(rows, generatedOn);

                _htmlChooser = std::make_unique<juce::FileChooser>(
                    "Export MIDI mapping", juce::File(), "*.html");
                _htmlChooser->launchAsync(
                    juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                    [html](const juce::FileChooser& fc)
                    {
                        auto file = fc.getResult();
                        if (file == juce::File())
                        {
                            return;
                        }
                        if (file.getFileExtension().isEmpty())
                        {
                            file = file.withFileExtension("html");
                        }
                        if (file.replaceWithText(html))
                        {
                            file.startAsProcess(); // open in the default browser
                        }
                        else
                        {
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::MessageBoxIconType::WarningIcon, "Export MIDI mapping",
                                "Unable to write " + file.getFullPathName());
                        }
                    });
            }

            void place(juce::Component& control, juce::Rectangle<int> row)
            {
                control.setBounds(row.withTrimmedLeft(LABEL_WIDTH));
            }

            void addCombo(juce::ComboBox& combo, juce::Label& label, const juce::String& caption)
            {
                label.setText(caption, juce::dontSendNotification);
                label.attachToComponent(&combo, true);
                label.setJustificationType(juce::Justification::centredRight);
                addAndMakeVisible(label);
                addAndMakeVisible(combo);
            }

            void addDeviceCombo(juce::ComboBox& combo, juce::Label& label, const juce::String& caption,
                                const std::vector<std::string>& names, const std::string& current)
            {
                addCombo(combo, label, caption);
                combo.addItem("(none)", 1);
                for (int i = 0; i < static_cast<int>(names.size()); ++i)
                {
                    combo.addItem(names[static_cast<std::size_t>(i)], i + 2);
                }
                combo.setSelectedId(1, juce::dontSendNotification);
                for (int i = 0; i < static_cast<int>(names.size()); ++i)
                {
                    if (names[static_cast<std::size_t>(i)] == current)
                    {
                        combo.setSelectedId(i + 2, juce::dontSendNotification);
                    }
                }
            }

            void addLabelledEditor(juce::TextEditor& editor, juce::Label& label, const juce::String& caption,
                                   const juce::String& value)
            {
                label.setText(caption, juce::dontSendNotification);
                label.attachToComponent(&editor, true);
                label.setJustificationType(juce::Justification::centredRight);
                editor.setText(value);
                editor.setInputRestrictions(6, "0123456789");
                addAndMakeVisible(label);
                addAndMakeVisible(editor);
            }

            juce::ComboBox _synthOut, _synthIn, _autoIn, _channel, _synthType;
            juce::Label _synthOutLabel, _synthInLabel, _autoInLabel, _channelLabel, _synthTypeLabel, _delayLabel;
            juce::TextEditor _delay;
            juce::ToggleButton _smartNotesOff;
            juce::Label _automationLabel;
            AutomationTableModel _automationModel; // declared before the table it backs
            juce::TableListBox _automationTable;
            TableHoverListener _tableHover{_automationTable, _automationModel};
            juce::TextButton _resetAutomation;
            juce::TextButton _exportHtml;
            std::unique_ptr<juce::FileChooser> _htmlChooser;
        };

        // ---- User interface page ------------------------------------------
        class UiSettingsPage final : public juce::Component, private juce::ChangeListener
        {
        public:
            explicit UiSettingsPage(settings::ISettingsService& settingsService)
            {
                const auto& ui = settingsService.allUsersSettings().uiConfig;
                _ledColour = juce::Colour(static_cast<juce::uint32>(ui.knobLedBorderColor));

                _ledLabel.setText("Knob LED colour", juce::dontSendNotification);
                addAndMakeVisible(_ledLabel);
                _chooseColour.setButtonText("Choose colour...");
                _chooseColour.onClick = [this] { openColourSelector(); };
                addAndMakeVisible(_chooseColour);
                addAndMakeVisible(_swatch);

                setupRadioPair(_movementLabel, "Knob movement", _linear, "Linear", _circular, "Circular",
                               MOVEMENT_GROUP, ui.knobMovementIsLinear);
                setupRadioPair(_styleLabel, "Knob style", _standard, "Standard", _flat, "Flat", STYLE_GROUP,
                               ui.knobStyleIsStandard);
            }

            void applyTo(settings::AllUsersSettings::UiConfiguration& ui) const
            {
                ui.knobLedBorderColor = static_cast<int>(_ledColour.getARGB());
                ui.knobMovementIsLinear = _linear.getToggleState();
                ui.knobStyleIsStandard = _standard.getToggleState();
            }

            [[nodiscard]] int ledColourArgb() const { return static_cast<int>(_ledColour.getARGB()); }

            void paint(juce::Graphics& g) override
            {
                g.setColour(_ledColour);
                g.fillRect(_swatch.getBounds());
                g.setColour(juce::Colours::grey);
                g.drawRect(_swatch.getBounds());
            }

            void resized() override
            {
                auto area = getLocalBounds().reduced(MARGIN);
                auto ledRow = rowBounds(area);
                _ledLabel.setBounds(ledRow.removeFromLeft(LABEL_WIDTH));
                _swatch.setBounds(ledRow.removeFromLeft(40).reduced(tokens::semantic::layoutHairline));
                _chooseColour.setBounds(ledRow.removeFromLeft(140));

                area.removeFromTop(tokens::semantic::layoutSectionGap);
                layoutRadioRow(area, _movementLabel, _linear, _circular);
                layoutRadioRow(area, _styleLabel, _standard, _flat);
            }

        private:
            static constexpr int MOVEMENT_GROUP = 4001;
            static constexpr int STYLE_GROUP = 4002;

            void openColourSelector()
            {
                auto selector = std::make_unique<juce::ColourSelector>(
                    juce::ColourSelector::showColourAtTop | juce::ColourSelector::showSliders
                    | juce::ColourSelector::showColourspace);
                selector->setCurrentColour(_ledColour);
                selector->setSize(300, 300);
                selector->addChangeListener(this);
                juce::CallOutBox::launchAsynchronously(std::move(selector), _chooseColour.getScreenBounds(),
                                                       nullptr);
            }

            void changeListenerCallback(juce::ChangeBroadcaster* source) override
            {
                if (auto* selector = dynamic_cast<juce::ColourSelector*>(source))
                {
                    _ledColour = selector->getCurrentColour();
                    repaint();
                }
            }

            void setupRadioPair(juce::Label& label, const juce::String& caption, juce::ToggleButton& first,
                                const juce::String& firstText, juce::ToggleButton& second,
                                const juce::String& secondText, int group, bool firstSelected)
            {
                label.setText(caption, juce::dontSendNotification);
                addAndMakeVisible(label);
                first.setButtonText(firstText);
                second.setButtonText(secondText);
                first.setRadioGroupId(group);
                second.setRadioGroupId(group);
                first.setToggleState(firstSelected, juce::dontSendNotification);
                second.setToggleState(!firstSelected, juce::dontSendNotification);
                addAndMakeVisible(first);
                addAndMakeVisible(second);
            }

            void layoutRadioRow(juce::Rectangle<int>& area, juce::Label& label, juce::ToggleButton& first,
                                juce::ToggleButton& second)
            {
                auto row = rowBounds(area);
                label.setBounds(row.removeFromLeft(LABEL_WIDTH));
                first.setBounds(row.removeFromLeft(120));
                second.setBounds(row.removeFromLeft(120));
            }

            juce::Colour _ledColour;
            juce::Label _ledLabel, _movementLabel, _styleLabel;
            juce::TextButton _chooseColour;
            juce::Component _swatch;
            juce::ToggleButton _linear, _circular, _standard, _flat;
        };

        // ---- Randomizer page ----------------------------------------------
        class RandomizerSettingsPage final : public juce::Component
        {
        public:
            explicit RandomizerSettingsPage(settings::ISettingsService& settingsService)
            {
                fillCombos();
                setFromConfig(settingsService.allUsersSettings().randomizerConfig);

                for (auto* toggle : {&_fm, &_noise, &_sync, &_amount, &_quantize, &_srcDest})
                {
                    addAndMakeVisible(*toggle);
                }
                _fm.setButtonText("FM");
                _noise.setButtonText("Noise");
                _sync.setButtonText("Sync");
                _amount.setButtonText("Amount");
                _quantize.setButtonText("Quantize");
                _srcDest.setButtonText("Sources & destinations");

                _vco2Label.setText("VCO2 random", juce::dontSendNotification);
                _matrixLabel.setText("Matrix random", juce::dontSendNotification);
                for (auto* label : {&_vco2Label, &_matrixLabel})
                {
                    addAndMakeVisible(*label);
                }

                _randomizeAll.setButtonText("Randomize all");
                _randomizeAll.onClick = [this]
                { setFromConfig(settings::defaultRandomizerConfiguration()); };
                addAndMakeVisible(_randomizeAll);
            }

            void applyTo(settings::AllUsersSettings::RandomizerConfiguration& cfg) const
            {
                using model::EnumRandomModMatrix;
                using model::EnumRandomVCO2;
                cfg.vcoFreq = static_cast<model::EnumRandomVCOFreq>(_freq.getSelectedId() - 1);
                cfg.vcoDetune = static_cast<model::EnumRandomVCODetune>(_detune.getSelectedId() - 1);
                cfg.vca2Env = static_cast<model::EnumRandomVCAEnv>(_env.getSelectedId() - 1);

                unsigned vco2 = 0;
                if (_fm.getToggleState()) vco2 |= static_cast<unsigned>(EnumRandomVCO2::EnableFM);
                if (_noise.getToggleState()) vco2 |= static_cast<unsigned>(EnumRandomVCO2::EnableNoise);
                if (_sync.getToggleState()) vco2 |= static_cast<unsigned>(EnumRandomVCO2::EnableSync);
                cfg.vco2FmNoiseSync = static_cast<EnumRandomVCO2>(vco2);

                unsigned matrix = 0;
                if (_amount.getToggleState()) matrix |= static_cast<unsigned>(EnumRandomModMatrix::EnableAmount);
                if (_quantize.getToggleState())
                    matrix |= static_cast<unsigned>(EnumRandomModMatrix::EnableQuantize);
                if (_srcDest.getToggleState())
                    matrix |= static_cast<unsigned>(EnumRandomModMatrix::EnableSourcesAndDestinations);
                cfg.modulationMatrix = static_cast<EnumRandomModMatrix>(matrix);
            }

            void resized() override
            {
                auto area = getLocalBounds().reduced(MARGIN);
                for (auto* combo : {&_freqPair(), &_detunePair(), &_envPair()})
                {
                    auto row = rowBounds(area);
                    _labelFor(*combo).setBounds(row.removeFromLeft(LABEL_WIDTH));
                    combo->setBounds(row.removeFromLeft(200));
                }
                area.removeFromTop(tokens::semantic::layoutSectionGap);
                auto vco2Row = rowBounds(area);
                _vco2Label.setBounds(vco2Row.removeFromLeft(LABEL_WIDTH));
                _fm.setBounds(vco2Row.removeFromLeft(70));
                _noise.setBounds(vco2Row.removeFromLeft(80));
                _sync.setBounds(vco2Row.removeFromLeft(80));

                auto matrixRow = rowBounds(area);
                _matrixLabel.setBounds(matrixRow.removeFromLeft(LABEL_WIDTH));
                _amount.setBounds(matrixRow.removeFromLeft(90));
                _quantize.setBounds(matrixRow.removeFromLeft(100));
                _srcDest.setBounds(matrixRow.removeFromLeft(180));

                area.removeFromTop(tokens::semantic::layoutSectionGap);
                _randomizeAll.setBounds(rowBounds(area).removeFromLeft(140).withTrimmedLeft(0));
            }

        private:
            juce::ComboBox& _freqPair() { return _freq; }
            juce::ComboBox& _detunePair() { return _detune; }
            juce::ComboBox& _envPair() { return _env; }
            juce::Label& _labelFor(juce::ComboBox& combo)
            {
                if (&combo == &_freq) return _freqLabel;
                if (&combo == &_detune) return _detuneLabel;
                return _envLabel;
            }

            void fillCombos()
            {
                addCombo(_freq, _freqLabel, "VCO frequency",
                         {"Free", "SameNote", "Third", "Fifth", "Seventh", "Octave", "Ninth", "Eleventh",
                          "Thirteenth"});
                addCombo(_detune, _detuneLabel, "VCO detune", {"Free", "Digital", "Analog"});
                addCombo(_env, _envLabel, "VCA2 envelope",
                         {"Free", "Organ", "Strings", "Percusive", "Percusive with release"});
            }

            void addCombo(juce::ComboBox& combo, juce::Label& label, const juce::String& caption,
                          const juce::StringArray& items)
            {
                label.setText(caption, juce::dontSendNotification);
                addAndMakeVisible(label);
                for (int i = 0; i < items.size(); ++i)
                {
                    combo.addItem(items[i], i + 1);
                }
                addAndMakeVisible(combo);
            }

            void setFromConfig(const settings::AllUsersSettings::RandomizerConfiguration& cfg)
            {
                using model::EnumRandomModMatrix;
                using model::EnumRandomVCO2;
                _freq.setSelectedId(static_cast<int>(cfg.vcoFreq) + 1, juce::dontSendNotification);
                _detune.setSelectedId(static_cast<int>(cfg.vcoDetune) + 1, juce::dontSendNotification);
                _env.setSelectedId(static_cast<int>(cfg.vca2Env) + 1, juce::dontSendNotification);

                const auto vco2 = static_cast<unsigned>(cfg.vco2FmNoiseSync);
                _fm.setToggleState((vco2 & static_cast<unsigned>(EnumRandomVCO2::EnableFM)) != 0,
                                   juce::dontSendNotification);
                _noise.setToggleState((vco2 & static_cast<unsigned>(EnumRandomVCO2::EnableNoise)) != 0,
                                      juce::dontSendNotification);
                _sync.setToggleState((vco2 & static_cast<unsigned>(EnumRandomVCO2::EnableSync)) != 0,
                                     juce::dontSendNotification);

                const auto matrix = static_cast<unsigned>(cfg.modulationMatrix);
                _amount.setToggleState((matrix & static_cast<unsigned>(EnumRandomModMatrix::EnableAmount)) != 0,
                                       juce::dontSendNotification);
                _quantize.setToggleState(
                    (matrix & static_cast<unsigned>(EnumRandomModMatrix::EnableQuantize)) != 0,
                    juce::dontSendNotification);
                _srcDest.setToggleState(
                    (matrix & static_cast<unsigned>(EnumRandomModMatrix::EnableSourcesAndDestinations)) != 0,
                    juce::dontSendNotification);
            }

            juce::ComboBox _freq, _detune, _env;
            juce::Label _freqLabel, _detuneLabel, _envLabel, _vco2Label, _matrixLabel;
            juce::ToggleButton _fm, _noise, _sync, _amount, _quantize, _srcDest;
            juce::TextButton _randomizeAll;
        };

        // ---- Dialog content ------------------------------------------------
        class SettingsContent final : public juce::Component
        {
        public:
            SettingsContent(controller::XpanderController& controller,
                            settings::ISettingsService& settingsService, xpl::midi::MidiBackend& backend,
                            std::function<void(int)> onLedColourChanged)
                : _controller(controller), _settingsService(settingsService), _backend(backend),
                  _onLedColourChanged(std::move(onLedColourChanged)),
                  _tabs(juce::TabbedButtonBar::TabsAtTop)
            {
                _originalLedColour = settingsService.allUsersSettings().uiConfig.knobLedBorderColor;

                auto* midiPage = new MidiSettingsPage(settingsService, backend);
                auto* uiPage = new UiSettingsPage(settingsService);
                auto* randomPage = new RandomizerSettingsPage(settingsService);
                _midiPage = midiPage;
                _uiPage = uiPage;
                _randomPage = randomPage;

                const auto bg = tokens::semantic::surfaceRecessed;
                _tabs.addTab("MIDI", bg, midiPage, true);
                _tabs.addTab("User interface", bg, uiPage, true);
                _tabs.addTab("Randomizer", bg, randomPage, true);
                addAndMakeVisible(_tabs);

                _ok.setButtonText("OK");
                _cancel.setButtonText("Cancel");
                _ok.onClick = [this] { accept(); };
                _cancel.onClick = [this] { close(); };
                addAndMakeVisible(_ok);
                addAndMakeVisible(_cancel);

                // Taller: the MIDI page now hosts the scrollable automation table.
                setSize(520, 600);
            }

            void resized() override
            {
                auto area = getLocalBounds();
                auto buttons = area.removeFromBottom(40).reduced(MARGIN, tokens::semantic::layoutSectionGap);
                _ok.setBounds(buttons.removeFromRight(90));
                buttons.removeFromRight(tokens::semantic::layoutButtonGap);
                _cancel.setBounds(buttons.removeFromRight(90));
                _tabs.setBounds(area);
            }

        private:
            void accept()
            {
                auto settings = _settingsService.allUsersSettings();
                _midiPage->applyTo(settings.midiConfig);
                _uiPage->applyTo(settings.uiConfig);
                _randomPage->applyTo(settings.randomizerConfig);
                _settingsService.saveSettings(settings);
                applyMidiSettings(_controller, _settingsService, _backend);

                const int newLed = _uiPage->ledColourArgb();
                if (newLed != _originalLedColour && _onLedColourChanged)
                {
                    _onLedColourChanged(newLed);
                }
                close();
            }

            void close()
            {
                if (auto* dw = findParentComponentOfClass<juce::DialogWindow>())
                {
                    dw->exitModalState(0);
                }
            }

            controller::XpanderController& _controller;
            settings::ISettingsService& _settingsService;
            xpl::midi::MidiBackend& _backend;
            std::function<void(int)> _onLedColourChanged;
            int _originalLedColour = 0;

            juce::TabbedComponent _tabs;
            MidiSettingsPage* _midiPage = nullptr;
            UiSettingsPage* _uiPage = nullptr;
            RandomizerSettingsPage* _randomPage = nullptr;
            juce::TextButton _ok, _cancel;
        };
    }

    void showSettingsDialog(controller::XpanderController& controller,
                            settings::ISettingsService& settingsService, xpl::midi::MidiBackend& backend,
                            std::function<void(int)> onLedColourChanged)
    {
        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(
            new SettingsContent(controller, settingsService, backend, std::move(onLedColourChanged)));
        options.dialogTitle = "Settings";
        options.dialogBackgroundColour = tokens::semantic::surfaceBase;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = true; // the automation table benefits from more height
        options.launchAsync();
    }
}
