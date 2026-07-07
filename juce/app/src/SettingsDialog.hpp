#pragma once

// Tabbed settings dialog (MIDI / User interface / Randomizer), port of the
// reference SettingsForm hosting MidiPage, UserInterfacePage and
// RandomizerPage. On OK it persists every page and re-applies the MIDI
// settings; a changed knob LED colour is signalled back so the skin updates
// live. [RQ-GUI-025]

#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/settings/SettingsService.hpp"
#include "xpl/midi/MidiPorts.hpp"

#include <functional>

namespace xplorer::app
{
    /// Opens the modal settings dialog. `onLedColourChanged` is invoked (with
    /// the new 32-bit ARGB value) when the accepted UI page changed the knob
    /// LED colour, so the caller can refresh the LookAndFeel.
    void showSettingsDialog(controller::XpanderController& controller,
                            settings::ISettingsService& settingsService,
                            xpl::midi::MidiBackend& backend,
                            std::function<void(int)> onLedColourChanged);
}
