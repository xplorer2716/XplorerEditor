#include "DisplayPanel.hpp"

#include "BinaryData.h"

namespace xplorer::app
{
    namespace
    {
        constexpr int SHEET_COLUMNS = 32;   // glyphs per sheet row
        constexpr int FIRST_GLYPH = 32;     // ' '
        constexpr int LAST_GLYPH = 126;     // '~'; 127 and below 32 -> space
    }

    DisplayPanel::DisplayPanel()
        : _glyphSheet(juce::ImageCache::getFromMemory(BinaryData::vfdmatrix_png,
                                                      BinaryData::vfdmatrix_pngSize))
    {
        // The whole surface is painted (black + glyph cells), and the text
        // only changes via setLines: let JUCE cache the rendered image so
        // unchanged text costs no paint at all (the reference achieved this
        // with a hand-managed buffer bitmap + changed-cell diffing). [ADR-007]
        setOpaque(true);
        setBufferedToImage(true);
        setInterceptsMouseClicks(false, false);
    }

    int DisplayPanel::maxCharsPerLine() const
    {
        const int cols = getWidth() / GLYPH_WIDTH;
        return cols > 0 ? cols : 22; // logical-canvas value before layout
    }

    int DisplayPanel::lineCount() const
    {
        return getHeight() / GLYPH_HEIGHT;
    }

    void DisplayPanel::setLines(juce::StringArray lines)
    {
        if (_lines == lines)
        {
            return; // reference SetText early-out
        }
        _lines = std::move(lines);
        repaint();
    }

    void DisplayPanel::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
        const int cols = getWidth() / GLYPH_WIDTH;
        const int rows = getHeight() / GLYPH_HEIGHT;
        if (cols <= 0 || rows <= 0 || _glyphSheet.isNull())
        {
            return;
        }

        // Centered glyph block, unlit-dot 'space' cells filling the grid,
        // as the reference (which pads its text to the full grid).
        const int left = (getWidth() - cols * GLYPH_WIDTH) / 2;
        const int top = (getHeight() - rows * GLYPH_HEIGHT) / 2;
        // Nearest-neighbour under the canvas scale keeps the dot matrix crisp.
        g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

        for (int row = 0; row < rows; ++row)
        {
            const auto line = row < _lines.size() ? _lines[row] : juce::String();
            for (int col = 0; col < cols; ++col)
            {
                auto character = col < line.length() ? line[col] : juce::juce_wchar(' ');
                if (character < FIRST_GLYPH || character > LAST_GLYPH)
                {
                    character = ' ';
                }
                const int index = static_cast<int>(character) - FIRST_GLYPH;
                g.drawImage(_glyphSheet,
                            left + col * GLYPH_WIDTH, top + row * GLYPH_HEIGHT,
                            GLYPH_WIDTH, GLYPH_HEIGHT,
                            (index % SHEET_COLUMNS) * GLYPH_WIDTH,
                            (index / SHEET_COLUMNS) * GLYPH_HEIGHT,
                            GLYPH_WIDTH, GLYPH_HEIGHT);
            }
        }
    }
}
