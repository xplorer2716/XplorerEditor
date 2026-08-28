#include <catch2/catch_test_macros.hpp>

#include "DesignTokens.hpp"
#include "VfdSegmentRenderer.hpp"

#include <juce_graphics/juce_graphics.h>

#include <array>
#include <cmath>
#include <vector>
#include <map>
#include <string>

// The renderer's whole reason to exist is that it does NOT degrade with scale
// (RQ-GUI-005, RQ-GUI-033): the sprite it replaces carried 12x16 px per glyph
// and could only be magnified. So these tests are about scale behaviour, not
// about glyph shapes — the shapes are the segment table's job, pinned by
// SegmentFontTests, and the look is the tokens' job, pinned by the fitting
// tool's --check against the baseline sheet.
// [RQ-GUI-033, RQ-GUI-005, RQ-DSN-097, ADR-JUC-023 (DEC-JUC-053, DEC-JUC-054)]

using namespace xplorer::app;

namespace
{
    namespace vfd = tokens::component;

    constexpr int ONE_COLUMN = 1;
    constexpr int ONE_ROW = 1;
    constexpr float CHANNEL_MAX = 255.0F;

    /// Mean luminance of the green channel, the phosphor's dominant one.
    double meanGreen(const juce::Image& image)
    {
        const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
        double total = 0.0;
        for (int y = 0; y < image.getHeight(); ++y)
        {
            for (int x = 0; x < image.getWidth(); ++x)
            {
                total += pixels.getPixelColour(x, y).getGreen();
            }
        }
        return total / (image.getWidth() * image.getHeight());
    }

    double maxGreen(const juce::Image& image)
    {
        const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
        int highest = 0;
        for (int y = 0; y < image.getHeight(); ++y)
        {
            for (int x = 0; x < image.getWidth(); ++x)
            {
                highest = juce::jmax(highest, static_cast<int>(pixels.getPixelColour(x, y).getGreen()));
            }
        }
        return highest;
    }
}

SCENARIO("The renderer rasterises at the requested scale rather than magnifying",
         "[RQ-GUI-033][RQ-GUI-005]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;
    const juce::StringArray lines{"8"};

    GIVEN("the same glyph rendered at scale 1 and at scale 2")
    {
        const auto small = renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, 1.0F);
        const auto large = renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, 2.0F);

        THEN("the larger image is exactly twice the size in each axis")
        {
            REQUIRE(small.getWidth() == VfdSegmentRenderer::CELL_WIDTH);
            REQUIRE(small.getHeight() == VfdSegmentRenderer::CELL_HEIGHT);
            REQUIRE(large.getWidth() == 2 * small.getWidth());
            REQUIRE(large.getHeight() == 2 * small.getHeight());
        }

        THEN("it carries detail a nearest-neighbour magnification could not")
        {
            // A magnified image has uniform 2x2 blocks by construction. A real
            // rasterisation at twice the resolution does not: this is exactly
            // the difference the sprite sheet could never provide.
            const juce::Image::BitmapData pixels(large, juce::Image::BitmapData::readOnly);
            int uniformBlocks = 0;
            int totalBlocks = 0;
            for (int y = 0; y + 1 < large.getHeight(); y += 2)
            {
                for (int x = 0; x + 1 < large.getWidth(); x += 2)
                {
                    const auto first = pixels.getPixelColour(x, y);
                    const bool uniform = pixels.getPixelColour(x + 1, y) == first
                                         && pixels.getPixelColour(x, y + 1) == first
                                         && pixels.getPixelColour(x + 1, y + 1) == first;
                    uniformBlocks += uniform ? 1 : 0;
                    ++totalBlocks;
                }
            }
            REQUIRE(totalBlocks > 0);
            // Flat regions (the dark surround) legitimately give uniform
            // blocks, so this is a floor on genuine detail, not a claim that
            // none may be uniform.
            REQUIRE(uniformBlocks < totalBlocks);
        }
    }
}

