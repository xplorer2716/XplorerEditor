#pragma once

// VFD display: a recessed bezel around black glass, on which text is drawn as
// 16-segment glyphs (VfdSegmentRenderer). The grid follows the reference
// VacuumFluoDisplayControl formula (cols = width/12, lines = height/16), and
// the line content is built by VfdDisplayHelper.
//
// The bounds cover BEZEL AND GLASS, not just the glass: the bezel's inner edge
// has to line up with the glass to the pixel at any render scale, and only the
// component that owns the geometry can guarantee that (DEC-JUC-058). The
// consequence is that every grid computation works off an *inset* rectangle —
// including maxCharsPerLine(), which VfdDisplayHelper uses as its wrap
// threshold, so text would otherwise wrap to a width the glass does not have.
//
// The grid formula, the centering, the setLines early-out and
// setBufferedToImage are carried over unchanged from ADR-JUC-007.
// [RQ-GUI-020, RQ-GUI-033, RQ-GUI-049, RQ-GUI-050, RQ-DSN-098,
//  ADR-JUC-023 (DEC-JUC-055), ADR-JUC-024 (DEC-JUC-057, DEC-JUC-058)]

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
        /// like the reference SetText. Every line is uppercased first: the
        /// physical Xpander VFD has no lowercase capability, so this is the
        /// one place all display content passes through before rendering.
        /// [RQ-GUI-049, PLAN-GUI-015]
        void setLines(juce::StringArray lines);

        void paint(juce::Graphics& g) override;

    private:
        /// The glass: the bounds minus the bezel band. Everything to do with
        /// the grid derives from this, never from the raw bounds.
        [[nodiscard]] juce::Rectangle<int> glassBounds() const;

        /// Band, rim hairlines and the shadow the band casts on the glass edge.
        void paintBezel(juce::Graphics& g, juce::Rectangle<int> glass) const;

        VfdSegmentRenderer _renderer;
        juce::StringArray _lines;
    };
}
