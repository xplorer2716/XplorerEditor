#include "Dialogs.hpp"

#include "xplorer/model/XpanderTone.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>

namespace xplorer::app
{
    void applyMidiSettings(controller::XpanderController& controller,
                           settings::ISettingsService& settingsService, xpl::midi::MidiBackend&)
    {
        const auto& midi = settingsService.allUsersSettings().midiConfig;
        controller.setSynthOutputDevice(midi.synthOutputDeviceName);
        controller.setSynthInputDevice(midi.synthInputDeviceName);
        controller.setAutomationInputDevice(midi.automationInputDeviceName);
        // Reference MIDI channel is 1-based in settings, 0-based on the wire.
        controller.setMidiChannel(juce::jlimit(0, 15, midi.midiChannel - 1));
        controller.setParameterTransmitDelay(juce::jmax(0, midi.sysexTransmitDelay));
        controller.setEditingProgramNumber(
            juce::jlimit(model::XpanderTone::MIN_PROGRAM_NUMBER, model::XpanderTone::MAX_PROGRAM_NUMBER,
                         midi.editingProgramNumber));
    }

    void showMidiSettingsDialog(controller::XpanderController& controller,
                                settings::ISettingsService& settingsService,
                                xpl::midi::MidiBackend& backend)
    {
        auto* window = new juce::AlertWindow("MIDI settings",
                                             "Configure MIDI devices and options",
                                             juce::MessageBoxIconType::NoIcon);
        const auto& midi = settingsService.allUsersSettings().midiConfig;

        auto addDeviceCombo = [&](const juce::String& label, const std::vector<std::string>& names,
                                  const std::string& current)
        {
            juce::StringArray items;
            items.add("(none)");
            for (const auto& name : names)
            {
                items.add(name);
            }
            window->addComboBox(label, items);
            auto* combo = window->getComboBoxComponent(label);
            combo->setSelectedItemIndex(0);
            for (int i = 0; i < static_cast<int>(names.size()); ++i)
            {
                if (names[static_cast<std::size_t>(i)] == current)
                {
                    combo->setSelectedItemIndex(i + 1);
                }
            }
        };

        addDeviceCombo("Synth output", backend.outputDeviceNames(), midi.synthOutputDeviceName);
        addDeviceCombo("Synth input", backend.inputDeviceNames(), midi.synthInputDeviceName);
        addDeviceCombo("Automation input", backend.inputDeviceNames(), midi.automationInputDeviceName);
        window->addTextEditor("MIDI channel (1-16)", juce::String(midi.midiChannel));
        window->addTextEditor("SysEx transmit delay (ms)", juce::String(midi.sysexTransmitDelay));
        window->addTextEditor("Synth type (Xpander/Matrix12)",
                              midi.synthTypeIsMatrix12 ? "Matrix12" : "Xpander");
        window->addTextEditor("Smart all notes off (yes/no)", midi.smartAllNotesOff ? "yes" : "no");
        window->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        window->enterModalState(true,
            juce::ModalCallbackFunction::create(
                [window, &controller, &settingsService, &backend](int result)
                {
                    std::unique_ptr<juce::AlertWindow> owner(window);
                    if (result == 0)
                    {
                        return;
                    }
                    auto settings = settingsService.allUsersSettings();
                    auto& midiConfig = settings.midiConfig;
                    auto comboText = [window](const juce::String& label)
                    {
                        const auto text = window->getComboBoxComponent(label)->getText();
                        return text == "(none)" ? juce::String() : text;
                    };
                    midiConfig.synthOutputDeviceName = comboText("Synth output").toStdString();
                    midiConfig.synthInputDeviceName = comboText("Synth input").toStdString();
                    midiConfig.automationInputDeviceName = comboText("Automation input").toStdString();
                    midiConfig.midiChannel =
                        juce::jlimit(1, 16, window->getTextEditorContents("MIDI channel (1-16)").getIntValue());
                    midiConfig.sysexTransmitDelay =
                        juce::jmax(0, window->getTextEditorContents("SysEx transmit delay (ms)").getIntValue());
                    midiConfig.synthTypeIsMatrix12 =
                        window->getTextEditorContents("Synth type (Xpander/Matrix12)")
                            .trim()
                            .equalsIgnoreCase("Matrix12");
                    midiConfig.smartAllNotesOff =
                        window->getTextEditorContents("Smart all notes off (yes/no)").trim().equalsIgnoreCase("yes");
                    settingsService.saveSettings(settings);
                    applyMidiSettings(controller, settingsService, backend);
                }),
            false);
    }

    void showStoreOrGotoDialog(const std::string& title, int currentProgram,
                               std::function<void(int)> onAccept)
    {
        auto* window = new juce::AlertWindow(title, "Choose a program number", juce::MessageBoxIconType::NoIcon);
        juce::StringArray items;
        for (int i = model::XpanderTone::MIN_PROGRAM_NUMBER; i <= model::XpanderTone::MAX_PROGRAM_NUMBER; ++i)
        {
            items.add(juce::String(i));
        }
        window->addComboBox("Program", items);
        window->getComboBoxComponent("Program")->setSelectedItemIndex(
            juce::jlimit(0, items.size() - 1, currentProgram));
        window->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        window->enterModalState(true,
            juce::ModalCallbackFunction::create(
                [window, onAccept = std::move(onAccept)](int result)
                {
                    std::unique_ptr<juce::AlertWindow> owner(window);
                    if (result != 0)
                    {
                        onAccept(window->getComboBoxComponent("Program")->getText().getIntValue());
                    }
                }),
            false);
    }

    bool isPatchNameValid(const std::string& name)
    {
        static const std::string valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789</>+-*$ ";
        for (const char c : name)
        {
            if (valid.find(c) == std::string::npos)
            {
                return false;
            }
        }
        return true;
    }

    void showRenameDialog(const std::string& currentName, std::function<void(const std::string&)> onAccept)
    {
        auto* window = new juce::AlertWindow("Rename patch",
                                             "Allowed: A-Z 0-9 < / > + - * $ and space",
                                             juce::MessageBoxIconType::NoIcon);
        window->addTextEditor("Name", juce::String(currentName));
        window->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

        window->enterModalState(true,
            juce::ModalCallbackFunction::create(
                [window, onAccept = std::move(onAccept)](int result)
                {
                    std::unique_ptr<juce::AlertWindow> owner(window);
                    if (result == 0)
                    {
                        return;
                    }
                    auto name = window->getTextEditorContents("Name").toUpperCase().toStdString();
                    if (isPatchNameValid(name))
                    {
                        onAccept(name);
                    }
                    else
                    {
                        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                                                              "Rename patch", "Invalid characters in name.");
                    }
                }),
            false);
    }

    void showAboutDialog(const std::string& productNameAndVersion)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::InfoIcon, "About Xplorer",
            juce::String(productNameAndVersion)
                + "\n\nReal-time editor for the Oberheim Xpander / Matrix-12."
                  "\ngithub.com/xplorer2716/XplorerEditor");
    }
}
