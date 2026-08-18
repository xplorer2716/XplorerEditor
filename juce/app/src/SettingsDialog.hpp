#pragma once

// Tabbed settings dialog (MIDI / User interface / Randomizer), port of the
// reference SettingsForm hosting MidiPage, UserInterfacePage and
// RandomizerPage. On OK it persists every page and re-applies the MIDI
// settings; both colour groups of the User interface page — the knob LED
// colour and the eight block colours — preview live through their callbacks.
// [RQ-GUI-025, RQ-GUI-046, RQ-GUI-073]

#include "BlockPalette.hpp"

#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/settings/SettingsService.hpp"
#include "xpl/midi/MidiPorts.hpp"

#include <functional>

namespace xplorer::app
{
    /// Opens the modal settings dialog. Both callbacks are invoked with the
    /// value to make live, on the same three occasions: on every edit of their
    /// control (preview), on accept (final accepted value) and on a non-accept
    /// close (snapshot restore). `onLedColourChanged` carries a 32-bit ARGB
    /// value, `onBlockPaletteChanged` the resolved palette.
    /// [ADR-JUC-020 (DEC-JUC-038, DEC-JUC-113)]
    void showSettingsDialog(controller::XpanderController& controller,
                            settings::ISettingsService& settingsService,
                            xpl::midi::MidiBackend& backend,
                            std::function<void(int)> onLedColourChanged,
                            std::function<void(const BlockPalette&)> onBlockPaletteChanged);
}
