#include "XplorerLookAndFeel.hpp"

namespace xplorer::app
{
    XplorerLookAndFeel::XplorerLookAndFeel(juce::Colour ledColour)
        : _ledColour(ledColour)
    {
        setColour(juce::ComboBox::backgroundColourId, juce::Colour::fromRGB(30, 36, 44));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour::fromRGB(30, 36, 44));
        setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        setColour(juce::Label::textColourId, juce::Colours::white);
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
            g.setFont(juce::Font(juce::jmin(12.0F, static_cast<float>(bounds.getHeight()) - 3.0F)));
            const auto textArea = bounds.withTrimmedLeft(boxSize + 2);
            g.drawText(button.getButtonText(), textArea, juce::Justification::centredLeft, false);
        }
    }
}
