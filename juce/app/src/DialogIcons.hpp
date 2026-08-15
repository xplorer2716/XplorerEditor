#pragma once

// Vector icons for the OS-native title bar of DialogWindow instances
// (Settings, About, Dependencies). Hand-authored juce::Path geometry, the same
// technique as the eight shortcut-button icons — no asset, a test can read the
// geometry — applied to a new context: a small badge rendered once to a
// juce::Image and handed to DocumentWindow::setIcon(), since a native title
// bar icon is OS chrome, not something Graphics paints directly each frame.
// [RQ-GUI-070, ADR-GUI-001 (DEC-GUI-001-A)]

#include <juce_graphics/juce_graphics.h>

namespace xplorer::app
{
    /// The three windows RQ-GUI-070 covers. Dependencies reuses Settings'
    /// glyph (owner decision, 2026-08-15) rather than getting a third one.
    enum class DialogIcon
    {
        Settings,  ///< gear
        About      ///< circled "i"
    };

    /// Render one dialog's title-bar icon at `pixelSize` square. Opaque
    /// `dialogIconAccent` badge behind a white glyph — a fixed pair, not the
    /// live `ledColour()`: the window is native OS chrome outside the app's
    /// own LookAndFeel, set once at launch, so it cannot live-retheme the way
    /// an on-canvas control does.
    [[nodiscard]] juce::Image dialogTitleBarIcon(DialogIcon icon, int pixelSize);
}