// --- Device-pixel grid [RQ-SCL-004, ADR-JUC-026] ---------------------------
//
// Rasterising at the device scale (above) was never in question; what these
// pin is the GRID the cells sit on. At a fractional scale the old float cell
// put every column at a different sub-pixel phase, so the same character came
// out slightly different depending on where it sat in the line. Full screen on
// every common 16:9 resolution is such a scale, which is why this is the
// normal case rather than an edge one.

SCENARIO("The glyph cell is a whole number of device pixels at any scale", "[RQ-SCL-004]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;
    const juce::StringArray lines{"8"};

    GIVEN("the fractional scales the application actually produces")
    {
        // Full screen on 1080p / 1440p / 2160p, then the 1x, 1.5x and 2x View
        // presets (canvas scale = window width / 1260).
        for (const auto scale : {1.3435F, 1.8015F, 2.7176F, 1.1429F, 1.7143F, 2.2857F})
        {
            const auto cellWidth = VfdSegmentRenderer::cellWidthForScale(scale);
            const auto cellHeight = VfdSegmentRenderer::cellHeightForScale(scale);

            THEN("the rendered block is an exact multiple of the whole-pixel cell")
            {
                const auto block = renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, scale);
                REQUIRE_FALSE(block.isNull());
                CHECK(block.getWidth() == cellWidth);
                CHECK(block.getHeight() == cellHeight);
            }

            THEN("the cell never exceeds its nominal size")
            {
                // One-sided on purpose: an oversized cell would push the
                // outermost glyphs under the bezel band, which has only ~3
                // logical px of slack to give. [DEC-JUC-069]
                CHECK(static_cast<float>(cellWidth)
                      <= static_cast<float>(VfdSegmentRenderer::CELL_WIDTH) * scale);
                CHECK(static_cast<float>(cellHeight)
                      <= static_cast<float>(VfdSegmentRenderer::CELL_HEIGHT) * scale);
            }

            THEN("it stays within one device pixel of nominal")
            {
                CHECK(static_cast<float>(VfdSegmentRenderer::CELL_WIDTH) * scale
                          - static_cast<float>(cellWidth)
                      < 1.0F);
                CHECK(static_cast<float>(VfdSegmentRenderer::CELL_HEIGHT) * scale
                          - static_cast<float>(cellHeight)
                      < 1.0F);
            }

            THEN("the glyph still fits the row pitch")
            {
                // paintGlyph draws 4/3 of the cell WIDTH tall, whatever the
                // height; the row pitch must clear that or lines would touch.
                CHECK(cellWidth * VfdSegmentRenderer::CELL_HEIGHT
                      <= cellHeight * VfdSegmentRenderer::CELL_WIDTH
                             + VfdSegmentRenderer::CELL_WIDTH);
            }
        }
    }

    GIVEN("a scale small enough to floor to nothing")
    {
        THEN("the cell clamps to one pixel instead of collapsing")
        {
            CHECK(VfdSegmentRenderer::cellWidthForScale(0.01F) == 1);
            CHECK(VfdSegmentRenderer::cellHeightForScale(0.01F) == 1);
            const auto block = renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, 0.01F);
            CHECK_FALSE(block.isNull());
        }
    }

    GIVEN("whole scales")
    {
        THEN("the snap leaves them untouched")
        {
            // The snap must not perturb the cases that were already aligned.
            CHECK(VfdSegmentRenderer::cellWidthForScale(1.0F) == VfdSegmentRenderer::CELL_WIDTH);
            CHECK(VfdSegmentRenderer::cellHeightForScale(1.0F) == VfdSegmentRenderer::CELL_HEIGHT);
            CHECK(VfdSegmentRenderer::cellWidthForScale(2.0F) == 2 * VfdSegmentRenderer::CELL_WIDTH);
            CHECK(VfdSegmentRenderer::cellHeightForScale(2.0F)
                  == 2 * VfdSegmentRenderer::CELL_HEIGHT);
        }
    }
}

