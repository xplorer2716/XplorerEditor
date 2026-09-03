#include "DisplayPanel.hpp"

#include "DesignTokens.hpp"

#include <cmath>

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

    // Both accessors measure the GLASS, never the bounds — the bounds include
    // the bezel band now (DEC-JUC-058). VfdDisplayHelper uses
    // maxCharsPerLine() as its wrap threshold, so measuring the bounds would
    // let a line wrap to a width the glass does not have and put characters
    // under the band.
    //
    // At today's margins the two happen to agree (275/12 and 267/12 both floor
    // to 22; 94/16 and 82/16 both to 5), which is exactly what would have kept
    // the mistake invisible until someone retuned a margin token.

    int DisplayPanel::maxCharsPerLine() const
    {
        const int cols = glassBounds().getWidth() / GLYPH_WIDTH;
        return cols > 0 ? cols : 22; // logical-canvas value before layout
    }

    int DisplayPanel::lineCount() const
    {
        return glassBounds().getHeight() / GLYPH_HEIGHT;
    }

    void DisplayPanel::setLines(juce::StringArray lines)
    {
        // The Xpander hardware has no lowercase capability (RQ-GUI-049,
        // PLAN-GUI-015): uppercase here, once, so every caller — tone names
        // from a .syx file included — displays the way the real unit would.
        for (auto& line : lines)
        {
            line = line.toUpperCase();
        }
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
        //
        // Square corners, deliberately (DEC-JUC-062). A rounded fill leaves the
        // four corner pixels of the bounds untouched, and this component is
        // opaque: JUCE then allocates its cache WITHOUT clearing it
        // (StandardCachedComponentImage passes `! isOpaque()` as clearImage),
        // so those pixels display uninitialised memory — measured as
        // (126,1,1) and (100,65,86) against a (68,69,78) plate. Square corners
        // make the setOpaque(true) promise true instead of patching around it,
        // and a metal bezel cut into a plate has sharp outer corners anyway.
        g.setGradientFill({bezel::vfdBezelBandTop, outer.getX(), outer.getY(),
                           bezel::vfdBezelBandBottom, outer.getX(), outer.getBottom(), false});
        g.fillRect(outer);

        // Outer rim, same inversion: shadow on top, catch-light underneath.
        g.setColour(juce::Colours::black.withAlpha(bezel::vfdBezelRimDark));
        g.drawLine(outer.getX(), outer.getY() + 0.5F, outer.getRight(), outer.getY() + 0.5F);
        g.setColour(juce::Colours::white.withAlpha(bezel::vfdBezelRimLight));
        g.drawLine(outer.getX(), outer.getBottom() - 0.5F,
                   outer.getRight(), outer.getBottom() - 0.5F);

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
        if (scale <= 0.0F)
        {
            return;
        }
        const auto block = _renderer.renderBlock(_lines, cols, rows, scale);
        if (block.isNull())
        {
            return;
        }

        // Centered glyph block, as the reference (which pads its text to the
        // full grid) — but positioned and sized in DEVICE space, then converted
        // back, so the blit is 1:1 with no resampling filter. [DEC-JUC-070]
        //
        // The renderer snapped the cell to whole device pixels (DEC-JUC-069),
        // so the image is no longer `cols * GLYPH_WIDTH * scale` wide: its own
        // dimensions are the authority. Sizing the destination from the logical
        // grid instead would stretch the image by the fraction the snap removed
        // and undo the alignment it was rendered for.
        const auto blockWidth = static_cast<float>(block.getWidth());
        const auto blockHeight = static_cast<float>(block.getHeight());
        const auto deviceLeft = std::round(static_cast<float>(glass.getX()) * scale
                                           + (static_cast<float>(glass.getWidth()) * scale - blockWidth) * 0.5F);
        const auto deviceTop = std::round(static_cast<float>(glass.getY()) * scale
                                          + (static_cast<float>(glass.getHeight()) * scale - blockHeight) * 0.5F);
        g.drawImage(block,
                    juce::Rectangle<float>(deviceLeft / scale, deviceTop / scale,
                                           blockWidth / scale, blockHeight / scale));
    }
}
