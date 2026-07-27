#pragma once

// A juce::ComboBox that repaints itself when the pointer enters or leaves it.
//
// XplorerLookAndFeel::drawComboBox reads the hover state live at paint time
// (isMouseOverOrDragging(), per RQ-DSN-062's "read, never cache" rule), but
// juce::ComboBox — unlike juce::Slider / juce::Button — never triggers a
// repaint of its own on a hover transition, so the last-painted brightened
// frame survives on screen until some unrelated repaint happens to correct it
// (GitHub issue #21: sweeping the pointer down the mod matrix left several
// combos stuck highlighted). Every combo box in the app derives from this
// class so the fix cannot be forgotten at a new call site.
// [RQ-GUI-041, ADR-JUC-017 (DEC-JUC-040), issue #21]

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class HoverRepaintingComboBox : public juce::ComboBox
    {
    public:
        using juce::ComboBox::ComboBox;

        void mouseEnter(const juce::MouseEvent&) override { repaint(); }
        void mouseExit(const juce::MouseEvent&) override { repaint(); }
    };
}
