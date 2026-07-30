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
        [[nodiscard]] juce::Image renderBlock(const juce::StringArray& lines,
                                              int columns,
                                              int rows,
                                              float scale) const;

    private:
        /// Coverage of one glyph's lit segments, painted into `target` at the
        /// given cell origin. `mask` selects the segments.
        void paintGlyph(juce::Graphics& target,
                        std::uint16_t mask,
                        float originX,
                        float originY,
                        float cellWidth) const;

        /// The 16 segment outlines, in cell-normalised units: every coordinate
        /// is a fraction of the cell WIDTH, y included. The cell is 12x16 so
        /// normalised y runs 0..4/3 — deliberately, because it keeps the unit
        /// cell isotropic and lets one uniform scale map it to device pixels
        /// without turning round glow into ellipses.
        std::array<juce::Path, SEGMENT_COUNT> _segments;
    };
}
