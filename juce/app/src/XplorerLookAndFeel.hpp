#pragma once

// Custom look reproducing the reference controls: rotary knob with a colored
// LED ring (UiConfiguration.knobLedBorderColor) and a compact checkbox.
// Applied in the skin pass; behavior is unchanged. [RQ-GUI-031]

#include "BlockPalette.hpp"
#include "DesignTokens.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

// (no <optional> needed: the size is a token, ADR-JUC-022 DEC-JUC-046)

namespace xplorer::app
{
    class XplorerLookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        explicit XplorerLookAndFeel(juce::Colour ledColour);

        /// The single runtime source of truth for the knob LED colour; every
        /// consumer (knobs, tick boxes, matrix highlight) derives from it, so a
        /// colour change only touches this object. [ADR-JUC-011]
        [[nodiscard]] juce::Colour ledColour() const { return _ledColour; }

        /// Retunes the LED colour IN PLACE, so the settings dialog can preview
        /// a pick live without rebuilding the whole LookAndFeel on every drag
        /// of the colour picker — the same technique setBlockPalette() already
        /// uses. Re-seeds the JUCE colour IDs derived from it at construction,
        /// which is what keeps ADR-JUC-011's no-cached-copy promise true under
        /// live preview. [RQ-GUI-073, ADR-JUC-020 (DEC-JUC-113), ADR-JUC-011]
        void setLedColour(juce::Colour colour);

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

        // The ONE fixed size every combo box uses, in the embedded condensed
        // face, so the panel never shows several sizes at once and no label is
        // ever ellipsized. The `box` parameter is deliberately unused: the size
        // is a design-system decision (a token), not a property of the asking
        // box — do not reintroduce a per-box computation here.
        // [RQ-GUI-047, RQ-DSN-096, ADR-JUC-022 (DEC-JUC-046, DEC-JUC-049), issue #12]
        juce::Font getComboBoxFont(juce::ComboBox& box) override;

        // Lays the combo's text out against the same arrow-zone / label-border
        // tokens drawComboBox uses for the arrow, so the two cannot drift apart.
        // Overriding this is required: LookAndFeel_V4 hard-codes its own 30px
        // here, independently of drawComboBox. [RQ-GUI-047, ADR-JUC-022 (DEC-JUC-047)]
        void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;

        // Reproduces the stock combo (fill/outline/arrow) from tokens and adds
        // the shared hover/disabled/focus states. [RQ-GUI-041..043, ADR-JUC-017]
        void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX,
                          int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;

        // Digits-only text entry for the Go to patch / Store spinner: the
        // stock Label the base class returns accepts arbitrary text (a typed
        // "abc" is silently parsed to 0 by valueFromTextFunction, harmless
        // but unpolished). No other Slider in the app uses a text box
        // (knobs/matrix sliders are NoTextBox), so this only affects the
        // spinner. [RQ-GUI-058]
        juce::Label* createSliderTextBox(juce::Slider& slider) override;

        // The percentage label must stay legible wherever the bar's own fill
        // happens to reach it, not just averaged across the whole bar: JUCE's
        // default LookAndFeel_V4::drawLinearProgressBar picks the label's
        // colour with Colour::contrasting(background, foreground) -- a single
        // colour compromised between both -- which reads as near-invisible
        // once the (bright, user-tunable) ledColour fill passes under the
        // label. Draws the bar itself unchanged via the base class, then
        // draws the label twice, each pass clipped to its own region and
        // contrasted against that region's own colour specifically.
        // [RQ-GUI-078, ADR-JUC-011]
        void drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar, int width, int height,
                             double progress, const juce::String& textToShow) override;

    private:
        // Circular counterpart of drawTickBox for radio-group toggles (a
        // ToggleButton with a non-zero radio group id), so two-way choices
        // read as classic radios instead of check boxes. Hover/disabled/focus
        // states mirror drawTickBox. [RQ-GUI-038, ADR-JUC-016, RQ-GUI-041..043,
        // ADR-JUC-017]
        void drawRadioBox(juce::Graphics& g, juce::Component& component, float x, float y, float w, float h,
                          bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted);

        /// The combo-box Font itself, independent of any ComboBox instance, so
        /// the start-up fit check (RQ-GUI-048) can measure with exactly the
        /// font the boxes will use. [ADR-JUC-022 (DEC-JUC-046, DEC-JUC-050)]
        [[nodiscard]] juce::Font comboFont() const;

        juce::Colour _ledColour;
        BlockPalette _blockPalette = defaultBlockPalette();

        /// The embedded combo-box typeface (RQ-DSN-096). Held so it can be
        /// attached EXPLICITLY to every combo Font: registering it as the
        /// LookAndFeel's default sans-serif mis-maps glyph indices on the
        /// pinned JUCE version. [ADR-JUC-022 (DEC-JUC-048, DEC-JUC-049)]
        juce::Typeface::Ptr _comboTypeface;
    };
}