SCENARIO("Repeated characters render identically along a line", "[RQ-SCL-004]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("a line of identical glyphs at a deliberately fractional scale")
    {
        // 1.8015 is full screen on 2560x1440 — the owner's own case.
        constexpr float FRACTIONAL_SCALE = 1.8015F;
        constexpr int COLUMNS = 8;
        const juce::StringArray lines{"88888888"};

        // Only cells with a full glyph on both sides AND both image edges at
        // least two cells away are compared. The glow is a Gaussian of sigma
        // 0.179 cell (vfdGlowRadius), so at two cells the contribution from
        // beyond the block is ~11 sigma, i.e. nothing: these cells see an
        // identical field and must therefore be bit-identical. Comparing the
        // outermost cells instead would test the blur's edge behaviour, which
        // is not what RQ-SCL-004 is about.
        constexpr int FIRST_INTERIOR = 2;
        constexpr int LAST_INTERIOR = COLUMNS - 3;

        const auto block = renderer.renderBlock(lines, COLUMNS, ONE_ROW, FRACTIONAL_SCALE);
        REQUIRE_FALSE(block.isNull());

        THEN("the interior cells are pixel-identical to one another")
        {
            const auto cellWidth = block.getWidth() / COLUMNS;
            REQUIRE(cellWidth * COLUMNS == block.getWidth());

            const juce::Image::BitmapData pixels(block, juce::Image::BitmapData::readOnly);
            for (int column = FIRST_INTERIOR + 1; column <= LAST_INTERIOR; ++column)
            {
                int differing = 0;
                for (int y = 0; y < block.getHeight(); ++y)
                {
                    for (int x = 0; x < cellWidth; ++x)
                    {
                        if (pixels.getPixelColour(FIRST_INTERIOR * cellWidth + x, y)
                            != pixels.getPixelColour(column * cellWidth + x, y))
                        {
                            ++differing;
                        }
                    }
                }
                INFO("column " << column << " differs from column " << FIRST_INTERIOR << " in "
                               << differing << " pixels");
                CHECK(differing == 0);
            }
        }
    }
}

SCENARIO("The glow radius follows the render scale", "[RQ-GUI-033][RQ-DSN-097]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;
    const juce::StringArray lines{"8"};

    GIVEN("the same glyph rendered at three scales")
    {
        // The glow radius is stored as a fraction of the cell, so a renderer
        // that forgot to take it through the scale would keep a constant
        // device-pixel halo. That halo would then cover a smaller *relative*
        // area as the scale grows, dropping the mean luminance. Comparing mean
        // luminance across scales therefore catches precisely that omission —
        // which is the failure mode DEC-JUC-053 exists to prevent.
        const auto atOne = meanGreen(renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, 1.0F));

        THEN("the light per unit area stays essentially constant")
        {
            REQUIRE(atOne > 0.0);
            constexpr double tolerance = 0.15; // 15% of the scale-1 value
            // Fractional scales are in this list on purpose. Dragging a window
            // edge produces them almost every time, so testing only 1/2/3
            // would have covered the rare case and missed the usual one.
            for (const auto scale : {2.0F, 3.0F, 1.33F, 2.5F, 2.87F})
            {
                INFO("scale " << scale);
                const auto here = meanGreen(
                    renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, scale));
                REQUIRE(std::abs(here - atOne) / atOne < tolerance);
            }
        }
    }
}

