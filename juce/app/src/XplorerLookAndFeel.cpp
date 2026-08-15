#include "XplorerLookAndFeel.hpp"

#include "BinaryData.h"
#include "DesignTokens.hpp"
#include "ModMatrixComboBox.hpp"

#include "xplorer/app/ComboBoxSizing.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace xplorer::app
{
    XplorerLookAndFeel::XplorerLookAndFeel(juce::Colour ledColour)
        : _ledColour(ledColour)
    {
        setColour(juce::ComboBox::backgroundColourId, tokens::semantic::surfaceRecessed);
        setColour(juce::ComboBox::textColourId, tokens::semantic::textPrimary);
        setColour(juce::PopupMenu::backgroundColourId, tokens::semantic::surfaceRecessed);
        // The hovered item of an open combo-box popup list previously fell back
        // to LookAndFeel_V4's own untokenised default highlight — barely visible
        // against surfaceRecessed. Reads live from the ctor's ledColour, so a
        // LookAndFeel rebuild (MainComponent::updateLedColour) keeps it in sync
        // with every other accent-coloured control, no cached copy.
        // [RQ-GUI-068, ADR-JUC-011, ADR-JUC-017 (DEC-JUC-111)]
        setColour(juce::PopupMenu::highlightedBackgroundColourId,
                  ledColour.withAlpha(tokens::component::popupHighlightAlpha));
        setColour(juce::PopupMenu::highlightedTextColourId, tokens::semantic::textPrimary);
        setColour(juce::ToggleButton::textColourId, tokens::semantic::textPrimary);
        setColour(juce::Label::textColourId, tokens::semantic::textPrimary);

        // Embedded condensed face for combo-box text only: it makes the label
        // widths a property of the build rather than of the host's default
        // sans-serif, which is what lets the fixed size of RQ-GUI-047 fit the
        // reference geometry on every platform. Deliberately NOT registered via
        // setDefaultSansSerifTypeface — see getComboBoxFont.
        // [RQ-DSN-096, ADR-JUC-022 (DEC-JUC-048)]
        _comboTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::RobotoCondensedRegular_ttf, BinaryData::RobotoCondensedRegular_ttfSize);
        jassert(_comboTypeface != nullptr);

#if JUCE_DEBUG
        // The fit of every label in every combo box is settled at build time by
        // measured values (RQ-GUI-047). The only ways it can silently break
        // later are a new or renamed label, a width edit, or a typeface change —
        // so a development build checks it once, against the real metrics, and
        // names the offender. Release builds skip it: the condition is already
        // decided. [RQ-GUI-048, ADR-JUC-022 (DEC-JUC-050)]
        {
            const auto font = comboFont();
            const auto overflowing = comboBoxesOverflowingAt(
                collectComboBoxSizingInputs(), tokens::semantic::comboTextSize,
                tokens::semantic::comboArrowZone, tokens::semantic::comboLabelBorder,
                [&font](std::string_view text, float)
                {
                    // GlyphArrangement, not the deprecated Font::getStringWidthFloat —
                    // same call BackgroundRenderer uses to measure section labels, so
                    // the two agree on what a string is wide. [ADR-JUC-022]
                    return juce::GlyphArrangement::getStringWidth(
                        font, juce::String(std::string(text)));
                });
            for (const auto& id : overflowing)
            {
                DBG("RQ-GUI-048: combo box '" << id << "' cannot show its widest label at "
                                              << tokens::semantic::comboTextSize << "pt");
            }
            jassert(overflowing.empty());
        }
#endif
    }

    void XplorerLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float startAngle, float endAngle,
                                              juce::Slider& slider)
    {
        const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0F);
        const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0F;
        const auto centre = bounds.getCentre();
        const auto angle = startAngle + sliderPos * (endAngle - startAngle);

        // No filled knob body: the interior is left transparent so the panel
        // background (with its shading) shows through — only the ring/crown is
        // drawn. [RQ-GUI-031, ADR-JUC-009]

        // Unlit ring track (full sweep), so the coloured arc reads against it.
        const auto ringRadius = radius - 1.0F;
        juce::Path track;
        track.addCentredArc(centre.x, centre.y, ringRadius, ringRadius, 0.0F, startAngle, endAngle, true);
        // Near-invisible white wash (reference DEFAULT_KNOB_LED_BACKGROUND_COLOR,
        // Standard style), not a solid ring — the panel shows through almost
        // entirely. [RQ-DSN-061]
        g.setColour(tokens::semantic::controlTrack.withAlpha(tokens::component::knobTrackAlpha));
        g.strokePath(track, juce::PathStrokeType(tokens::semantic::strokeKnobRing));

        // Coloured LED value arc from start to the current position. Brighter
        // while the mouse is over or dragging the knob (reference _isMouseEntered
        // light-colour highlight); no centre pointer (owner decision). [RQ-GUI-031]
        juce::Path ring;
        ring.addCentredArc(centre.x, centre.y, ringRadius, ringRadius, 0.0F, startAngle, angle, true);
        g.setColour(slider.isMouseOverOrDragging(true)
                        ? _ledColour.brighter(tokens::component::knobRingHoverBrighten)
                        : _ledColour);
        g.strokePath(ring, juce::PathStrokeType(tokens::semantic::strokeKnobRing));
    }

    void XplorerLookAndFeel::drawTickBox(juce::Graphics& g, juce::Component&, float x, float y, float w,
                                         float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted,
                                         bool)
    {
        // Hover brightens the existing border/fill; Disabled mutes the whole
        // box -- the two states compose rather than replace each other.
        // [RQ-GUI-041, RQ-GUI-043, ADR-JUC-017]
        const auto box = juce::Rectangle<float>(x, y, w, h).reduced(1.0F);
        const float disabledMul = isEnabled ? 1.0F : tokens::component::disabledAlpha;
        const bool hovered = isEnabled && shouldDrawButtonAsHighlighted;

        g.setColour(tokens::semantic::surfaceBase.withMultipliedAlpha(disabledMul));
        g.fillRoundedRectangle(box, tokens::semantic::radiusControl);

        auto borderColour = _ledColour.withAlpha(tokens::component::tickBoxBorderAlpha);
        if (hovered)
        {
            borderColour = borderColour.brighter(tokens::semantic::hoverBrighten);
        }
        g.setColour(borderColour.withMultipliedAlpha(disabledMul));
        g.drawRoundedRectangle(box, tokens::semantic::radiusControl, tokens::semantic::strokeBorder);

        if (ticked)
        {
            auto fillColour = hovered ? _ledColour.brighter(tokens::semantic::hoverBrighten) : _ledColour;
            g.setColour(fillColour.withMultipliedAlpha(disabledMul));
            g.fillRoundedRectangle(box.reduced(2.0F), tokens::semantic::radiusControlInner);
        }
    }

    void XplorerLookAndFeel::drawRadioBox(juce::Graphics& g, juce::Component&, float x, float y, float w,
                                          float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted)
    {
        // Circular sibling of drawTickBox; same token palette, AA insets and
        // hover/disabled behaviour so radios and check boxes stay visually
        // consistent. [RQ-GUI-038, ADR-JUC-016, RQ-GUI-041, RQ-GUI-043, ADR-JUC-017]
        const auto box = juce::Rectangle<float>(x, y, w, h).reduced(1.0F);
        const float disabledMul = isEnabled ? 1.0F : tokens::component::disabledAlpha;
        const bool hovered = isEnabled && shouldDrawButtonAsHighlighted;

        g.setColour(tokens::semantic::surfaceBase.withMultipliedAlpha(disabledMul));
        g.fillEllipse(box);

        auto borderColour = _ledColour.withAlpha(tokens::component::tickBoxBorderAlpha);
        if (hovered)
        {
            borderColour = borderColour.brighter(tokens::semantic::hoverBrighten);
        }
        g.setColour(borderColour.withMultipliedAlpha(disabledMul));
        g.drawEllipse(box, tokens::semantic::strokeBorder);

        if (ticked)
        {
            auto fillColour = hovered ? _ledColour.brighter(tokens::semantic::hoverBrighten) : _ledColour;
            g.setColour(fillColour.withMultipliedAlpha(disabledMul));
            g.fillEllipse(box.reduced(2.0F));
        }
    }

    void XplorerLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                              bool shouldDrawButtonAsHighlighted, bool)
    {
        // A small tick box on the left, then the caption in a compact font
        // sized to the control height so short captions (e.g. "TRI") do not get
        // ellipsized in the tight reference bounds. A radio-group toggle (non-zero
        // radio group id) gets a circular indicator instead. [RQ-GUI-032, RQ-GUI-038]
        const auto bounds = button.getLocalBounds();
        const auto boxSize = juce::jmin(14, bounds.getHeight());
        const auto box = juce::Rectangle<float>(0.0F, (bounds.getHeight() - boxSize) * 0.5F,
                                                static_cast<float>(boxSize), static_cast<float>(boxSize));
        if (button.getRadioGroupId() != 0)
        {
            drawRadioBox(g, button, box.getX(), box.getY(), box.getWidth(), box.getHeight(),
                        button.getToggleState(), button.isEnabled(), shouldDrawButtonAsHighlighted);
        }
        else
        {
            drawTickBox(g, button, box.getX(), box.getY(), box.getWidth(), box.getHeight(),
                        button.getToggleState(), button.isEnabled(), shouldDrawButtonAsHighlighted, false);
        }

        if (button.getButtonText().isNotEmpty())
        {
            const float textAlpha = button.isEnabled() ? 1.0F : tokens::component::disabledAlpha;
            g.setColour(button.findColour(juce::ToggleButton::textColourId).withMultipliedAlpha(textAlpha));
            // Settings-style dialogs match their checkbox/radio caption to the
            // dialog's own row-label size instead of the compact main-panel
            // caption — same context rule as getComboBoxFont above.
            // [RQ-GUI-061, ADR-JUC-033 (DEC-JUC-105)]
            const auto captionSize = button.findParentComponentOfClass<juce::DialogWindow>() != nullptr
                                         ? tokens::semantic::textTitle
                                         : tokens::semantic::textCaption;
            g.setFont(juce::Font{juce::FontOptions{
                juce::jmin(captionSize, static_cast<float>(bounds.getHeight()) - 3.0F)}});
            const auto textArea = bounds.withTrimmedLeft(boxSize + 2);
            g.drawText(button.getButtonText(), textArea, juce::Justification::centredLeft, false);
        }
    }

    void XplorerLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int,
                                          juce::ComboBox& box)
    {
        // Verbatim reproduction of LookAndFeel_V4::drawComboBox with tokens for
        // every colour/stroke (outline 1px == strokeBorder, arrow 2px ==
        // strokeLine, corner unified to the shared control radius) plus the
        // two interaction states: hover brightens the recessed fill, disabled
        // mutes the whole control at disabledAlpha. The arrow zone is the
        // design-system value that positionComboBoxText also reads, so text
        // and arrow stay consistent; it is narrower than LookAndFeel_V4's
        // 30 px, which is sized for far taller combo boxes. [RQ-GUI-041,
        // RQ-GUI-043, ADR-JUC-017, RQ-GUI-047, ADR-JUC-022 (DEC-JUC-047)]
        constexpr int ARROW_ZONE_X = tokens::semantic::comboArrowZone; // right inset of the arrow zone
        constexpr int ARROW_ZONE_W = tokens::semantic::comboArrowZone - 4; // arrow glyph zone
        constexpr float ARROW_INSET = 3.0F;
        constexpr float ARROW_RISE = 2.0F;
        constexpr float ARROW_DROP = 3.0F;
        constexpr float ARROW_ENABLED_ALPHA = 0.9F; // reference LookAndFeel_V4

        const bool enabled = box.isEnabled();
        const bool hovered = enabled && box.isMouseOverOrDragging(true);
        const float disabledMul = enabled ? 1.0F : tokens::component::disabledAlpha;
        const float corner = tokens::semantic::radiusControl;
        const auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();

        // Modulation-matrix combos carry the identity of the functional block
        // their SELECTED VALUE belongs to, and are frame-highlighted when the
        // RQ-GUI-018 cross-reference matches them. Every other combo box in the
        // app resolves to nullptr here and is painted exactly as before.
        // [RQ-GUI-052, RQ-DSN-100, ADR-JUC-028 (DEC-JUC-078, DEC-JUC-080)]
        // The combo stores the block IDENTITY; the hue is resolved here, from
        // the live palette, so a user re-theme applies with no cached copy
        // anywhere. [RQ-DSN-095, ADR-JUC-011, ADR-JUC-020 (DEC-JUC-036)]
        const auto* matrixBox = dynamic_cast<const ModMatrixComboBox*>(&box);
        const std::optional<juce::Colour> blockColour =
            (matrixBox != nullptr && matrixBox->blockId().has_value())
                ? std::optional<juce::Colour>{blockColourOf(_blockPalette, *matrixBox->blockId())}
                : std::nullopt;
        const bool blockHighlighted = matrixBox != nullptr && matrixBox->isHighlighted();

        // The block tint is composited into an OPAQUE fill rather than left as
        // an alpha to blend at paint time: the matrix sits on the vector panel
        // background, so a translucent combo would take its colour from
        // whatever row it happens to sit over, and its text contrast with it.
        // The design system specifies a fill over the CONTROL surface, not a
        // window into the panel. [ADR-JUC-028 (DEC-JUC-081), RQ-DSN-100]
        auto fill = blockColour.has_value()
                        ? box.findColour(juce::ComboBox::backgroundColourId)
                              .overlaidWith(blockColour->withAlpha(
                                  tokens::component::blockFillAlpha))
                        : box.findColour(juce::ComboBox::backgroundColourId);
        if (hovered)
        {
            fill = fill.brighter(tokens::semantic::hoverBrighten);
        }
        g.setColour(fill.withMultipliedAlpha(disabledMul));
        g.fillRoundedRectangle(bounds, corner);

        // Frame. A highlighted block combo thickens to the block-frame width AND
        // brightens: on a control already saturated with its block hue, neither
        // cue carries on its own — the width step is half a pixel, and a
        // brightness step alone reads as a rendering artefact. The background is
        // deliberately NOT touched, because it is what carries the block
        // identity. [RQ-GUI-052, ADR-JUC-028 (DEC-JUC-078)]
        const auto frameColour =
            blockColour.has_value()
                ? (blockHighlighted ? blockColour->brighter(tokens::semantic::hoverBrighten)
                                    : *blockColour)
                : box.findColour(juce::ComboBox::outlineColourId);
        const float frameWidth = (blockColour.has_value() && blockHighlighted)
                                     ? tokens::semantic::strokeDiagram
                                     : tokens::semantic::strokeBorder;
        // Inset by HALF THE STROKE, not by a fixed 0.5: a stroke is centred on
        // its path, so only that inset puts its outer edge exactly on the
        // component bounds. The former 0.5 was calibrated for a 1 px frame; at
        // the 1.5 px highlight width the outer edge fell 0.25 px outside the
        // component and JUCE clipped it — invisible along the straight edges,
        // but it sliced the corner arc off square and left a dark stair-step in
        // each corner. [RQ-GUI-052, ADR-JUC-028 (DEC-JUC-084)]
        const auto frameBounds = bounds.reduced(frameWidth * 0.5F);
        g.setColour(frameColour.withMultipliedAlpha(disabledMul));
        g.drawRoundedRectangle(frameBounds, corner, frameWidth);

        const auto arrowZone =
            juce::Rectangle<int>(width - ARROW_ZONE_X, 0, ARROW_ZONE_W, height).toFloat();
        juce::Path path;
        path.startNewSubPath(arrowZone.getX() + ARROW_INSET, arrowZone.getCentreY() - ARROW_RISE);
        path.lineTo(arrowZone.getCentreX(), arrowZone.getCentreY() + ARROW_DROP);
        path.lineTo(arrowZone.getRight() - ARROW_INSET, arrowZone.getCentreY() - ARROW_RISE);
        g.setColour(box.findColour(juce::ComboBox::arrowColourId)
                        .withMultipliedAlpha(ARROW_ENABLED_ALPHA * disabledMul));
        g.strokePath(path, juce::PathStrokeType(tokens::semantic::strokeLine));
    }

    juce::Font XplorerLookAndFeel::getComboBoxFont(juce::ComboBox& box)
    {
        // ONE fixed size for every combo box. Sizing each box against its own
        // widest label (the mechanism this replaces) put up to seven different
        // sizes on screen at once; deriving one size from the whole inventory
        // was tried next and collapsed to the legibility floor, shrinking the
        // whole panel. The size is therefore a design decision expressed as a
        // token — do NOT reintroduce a per-box (content-based) computation
        // here. [RQ-GUI-047, ADR-JUC-022 (DEC-JUC-046)]
        //
        // The typeface is attached EXPLICITLY. Registering it through
        // setDefaultSansSerifTypeface and letting this Font resolve to it
        // implicitly renders text as unrelated characters on the pinned JUCE
        // version (a systematic glyph-index offset, reproduced with two
        // independent valid font files). [RQ-DSN-096, ADR-JUC-022 (DEC-JUC-049)]
        //
        // Settings-style dialogs (MIDI/UI/Randomizer pages) are a second,
        // equally fixed context: their combos match the dialog's own row
        // labels instead of the main panel's condensed size — still exactly
        // ONE size per surface, chosen by context (main window vs. dialog),
        // never by the box's own content, so RQ-GUI-047's rule above still
        // holds. [RQ-GUI-061, ADR-JUC-033 (DEC-JUC-105)]
        if (box.findParentComponentOfClass<juce::DialogWindow>() != nullptr)
        {
            return juce::Font{juce::FontOptions{tokens::semantic::textTitle}};
        }
        return comboFont();
    }

    juce::Font XplorerLookAndFeel::comboFont() const
    {
        // Built FROM the typeface, not with a later withTypeface(): that setter
        // requires the name/style fields to still be empty, which the default
        // FontOptions does not guarantee, and asserts otherwise.
        if (_comboTypeface != nullptr)
        {
            return juce::Font{
                juce::FontOptions{_comboTypeface}.withHeight(tokens::semantic::comboTextSize)};
        }
        return juce::Font{juce::FontOptions{tokens::semantic::comboTextSize}};
    }

    void XplorerLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
    {
        // Same two tokens drawComboBox uses for the arrow, so text and arrow
        // cannot drift apart. They reclaim the 40 px LookAndFeel_V4 reserves
        // (30 px arrow zone + the Label's 5 px borders) on controls it sizes
        // for far taller combo boxes than the 21 px ones used here — which is
        // what lets the fixed size fit the reference widths unchanged.
        // [RQ-GUI-047, ADR-JUC-022 (DEC-JUC-047)]
        label.setBorderSize(juce::BorderSize<int>(0, tokens::semantic::comboLabelBorder, 0,
                                                  tokens::semantic::comboLabelBorder));
        label.setBounds(1, 1, box.getWidth() - tokens::semantic::comboArrowZone,
                        box.getHeight() - 2);
        label.setFont(getComboBoxFont(box));
    }

    namespace
    {
        // TextEditor::setInputRestrictions rejects each keystroke and pasted
        // character outside the allowed set as it happens, rather than
        // parsing whatever was typed afterwards -- so the field can never
        // show anything but digits, not even transiently. 2 characters caps
        // it at the model's own 0..99 range (XpanderTone::MAX_PROGRAM_NUMBER).
        // [RQ-GUI-058]
        class DigitsOnlyLabel final : public juce::Label
        {
        public:
            DigitsOnlyLabel() : juce::Label({}, {}) {}

            juce::TextEditor* createEditorComponent() override
            {
                auto* editor = juce::Label::createEditorComponent();
                editor->setInputRestrictions(2, "0123456789");
                return editor;
            }
        };
    }

    juce::Label* XplorerLookAndFeel::createSliderTextBox(juce::Slider& slider)
    {
        // Reproduces LookAndFeel_V2::createSliderTextBox's colours/justification
        // (the app inherits that implementation for every other Slider, since
        // this is the only one that needs a text box at all -- knobs and the
        // mod-matrix amount sliders are all NoTextBox) with a digits-only
        // Label swapped in. [RQ-GUI-058]
        auto* label = new DigitsOnlyLabel();
        label->setJustificationType(juce::Justification::centred);
        label->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
        label->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
        label->setColour(juce::Label::backgroundColourId, slider.findColour(juce::Slider::textBoxBackgroundColourId));
        label->setColour(juce::Label::outlineColourId, slider.findColour(juce::Slider::textBoxOutlineColourId));
        label->setColour(juce::TextEditor::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
        label->setColour(juce::TextEditor::backgroundColourId, slider.findColour(juce::Slider::textBoxBackgroundColourId));
        label->setColour(juce::TextEditor::outlineColourId, slider.findColour(juce::Slider::textBoxOutlineColourId));
        label->setColour(juce::TextEditor::highlightColourId, slider.findColour(juce::Slider::textBoxHighlightColourId));
        return label;
    }
}
