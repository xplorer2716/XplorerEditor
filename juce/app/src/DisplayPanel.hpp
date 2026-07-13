#pragma once

// VFD display: renders text as bitmap glyphs from the reference MATRIXTINY
// sprite sheet (96 glyphs of 12x16, ASCII 32-126), black background, glyph
// block centered, grid derived from the bounds like the reference
// VacuumFluoDisplayControl (cols = width/12, lines = height/16). Direct
// sprite-sheet paint + setBufferedToImage per ADR-007; the line content is
// built by VfdDisplayHelper. [RQ-GUI-020, RQ-GUI-033]

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class DisplayPanel final : public juce::Component
    {
    public:
        static constexpr int GLYPH_WIDTH = 12;
        static constexpr int GLYPH_HEIGHT = 16;

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
        juce::Image _glyphSheet;
        juce::StringArray _lines;
    };
}
