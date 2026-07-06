#pragma once

// Application dialogs (functional-first, AlertWindow-based): MIDI settings,
// store/goto patch, rename patch, about. Port of the reference SettingsForm
// (MIDI page), StoreAndGotoPatchForm, RenamePatchForm, AboutForm.
// [RQ-GUI-025]

#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/settings/SettingsService.hpp"
#include "xpl/midi/MidiPorts.hpp"

#include <functional>
#include <string>

namespace xplorer::app
{
    /// Applies the persisted MIDI settings to the controller (device
    /// assignment, channel, transmit delay). Called at startup and after the
    /// settings dialog is accepted.
    void applyMidiSettings(controller::XpanderController& controller,
                           settings::ISettingsService& settingsService,
                           xpl::midi::MidiBackend& backend);

    /// MIDI settings dialog: device selection + channel + transmit delay +
    /// synth type + smart all-notes-off. On accept, persists and re-applies.
    void showMidiSettingsDialog(controller::XpanderController& controller,
                                settings::ISettingsService& settingsService,
                                xpl::midi::MidiBackend& backend);

    /// Program-number chooser; `title` is "Go to patch" or "Store". Calls
    /// `onAccept(programNumber)`.
    void showStoreOrGotoDialog(const std::string& title, int currentProgram,
                               std::function<void(int)> onAccept);

    /// Rename dialog with the reference character-set validation.
    void showRenameDialog(const std::string& currentName, std::function<void(const std::string&)> onAccept);

    /// True when every character is in the reference patch-name character set.
    [[nodiscard]] bool isPatchNameValid(const std::string& name);

    void showAboutDialog(const std::string& productNameAndVersion);
}
