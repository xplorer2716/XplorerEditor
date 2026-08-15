#include "SettingsDialog.hpp"

#include "BlockPalette.hpp"
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
        constexpr int MIDI_CC_COLUMN_MIN_WIDTH = tokens::semantic::midiCcColumnMinWidth;

        // Shared by every row-caption Label across the MIDI/UI/Randomizer
        // pages; the matching combo-box and checkbox/radio-caption size is
        // resolved by XplorerLookAndFeel's dialog-context branch instead, so
        // a row's label and its control read as one family. Muted hint text
        // (setupHint) deliberately keeps its own smaller size.
        // [RQ-GUI-061, ADR-JUC-033 (DEC-JUC-105)]
        juce::Font dialogControlFont()
        {
            return juce::Font{juce::FontOptions{tokens::semantic::textTitle}};
        }

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
                    // Number-first, owner-curated short label — RQ-GUI-059,
                    // ADR-JUC-012 DEC-JUC-102/103.
                    for (int i = 0; i < controlChangeNameCount(); ++i)
                    {
                        addItem(controlChangeDisplayLabel(i), i + 1);
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
                _automationLabel.setFont(dialogControlFont());
                addAndMakeVisible(_automationLabel);
                _automationTable.setModel(&_automationModel);
                // "Parameter" is pinned at LABEL_WIDTH (min==max); "MIDI CC" is
                // the only flexible column, so stretch-to-fit hands it 100% of
                // any width the dialog gains or loses on resize — leaving no
                // unclaimed header area. TableListBox already fits columns to
                // the viewport width (excluding the vertical scrollbar) when
                // stretch-to-fit is active. [RQ-GUI-060, ADR-JUC-012 DEC-JUC-104]
                _automationTable.getHeader().addColumn("Parameter", 1, LABEL_WIDTH, LABEL_WIDTH, LABEL_WIDTH);
                _automationTable.getHeader().addColumn("MIDI CC", 2, MIDI_CC_COLUMN_MIN_WIDTH,
                                                       MIDI_CC_COLUMN_MIN_WIDTH, -1);
                _automationTable.getHeader().setStretchToFitActive(true);
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
                    rows.emplace_back(row.friendly.toStdString(), controlChangeDisplayLabel(row.cc));
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
                label.setFont(dialogControlFont());
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
                label.setFont(dialogControlFont());
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
        // COLOURS group (knob LED + the eight block colours + Reset to
        // defaults, presented as one coherent set) and KNOB BEHAVIOUR group
        // (movement/style radios) — owner-validated mockup layout. Block
        // colour edits preview live through the LookAndFeel palette; the
        // dialog owns the cancel-restore snapshot. [RQ-GUI-046, RQ-DSN-095,
        // ADR-JUC-020 (DEC-JUC-038/039)]
        class UiSettingsPage final : public juce::Component, private juce::ChangeListener
        {
        public:
            UiSettingsPage(settings::ISettingsService& settingsService,
                           std::function<void(const BlockPalette&)> onPalettePreview)
                : _onPalettePreview(std::move(onPalettePreview))
            {
                const auto& ui = settingsService.allUsersSettings().uiConfig;
                _ledColour = juce::Colour(static_cast<juce::uint32>(ui.knobLedBorderColor));
                _palette = resolveBlockPalette(ui);

                _coloursGroup.setText("Colours");
                addAndMakeVisible(_coloursGroup);
                _knobGroup.setText("Knob behaviour");
                addAndMakeVisible(_knobGroup);

                // The unity message the owner asked for: one set, one reset.
                setupHint(_unityHint,
                          "Knob and functional-block colours form one set - "
                          "Reset to defaults restores every colour below.");
                setupHint(_blockHint,
                          "Functional blocks - each colours its frame, fill, section "
                          "header and instance-selector buttons.");

                _ledLabel.setText("Knob LED colour", juce::dontSendNotification);
                _ledLabel.setFont(dialogControlFont());
                addAndMakeVisible(_ledLabel);
                _ledSwatch.colour = _ledColour;
                addAndMakeVisible(_ledSwatch);
                _ledChoose.setButtonText(CHOOSE_BUTTON_TEXT);
                _ledChoose.onClick = [this] { openColourSelector(LED_TARGET, _ledColour, _ledChoose); };
                addAndMakeVisible(_ledChoose);

                for (std::size_t i = 0; i < BLOCK_COLOUR_COUNT; ++i)
                {
                    const auto& descriptor = blockColourDescriptors()[i];
                    auto& row = _blockRows[i];
                    row.label.setText(descriptor.displayName, juce::dontSendNotification);
                    row.label.setFont(dialogControlFont());
                    addAndMakeVisible(row.label);
                    row.swatch.colour = _palette.*(descriptor.member);
                    addAndMakeVisible(row.swatch);
                    row.choose.setButtonText(CHOOSE_BUTTON_TEXT);
                    const auto target = static_cast<int>(i);
                    row.choose.onClick = [this, target]
                    {
                        const auto index = static_cast<std::size_t>(target);
                        openColourSelector(target,
                                           _palette.*(blockColourDescriptors()[index].member),
                                           _blockRows[index].choose);
                    };
                    addAndMakeVisible(row.choose);
                }

                _resetDefaults.setButtonText("Reset to defaults");
                _resetDefaults.onClick = [this] { resetToDefaults(); };
                addAndMakeVisible(_resetDefaults);

                setupRadioPair(_movementLabel, "Knob movement", _linear, "Linear", _circular, "Circular",
                               MOVEMENT_GROUP, ui.knobMovementIsLinear);
            }

            void applyTo(settings::AllUsersSettings::UiConfiguration& ui) const
            {
                ui.knobLedBorderColor = static_cast<int>(_ledColour.getARGB());
                ui.knobMovementIsLinear = _linear.getToggleState();

                // A block equal to its default stores NO entry, so users who
                // never customised (or who reset) keep following future palette
                // revisions. [RQ-SET-007, DEC-JUC-039]
                const auto defaults = defaultBlockPalette();
                for (std::size_t i = 0; i < BLOCK_COLOUR_COUNT; ++i)
                {
                    const auto member = blockColourDescriptors()[i].member;
                    const auto colour = _palette.*member;
                    ui.blockColours[i] = colour == defaults.*member
                                             ? std::nullopt
                                             : std::make_optional(static_cast<int>(colour.getARGB()));
                }
            }

            [[nodiscard]] int ledColourArgb() const { return static_cast<int>(_ledColour.getARGB()); }

            void paint(juce::Graphics& g) override
            {
                // Separator between the knob-LED row and the block grid.
                g.setColour(tokens::semantic::borderDefault);
                g.fillRect(_separator);
            }

            void resized() override
            {
                auto area = getLocalBounds().reduced(MARGIN);
                const int gap = tokens::semantic::layoutSectionGap;
                const int header = tokens::semantic::dialogGroupHeaderHeight;

                // ---- COLOURS group: hint, LED row, separator, block hint,
                //      2x4 grid, reset row.
                const int gridRows = static_cast<int>(BLOCK_COLOUR_COUNT) / GRID_COLUMNS;
                const int coloursHeight = header + (3 + gridRows + 1) * ROW_HEIGHT + gap + 2 * MARGIN;
                auto coloursArea = area.removeFromTop(coloursHeight);
                _coloursGroup.setBounds(coloursArea);
                auto inner = coloursArea.reduced(MARGIN).withTrimmedTop(header);
                _unityHint.setBounds(inner.removeFromTop(ROW_HEIGHT));

                auto ledRow = rowBounds(inner);
                _ledLabel.setBounds(ledRow.removeFromLeft(LABEL_WIDTH));
                _ledSwatch.setBounds(ledRow.removeFromLeft(tokens::semantic::dialogSwatchWidth)
                                         .reduced(tokens::semantic::layoutHairline));
                _ledChoose.setBounds(ledRow.removeFromLeft(tokens::semantic::dialogChooseWidth));

                auto sepArea = inner.removeFromTop(gap);
                _separator = sepArea.withSizeKeepingCentre(sepArea.getWidth(), 1);

                _blockHint.setBounds(inner.removeFromTop(ROW_HEIGHT));
                for (int r = 0; r < gridRows; ++r)
                {
                    auto row = rowBounds(inner);
                    auto left = row.removeFromLeft(row.getWidth() / GRID_COLUMNS);
                    layoutBlockCell(_blockRows[static_cast<std::size_t>(r)], left);
                    layoutBlockCell(_blockRows[static_cast<std::size_t>(r + gridRows)], row);
                }
                _resetDefaults.setBounds(
                    rowBounds(inner).removeFromRight(tokens::semantic::dialogResetWidth));

                // ---- KNOB BEHAVIOUR group.
                area.removeFromTop(gap);
                auto knobArea = area.removeFromTop(header + 1 * ROW_HEIGHT + 2 * MARGIN);
                _knobGroup.setBounds(knobArea);
                auto knobInner = knobArea.reduced(MARGIN).withTrimmedTop(header);
                layoutRadioRow(knobInner, _movementLabel, _linear, _circular);
            }

        private:
            static constexpr int MOVEMENT_GROUP = 4001;
            static constexpr int GRID_COLUMNS = 2;    // block-colour grid, mockup 2x4
            static constexpr int LED_TARGET = -1;     // openColourSelector target: knob LED
            static constexpr int SELECTOR_SIZE = 300; // ColourSelector call-out edge
            static constexpr const char* CHOOSE_BUTTON_TEXT = "Choose..."; // uniform per mockup

            /// A colour cell: repaints itself, so previews stay cheap.
            struct Swatch final : juce::Component
            {
                juce::Colour colour;
                void paint(juce::Graphics& g) override
                {
                    g.setColour(colour);
                    g.fillRect(getLocalBounds());
                    g.setColour(tokens::semantic::borderDefault);
                    g.drawRect(getLocalBounds());
                }
            };

            struct BlockRow
            {
                juce::Label label;
                Swatch swatch;
                juce::TextButton choose;
            };

            void setupHint(juce::Label& hint, const juce::String& text)
            {
                hint.setText(text, juce::dontSendNotification);
                hint.setColour(juce::Label::textColourId, tokens::semantic::textHint);
                hint.setFont(juce::Font{juce::FontOptions{tokens::semantic::textCaption}});
                addAndMakeVisible(hint);
            }

            void layoutBlockCell(BlockRow& row, juce::Rectangle<int> cell)
            {
                row.label.setBounds(cell.removeFromLeft(tokens::semantic::dialogBlockLabelWidth));
                row.swatch.setBounds(cell.removeFromLeft(tokens::semantic::dialogSwatchWidth)
                                         .reduced(tokens::semantic::layoutHairline));
                row.choose.setBounds(cell.removeFromLeft(tokens::semantic::dialogChooseWidth));
            }

            void openColourSelector(int target, juce::Colour current, juce::Component& anchor)
            {
                _editTarget = target;
                auto selector = std::make_unique<juce::ColourSelector>(
                    juce::ColourSelector::showColourAtTop | juce::ColourSelector::showSliders
                    | juce::ColourSelector::showColourspace);
                selector->setCurrentColour(current);
                selector->setSize(SELECTOR_SIZE, SELECTOR_SIZE);
                selector->addChangeListener(this);
                juce::CallOutBox::launchAsynchronously(std::move(selector), anchor.getScreenBounds(),
                                                       nullptr);
            }

            void changeListenerCallback(juce::ChangeBroadcaster* source) override
            {
                auto* selector = dynamic_cast<juce::ColourSelector*>(source);
                if (selector == nullptr)
                {
                    return;
                }
                const auto colour = selector->getCurrentColour();
                if (_editTarget == LED_TARGET)
                {
                    // LED colour keeps its apply-on-accept path (no live
                    // preview) — unchanged behaviour. [ADR-JUC-020]
                    _ledColour = colour;
                    _ledSwatch.colour = colour;
                    _ledSwatch.repaint();
                    return;
                }
                const auto index = static_cast<std::size_t>(_editTarget);
                _palette.*(blockColourDescriptors()[index].member) = colour;
                _blockRows[index].swatch.colour = colour;
                _blockRows[index].swatch.repaint();
                if (_onPalettePreview) // live preview [DEC-JUC-038]
                {
                    _onPalettePreview(_palette);
                }
            }

            void resetToDefaults()
            {
                // One coherent set: knob LED + all eight blocks. [RQ-GUI-046]
                _ledColour = juce::Colour(static_cast<juce::uint32>(
                    settings::defaultAllUsersSettings().uiConfig.knobLedBorderColor));
                _ledSwatch.colour = _ledColour;
                _palette = defaultBlockPalette();
                for (std::size_t i = 0; i < BLOCK_COLOUR_COUNT; ++i)
                {
                    _blockRows[i].swatch.colour = _palette.*(blockColourDescriptors()[i].member);
                }
                repaint();
                if (_onPalettePreview)
                {
                    _onPalettePreview(_palette);
                }
            }

            void setupRadioPair(juce::Label& label, const juce::String& caption, juce::ToggleButton& first,
                                const juce::String& firstText, juce::ToggleButton& second,
                                const juce::String& secondText, int group, bool firstSelected)
            {
                label.setText(caption, juce::dontSendNotification);
                label.setFont(dialogControlFont());
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

            std::function<void(const BlockPalette&)> _onPalettePreview;
            juce::Colour _ledColour;
            BlockPalette _palette;
            int _editTarget = LED_TARGET;

            juce::GroupComponent _coloursGroup, _knobGroup;
            juce::Label _unityHint, _blockHint;
            juce::Label _ledLabel, _movementLabel;
            Swatch _ledSwatch;
            juce::TextButton _ledChoose, _resetDefaults;
            std::array<BlockRow, BLOCK_COLOUR_COUNT> _blockRows;
            juce::Rectangle<int> _separator;
            juce::ToggleButton _linear, _circular;
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
                _srcDest.setButtonText("source & dest.");

                _vco2Label.setText("VCO2 random", juce::dontSendNotification);
                _matrixLabel.setText("Matrix random", juce::dontSendNotification);
                for (auto* label : {&_vco2Label, &_matrixLabel})
                {
                    label->setFont(dialogControlFont());
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
                // Shared column widths across the two rows -- not each row's own
                // widest label -- so a checkbox aligns with its counterpart below
                // it (FM/Amount, Noise/Quantize, Sync/source & dest.). Owner
                // report 2026-08-15: only column 1 aligned before this, by
                // accident of both rows starting at the same x.
                // [RQ-GUI-069, ADR-JUC-033]
                area.removeFromTop(tokens::semantic::layoutSectionGap);
                auto vco2Row = rowBounds(area);
                _vco2Label.setBounds(vco2Row.removeFromLeft(LABEL_WIDTH));
                _fm.setBounds(vco2Row.removeFromLeft(tokens::semantic::dialogRandomizerCol1Width));
                _noise.setBounds(vco2Row.removeFromLeft(tokens::semantic::dialogRandomizerCol2Width));
                _sync.setBounds(vco2Row.removeFromLeft(tokens::semantic::dialogRandomizerCol3Width));

                auto matrixRow = rowBounds(area);
                _matrixLabel.setBounds(matrixRow.removeFromLeft(LABEL_WIDTH));
                _amount.setBounds(matrixRow.removeFromLeft(tokens::semantic::dialogRandomizerCol1Width));
                _quantize.setBounds(matrixRow.removeFromLeft(tokens::semantic::dialogRandomizerCol2Width));
                _srcDest.setBounds(matrixRow.removeFromLeft(tokens::semantic::dialogRandomizerCol3Width));

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
                label.setFont(dialogControlFont());
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
                            std::function<void(int)> onLedColourChanged,
                            std::function<void(const BlockPalette&)> onBlockPaletteChanged)
                : _controller(controller), _settingsService(settingsService), _backend(backend),
                  _onLedColourChanged(std::move(onLedColourChanged)),
                  _onBlockPaletteChanged(std::move(onBlockPaletteChanged)),
                  _tabs(juce::TabbedButtonBar::TabsAtTop)
            {
                _originalLedColour = settingsService.allUsersSettings().uiConfig.knobLedBorderColor;
                // Palette snapshot taken on open, restored on any non-accept
                // close (Cancel, Esc, title bar). [DEC-JUC-038]
                _originalPalette = resolveBlockPalette(settingsService.allUsersSettings().uiConfig);

                auto* midiPage = new MidiSettingsPage(settingsService, backend);
                auto* uiPage = new UiSettingsPage(settingsService, _onBlockPaletteChanged);
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

            ~SettingsContent() override
            {
                // Any close that did not go through accept() reverts the live
                // preview to the palette snapshot. [DEC-JUC-038, RQ-GUI-046]
                if (!_accepted && _onBlockPaletteChanged)
                {
                    _onBlockPaletteChanged(_originalPalette);
                }
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
                // Commit the accepted palette (override ?? default) as the live
                // one; the destructor must not revert it. [DEC-JUC-039]
                _accepted = true;
                if (_onBlockPaletteChanged)
                {
                    _onBlockPaletteChanged(resolveBlockPalette(settings.uiConfig));
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
            std::function<void(const BlockPalette&)> _onBlockPaletteChanged;
            int _originalLedColour = 0;
            BlockPalette _originalPalette;
            bool _accepted = false;

            juce::TabbedComponent _tabs;
            MidiSettingsPage* _midiPage = nullptr;
            UiSettingsPage* _uiPage = nullptr;
            RandomizerSettingsPage* _randomPage = nullptr;
            juce::TextButton _ok, _cancel;
        };
    }

    void showSettingsDialog(controller::XpanderController& controller,
                            settings::ISettingsService& settingsService, xpl::midi::MidiBackend& backend,
                            std::function<void(int)> onLedColourChanged,
                            std::function<void(const BlockPalette&)> onBlockPaletteChanged)
    {
        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(
            new SettingsContent(controller, settingsService, backend, std::move(onLedColourChanged),
                                std::move(onBlockPaletteChanged)));
        options.dialogTitle = "Settings";
        options.dialogBackgroundColour = tokens::semantic::surfaceBase;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = true; // the automation table benefits from more height
        options.launchAsync();
    }
}
