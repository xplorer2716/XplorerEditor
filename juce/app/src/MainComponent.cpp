#include "MainComponent.hpp"

#include "BinaryData.h"
#include "xplorer/app/ControlTable.hpp"

namespace xplorer::app
{
    MainComponent::MainComponent()
    {
        _background = juce::ImageCache::getFromMemory(BinaryData::mainbackground_jpg,
                                                      BinaryData::mainbackground_jpgSize);
        setSize(LOGICAL_CANVAS_WIDTH, LOGICAL_CANVAS_HEIGHT);
        // Controls are added from the extracted table in TASK-JUCE-063+.
    }

    void MainComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
        if (_background.isValid())
        {
            g.drawImage(_background, getLocalBounds().toFloat()); // [RQ-GUI-007]
        }
    }

    ScaledCanvasComponent::ScaledCanvasComponent()
    {
        addAndMakeVisible(_canvas);
        setSize(LOGICAL_CANVAS_WIDTH, LOGICAL_CANVAS_HEIGHT);
    }

    void ScaledCanvasComponent::resized()
    {
        // Uniform scale, aspect ratio preserved, canvas centered. [RQ-GUI-005]
        const auto scale = juce::jmin(static_cast<float>(getWidth()) / LOGICAL_CANVAS_WIDTH,
                                      static_cast<float>(getHeight()) / LOGICAL_CANVAS_HEIGHT);
        const auto scaledWidth = LOGICAL_CANVAS_WIDTH * scale;
        const auto scaledHeight = LOGICAL_CANVAS_HEIGHT * scale;
        _canvas.setTransform(juce::AffineTransform::scale(scale).translated(
            (static_cast<float>(getWidth()) - scaledWidth) * 0.5F,
            (static_cast<float>(getHeight()) - scaledHeight) * 0.5F));
    }

    void ScaledCanvasComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black); // letterbox bars when aspect differs
    }
}
