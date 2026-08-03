#pragma once

// A modulation-matrix source/destination combo box: a HoverRepaintingComboBox
// (so it inherits the issue-#21 hover-repaint fix) that additionally carries the
// two pieces of state XplorerLookAndFeel::drawComboBox needs to paint it —
//
//   * blockColour : the hue of the functional block its SELECTED VALUE belongs
//                   to, or nullopt when that value has no block (VEL, NONE,
//                   KBD, the pedals...). nullopt is a real state: the control
//                   then renders exactly like every other combo box in the app.
//   * highlighted : whether the RQ-GUI-018 cross-reference currently matches
//                   this combo, i.e. the pointer is over a related knob or
//                   page-family selector ELSEWHERE on the panel. This is not
//                   the same thing as the pointer being over this combo, which
//                   is RQ-GUI-041's direct hover and stays owned by
//                   isMouseOverOrDragging().
//
// Typed state on a subclass rather than juce::Component::getProperties(): the
// LookAndFeel resolves it with one dynamic_cast, the same way MainComponent
// already resolves BoundControl, and the compiler checks it.
// [RQ-GUI-052, RQ-GUI-018, RQ-DSN-100, ADR-JUC-028 (DEC-JUC-080)]

#include "HoverRepaintingComboBox.hpp"

#include <juce_graphics/juce_graphics.h>

#include <optional>

namespace xplorer::app
{
    class ModMatrixComboBox : public HoverRepaintingComboBox
    {
    public:
        using HoverRepaintingComboBox::HoverRepaintingComboBox;

        /// Sets the hue of the block the selected value belongs to (nullopt =
        /// no block). Called on every value change, not only at construction:
        /// the tint follows the VALUE, so a patch load must move it too.
        /// [ADR-JUC-028 (DEC-JUC-082)]
        void setBlockColour(std::optional<juce::Colour> colour)
        {
            if (_blockColour == colour)
            {
                return; // repaint only on an actual change
            }
            _blockColour = colour;
            repaint();
        }

        [[nodiscard]] std::optional<juce::Colour> blockColour() const { return _blockColour; }

        /// Marks this combo as matched by the modulation cross-reference.
        void setHighlighted(bool highlighted)
        {
            if (_highlighted == highlighted)
            {
                return;
            }
            _highlighted = highlighted;
            repaint();
        }

        [[nodiscard]] bool isHighlighted() const { return _highlighted; }

    private:
        std::optional<juce::Colour> _blockColour;
        bool _highlighted = false;
    };
}
