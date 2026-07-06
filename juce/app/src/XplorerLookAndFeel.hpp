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

    private:
        juce::Colour _ledColour;
    };
}
