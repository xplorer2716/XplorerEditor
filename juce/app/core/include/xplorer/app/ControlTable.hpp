#pragma once

// Declarative description of the main-window controls, mechanically
// extracted from the reference MainForm (geometry from MainForm.resx,
// types and tags from MainForm.Designer.cs). Coordinates are in the
// logical canvas space (the reference client area). [RQ-GUI-001, ADR-JUC-006]

#include <array>
#include <span>

namespace xplorer::app
{
    inline constexpr int LOGICAL_CANVAS_WIDTH = 1260;
    /// Reference client height (813) minus the cropped menustrip band. The
    /// WinForms form reserved 32 px at the top for its menustrip; the JUCE
    /// menu bar lives outside the canvas, so 27 px are cropped (the band minus
    /// a 5 px black top margin kept for cosmetics) — extract_control_table.py
    /// CANVAS_TOP_CROP shifts every control up by the same amount. A matching
    /// 5 px black margin sits at the bottom. [ADR-JUC-013]
    inline constexpr int LOGICAL_CANVAS_HEIGHT = 786;

    // --- Window geometry [RQ-SCL-001, RQ-SCL-002, ADR-JUC-025] --------------
    //
    // The canvas constants above are a COORDINATE GRID: the space the control
    // table is expressed in. The two below are a WINDOW SIZE. They are not the
    // same quantity and no ratio between them is computed anywhere — the only
    // place the two systems meet is ScaledCanvasComponent's render transform
    // (RQ-GUI-005). [DEC-JUC-068]

    /// The in-window menu-bar strip. The port renders File/Patch/View/Tools/
    /// Help itself rather than using a native OS menu, so the strip lives
    /// inside the content area and every window height carries it.
    inline constexpr int MENU_BAR_HEIGHT = 24;

    /// The application's 1x display scale, as a content width in logical px.
    /// Owner-chosen (2026-08-02): the .NET reference was measured launching at
    /// ~1473 px because WinForms DPI-autoscaled it, so its declared
    /// ClientSize width of 1260 was never what the user actually saw; 1440
    /// reads as that familiar size while clearing a 1920x1080 display.
    inline constexpr int WINDOW_WIDTH_AT_1X = 1440;

    /// Main-window content size, in logical pixels.
    struct WindowSize
    {
        int width;
        int height;
    };

    /// The window content size for a View-menu scale. Width is a plain
    /// multiple of WINDOW_WIDTH_AT_1X; height is whatever preserves the canvas
    /// aspect ratio, plus the menu bar. Sole authority for "what size is scale
    /// N" — the launch size and every menu preset both come through here.
    /// [DEC-JUC-063]
    [[nodiscard]] WindowSize windowSizeForScale(float scale);

    /// The View-menu presets, in menu order. [RQ-SCL-002]
    inline constexpr std::array<float, 5> WINDOW_SCALE_PRESETS{1.0F, 1.25F, 1.5F, 1.75F, 2.0F};

    /// Reference control types (WinForms / MidiApp.UIControls vocabulary,
    /// kept verbatim so the table stays diffable against the reference).
    enum class ControlKind
    {
        KnobControl,
        ComboBoxValuedControl,
        CheckBoxValuedControl,
        RadioButton,
        RadioButtonPanel,
        BackgroundImageButton,
        BackgroundImageRadioButton,
        LedPanelControl,
        VacuumFluoDisplayControl,
        FakePanel,
        Label,
    };

    struct ControlSpec
    {
        const char* id;   ///< WinForms field name (unique)
        ControlKind kind;
        int x, y, width, height; ///< logical canvas bounds
        const char* tag;  ///< parameter name / binding tag ("" when none)
        const char* label; ///< caption (checkbox/label .Text from the resx; "" when none)
    };

    /// All positioned main-window controls, in extraction order.
    [[nodiscard]] std::span<const ControlSpec> controlTable();
}
