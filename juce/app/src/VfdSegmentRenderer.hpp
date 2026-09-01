#pragma once

// Vector 16-segment VFD glyph rendering. Replaces the 12x16 sprite blit of
// ADR-JUC-007: the sprite carried only 12x16 px of information per glyph, so it
// could not stay crisp once the physical pixel scale left 1.0 — which happens on
// any HiDPI display and on any window enlargement (RQ-GUI-005).
//
// Everything visual comes from design tokens (RQ-DSN-097): this file holds no
// colour, length or ratio of its own. Glyph topology comes from the vendored
// segment table (SegmentFont.hpp).
//
// The renderer produces an IMAGE rather than painting into a Graphics, because
// the glow is a blur and a blur needs a raster. The image is built at the
// caller's device scale and drawn 1:1, so nothing is ever magnified.
// [RQ-GUI-033, RQ-DSN-097, ADR-JUC-023 (DEC-JUC-053, DEC-JUC-054)]

#include "xplorer/app/SegmentFont.hpp"

#include <juce_graphics/juce_graphics.h>

#include <array>
#include <vector>

namespace xplorer::app
{
    class VfdSegmentRenderer
    {
    public:
        /// Glyph cell of the reference grid, in logical pixels. The renderer is
        /// the authority on glyph geometry, so the grid unit lives here and
        /// DisplayPanel's layout maths refers to these.
        static constexpr int CELL_WIDTH = 12;
        static constexpr int CELL_HEIGHT = 16;

        /// The cell advance in WHOLE device pixels at `scale`, per axis.
        ///
        /// Floored, never rounded: the glass has about three logical pixels of
        /// horizontal slack over the grid it holds, so rounding up would push
        /// the outermost glyphs under the bezel band. Flooring costs under one
        /// device pixel per cell and can never overflow.
        ///
        /// The two axes are independent, and that is sound rather than sloppy:
        /// paintGlyph takes only the cell WIDTH and draws the glyph in
        /// normalised units with y running 0..4/3, so glyph SHAPE is always
        /// exactly 3:4 whatever the height. The height is a row pitch — line
        /// leading — not a drawing dimension.
        /// [RQ-SCL-004, ADR-JUC-026 (DEC-JUC-069)]
        [[nodiscard]] static int cellWidthForScale(float scale) noexcept;
        [[nodiscard]] static int cellHeightForScale(float scale) noexcept;

        /// Characters drawn by an off-model primitive instead of by segments.
        /// The list is short and lives in exactly one place on purpose: it is
        /// the whole of this port's divergence from the vendored table, and it
        /// has to stay auditable. [DEC-JUC-052]
        ///
        ///   ':'  two separated dots — no segment combination makes two
        ///        disconnected marks. Not decoration: it appears in every
        ///        NAME:VALUE and every "MIDI CC:" line.
        ///   '_'  a bar BELOW the glyph body, outside every rail.
        ///   '.'  a single dot at the ':' glyph's lower-dot position. The
        ///        vendored table has no true period — its closest entry lights
        ///        one diagonal segment (R), which reads as a stray half-stroke,
        ///        not a dot. The Xpander hardware shows a real dot (PLAN-GUI-015).
        ///
        /// Lowercase 'x' is deliberately NOT here any more: the table gives it
        /// the same four diagonals as 'X' (a real 16-segment device has no
        /// lower-half crossing to distinguish them), which used to need a
        /// half-height primitive to satisfy RQ-GUI-049's distinctness rule.
        /// That rule was narrowed to the always-uppercase range (PLAN-GUI-015,
        /// 2026-09-02): DisplayPanel::setLines now uppercases everything before
        /// it reaches this renderer, since the physical Xpander VFD has no
        /// lowercase capability, so no lowercase code point can reach this
        /// class through the application any more.
        static constexpr std::array<int, 3> OVERRIDDEN_CHARACTERS{':', '_', '.'};
        static constexpr int OVERRIDE_COUNT = static_cast<int>(OVERRIDDEN_CHARACTERS.size());

        VfdSegmentRenderer();

        /// The glyph block for `lines` laid out on a `columns` x `rows` grid,
        /// at `scale` device pixels per logical pixel. Lines shorter than the
        /// grid are padded with spaces, so every cell shows its unlit bed —
        /// as the reference does.
        ///
        /// `scale` is what the caller reads from
        /// `Graphics::getInternalContext().getPhysicalPixelScaleFactor()`; the
        /// glow radius is multiplied by it, without which the halo would shrink
        /// visually as the window grows (DEC-JUC-053).
        ///
        /// The cell is snapped to whole device pixels (DEC-JUC-069), so the
        /// returned image is NOT `columns * CELL_WIDTH * scale` wide in
        /// general — read its actual dimensions and draw it 1:1 (DEC-JUC-070).
        [[nodiscard]] juce::Image renderBlock(const juce::StringArray& lines,
                                              int columns,
                                              int rows,
                                              float scale) const;

        /// Whether `codePoint` is drawn by an off-model primitive rather than
        /// by segments. Exposed so tests can assert the override table's
        /// membership without reaching into the renderer's internals.
        [[nodiscard]] static bool hasOverride(int codePoint) noexcept;

    private:
        /// Coverage of one glyph, painted into `target` at the given cell
        /// origin: the override primitives when the character has one,
        /// otherwise its lit segments.
        void paintGlyph(juce::Graphics& target,
                        int codePoint,
                        float originX,
                        float originY,
                        float cellWidth) const;

        /// Every segment of the cell, for the always-drawn unlit bed.
        void paintUnlitBed(juce::Graphics& target,
                           float originX,
                           float originY,
                           float cellWidth) const;

        /// The 16 segment outlines, in cell-normalised units: every coordinate
        /// is a fraction of the cell WIDTH, y included. The cell is 12x16 so
        /// normalised y runs 0..4/3 — deliberately, because it keeps the unit
        /// cell isotropic and lets one uniform scale map it to device pixels
        /// without turning round glow into ellipses.
        std::array<juce::Path, SEGMENT_COUNT> _segments;

        /// Off-model primitives, keyed by the character they replace
        /// (DEC-JUC-052). Same normalised space as `_segments`.
        std::array<juce::Path, OVERRIDE_COUNT> _overrides;
    };
}
