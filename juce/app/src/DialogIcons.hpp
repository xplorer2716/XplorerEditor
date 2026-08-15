#pragma once

// Vector icons for the OS-native title bar of DialogWindow instances
// (Settings, About, Dependencies). Hand-authored juce::Path geometry, the same
// technique as the eight shortcut-button icons — no asset, a test can read the
// geometry — applied to a new context: a small badge rendered once to a
// juce::Image and handed to the window's ComponentPeer, since a native title
// bar icon is OS chrome, not something Graphics paints directly each frame.
// [RQ-GUI-070, ADR-GUI-001 (DEC-GUI-001-A)]

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

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

    /// Put `icon` on `window`'s title bar, at the design-system icon size.
    ///
    /// `DocumentWindow::setIcon()` on its own does NOTHING for these three
    /// windows, and that is what shipped first (owner report, 2026-08-15: "c'est
    /// toujours l'icône par défaut qui est affichée"). It only stores the image
    /// for `LookAndFeel::drawDocumentWindowTitleBar` — the title bar JUCE draws
    /// itself — and all three set `useNativeTitleBar = true`, so that painter
    /// never runs and the image is never read.
    ///
    /// The icon has to reach the `ComponentPeer`, which is the object that talks
    /// to the window manager: `WM_SETICON` (ICON_BIG + ICON_SMALL) on Windows,
    /// `_NET_WM_ICON` on X11, per `juce_Windowing_windows.cpp` /
    /// `juce_Windowing_linux.cpp`. Both calls are made — the peer for the OS
    /// chrome that is actually shown, and the DocumentWindow so a future
    /// non-native variant of these dialogs still carries the glyph.
    /// [RQ-GUI-070, RQ-GUI-072, ADR-GUI-001 (DEC-GUI-001-A)]
    void applyDialogTitleBarIcon(juce::DocumentWindow& window, DialogIcon icon);
}
