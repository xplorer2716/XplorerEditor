#pragma once

// Main window content: logical canvas scaled uniformly into the window, the
// reference background bitmap, and the bound controls placed from the
// extracted table. [RQ-GUI-001, RQ-GUI-005, RQ-GUI-007, ADR-006]

#include "BoundControls.hpp"
#include "JuceEventDispatcher.hpp"
#include "ModMatrixPanel.hpp"
#include "PageFamilyBlock.hpp"

#include "xplorer/app/ParameterBindingRegistry.hpp"
#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/settings/SettingsService.hpp"
#include "xpl/midi/JuceMidiBackend.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>
#include <vector>

namespace xplorer::app
{
    class MainComponent final : public juce::Component
    {
    public:
        MainComponent();
        ~MainComponent() override;

        void paint(juce::Graphics& g) override;

    private:
        void placeFixedBlockControls();
        void createPageFamilyBlocks();
        void onSynthPageChanged(const controller::PageChangeEvent& event);

        juce::Image _background;

        xpl::midi::JuceMidiBackend _backend;
        std::shared_ptr<JuceEventDispatcher> _dispatcher;
        std::unique_ptr<settings::XmlSettingsService> _settingsService;
        std::unique_ptr<controller::XpanderController> _controller;
        std::unique_ptr<ParameterBindingRegistry> _registry;
        std::vector<std::unique_ptr<juce::Component>> _controls;
        std::vector<std::unique_ptr<PageFamilyBlock>> _familyBlocks;
        std::unique_ptr<ModMatrixPanel> _matrixPanel;
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
