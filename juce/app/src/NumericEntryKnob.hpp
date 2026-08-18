#pragma once

// A juce::Slider that opens an inline numeric text-entry box on double-click,
// instead of JUCE's default (no-op unless setDoubleClickReturnValue is set).
//
// RQ-GUI-034 mandates this for EVERY rotary knob: no permanent text box, a
// transient popup bubble while dragging, double-click for an exact value.
// It used to live only on BoundKnob (the parameter-knob wrapper), so the
// modulation-matrix amount knob — a rotary knob too (RQ-GUI-015), but not
// parameter-bound — never got it. One shared base means one implementation
// for both, the same fix HoverRepaintingComboBox already applied to combo
// boxes (ADR-JUC-017, DEC-JUC-040).
// [RQ-GUI-034, RQ-GUI-015, PLAN-GUI-009 (TASK-GUI-036)]

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>

namespace xplorer::app
{
    class NumericEntryKnob : public juce::Slider
    {
    public:
        using juce::Slider::Slider;

        void mouseDoubleClick(const juce::MouseEvent&) override { openNumericEntry(); }

        /// The double-click action itself, factored out of mouseDoubleClick so
        /// it is callable without a live juce::MouseEvent (which needs a real
        /// MouseInputSource/Desktop — unavailable to the headless test target).
        /// [RQ-GUI-034]
        void openNumericEntry();

    private:
        void applyTextEntry();
        void dismissTextEntry();

        std::unique_ptr<juce::TextEditor> _entryEditor;
    };
}
