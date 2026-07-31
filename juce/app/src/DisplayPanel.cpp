#include "DisplayPanel.hpp"

#include "DesignTokens.hpp"

namespace xplorer::app
{
    namespace
    {
        namespace bezel = tokens::component;
    }

    DisplayPanel::DisplayPanel()
    {
        // The whole surface is painted (black + glyph cells), and the text
        // only changes via setLines: let JUCE cache the rendered component so
        // unchanged text costs no paint at all (the reference achieved this
        // with a hand-managed buffer bitmap + changed-cell diffing).
        //
        // The cache is not a resolution trap, contrary to what one might
        // expect of it: StandardCachedComponentImage allocates at
        // bounds * getPhysicalPixelScaleFactor() and paints into it with that
        // scale applied, so it follows the device resolution rather than the
        // logical size. [ADR-JUC-007, carried over by ADR-JUC-023 DEC-JUC-055]
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

    juce::Rectangle<int> DisplayPanel::glassBounds() const
    {
        return getLocalBounds().reduced(bezel::vfdBezelMarginH, bezel::vfdBezelMarginV);
    }

    void DisplayPanel::paintBezel(juce::Graphics& g, juce::Rectangle<int> glass) const
    {
        const auto outer = getLocalBounds().toFloat();

        // The band is the panel's own metal, inverted: dark at the top, light
        // at the bottom. Under light from above that inversion is the whole
        // difference between a recess and a bump, which is why it must not be
        // confused with the raised-plate relief of RQ-DSN-094. [DEC-JUC-057]
        g.setGradientFill({bezel::vfdBezelBandTop, outer.getX(), outer.getY(),
                           bezel::vfdBezelBandBottom, outer.getX(), outer.getBottom(), false});
        g.fillRoundedRectangle(outer, bezel::vfdBezelRadius);

        // Outer rim, same inversion: shadow on top, catch-light underneath.
        const auto inset = outer.reduced(bezel::vfdBezelRadius, 0.0F);
        g.setColour(juce::Colours::black.withAlpha(bezel::vfdBezelRimDark));
        g.drawLine(inset.getX(), outer.getY() + 0.5F, inset.getRight(), outer.getY() + 0.5F);
        g.setColour(juce::Colours::white.withAlpha(bezel::vfdBezelRimLight));
        g.drawLine(inset.getX(), outer.getBottom() - 0.5F,
                   inset.getRight(), outer.getBottom() - 0.5F);

        // The shadow the band casts onto the glass edge — the cue that the
        // glass sits *below* the band rather than flush with it. Drawn as a
        // stroke straddling the glass boundary, so it darkens the band's inner
        // lip and the glass's outer edge alike.
        g.setColour(juce::Colours::black.withAlpha(bezel::vfdBezelInnerShadow));
        g.drawRect(glass.toFloat().expanded(bezel::vfdBezelInnerWidth * 0.5F),
                   bezel::vfdBezelInnerWidth);
    }

    void DisplayPanel::paint(juce::Graphics& g)
    {
        const auto glass = glassBounds();
        paintBezel(g, glass);

        g.setColour(juce::Colours::black);
        g.fillRect(glass);

        const int cols = glass.getWidth() / GLYPH_WIDTH;
        const int rows = glass.getHeight() / GLYPH_HEIGHT;
        if (cols <= 0 || rows <= 0)
        {
            return;
        }

        // Render at the resolution this context will actually be rasterised
        // at, not at the logical size. getPhysicalPixelScaleFactor() is the
        // square root of the accumulated transform's determinant, so it folds
        // in both ScaledCanvasComponent's window scale and the display's DPI
        // scale. Rendering at the logical size and letting the image be scaled
        // is exactly the degradation this whole change removes. [DEC-JUC-053]
        const auto scale = g.getInternalContext().getPhysicalPixelScaleFactor();
        const auto block = _renderer.renderBlock(_lines, cols, rows, scale);
        if (block.isNull())
        {
            return;
        }

        // Centered glyph block, as the reference (which pads its text to the
        // full grid). Drawn into its logical-size rectangle, which the context
        // maps back to the device size the image was built for.
        //
        // That lands one image pixel per device pixel only when `scale` is a
        // whole number. It usually is not — dragging a window edge gives 2.87
        // as readily as 3.0 — and an image is a whole number of pixels while
        // the device rectangle is not, so the blit then applies a sub-pixel
        // resample (under 0.1%, measured; invisible side by side). That is
        // categorically different from the sprite this replaced: the detail is
        // genuinely rendered at the target resolution and merely filtered by a
        // fraction of a pixel, rather than absent and magnified.
        const int left = glass.getX() + (glass.getWidth() - cols * GLYPH_WIDTH) / 2;
        const int top = glass.getY() + (glass.getHeight() - rows * GLYPH_HEIGHT) / 2;
        g.drawImage(block,
                    juce::Rectangle<float>(static_cast<float>(left),
                                           static_cast<float>(top),
                                           static_cast<float>(cols * GLYPH_WIDTH),
                                           static_cast<float>(rows * GLYPH_HEIGHT)));
    }
}
