#pragma once

// Single source of truth for the FM modulation-bus branch geometry, in logical
// canvas coordinates. Shared by the static background painter (BackgroundRenderer,
// which draws both branches in the neutral diagram colour) and the active-path
// overlay (FmSignalPathOverlay, which re-strokes the selected branch in the LED
// accent), so the highlighted route can never drift from the drawn route.
// [RQ-GUI-037, RQ-GUI-039, ADR-JUC-016 (DEC-JUC-019)]

namespace xplorer::app
{
    /// A straight segment of a signal-path polyline, in logical canvas px.
    struct FmSeg
    {
        float x1, y1, x2, y2;
    };

    // FM DESTINATION selector -> VCO1 FREQUENCY stub (destination value 0).
    inline constexpr FmSeg FM_BRANCH_VCO1[] = {
        {276.0F, 220.0F, 284.0F, 220.0F},
        {284.0F, 220.0F, 284.0F, 180.0F},
        {40.0F, 180.0F, 284.0F, 180.0F},
        {40.0F, 88.0F, 40.0F, 180.0F},
        {40.0F, 88.0F, 82.0F, 88.0F},
    };

    // FM DESTINATION selector -> VCF FREQ stub (destination value 1); straight
    // segments only. The semicircular hop over the x=499 vertical sits between the
    // pre-hop segment ending at (492,180) and the post-hop segment at (506,180).
    inline constexpr FmSeg FM_BRANCH_VCF[] = {
        {276.0F, 230.0F, 289.0F, 230.0F},
        {289.0F, 230.0F, 289.0F, 180.0F},
        {289.0F, 180.0F, 492.0F, 180.0F},
        {506.0F, 180.0F, 513.0F, 180.0F},
        {513.0F, 180.0F, 513.0F, 82.0F},
        {513.0F, 82.0F, 541.0F, 82.0F},
    };

    // Semicircular hop over x=499 (SVG arc M492 180 A7 7 0 0 1 506 180, reproduced
    // as a quadratic bump), on the VCF branch only: endpoints + quad control point.
    inline constexpr FmSeg FM_VCF_HOP_ENDS = {492.0F, 180.0F, 506.0F, 180.0F};
    inline constexpr float FM_VCF_HOP_CTRL_X = 499.0F;
    inline constexpr float FM_VCF_HOP_CTRL_Y = 166.0F;
}
