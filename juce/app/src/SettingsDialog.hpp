#pragma once

// Tabbed settings dialog (MIDI / User interface / Randomizer), port of the
// reference SettingsForm hosting MidiPage, UserInterfacePage and
// RandomizerPage. On OK it persists every page and re-applies the MIDI
// settings; a changed knob LED colour is signalled back so the skin updates
// live, and block-colour edits preview live through the palette callback.
// [RQ-GUI-025, RQ-GUI-046]

#include "BlockPalette.hpp"

#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/settings/SettingsService.hpp"
#include "xpl/midi/MidiPorts.hpp"

#include <functional>

namespace xplorer::app
{
    /// Opens the modal settings dialog. `onLedColourChanged` is invoked (with
    /// the new 32-bit ARGB value) when the accepted UI page changed the knob
    /// LED colour, so the caller can refresh the LookAndFeel.
    /// `onBlockPaletteChanged` is invoked with the palette to make live: on
    /// every block-colour edit (preview), on accept (final resolved palette)
    /// and on a non-accept close (snapshot restore). [ADR-JUC-020
    /// (DEC-JUC-038)]
    void showSettingsDialog(controller::XpanderController& controller,
                            settings::ISettingsService& settingsService,
                            xpl::midi::MidiBackend& backend,
                            std::function<void(int)> onLedColourChanged,
                            std::function<void(const BlockPalette&)> onBlockPaletteChanged);
}
