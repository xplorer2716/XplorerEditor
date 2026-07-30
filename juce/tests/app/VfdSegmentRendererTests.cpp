#include <catch2/catch_test_macros.hpp>

#include "DesignTokens.hpp"
#include "VfdSegmentRenderer.hpp"

#include <juce_graphics/juce_graphics.h>

#include <cmath>

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
        const auto atTwo = meanGreen(renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, 2.0F));
        const auto atThree = meanGreen(renderer.renderBlock(lines, ONE_COLUMN, ONE_ROW, 3.0F));

        THEN("the light per unit area stays essentially constant")
        {
            REQUIRE(atOne > 0.0);
            constexpr double tolerance = 0.15; // 15% of the scale-1 value
            REQUIRE(std::abs(atTwo - atOne) / atOne < tolerance);
            REQUIRE(std::abs(atThree - atOne) / atOne < tolerance);
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
