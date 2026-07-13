#pragma once

// Main window content: logical canvas scaled uniformly into the window, the
// reference background bitmap, and the bound controls placed from the
// extracted table. [RQ-GUI-001, RQ-GUI-005, RQ-GUI-007, ADR-006]

#include "BoundControls.hpp"
#include "DisplayPanel.hpp"
#include "VfdDisplayHelper.hpp"
#include "JuceEventDispatcher.hpp"
#include "ModMatrixPanel.hpp"
#include "PageFamilyBlock.hpp"
#include "PianoWindow.hpp"
#include "ProgressWindow.hpp"
#include "XplorerLookAndFeel.hpp"

#include <map>

#include "xplorer/app/ParameterBindingRegistry.hpp"
#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/settings/SettingsService.hpp"
#include "xpl/midi/JuceMidiBackend.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <array>
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

        /// Loads a .syx file, dispatching by detected type (single tone ->
        /// load & transmit; all-data dump -> confirm then restore with
        /// progress; unknown -> warning). Shared by the File menu and the
        /// drag & drop target. Port of FileOperationsManager.LoadSysexFileByType.
        /// [RQ-MOD-043, RQ-CTL-001]
        void loadSysexFileByType(const juce::String& filePath);

    private:
        void placeFixedBlockControls();
        void placeStaticLabels();
        void createPageFamilyBlocks();
        void createShortcutButtonsAndDisplay();
        void onSynthPageChanged(const controller::PageChangeEvent& event);
        void onAllDataDumpProgression(const controller::AllDataDumpProgressionEvent& event);
        void openSettingsDialog();
        void updateLedColour(int argb);
        void backupAllData();
        void restoreAllData();
        void getAllSinglePatchesFromSynth();
        void onControlHovered(juce::Component* component); // matrix highlight [RQ-GUI-018]

        juce::Image _background;

        // Forwards knob/selector hover to the matrix highlight. A dedicated
        // MouseListener (not MainComponent itself) avoids clashing with the
        // component's own mouseEnter/Exit. [RQ-GUI-018, ADR-010]
        struct HoverHighlighter final : juce::MouseListener
        {
            std::function<void(juce::Component*)> onEnter;
            std::function<void()> onExit;
            void mouseEnter(const juce::MouseEvent& e) override
            {
                if (onEnter)
                {
                    onEnter(e.eventComponent);
                }
            }
            void mouseExit(const juce::MouseEvent&) override
            {
                if (onExit)
                {
                    onExit();
                }
            }
        };
        HoverHighlighter _hover;
        std::map<juce::Component*, std::string> _selectorSourceId;

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
        std::unique_ptr<VfdDisplayHelper> _vfd;
        std::vector<std::unique_ptr<juce::Button>> _shortcutButtons;
        std::map<std::string, std::function<void()>> _shortcutActions;
        std::unique_ptr<juce::FileChooser> _fileChooser;
        std::unique_ptr<PianoWindow> _pianoWindow;
        std::unique_ptr<ProgressWindow> _progressWindow;
        // Reception mode of the in-flight all-data-dump request, so the
        // progress window shows the right range/labels (event has no mode).
        bool _allDataDumpModeIsAll = false;

        // MIDI traffic LED panel (LedPanelControl port): three 5 px square
        // LEDs — automation-in green, synth-in blue, synth-out red — each
        // holding ~100 ms past the last event of its source, retriggered by
        // traffic. The 30 ms decay timer only runs while a LED is lit.
        // [RQ-GUI-022, ADR-008]
        class LedPanelComponent final : public juce::Component, private juce::Timer
        {
        public:
            LedPanelComponent();
            void flash(controller::EnumMidiDevice device);
            void paint(juce::Graphics& g) override;

        private:
            static constexpr int LED_COUNT = 3;
            static constexpr int LED_SIZE = 5;         // reference LedSize
            static constexpr int HOLD_MILLISECONDS = 100;
            static constexpr int TICK_MILLISECONDS = 30;

            void timerCallback() override;

            std::array<juce::int64, LED_COUNT> _litUntil{}; // 0 = dark
        };
        LedPanelComponent _midiLed;
    };

    /// Resizable host: a menu bar strip on top, the uniformly-scaled canvas
    /// below. Also the window-wide drop target for .syx files (reference
    /// MainForm AllowDrop). [RQ-GUI-008, RQ-GUI-005]
    class ScaledCanvasComponent final : public juce::Component,
                                        public juce::FileDragAndDropTarget
    {
    public:
        ScaledCanvasComponent();
        ~ScaledCanvasComponent() override;

        void resized() override;
        void paint(juce::Graphics& g) override;

        bool isInterestedInFileDrag(const juce::StringArray& files) override;
        void filesDropped(const juce::StringArray& files, int x, int y) override;

    private:
        MainComponent _canvas;
        juce::MenuBarComponent _menuBar;
    };
}
