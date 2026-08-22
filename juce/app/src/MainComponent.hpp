#pragma once

// Main window content: logical canvas scaled uniformly into the window, the
// vector-drawn background (BackgroundRenderer), and the bound controls placed
// from the extracted table. [RQ-GUI-001, RQ-GUI-005, RQ-GUI-037, ADR-JUC-006, ADR-JUC-013]

#include "BoundControls.hpp"
#include "DesignTokens.hpp"
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
#include <cmath>
#include <memory>
#include <vector>

namespace xplorer::app
{
    /// Resizes `window` to a View-menu scale and re-centres it. One
    /// implementation, two callers (the launch size in Main.cpp and the menu
    /// handler here), so the size and the centring are stated once.
    /// [RQ-SCL-001, RQ-SCL-002, ADR-JUC-025 (DEC-JUC-063, DEC-JUC-064)]
    void applyWindowScale(juce::ResizableWindow& window, float scale);

    class MainComponent final : public juce::Component, public juce::MenuBarModel
    {
    public:
        MainComponent();
        ~MainComponent() override;

        void paint(juce::Graphics& g) override;

        // MenuBarModel (File / Patch / View / Tools / Help). [RQ-GUI-008,
        // RQ-SCL-002, RQ-SCL-003]
        juce::StringArray getMenuBarNames() override;
        juce::PopupMenu getMenuForIndex(int index, const juce::String& name) override;
        void menuItemSelected(int menuItemId, int topLevelMenuIndex) override;

        /// The reference's menu shortcuts, dispatched to the very same
        /// `menuItemSelected` a click goes through, so a key and a click can
        /// never diverge in what they do. Returns false for anything not in
        /// the table, leaving child components (e.g. PageSelectorButton's own
        /// Ctrl+C/Ctrl+V, RQ-GUI-027) to handle their own keys.
        /// [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-099)]
        bool keyPressed(const juce::KeyPress& key) override;

        /// Double-clicking the VFD opens the rename dialog, the same one the
        /// Patch > Rename menu item opens. DisplayPanel passes clicks through
        /// (setInterceptsMouseClicks(false, false)), so the double click lands
        /// here rather than on the display itself. [RQ-GUI-025]
        void mouseDoubleClick(const juce::MouseEvent& event) override;

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
        /// The window's current content width, or 0 when not yet parented —
        /// what DEC-JUC-066 ticks the View menu against.
        [[nodiscard]] int currentWindowWidth() const;
        /// The top-level window, or nullptr before the component is parented.
        [[nodiscard]] juce::ResizableWindow* topLevelWindow() const;
        void updateLedColour(int argb);
        void updateBlockPalette(const BlockPalette& palette); // [RQ-DSN-095, ADR-JUC-020]
        void backupAllData();
        void restoreAllData();
        void getAllSinglePatchesFromSynth();
        void onControlHovered(juce::Component* component); // matrix highlight [RQ-GUI-018]
        /// Shared by the Patch > Rename menu item and the VFD double-click so
        /// the two triggers can never diverge in what they do. [RQ-GUI-025]
        void showRenameDialogForCurrentTone();
        /// Shared by the shortcut button and the Patch menu item, following the
        /// rename idiom above: one implementation per action, two triggers.
        /// [RQ-QLT-005]
        void showGotoPatchDialog();
        void showStorePatchDialog();

        // Forwards knob/selector hover to the matrix highlight. A dedicated
        // MouseListener (not MainComponent itself) avoids clashing with the
        // component's own mouseEnter/Exit. [RQ-GUI-018, ADR-JUC-010]
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

        // MIDI traffic LED panel (LedPanelControl port): three round lamps —
        // automation-in green, synth-in blue, synth-out red — each holding
        // ~100 ms past the last event of its source, retriggered by traffic.
        // The 30 ms decay timer only runs while a LED is lit. A lit lamp shows
        // a radial glow beneath its body; the extracted panel is only 8 px
        // tall and has no room for one, so this component's bounds are the
        // panel expanded by ledGlowMarginPx() on every side (the one place in
        // the app where a component's bounds differ from its control-table
        // spec) and paint() re-derives the un-inflated panel area from its own
        // bounds so no lamp moves when the margin changes.
        // [RQ-GUI-022, RQ-GUI-056, ADR-JUC-008, ADR-JUC-031 (DEC-JUC-095,
        // DEC-JUC-097)]
        class LedPanelComponent final : public juce::Component, private juce::Timer
        {
        public:
            LedPanelComponent();
            void flash(controller::EnumMidiDevice device);
            void paint(juce::Graphics& g) override;

        private:
            static constexpr int LED_COUNT = 3;
            /// Lamp order, left to right, as the reference LedPanelControl has
            /// it. flash()'s device mapping and paint()'s colour array are both
            /// indexed by these, which is what keeps the two orderings aligned.
            /// [RQ-GUI-022, RQ-GUI-056, RQ-QLT-007]
            static constexpr std::size_t LED_INDEX_AUTOMATION_IN = 0;
            static constexpr std::size_t LED_INDEX_SYNTH_IN = 1;
            static constexpr std::size_t LED_INDEX_SYNTH_OUT = 2;
            static constexpr int HOLD_MILLISECONDS = tokens::semantic::indicatorHoldMs;
            static constexpr int TICK_MILLISECONDS = 30;

            void timerCallback() override;

            std::array<juce::int64, LED_COUNT> _litUntil{}; // 0 = dark
        };
        LedPanelComponent _midiLed;
    };

    /// How far LedPanelComponent's bounds must extend past its extracted
    /// _ledPanelControl rect on every side so a lit lamp's glow is never
    /// clipped by its own component bounds -- the full glow radius, which is
    /// also the largest distance any lamp centre is from that rect's edge in
    /// practice, so this stays generous rather than tightly fitted per lamp.
    /// [RQ-GUI-056, ADR-JUC-031 (DEC-JUC-097)]
    [[nodiscard]] inline int ledGlowMarginPx() noexcept
    {
        const float radius = static_cast<float>(tokens::component::indicatorSize) * 0.5F
                            * tokens::component::indicatorGlowRadius;
        return static_cast<int>(std::ceil(radius));
    }

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

        /// Menu shortcuts must work whatever has focus, including nothing.
        /// JUCE routes a key to the focused component and bubbles it UP the
        /// parent chain, so with no focus it stops at this content component
        /// and never reaches `_canvas` below it — hence this forward.
        /// [RQ-GUI-008, ADR-JUC-032 (DEC-JUC-099)]
        bool keyPressed(const juce::KeyPress& key) override;

        bool isInterestedInFileDrag(const juce::StringArray& files) override;
        void filesDropped(const juce::StringArray& files, int x, int y) override;

    private:
        MainComponent _canvas;
        juce::MenuBarComponent _menuBar;
    };
}