SCENARIO("A fractional render scale produces a correctly sized block",
         "[RQ-GUI-033][RQ-GUI-005]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("the grid the display actually uses, at fractional scales")
    {
        constexpr int columns = 22;
        constexpr int rows = 5;

        THEN("the block is at most one device pixel per cell under the exact size")
        {
            // AMENDED for RQ-SCL-004 (was: "never drifts more than half a
            // pixel from the exact size"). The old bound asserted the absence
            // of the very snap ADR-JUC-026 introduces, so it could not survive
            // the change on either side — it is the contract that moved, not
            // an expectation bent to make a failure go away.
            //
            // The new contract is one-sided and tighter where it matters: the
            // block may be SMALLER than nominal by the floor's remainder — up
            // to one device pixel per cell — and may never be LARGER, because
            // the glass it is centred in has only ~3 logical px of slack and an
            // oversized block would slide under the bezel band. The one-sided
            // bound is what pins that, and it is what the original half-pixel
            // bound was really protecting: no error accumulating across cells.
            for (const auto scale : {1.33F, 2.5F, 2.87F, 3.7F})
            {
                const auto block = renderer.renderBlock({"599 XPLORER"},
                                                        columns, rows, scale);
                const auto exactWidth = static_cast<float>(columns)
                                        * VfdSegmentRenderer::CELL_WIDTH * scale;
                const auto exactHeight = static_cast<float>(rows)
                                         * VfdSegmentRenderer::CELL_HEIGHT * scale;
                INFO("scale " << scale << " gave " << block.getWidth()
                              << "x" << block.getHeight());
                REQUIRE(static_cast<float>(block.getWidth()) <= exactWidth);
                REQUIRE(static_cast<float>(block.getHeight()) <= exactHeight);
                REQUIRE(exactWidth - static_cast<float>(block.getWidth())
                        < static_cast<float>(columns));
                REQUIRE(exactHeight - static_cast<float>(block.getHeight())
                        < static_cast<float>(rows));
            }
        }
    }
}

SCENARIO("Unlit segments are always drawn", "[RQ-GUI-033]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("a cell holding a space")
    {
        const auto image = renderer.renderBlock({" "}, ONE_COLUMN, ONE_ROW, 2.0F);

        THEN("it is not black — the unlit bed shows, as on the reference")
        {
            REQUIRE(maxGreen(image) > 0.0);
        }

        THEN("its brightest point sits at the unlit token's level")
        {
            // Not merely "non-zero": the bed must be at the level the tokens
            // say, otherwise the display reads as washed out or as dead black.
            const auto expected = static_cast<double>(vfd::vfdUnlitLevel)
                                  * vfd::vfdPhosphor.getFloatGreen() * CHANNEL_MAX;
            REQUIRE(std::abs(maxGreen(image) - expected) <= 2.0);
        }
    }

    GIVEN("a cell holding a lit glyph")
    {
        const auto lit = renderer.renderBlock({"8"}, ONE_COLUMN, ONE_ROW, 2.0F);
        const auto blank = renderer.renderBlock({" "}, ONE_COLUMN, ONE_ROW, 2.0F);

        THEN("it is far brighter than the bed")
        {
            REQUIRE(maxGreen(lit) > maxGreen(blank) * 2.0);
        }
    }
}

SCENARIO("The phosphor colour comes from the token, not from the code",
         "[RQ-DSN-097]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("a rendered glyph")
    {
        const auto image = renderer.renderBlock({"8"}, ONE_COLUMN, ONE_ROW, 3.0F);

        WHEN("a pixel below saturation is sampled")
        {
            // Below saturation the hue is the token's, untouched by the
            // white-lift. Sampling there proves the token is consumed rather
            // than a colour being hard-coded — a check a source grep cannot
            // make, since it could not tell a visual literal from an
            // algorithmic constant.
            const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
            bool sampled = false;
            for (int y = 0; y < image.getHeight() && !sampled; ++y)
            {
                for (int x = 0; x < image.getWidth() && !sampled; ++x)
                {
                    const auto colour = pixels.getPixelColour(x, y);
                    const auto green = colour.getGreen();
                    if (green > 60 && green < 200)
                    {
                        const auto ratio = static_cast<double>(colour.getBlue()) / green;
                        const auto expected = static_cast<double>(vfd::vfdPhosphor.getFloatBlue())
                                              / vfd::vfdPhosphor.getFloatGreen();
                        INFO("sampled rgb " << colour.getRed() << "," << green
                                            << "," << colour.getBlue());
                        REQUIRE(std::abs(ratio - expected) < 0.05);
                        sampled = true;
                    }
                }
            }

            THEN("such a pixel existed and carried the token's hue")
            {
                REQUIRE(sampled);
            }
        }
    }
}

