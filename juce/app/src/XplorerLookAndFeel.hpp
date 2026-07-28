#pragma once

// Custom look reproducing the reference controls: rotary knob with a colored
// LED ring (UiConfiguration.knobLedBorderColor) and a compact checkbox.
// Applied in the skin pass; behavior is unchanged. [RQ-GUI-031]

#include "BlockPalette.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <optional>

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

        /// The single runtime source of truth for the block-identity palette
        /// (defaults = design tokens, user overrides win). The painter takes it
        /// as a parameter, selector buttons read it at paint time; live preview
        /// mutates it in place (setBlockPalette + sendLookAndFeelChange), no
        /// LookAndFeel rebuild. [RQ-DSN-095, ADR-JUC-020 (DEC-JUC-036/038)]
        [[nodiscard]] const BlockPalette& blockPalette() const { return _blockPalette; }
        void setBlockPalette(const BlockPalette& palette) { _blockPalette = palette; }

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

        // Returns the ONE font size shared by every combo box in the app, so
        // the panel never shows several sizes at once, with no label ever
        // ellipsized. The `box` parameter is deliberately unused: the size is
        // a property of the whole inventory, not of the asking box. Computed
        // once and cached (see _sharedComboBoxFontSize).
        // [RQ-GUI-047, RQ-DSN-011, ADR-JUC-021 (DEC-JUC-041, DEC-JUC-045),
        //  issue #12]
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
        BlockPalette _blockPalette = defaultBlockPalette();

        /// Memoised shared combo-box font size. Its inputs are the static
        /// control table and enum label sets, so it is computed once on first
        /// use and never invalidated — in particular NOT on window resize
        /// (control bounds are logical; ScaledCanvasComponent only transforms
        /// the canvas) and NOT per combo-box construction. A fresh value only
        /// arises when this whole object is rebuilt on an LED-colour change
        /// (ADR-JUC-011). [ADR-JUC-021 (DEC-JUC-045)]
        mutable std::optional<float> _sharedComboBoxFontSize;
    };
}
