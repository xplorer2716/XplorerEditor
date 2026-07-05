#pragma once

// Main window content: logical canvas scaled uniformly into the window,
// reference background bitmap behind the (future) controls. [RQ-GUI-005, RQ-GUI-007, ADR-006]

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class MainComponent final : public juce::Component
    {
    public:
        MainComponent();

        void paint(juce::Graphics& g) override;

    private:
        juce::Image _background;
    };

    /// Resizable host applying the uniform scale transform to the canvas.
    class ScaledCanvasComponent final : public juce::Component
    {
    public:
        ScaledCanvasComponent();

        void resized() override;
        void paint(juce::Graphics& g) override;

    private:
        MainComponent _canvas;
    };
}
