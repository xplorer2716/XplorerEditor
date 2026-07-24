#pragma once

// Custom look reproducing the reference controls: rotary knob with a colored
// LED ring (UiConfiguration.knobLedBorderColor) and a compact checkbox.
// Applied in the skin pass; behavior is unchanged. [RQ-GUI-031]

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class XplorerLookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        explicit XplorerLookAndFeel(juce::Colour ledColour);

        /// The single runtime source of truth for the knob LED colour; every
        /// consumer (knobs, tick boxes, matrix highlight) derives from it, so a
        /// colour change only rebuilds this object. [ADR-JUC-011]
        [[nodiscard]] juce::Colour ledColour() const { return _ledColour; }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                              juce::Slider& slider) override;

        void drawTickBox(juce::Graphics& g, juce::Component& component, float x, float y, float w, float h,
                         bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted,
                         bool shouldDrawButtonAsDown) override;

        // Compact box + caption laid out to fit tight reference bounds (no
        // ellipsis on short captions like "TRI"). [RQ-GUI-032]
        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        // Shrinks the font per combo box so its widest item label always fits
        // without ellipsis, instead of JUCE's default 70%-squish-then-truncate
        // (reference combos never show "TRA..."). [RQ-GUI-032, issue #12]
        juce::Font getComboBoxFont(juce::ComboBox& box) override;

        // Reproduces the stock combo (fill/outline/arrow) from tokens and adds
        // the shared hover/disabled/focus states. [RQ-GUI-041..043, ADR-JUC-017]
        void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX,
                          int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;

    private:
        // Circular counterpart of drawTickBox for radio-group toggles (a
        // ToggleButton with a non-zero radio group id), so two-way choices
        // read as classic radios instead of check boxes. Hover/disabled/focus
        // states mirror drawTickBox. [RQ-GUI-038, ADR-JUC-016, RQ-GUI-041..043,
        // ADR-JUC-017]
        void drawRadioBox(juce::Graphics& g, juce::Component& component, float x, float y, float w, float h,
                          bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted);

        juce::Colour _ledColour;
    };
}
