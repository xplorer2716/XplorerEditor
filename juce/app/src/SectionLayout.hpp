#pragma once

// Vertical placement of the canvas section separators, and the references each
// section's rhythm is measured against.
//
// This header exists so BackgroundRenderer (which DRAWS the separators) and
// SectionRhythmTests (which ASSERTS the rhythm they must satisfy) read the same
// numbers instead of each holding its own copy. A test that restated the
// anchors would only prove it could copy them; reading them from here means an
// edit to a separator's y is checked against the rhythm rule, which is the
// point. [RQ-CLR-001, RQ-CLR-010, ADR-CLR-001 (DEC-CLR-001-D)]
//
// Two coordinate frames meet here, and mixing them is the easy mistake:
//   * REFERENCE frame — what BackgroundRenderer's draw calls use, inherited
//     from the .NET MainForm bitmap. Includes the 32 px menustrip band.
//   * CANVAS frame — what GeneratedControlTable.inc uses, and what
//     LOGICAL_CANVAS_HEIGHT measures. canvas = reference - CANVAS_TOP_CROP.
// Everything below is named for the frame it is in.

namespace xplorer::app::layout
{
    /// Reference-to-canvas shift: the cropped menustrip band less the 5 px
    /// black top margin. Mirrors BackgroundRenderer's local CANVAS_TOP_CROP and
    /// extract_control_table.py's constant of the same name. [ADR-JUC-013]
    inline constexpr int CANVAS_TOP_CROP = 27;

    /// What a `section(x, y, ...)` anchor means vertically: the label's baseline
    /// sits `int(tokens::component::sectionBarHeight)` = 4 px below the anchor,
    /// and the whole thing is then translated up by CANVAS_TOP_CROP. So
    ///     labelBaselineCanvasY = anchorReferenceY - 27 + 4 = anchorReferenceY - 23.
    /// The baseline — not the anchor, not the bar's top — is what the rhythm is
    /// measured to, because label and bar are bottom-aligned (DEC-JUC-107) and
    /// the baseline is the edge the eye reads. [RQ-CLR-001]
    inline constexpr int SECTION_BASELINE_BELOW_ANCHOR = -23;

    constexpr int sectionBaselineCanvasY(int anchorReferenceY) noexcept
    {
        return anchorReferenceY + SECTION_BASELINE_BELOW_ANCHOR;
    }

    // --- Separator left edges, one per column ------------------------------
    // A column's separators SHALL share one x and one width, so their two ends
    // line up down the column. They did not: the centre column was transcribed
    // from the mockup with VCF/VCA and ENV X at 526 and LFO X and RAMP X at 527,
    // and since all four take the same 370 px width, the odd pair was one px off
    // at BOTH ends. Visible as a stagger once the bars were re-placed and the eye
    // had a clean vertical to compare against (owner report, 2026-08-10).
    // Naming the edge per column is what stops it drifting again — a literal at
    // each call site is what let it drift in the first place. [RQ-CLR-006]

    inline constexpr int SECTION_X_LEFT = 53;    // VCO1/VCO2/FM, LAG, TRACK X
    inline constexpr int SECTION_X_CENTRE = 526; // VCF/VCA, ENV X, LFO X, RAMP X

    /// MOD MATRIX is the one section whose bar runs alongside a control grid, so
    /// both its ends are DEFINED by that grid rather than chosen: it starts on the
    /// left edge of the SOURCE combo column and stops on the right edge of the
    /// QUANTIZE tick-box column. DEC-JUC-110 already fixed the right end; the left
    /// end was a separate literal at 958 and sat two px outside the grid, which
    /// reads as the bar overhanging the matrix (owner report, 2026-08-10).
    ///
    /// The values are stated here rather than computed, because the painter must
    /// not do a control-table lookup on every repaint — but SectionRhythmTests
    /// asserts both against the real table, so a control-column move that leaves
    /// these behind fails the build instead of quietly misaligning the bar.
    /// [RQ-CLR-007, ADR-JUC-034 (DEC-JUC-110)]
    inline constexpr int SECTION_X_MATRIX = 960;          // == MOD_SRC_n.x
    inline constexpr int SECTION_MATRIX_BAR_WIDTH = 258;  // ends at MOD_QUANTIZE_n right edge (1218)

    // --- Separator anchors, REFERENCE frame --------------------------------
    // Left and centre columns are placed by RQ-CLR-001: each anchor is derived
    // from the bottom of the section ABOVE it, never chosen. The three bottom
    // anchors are pinned to one shared baseline by RQ-CLR-003 and must not move.

    inline constexpr int SECTION_VCO_Y = 465;    // VCO1/VCO2/FM  [TASK-CLR-001]
    inline constexpr int SECTION_LAG_Y = 615;    // LAG           [TASK-CLR-001]
    inline constexpr int SECTION_TRACK_Y = 799;  // TRACK X       pinned, RQ-CLR-003
    inline constexpr int SECTION_VCF_Y = 182;    // VCF/VCA       [TASK-CLR-002]
    inline constexpr int SECTION_ENV_Y = 415;    // ENV X         [TASK-CLR-002]
    inline constexpr int SECTION_LFO_Y = 599;    // LFO X         [TASK-CLR-002]
    inline constexpr int SECTION_RAMP_Y = 799;   // RAMP X        pinned, RQ-CLR-003
    inline constexpr int SECTION_MATRIX_Y = 799; // MOD MATRIX    pinned, RQ-CLR-003

    /// The shared baseline the three bottom separators shall keep. [RQ-CLR-003]
    inline constexpr int BOTTOM_SECTION_BASELINE_CANVAS_Y = sectionBaselineCanvasY(SECTION_TRACK_Y);

    // --- Section bottoms drawn by the painter, CANVAS frame ----------------
    // Most sections end on a control, which the test reads straight out of the
    // control table. These three end on something BackgroundRenderer draws, so
    // the painter and the test have to agree on them explicitly.

    /// LAG's frame top edge: box(81, .., 268, 36) — the first visible element of
    /// the LAG section, and the only section start that is painted rather than
    /// being a control. Every other section starts on its radio-button row,
    /// which the rhythm test reads straight from the control table. [RQ-CLR-002]
    inline constexpr int LAG_FRAME_TOP_CANVAS_Y = 487;

    /// LAG's "RATE" caption baseline. Sits BELOW the EXPO/LEGATO row, so it —
    /// not a control — is what LAG's separator is measured from. [RQ-CLR-001]
    inline constexpr int LAG_RATE_CAPTION_BASELINE_CANVAS_Y = 576;

    /// TRACK's "PT n" caption baselines, likewise below the PT knobs.
    inline constexpr int TRACK_PT_CAPTION_BASELINE_CANVAS_Y = 760;

    /// RAMP's TRIGGER frame, which closes the RAMP section: lower than any
    /// RAMP_X_TRIG_* control inside it. The block's "TRIGGER IN" label reaches
    /// two px lower still (baseline 750 before TASK-CLR-002 moved the group) but
    /// sits at x=508, in the gutter left of a bar that starts at x=527, so
    /// RQ-CLR-001 excludes it — the rhythm is scoped to the bar's own span.
    inline constexpr int RAMP_TRIGGER_FRAME_TOP_CANVAS_Y = 719;
    inline constexpr int RAMP_TRIGGER_FRAME_HEIGHT = 41;
    inline constexpr int RAMP_TRIGGER_FRAME_BOTTOM_CANVAS_Y =
        RAMP_TRIGGER_FRAME_TOP_CANVAS_Y + RAMP_TRIGGER_FRAME_HEIGHT;
}
