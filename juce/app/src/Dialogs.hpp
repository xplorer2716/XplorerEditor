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

    /// Extract-single-tones dialog: bank sysex file + destination folder, then
    /// runs the extraction and reports the file count. [RQ-GUI-025]
    void showExtractSingleTonesDialog(controller::XpanderController& controller);

    /// Runs the (blocking) all-data-dump restore off the message thread with a
    /// modal progress window; reports any error. [RQ-GUI-026]
    void runRestoreAllDataWithProgress(controller::XpanderController& controller,
                                       const std::string& fileName);

    /// Program-number chooser; `title` is "Go to patch" or "Store". Calls
    /// `onAccept(programNumber)`.
    void showStoreOrGotoDialog(const std::string& title, int currentProgram,
                               std::function<void(int)> onAccept);

    /// Rename dialog: the text field restricts keystrokes/paste to the
    /// reference patch-name character set and length live, with
    /// isPatchNameValid() re-checked on accept as a safety net. `currentName`
    /// is the stored, space-padded name -- the dialog strips the padding
    /// before showing it (patchNameForEditing). [RQ-GUI-081]
    void showRenameDialog(const std::string& currentName, std::function<void(const std::string&)> onAccept);

    /// The stored patch name as it should appear in an editable field: the
    /// model's fixed-width space padding removed, leading and interior spaces
    /// kept. [RQ-GUI-081]
    [[nodiscard]] std::string patchNameForEditing(const std::string& storedName);

    /// True when every character is in the reference patch-name character
    /// set and the name is within the stored patch-name length.
    [[nodiscard]] bool isPatchNameValid(const std::string& name);

    void showAboutDialog(const std::string& productNameAndVersion);

    /// Third-party dependency disclosure, opened from the About dialog. Reads
    /// the SPDX SBOM shipped beside the executable at call time and lists what
    /// it finds alphabetically -- or explains why it found nothing. No
    /// dependency metadata is compiled in. [RQ-GUI-057, ADR-ABT-001]
    void showDependenciesWindow();
}
