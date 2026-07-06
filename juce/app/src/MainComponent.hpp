#pragma once

// Main window content: logical canvas scaled uniformly into the window, the
// reference background bitmap, and the bound controls placed from the
// extracted table. [RQ-GUI-001, RQ-GUI-005, RQ-GUI-007, ADR-006]

#include "BoundControls.hpp"
#include "DisplayPanel.hpp"
#include "JuceEventDispatcher.hpp"
#include "ModMatrixPanel.hpp"
#include "PageFamilyBlock.hpp"
#include "PianoWindow.hpp"
#include "XplorerLookAndFeel.hpp"

#include <map>

#include "xplorer/app/ParameterBindingRegistry.hpp"
#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/settings/SettingsService.hpp"
#include "xpl/midi/JuceMidiBackend.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>
#include <vector>

namespace xplorer::app
{
    class MainComponent final : public juce::Component, public juce::MenuBarModel
    {
    public:
        MainComponent();
        ~MainComponent() override;

        void paint(juce::Graphics& g) override;

        // MenuBarModel (File / Patch / Tools / Help). [RQ-GUI-008]
        juce::StringArray getMenuBarNames() override;
        juce::PopupMenu getMenuForIndex(int index, const juce::String& name) override;
        void menuItemSelected(int menuItemId, int topLevelMenuIndex) override;

    private:
        void placeFixedBlockControls();
        void createPageFamilyBlocks();
        void createShortcutButtonsAndDisplay();
        void onSynthPageChanged(const controller::PageChangeEvent& event);
        void flashMidiActivity();

        juce::Image _background;

        xpl::midi::JuceMidiBackend _backend;
        std::shared_ptr<JuceEventDispatcher> _dispatcher;
        std::unique_ptr<settings::XmlSettingsService> _settingsService;
        std::unique_ptr<controller::XpanderController> _controller;
        std::unique_ptr<ParameterBindingRegistry> _registry;
        std::vector<std::unique_ptr<juce::Component>> _controls;
        std::vector<std::unique_ptr<PageFamilyBlock>> _familyBlocks;
        std::unique_ptr<ModMatrixPanel> _matrixPanel;

        std::unique_ptr<XplorerLookAndFeel> _lookAndFeel;
        DisplayPanel _display;
        std::vector<std::unique_ptr<juce::Button>> _shortcutButtons;
        std::map<std::string, std::function<void()>> _shortcutActions;
        std::unique_ptr<juce::FileChooser> _fileChooser;
        std::unique_ptr<PianoWindow> _pianoWindow;

        // MIDI activity indicator (LedPanelControl replacement).
        class MidiActivityLed final : public juce::Component, private juce::Timer
        {
        public:
            void flash();
            void paint(juce::Graphics& g) override;

        private:
            void timerCallback() override;
            bool _lit = false;
        };
        MidiActivityLed _midiLed;
    };

    /// Resizable host: a menu bar strip on top, the uniformly-scaled canvas
    /// below. [RQ-GUI-008, RQ-GUI-005]
    class ScaledCanvasComponent final : public juce::Component
    {
    public:
        ScaledCanvasComponent();
        ~ScaledCanvasComponent() override;

        void resized() override;
        void paint(juce::Graphics& g) override;

    private:
        MainComponent _canvas;
        juce::MenuBarComponent _menuBar;
    };
}
