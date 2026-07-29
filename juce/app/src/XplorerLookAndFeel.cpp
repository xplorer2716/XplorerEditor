#include "XplorerLookAndFeel.hpp"

#include "BinaryData.h"
#include "DesignTokens.hpp"

#include "xplorer/app/ComboBoxSizing.hpp"

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
                { return font.getStringWidthFloat(juce::String(std::string(text))); });
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

    void XplorerLookAndFeel::drawTickBox(juce::Graphics& g, juce::Component& component, float x, float y, float w,
                                         float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted,
                                         bool)
    {
        // Hover brightens the existing border/fill; Disabled mutes the whole
        // box; Focus adds an accent-ring outline on top of either -- the three
        // states compose rather than replace each other. [RQ-GUI-041..043, ADR-JUC-017]
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

        if (isEnabled && component.hasKeyboardFocus(true))
        {
            g.setColour(_ledColour);
            g.drawRoundedRectangle(box.expanded(1.0F), tokens::semantic::radiusControl,
                                   tokens::semantic::strokeLine);
        }
    }

    void XplorerLookAndFeel::drawRadioBox(juce::Graphics& g, juce::Component& component, float x, float y, float w,
                                          float h, bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted)
    {
        // Circular sibling of drawTickBox; same token palette, AA insets and
        // hover/disabled/focus behaviour so radios and check boxes stay
        // visually consistent. [RQ-GUI-038, ADR-JUC-016, RQ-GUI-041..043, ADR-JUC-017]
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

        if (isEnabled && component.hasKeyboardFocus(true))
        {
            g.setColour(_ledColour);
            g.drawEllipse(box.expanded(1.0F), tokens::semantic::strokeLine);
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
            g.setFont(juce::Font(juce::jmin(tokens::semantic::textCaption,
                                            static_cast<float>(bounds.getHeight()) - 3.0F)));
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
        // three interaction states: hover brightens the recessed fill, disabled
        // mutes the whole control at disabledAlpha, keyboard focus adds an accent
        // ring. The arrow zone is the design-system value that
        // positionComboBoxText also reads, so text and arrow stay consistent;
        // it is narrower than LookAndFeel_V4's 30 px, which is sized for far
        // taller combo boxes. [RQ-GUI-041..043, ADR-JUC-017,
        // RQ-GUI-047, ADR-JUC-022 (DEC-JUC-047)]
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

        auto fill = box.findColour(juce::ComboBox::backgroundColourId);
        if (hovered)
        {
            fill = fill.brighter(tokens::semantic::hoverBrighten);
        }
        g.setColour(fill.withMultipliedAlpha(disabledMul));
        g.fillRoundedRectangle(bounds, corner);

        g.setColour(box.findColour(juce::ComboBox::outlineColourId).withMultipliedAlpha(disabledMul));
        g.drawRoundedRectangle(bounds.reduced(0.5F), corner, tokens::semantic::strokeBorder);

        const auto arrowZone =
            juce::Rectangle<int>(width - ARROW_ZONE_X, 0, ARROW_ZONE_W, height).toFloat();
        juce::Path path;
        path.startNewSubPath(arrowZone.getX() + ARROW_INSET, arrowZone.getCentreY() - ARROW_RISE);
        path.lineTo(arrowZone.getCentreX(), arrowZone.getCentreY() + ARROW_DROP);
        path.lineTo(arrowZone.getRight() - ARROW_INSET, arrowZone.getCentreY() - ARROW_RISE);
        g.setColour(box.findColour(juce::ComboBox::arrowColourId)
                        .withMultipliedAlpha(ARROW_ENABLED_ALPHA * disabledMul));
        g.strokePath(path, juce::PathStrokeType(tokens::semantic::strokeLine));

        if (enabled && box.hasKeyboardFocus(true))
        {
            g.setColour(_ledColour);
            g.drawRoundedRectangle(bounds.reduced(0.5F), corner, tokens::semantic::strokeLine);
        }
    }

    juce::Font XplorerLookAndFeel::getComboBoxFont(juce::ComboBox&)
    {
        // ONE fixed size for every combo box. Sizing each box against its own
        // widest label (the mechanism this replaces) put up to seven different
        // sizes on screen at once; deriving one size from the whole inventory
        // was tried next and collapsed to the legibility floor, shrinking the
        // whole panel. The size is therefore a design decision expressed as a
        // token, and the `juce::ComboBox&` parameter is intentionally unused —
        // do NOT reintroduce a per-box computation here.
        // [RQ-GUI-047, ADR-JUC-022 (DEC-JUC-046)]
        //
        // The typeface is attached EXPLICITLY. Registering it through
        // setDefaultSansSerifTypeface and letting this Font resolve to it
        // implicitly renders text as unrelated characters on the pinned JUCE
        // version (a systematic glyph-index offset, reproduced with two
        // independent valid font files). [RQ-DSN-096, ADR-JUC-022 (DEC-JUC-049)]
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
}
