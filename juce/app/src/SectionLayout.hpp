#pragma once

// Canvas geometry that more than one file has to agree on: the vertical
// placement of the section separators and the references each section's rhythm
// is measured against, and the rule that decides where a control tick stops.
//
// This header exists so BackgroundRenderer (which DRAWS them), XplorerLookAndFeel
// (which draws the knob a tick runs into) and the tests (which ASSERT what they
// must satisfy) read the same numbers instead of each holding its own copy. A
// test that restated the anchors would only prove it could copy them; reading
// them from here means an edit to a separator's y is checked against the rhythm
// rule, which is the point.
// [RQ-CLR-001, RQ-CLR-010, RQ-GUI-071, ADR-CLR-001 (DEC-CLR-001-D), ADR-JUC-027 (DEC-JUC-112)]
//
// Two coordinate frames meet here, and mixing them is the easy mistake:
//   * REFERENCE frame — what BackgroundRenderer's draw calls use, inherited
//     from the .NET MainForm bitmap. Includes the 32 px menustrip band.
//   * CANVAS frame — what GeneratedControlTable.inc uses, and what
//     LOGICAL_CANVAS_HEIGHT measures. canvas = reference - CANVAS_TOP_CROP.
// Everything below is named for the frame it is in.

#include "DesignTokens.hpp"

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

    // --- Control ticks: where a tick STOPS ---------------------------------
    //
    // A tick is the short line dropping from a block's bottom edge onto the
    // control it feeds. It SHALL end on the OUTER edge of the first pixel that
    // control paints: touching it, never crossing into it, never stopping short
    // of it. [RQ-GUI-071, ADR-JUC-027 (DEC-JUC-112)]
    //
    // Before this rule the end was an authored LENGTH (12, 23 or 24 px) with no
    // relation to the control it aimed at, so it was out by -3 to +3 px
    // depending on the row — a visible gap under the two VCA VOLUME knobs and a
    // line crossing into the ring on the LAG, TRACK, LFO, ENV, VCO and RAMP
    // rows (owner report, 2026-08-15). Deriving the end from the target's own
    // geometry is what stops the two drifting apart again.

    /// The knob's own two insets, owned here rather than in
    /// XplorerLookAndFeel::drawRotarySlider, so the painter that ends a tick on
    /// the ring and the LookAndFeel that draws the ring cannot disagree.
    /// [RQ-GUI-031, ADR-JUC-009]
    inline constexpr float KNOB_BOUNDS_INSET = 2.0F; ///< anti-aliasing margin inside the control bounds
    inline constexpr float KNOB_RING_INSET = 1.0F;   ///< ring path radius, inside that margin

    /// Distance from a knob's control-bounds top to the outer edge of its ring,
    /// in canvas px.
    ///
    /// drawRotarySlider insets the bounds by KNOB_BOUNDS_INSET, takes
    /// `radius = min(w,h)/2` of what is left, and strokes the ring at
    /// `radius - KNOB_RING_INSET` with `strokeKnobRing` CENTRED on that path. The
    /// ring's top is therefore `y + inset + (r) - (r - ringInset)` — the radius
    /// cancels, so this is the same for a 25 px matrix knob as for a 32 px panel
    /// knob — and half the stroke sits outside it.
    inline constexpr float KNOB_RING_TOP_CANVAS_INSET =
        KNOB_BOUNDS_INSET + KNOB_RING_INSET - (tokens::semantic::strokeKnobRing / 2.0F);

    /// A combo box paints from its bounds' top edge: `drawComboBox` fills and
    /// strokes the full local bounds, and a Component clips to itself, so the
    /// outer half of its 1 px outline never reaches the canvas. Inset is zero —
    /// stated so the asymmetry with a knob is a decision on the page rather than
    /// an omission.
    inline constexpr float COMBO_TOP_CANVAS_INSET = 0.0F;

    /// Reference-frame y at which a tick must END, given the canvas y of the
    /// first pixel its target paints. The tick is stroked at `strokeDiagram`
    /// with a ROUNDED end cap (BackgroundRenderer's frameStroke), so its painted
    /// end reaches half a stroke past the endpoint: subtracting that half width
    /// is exactly what makes the cap land ON the target instead of over it.
    constexpr float tickEndReferenceY(float firstPaintedCanvasY) noexcept
    {
        return firstPaintedCanvasY + static_cast<float>(CANVAS_TOP_CROP)
               - (tokens::semantic::strokeDiagram / 2.0F);
    }

    /// tickEndReferenceY for a knob, from its control-table top edge.
    constexpr float knobTickEndReferenceY(int knobCanvasTopY) noexcept
    {
        return tickEndReferenceY(static_cast<float>(knobCanvasTopY) + KNOB_RING_TOP_CANVAS_INSET);
    }

    /// tickEndReferenceY for a combo box, from its control-table top edge.
    constexpr float comboTickEndReferenceY(int comboCanvasTopY) noexcept
    {
        return tickEndReferenceY(static_cast<float>(comboCanvasTopY) + COMBO_TOP_CANVAS_INSET);
    }

    /// The integer x a caption must be drawn at to sit on a control's centre.
    /// `juce::Graphics::drawSingleLineText` takes an int, so a control of ODD
    /// width (VCF_MODE is 127 wide) rounds to the nearer pixel instead of
    /// truncating half a pixel to the left.
    constexpr int captionCentreX(float centreX) noexcept
    {
        return static_cast<int>(centreX + 0.5F);
    }

    // --- The rows a tick aims at, CANVAS frame ------------------------------
    // Stated here rather than looked up, for the same reason SECTION_X_MATRIX
    // is: the painter must not scan the control table on every repaint. Every
    // value below is asserted against the real table by BackgroundRendererTests,
    // so a control that moves fails the build instead of quietly unpicking its
    // own tick. [RQ-GUI-071]

    inline constexpr int KNOB_ROW_TOP_CANVAS_Y = 67;      ///< VCO1 FREQ/DETUNE/PW/VOLUME + VCF FREQ/RES/VCA1/VCA2
    inline constexpr int KNOB_ROW_FM_CANVAS_Y = 229;      ///< FM AMPLITUDE
    inline constexpr int KNOB_ROW_VCO2_VCA_CANVAS_Y = 245; ///< VCO2 VOLUME (on the FM row, not the VCO2 row)
    inline constexpr int KNOB_ROW_ENV_CANVAS_Y = 251;     ///< DELAY..RELEASE + VOLUME
    inline constexpr int KNOB_ROW_VCO2_CANVAS_Y = 347;    ///< VCO2 FREQ/DETUNE/PW
    inline constexpr int KNOB_ROW_LFO_CANVAS_Y = 483;     ///< SPEED/RETRIG/AMPLITUDE
    inline constexpr int KNOB_ROW_LAG_CANVAS_Y = 532;     ///< LAG RATE
    inline constexpr int KNOB_ROW_RAMP_CANVAS_Y = 667;    ///< RAMP RATE
    inline constexpr int KNOB_ROW_TRACK_CANVAS_Y = 718;   ///< PT 1..5

    /// The two combo boxes a tick lands on, and the x they are CENTRED on.
    /// Both ticks (and both captions) used to sit on a hand-carried x that was
    /// 6.5 px and 4 px left of the combo they belong to — the same drift as the
    /// vertical one, in the other axis. [RQ-GUI-071]
    inline constexpr int COMBO_VCF_MODE_CANVAS_Y = 68;
    inline constexpr float COMBO_VCF_MODE_CENTRE_X = 675.5F; ///< VCF_MODE.x 612 + 127/2
    inline constexpr int COMBO_LFO_WAVE_CANVAS_Y = 486;
    inline constexpr float COMBO_LFO_WAVE_CENTRE_X = 661.0F; ///< LFO_X_WAVESHAPE.x 611 + 100/2
}
