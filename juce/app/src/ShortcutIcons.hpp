#pragma once

// Vector icons for the eight shortcut buttons under the VFD (RQ-GUI-021).
//
// They replace 24 GIF resources of 19x15 px carried over from the .NET editor:
// the last raster elements of the control surface, and the only ones that blur
// when the window is scaled. Each icon is built here as juce::Path geometry, the
// way BackgroundRenderer builds the diagram and VfdSegmentRenderer the glyphs —
// no assets, and geometry a test can read.
// [RQ-GUI-063, RQ-GUI-064, ADR-GUI-001 (DEC-GUI-001-A)]

#include <juce_graphics/juce_graphics.h>

namespace xplorer::app
{
    /// The eight actions of RQ-GUI-021, in the order they appear on screen.
    enum class ShortcutIcon
    {
        PreviousProgram,  ///< left triangle
        NextProgram,      ///< right triangle
        GoToProgram,      ///< arrow meeting a stop bar
        Randomise,        ///< die, five pips
        LoadFile,         ///< open folder — a third floppy would not read at 29 px
        SaveFile,         ///< floppy disk
        StoreToSynth,     ///< the same floppy, in the accent red
        MidiSettings      ///< DIN-5 socket: keyway on top, five pins on the lower arc
    };

    /// An icon splits into what is stroked and what is filled, because the two
    /// need different paint calls: a pip or a connector pin is a solid dot, while
    /// every outline is a stroked contour. Keeping them apart here is what lets
    /// the caller apply ONE stroke type to the whole outline (DEC-GUI-001-C)
    /// instead of fudging thin filled shapes.
    struct ShortcutIconPaths
    {
        juce::Path stroked;
        juce::Path filled;
    };

    /// Build one icon inside a square box of the given side, centred.
    /// The box is the key's own bounds; the artwork occupies its middle ~60%.
    [[nodiscard]] ShortcutIconPaths shortcutIconPaths(ShortcutIcon icon, float boxSize);

    /// The stroke type every shortcut icon is drawn with. Rounded caps and curved
    /// joints are load-bearing, not decoration: at 29 px JUCE's default mitred
    /// joint and butt cap leave a visible burr on every corner (owner review,
    /// 2026-08-10). [RQ-GUI-064, ADR-GUI-001 (DEC-GUI-001-C)]
    [[nodiscard]] juce::PathStrokeType shortcutIconStroke(float boxSize);

    /// Paint one key: flat fill, 1 px outline, then its icon.
    ///
    /// `accent` is the LookAndFeel's `ledColour()` — the runtime source of truth
    /// every other control derives its highlight from (ADR-JUC-011), and which
    /// the user can retheme. It is a PARAMETER rather than a token because a
    /// token would freeze the buttons on one colour while the knobs, tick boxes
    /// and radios around them followed the user's choice.
    /// States follow ADR-JUC-017 — hover lights in the accent, down inverts.
    /// [RQ-GUI-067, ADR-JUC-011, ADR-JUC-020]
    void paintShortcutButton(juce::Graphics& g, juce::Rectangle<float> bounds,
                             ShortcutIcon icon, juce::Colour accent,
                             bool isHovered, bool isDown);
}
