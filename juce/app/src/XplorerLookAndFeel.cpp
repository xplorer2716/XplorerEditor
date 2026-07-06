#include "XplorerLookAndFeel.hpp"

namespace xplorer::app
{
    XplorerLookAndFeel::XplorerLookAndFeel(juce::Colour ledColour)
        : _ledColour(ledColour)
    {
        setColour(juce::ComboBox::backgroundColourId, juce::Colour::fromRGB(30, 36, 44));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour::fromRGB(30, 36, 44));
    }

    void XplorerLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float startAngle, float endAngle,
                                              juce::Slider&)
    {
        const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0F);
        const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0F;
        const auto centre = bounds.getCentre();
        const auto angle = startAngle + sliderPos * (endAngle - startAngle);

        // Knob body.
        g.setColour(juce::Colour::fromRGB(24, 28, 34));
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0F, radius * 2.0F);

        // LED ring: filled arc from start to the current position.
        const auto ringRadius = radius - 1.0F;
        juce::Path ring;
        ring.addCentredArc(centre.x, centre.y, ringRadius, ringRadius, 0.0F, startAngle, angle, true);
        g.setColour(_ledColour);
        g.strokePath(ring, juce::PathStrokeType(2.4F));

        // Pointer.
        juce::Path pointer;
        const auto pointerLength = radius * 0.7F;
        pointer.addRectangle(-1.0F, -pointerLength, 2.0F, pointerLength);
        g.setColour(_ledColour.brighter(0.3F));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
    }

    void XplorerLookAndFeel::drawTickBox(juce::Graphics& g, juce::Component&, float x, float y, float w,
                                         float h, bool ticked, bool, bool, bool)
    {
        const auto box = juce::Rectangle<float>(x, y, w, h).reduced(1.0F);
        g.setColour(juce::Colour::fromRGB(24, 28, 34));
        g.fillRoundedRectangle(box, 2.0F);
        g.setColour(_ledColour.withAlpha(0.6F));
        g.drawRoundedRectangle(box, 2.0F, 1.0F);
        if (ticked)
        {
            g.setColour(_ledColour);
            g.fillRoundedRectangle(box.reduced(2.0F), 1.5F);
        }
    }
}
