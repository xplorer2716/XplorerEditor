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
#include <vector>

namespace xplorer::app
{
    class NumericEntryKnob : public juce::Slider
    {
    public:
        NumericEntryKnob() : NumericEntryKnob(RotaryHorizontalVerticalDrag, NoTextBox) {}

        NumericEntryKnob(SliderStyle style, TextEntryBoxPosition textBoxPosition)
            : juce::Slider(style, textBoxPosition)
        {
            // Transient value bubble while dragging (RQ-GUI-034) lives here,
            // not on a specific consumer, so BoundKnob (parameter knobs) and
            // the modulation-matrix amount knob (RQ-GUI-015) get it from the
            // same one implementation the double-click entry already shares.
            // [RQ-GUI-034, RQ-GUI-015, PLAN-GUI-009 (TASK-GUI-043)]
            setPopupDisplayEnabled(true, true, nullptr);
        }

        void mouseDoubleClick(const juce::MouseEvent&) override { openNumericEntry(); }

        /// The double-click action itself, factored out of mouseDoubleClick so
        /// it is callable without a live juce::MouseEvent (which needs a real
        /// MouseInputSource/Desktop — unavailable to the headless test target).
        /// [RQ-GUI-034]
        void openNumericEntry();

        /// Replaces the range-derived presets of RQ-GUI-079. Only the VCO
        /// frequency knobs use this, to carry the reference's harmonic
        /// semitone steps. [RQ-GUI-079, ADR-JUC-037 (DEC-JUC-130)]
        void setPresetValues(std::vector<int> values);

        /// The presets this knob offers, derived from its own range on first
        /// use unless setPresetValues() already supplied them. Derived LAZILY
        /// because callers set the range after construction (the
        /// modulation-matrix amount knob does), so a value computed in the
        /// constructor would describe the wrong range. [RQ-GUI-079]
        [[nodiscard]] const std::vector<int>& presetValues();

        /// Sets this knob to its `index`-th preset, as if the user had turned
        /// it there. Returns false when this knob has no preset at that index
        /// — which is how the eleventh key stays inert on a knob whose range
        /// is not symmetric about zero. [RQ-GUI-079]
        bool applyPresetValue(int index);

    private:
        void applyTextEntry();
        void dismissTextEntry();
        /// The one place a value enters this knob from a discrete gesture, so
        /// the typed entry and the preset key cannot drift apart in what they
        /// notify. [RQ-GUI-034, RQ-GUI-079]
        void applyValue(int value);

        std::unique_ptr<juce::TextEditor> _entryEditor;
        std::vector<int> _presetValues;
    };
}