SCENARIO("The grid is filled even when the text is shorter", "[RQ-GUI-033]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("a two-by-two grid fed a single short line")
    {
        constexpr int columns = 2;
        constexpr int rows = 2;
        const auto image = renderer.renderBlock({"8"}, columns, rows, 2.0F);

        THEN("the image spans the whole grid")
        {
            REQUIRE(image.getWidth() == columns * VfdSegmentRenderer::CELL_WIDTH * 2);
            REQUIRE(image.getHeight() == rows * VfdSegmentRenderer::CELL_HEIGHT * 2);
        }

        THEN("the cells with no character still show their unlit bed")
        {
            // The reference pads its text to the full grid; a cell left truly
            // black would read as a hole in the display.
            const auto cell = VfdSegmentRenderer::CELL_WIDTH * 2;
            const auto cellHeight = VfdSegmentRenderer::CELL_HEIGHT * 2;
            const auto lastCell = image.getClippedImage(
                {cell, cellHeight, cell, cellHeight});
            REQUIRE(maxGreen(lastCell) > 0.0);
        }
    }
}

// --- off-model primitives (DEC-JUC-052) ------------------------------------

namespace
{
    /// Of the rows between the first and last lit one, the fraction that are
    /// dark.
    ///
    /// Counting *runs* of lit rows does not separate a colon from a pipe: on a
    /// 16-segment cell a vertical bar is two stacked segments with the standard
    /// hairline between them, so both give two runs. What separates them is how
    /// much of their vertical extent is empty — a colon is mostly gap, a bar is
    /// mostly bar.
    double darkFractionOfSpan(const juce::Image& image, int threshold)
    {
        const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
        int first = -1;
        int last = -1;
        int darkRows = 0;
        std::vector<bool> lit(static_cast<std::size_t>(image.getHeight()), false);
        for (int y = 0; y < image.getHeight(); ++y)
        {
            int brightest = 0;
            for (int x = 0; x < image.getWidth(); ++x)
            {
                brightest = juce::jmax(brightest,
                                       static_cast<int>(pixels.getPixelColour(x, y).getGreen()));
            }
            lit[static_cast<std::size_t>(y)] = brightest >= threshold;
            if (lit[static_cast<std::size_t>(y)])
            {
                first = first < 0 ? y : first;
                last = y;
            }
        }
        if (first < 0 || last <= first)
        {
            return 0.0;
        }
        for (int y = first; y <= last; ++y)
        {
            darkRows += lit[static_cast<std::size_t>(y)] ? 0 : 1;
        }
        return static_cast<double>(darkRows) / static_cast<double>(last - first + 1);
    }

    /// Topmost row whose brightest pixel clears `threshold`, or -1.
    int firstLitRow(const juce::Image& image, int threshold)
    {
        const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
        for (int y = 0; y < image.getHeight(); ++y)
        {
            for (int x = 0; x < image.getWidth(); ++x)
            {
                if (pixels.getPixelColour(x, y).getGreen() >= threshold)
                {
                    return y;
                }
            }
        }
        return -1;
    }

    bool sameImage(const juce::Image& a, const juce::Image& b)
    {
        if (a.getWidth() != b.getWidth() || a.getHeight() != b.getHeight())
        {
            return false;
        }
        const juce::Image::BitmapData left(a, juce::Image::BitmapData::readOnly);
        const juce::Image::BitmapData right(b, juce::Image::BitmapData::readOnly);
        for (int y = 0; y < a.getHeight(); ++y)
        {
            for (int x = 0; x < a.getWidth(); ++x)
            {
                if (left.getPixelColour(x, y) != right.getPixelColour(x, y))
                {
                    return false;
                }
            }
        }
        return true;
    }

    constexpr int LIT_THRESHOLD = 120;   // well clear of the unlit bed (~22)
    constexpr float PROBE_SCALE = 8.0F;  // enough resolution to resolve a dot gap
}

