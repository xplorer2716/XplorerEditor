#include "XplorerLookAndFeel.hpp"

#include "DesignTokens.hpp"

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
                                         float h, bool ticked, bool, bool, bool)
    {
        const auto box = juce::Rectangle<float>(x, y, w, h).reduced(1.0F);
        g.setColour(tokens::semantic::surfaceBase);
        g.fillRoundedRectangle(box, tokens::semantic::radiusControl);
        g.setColour(_ledColour.withAlpha(tokens::component::tickBoxBorderAlpha));
        g.drawRoundedRectangle(box, tokens::semantic::radiusControl, tokens::semantic::strokeBorder);
        if (ticked)
        {
            g.setColour(_ledColour);
            g.fillRoundedRectangle(box.reduced(2.0F), tokens::semantic::radiusControlInner);
        }
    }

    void XplorerLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                              bool, bool)
    {
        // A small square tick box on the left, then the caption in a compact
        // font sized to the control height so short captions (e.g. "TRI") do
        // not get ellipsized in the tight reference bounds. [RQ-GUI-032]
        const auto bounds = button.getLocalBounds();
        const auto boxSize = juce::jmin(14, bounds.getHeight());
        const auto box = juce::Rectangle<float>(0.0F, (bounds.getHeight() - boxSize) * 0.5F,
                                                static_cast<float>(boxSize), static_cast<float>(boxSize));
        drawTickBox(g, button, box.getX(), box.getY(), box.getWidth(), box.getHeight(),
                    button.getToggleState(), button.isEnabled(), false, false);

        if (button.getButtonText().isNotEmpty())
        {
            g.setColour(button.findColour(juce::ToggleButton::textColourId));
            g.setFont(juce::Font(juce::jmin(tokens::semantic::textCaption,
                                            static_cast<float>(bounds.getHeight()) - 3.0F)));
            const auto textArea = bounds.withTrimmedLeft(boxSize + 2);
            g.drawText(button.getButtonText(), textArea, juce::Justification::centredLeft, false);
        }
    }

    juce::Font XplorerLookAndFeel::getComboBoxFont(juce::ComboBox& box)
    {
        // Base size mirrors the stock LookAndFeel_V4::getComboBoxFont; shrunk
        // (down to a legibility floor) so the widest item in THIS box's list
        // fits the text area LookAndFeel_V4::positionComboBoxText lays out
        // (box width minus its 30px arrow zone, minus the Label's default
        // 5px left/right border). A per-box, not per-selection, size keeps it
        // stable as the user changes the selection.
        // Font-size bounds come from the shared type scale (RQ-DSN-011); the
        // arrow/label geometry stays a local layout constant (spacing scale
        // deferred, RQ-DSN-020).
        constexpr float BASE_SIZE = tokens::semantic::textDisplay;
        constexpr float MIN_SIZE = tokens::semantic::textDense;
        constexpr int ARROW_ZONE = 30;
        constexpr int LABEL_MARGIN = 10;

        float size = juce::jmin(BASE_SIZE, static_cast<float>(box.getHeight()) * 0.85F);
        const float availableWidth =
            static_cast<float>(box.getWidth() - ARROW_ZONE - LABEL_MARGIN);
        if (availableWidth <= 0.0F)
        {
            return juce::Font{juce::FontOptions{size}};
        }

        const juce::Font probe{juce::FontOptions{size}};
        float widest = 0.0F;
        for (int i = 0; i < box.getNumItems(); ++i)
        {
            widest = juce::jmax(widest, probe.getStringWidthFloat(box.getItemText(i)));
        }
        if (widest > availableWidth)
        {
            size = juce::jmax(MIN_SIZE, size * (availableWidth / widest));
        }
        return juce::Font{juce::FontOptions{size}};
    }
}
