#pragma once

// Declarative description of the main-window controls, mechanically
// extracted from the reference MainForm (geometry from MainForm.resx,
// types and tags from MainForm.Designer.cs). Coordinates are in the
// logical canvas space (the reference client area). [RQ-GUI-001, ADR-JUC-006]

#include <span>

namespace xplorer::app
{
    inline constexpr int LOGICAL_CANVAS_WIDTH = 1260;
    /// Reference client height (813) minus the 32 px top band the WinForms
    /// form reserved for its menustrip — the JUCE menu bar lives outside the
    /// canvas, so the band is cropped (extract_control_table.py
    /// CANVAS_TOP_CROP shifts every control up accordingly). [ADR-JUC-013]
    inline constexpr int LOGICAL_CANVAS_HEIGHT = 781;

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