SCENARIO("The colon is two dots, which no segment combination can make",
         "[RQ-GUI-033][RQ-GUI-049]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("':' and '|', which share one mask in the vendored table (0x2200)")
    {
        const auto colon = renderer.renderBlock({":"}, ONE_COLUMN, ONE_ROW, PROBE_SCALE);
        const auto pipe = renderer.renderBlock({"|"}, ONE_COLUMN, ONE_ROW, PROBE_SCALE);

        THEN("the colon is mostly gap — two marks, not a broken bar")
        {
            REQUIRE(darkFractionOfSpan(colon, LIT_THRESHOLD) > 0.2);
        }

        THEN("the pipe stays essentially continuous, correctly")
        {
            // The override splits the collision without spoiling '|': it keeps
            // the centre verticals, which is genuinely what a pipe looks like
            // on this hardware — two stacked segments with the standard
            // hairline, not two separated marks.
            REQUIRE(darkFractionOfSpan(pipe, LIT_THRESHOLD) < 0.1);
        }

        THEN("the two render differently")
        {
            REQUIRE_FALSE(sameImage(colon, pipe));
        }
    }
}

SCENARIO("Lowercase x is distinguishable from uppercase X", "[RQ-GUI-049]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("'x' and 'X', which share one mask in the vendored table (0x5500)")
    {
        const auto lower = renderer.renderBlock({"x"}, ONE_COLUMN, ONE_ROW, PROBE_SCALE);
        const auto upper = renderer.renderBlock({"X"}, ONE_COLUMN, ONE_ROW, PROBE_SCALE);

        THEN("they render differently")
        {
            REQUIRE_FALSE(sameImage(lower, upper));
        }

        THEN("the lowercase form sits in the lower half of the cell")
        {
            // Which is where a 16-segment cell draws its lowercase, and why
            // the table cannot express it: a crossing needs both diagonal
            // pairs and those start at the top corners.
            const auto lowerTop = firstLitRow(lower, LIT_THRESHOLD);
            const auto upperTop = firstLitRow(upper, LIT_THRESHOLD);
            REQUIRE(lowerTop > 0);
            REQUIRE(upperTop > 0);
            REQUIRE(lowerTop > upperTop);
            REQUIRE(lowerTop > lower.getHeight() / 3);
        }
    }
}

SCENARIO("The underscore sits below the glyph body", "[RQ-GUI-033]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("'_' and '-', one below the body and one across its middle")
    {
        const auto underscore = renderer.renderBlock({"_"}, ONE_COLUMN, ONE_ROW, PROBE_SCALE);
        const auto hyphen = renderer.renderBlock({"-"}, ONE_COLUMN, ONE_ROW, PROBE_SCALE);

        THEN("the underscore's bar is lower than the hyphen's")
        {
            // The vendored mask draws '_' with the bottom horizontals, which
            // lands it inside the body and reads as a strikethrough.
            REQUIRE(firstLitRow(underscore, LIT_THRESHOLD)
                    > firstLitRow(hyphen, LIT_THRESHOLD));
        }
    }
}

SCENARIO("The override table is the only divergence from the vendored data",
         "[RQ-GUI-033]")
{
    GIVEN("the printable ASCII range")
    {
        THEN("exactly ':', '_' and 'x' are overridden")
        {
            // DEC-JUC-052 requires the divergence to live in one auditable
            // place. If a fourth override appeared without this list changing,
            // the audit trail would be a lie — so the list is the assertion.
            for (int codePoint = FIRST_GLYPH; codePoint <= LAST_GLYPH; ++codePoint)
            {
                const auto expected = codePoint == ':' || codePoint == '_'
                                      || codePoint == 'x';
                INFO("code point " << codePoint);
                REQUIRE(VfdSegmentRenderer::hasOverride(codePoint) == expected);
            }
        }
    }
}

// --- ASCII coverage (RQ-GUI-049) -------------------------------------------

