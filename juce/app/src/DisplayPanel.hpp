#pragma once

// VFD display: renders text as 16-segment glyphs drawn vectorially, on a black
// background, glyph block centered, grid derived from the bounds like the
// reference VacuumFluoDisplayControl (cols = width/12, lines = height/16).
// The line content is built by VfdDisplayHelper.
//
// The grid, the centering, the setLines early-out and setBufferedToImage are
// carried over unchanged from ADR-JUC-007; only the artwork inside each cell
// changed, from a 12x16 sprite blit to VfdSegmentRenderer.
// [RQ-GUI-020, RQ-GUI-033, RQ-GUI-049, ADR-JUC-023 (DEC-JUC-055)]

#include "VfdSegmentRenderer.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class DisplayPanel final : public juce::Component
    {
    public:
        /// The grid unit, in logical pixels. The renderer is the authority on
        /// glyph geometry, so these name its cell rather than repeating it.
        static constexpr int GLYPH_WIDTH = VfdSegmentRenderer::CELL_WIDTH;
        static constexpr int GLYPH_HEIGHT = VfdSegmentRenderer::CELL_HEIGHT;

        DisplayPanel();

        /// Columns available at the current bounds (reference grid formula);
        /// VfdDisplayHelper's wrap threshold, so content wrapping and
        /// rendering share one source of truth.
        [[nodiscard]] int maxCharsPerLine() const;
        /// Rows available at the current bounds.
        [[nodiscard]] int lineCount() const;

        /// Replaces the text block; no-op (and no repaint) when identical,
        /// like the reference SetText.
        void setLines(juce::StringArray lines);

        void paint(juce::Graphics& g) override;

    private:
        VfdSegmentRenderer _renderer;
        juce::StringArray _lines;
    };
}