namespace
{
    /// The rendered cell as an opaque key, for distinctness comparisons.
    std::string cellSignature(const juce::Image& image)
    {
        const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
        std::string signature;
        signature.reserve(static_cast<std::size_t>(image.getWidth() * image.getHeight()));
        for (int y = 0; y < image.getHeight(); ++y)
        {
            for (int x = 0; x < image.getWidth(); ++x)
            {
                signature += static_cast<char>(pixels.getPixelColour(x, y).getGreen());
            }
        }
        return signature;
    }

    juce::String oneCharacter(int codePoint)
    {
        return juce::String::charToString(static_cast<juce::juce_wchar>(codePoint));
    }
}

SCENARIO("Every printable character renders, and no two render alike",
         "[RQ-GUI-049]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    // Scale 1 is the demanding case, not the easy one: it is the reference's
    // own 12x16 cell, where two shapes have the fewest pixels to differ in. If
    // distinctness holds here it holds at every larger scale.
    constexpr auto scales = std::to_array<float>({1.0F, 4.0F});

    for (const auto scale : scales)
    {
        GIVEN("all 95 printable code points rendered at scale " + std::to_string(scale))
        {
            std::map<std::string, std::vector<int>> byAppearance;
            for (int codePoint = FIRST_GLYPH; codePoint <= LAST_GLYPH; ++codePoint)
            {
                const auto image = renderer.renderBlock({oneCharacter(codePoint)},
                                                        ONE_COLUMN, ONE_ROW, scale);
                byAppearance[cellSignature(image)].push_back(codePoint);
            }

            THEN("all 95 are distinguishable from one another")
            {
                // The inherited sprite drew only 51 of these and left 44 cells
                // blank — every lowercase letter among them. The vendored table
                // covers all 95 but collides on ':'/'|' and 'x'/'X'; the
                // off-model primitives (DEC-JUC-052) split both. This is where
                // that claim is actually proved, at the pixel.
                for (const auto& [appearance, characters] : byAppearance)
                {
                    juce::ignoreUnused(appearance);
                    if (characters.size() > 1)
                    {
                        std::string clash;
                        for (const auto codePoint : characters)
                        {
                            clash += static_cast<char>(codePoint);
                        }
                        INFO("characters sharing one appearance: " << clash);
                        REQUIRE(characters.size() == 1);
                    }
                }
                REQUIRE(static_cast<int>(byAppearance.size()) == GLYPH_COUNT);
            }
        }
    }
}

SCENARIO("Lowercase letters are legible, where the sprite showed nothing",
         "[RQ-GUI-049]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("a tone name in lowercase, as a synth or a .syx file can supply")
    {
        // Dialogs.cpp uppercases a name on *rename*, but a name read from the
        // instrument is not normalised anywhere before it reaches the display.
        const auto blank = cellSignature(
            renderer.renderBlock({" "}, ONE_COLUMN, ONE_ROW, 4.0F));

        THEN("every letter differs from a blank cell")
        {
            for (char letter = 'a'; letter <= 'z'; ++letter)
            {
                INFO("letter '" << letter << "'");
                const auto image = renderer.renderBlock({juce::String::charToString(letter)},
                                                        ONE_COLUMN, ONE_ROW, 4.0F);
                REQUIRE(cellSignature(image) != blank);
            }
        }
    }
}

SCENARIO("Characters outside the printable range render as a space",
         "[RQ-GUI-033]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;
    const VfdSegmentRenderer renderer;

    GIVEN("code points below, above and well outside 32..126")
    {
        const auto blank = cellSignature(
            renderer.renderBlock({" "}, ONE_COLUMN, ONE_ROW, 2.0F));

        THEN("each renders exactly as the space does")
        {
            for (const auto codePoint : {FIRST_GLYPH - 1, LAST_GLYPH + 1, 0x20AC, 0x00E9})
            {
                INFO("code point " << codePoint);
                const auto image = renderer.renderBlock({oneCharacter(codePoint)},
                                                        ONE_COLUMN, ONE_ROW, 2.0F);
                REQUIRE(cellSignature(image) == blank);
            }
        }
    }
}
