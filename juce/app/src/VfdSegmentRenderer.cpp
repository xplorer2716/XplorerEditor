#include "VfdSegmentRenderer.hpp"

#include "DesignTokens.hpp"

#include <algorithm>
#include <iterator>
#include <cmath>

namespace xplorer::app
{
    namespace
    {
        namespace vfd = tokens::component;

        /// Rail positions, all fractions of the cell width (RQ-DSN-097).
        constexpr float RAIL_LEFT = vfd::vfdSegLeft;
        constexpr float RAIL_RIGHT = vfd::vfdSegRight;
        constexpr float RAIL_TOP = vfd::vfdSegTop;
        constexpr float RAIL_BOTTOM = vfd::vfdSegBottom;
        constexpr float RAIL_CENTRE_X = (RAIL_LEFT + RAIL_RIGHT) * 0.5F;
        constexpr float RAIL_MIDDLE_Y = (RAIL_TOP + RAIL_BOTTOM) * 0.5F;

        /// A segment shorter than this many gap-widths keeps a proportional
        /// gap instead of a full one, so a short segment is never eaten whole.
        constexpr float MIN_GAP_RATIO = 2.2F;

        /// Gaussian support: beyond three sigma the tail contributes nothing a
        /// byte can represent.
        constexpr float KERNEL_SIGMAS = 3.0F;

        constexpr float CHANNEL_MAX = 255.0F;
        constexpr float FULLY_LIT = 1.0F;

        struct SegmentLine
        {
            float x0, y0, x1, y1;
        };

        /// Endpoints of the 16 segments, in cell-width fractions. Order matches
        /// the Segment enum: A..H outer ring clockwise from top-left, then the
        /// inner K,M,N,P,R,S,T,U. [SegmentFont.hpp documents the layout]
        constexpr std::array<SegmentLine, SEGMENT_COUNT> SEGMENT_LINES{{
            {RAIL_LEFT, RAIL_TOP, RAIL_CENTRE_X, RAIL_TOP},          // A
            {RAIL_CENTRE_X, RAIL_TOP, RAIL_RIGHT, RAIL_TOP},         // B
            {RAIL_RIGHT, RAIL_TOP, RAIL_RIGHT, RAIL_MIDDLE_Y},       // C
            {RAIL_RIGHT, RAIL_MIDDLE_Y, RAIL_RIGHT, RAIL_BOTTOM},    // D
            {RAIL_CENTRE_X, RAIL_BOTTOM, RAIL_RIGHT, RAIL_BOTTOM},   // E
            {RAIL_LEFT, RAIL_BOTTOM, RAIL_CENTRE_X, RAIL_BOTTOM},    // F
            {RAIL_LEFT, RAIL_MIDDLE_Y, RAIL_LEFT, RAIL_BOTTOM},      // G
            {RAIL_LEFT, RAIL_TOP, RAIL_LEFT, RAIL_MIDDLE_Y},         // H
            {RAIL_LEFT, RAIL_TOP, RAIL_CENTRE_X, RAIL_MIDDLE_Y},     // K
            {RAIL_CENTRE_X, RAIL_TOP, RAIL_CENTRE_X, RAIL_MIDDLE_Y}, // M
            {RAIL_RIGHT, RAIL_TOP, RAIL_CENTRE_X, RAIL_MIDDLE_Y},    // N
            {RAIL_CENTRE_X, RAIL_MIDDLE_Y, RAIL_RIGHT, RAIL_MIDDLE_Y},   // P
            {RAIL_RIGHT, RAIL_BOTTOM, RAIL_CENTRE_X, RAIL_MIDDLE_Y}, // R
            {RAIL_CENTRE_X, RAIL_MIDDLE_Y, RAIL_CENTRE_X, RAIL_BOTTOM},  // S
            {RAIL_LEFT, RAIL_BOTTOM, RAIL_CENTRE_X, RAIL_MIDDLE_Y},  // T
            {RAIL_LEFT, RAIL_MIDDLE_Y, RAIL_CENTRE_X, RAIL_MIDDLE_Y},    // U
        }};

        /// Italic shear, pivoting on the bottom rail so the baseline stays put.
        constexpr float sheared(float x, float y) noexcept
        {
            return x + vfd::vfdSegSlant * (RAIL_BOTTOM - y);
        }

        /// A sheared square centred on (x, y), used for the colon's dots.
        juce::Path dotAt(float x, float y, float side)
        {
            const auto half = side * 0.5F;
            juce::Path dot;
            dot.startNewSubPath(sheared(x - half, y - half), y - half);
            dot.lineTo(sheared(x + half, y - half), y - half);
            dot.lineTo(sheared(x + half, y + half), y + half);
            dot.lineTo(sheared(x - half, y + half), y + half);
            dot.closeSubPath();
            return dot;
        }

        /// A sheared stroke between two points, matching the segments' weight.
        void addStroke(juce::Path& path, float x0, float y0, float x1, float y1)
        {
            path.addLineSegment({sheared(x0, y0), y0, sheared(x1, y1), y1},
                                vfd::vfdSegStroke);
        }

        /// The off-model primitive for `codePoint`. [DEC-JUC-052]
        juce::Path buildOverride(int codePoint)
        {
            juce::Path path;
            switch (codePoint)
            {
                case ':':
                    // Two disconnected marks — the one shape a segment display
                    // fundamentally cannot form.
                    path.addPath(dotAt(vfd::vfdDotX, vfd::vfdDotUpperY, vfd::vfdDotSize));
                    path.addPath(dotAt(vfd::vfdDotX, vfd::vfdDotLowerY, vfd::vfdDotSize));
                    break;

                case '_':
                    // Below the bottom rail, so outside the segment geometry
                    // entirely. The vendored table approximates it with the two
                    // bottom horizontals, which reads as a strikethrough.
                    addStroke(path, RAIL_LEFT, vfd::vfdUnderscoreY,
                              RAIL_RIGHT, vfd::vfdUnderscoreY);
                    break;

                case 'x':
                    // A crossing confined to the lower half, which is where a
                    // 16-segment cell draws its lowercase. Built from the
                    // existing rails, so it needs no geometry of its own.
                    addStroke(path, RAIL_LEFT, RAIL_MIDDLE_Y, RAIL_RIGHT, RAIL_BOTTOM);
                    addStroke(path, RAIL_RIGHT, RAIL_MIDDLE_Y, RAIL_LEFT, RAIL_BOTTOM);
                    break;

                default:
                    jassertfalse; // OVERRIDDEN_CHARACTERS and this switch disagree
                    break;
            }
            return path;
        }

        /// One-dimensional Gaussian weights, normalised to sum to 1.
        std::vector<float> gaussianKernel(float sigma)
        {
            const int radius = std::max(1, static_cast<int>(std::ceil(KERNEL_SIGMAS * sigma)));
            std::vector<float> kernel(static_cast<std::size_t>(2 * radius + 1));
            float sum = 0.0F;
            for (int i = -radius; i <= radius; ++i)
            {
                const auto weight = std::exp(-static_cast<float>(i * i) / (2.0F * sigma * sigma));
                kernel[static_cast<std::size_t>(i + radius)] = weight;
                sum += weight;
            }
            for (auto& weight : kernel)
            {
                weight /= sum;
            }
            return kernel;
        }

        /// Separable Gaussian blur, in place, via one scratch buffer.
        ///
        /// Hand-rolled because JUCE offers neither: `ImageConvolutionKernel` is
        /// a 2D convolution, which for this radius costs ~20x a separable pass
        /// for an identical result, and `applySingleChannelBoxBlurEffect` is a
        /// box blur — a different kernel, which would silently diverge from the
        /// Gaussian the tokens were fitted against (DEC-JUC-054).
        void blur(std::vector<float>& field, int width, int height, float sigma)
        {
            const auto kernel = gaussianKernel(sigma);
            const auto radius = static_cast<int>(kernel.size()) / 2;
            std::vector<float> scratch(field.size(), 0.0F);

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    float accumulated = 0.0F;
                    for (int k = -radius; k <= radius; ++k)
                    {
                        const int sampleX = x + k;
                        if (sampleX >= 0 && sampleX < width)
                        {
                            accumulated += kernel[static_cast<std::size_t>(k + radius)]
                                           * field[static_cast<std::size_t>(y * width + sampleX)];
                        }
                    }
                    scratch[static_cast<std::size_t>(y * width + x)] = accumulated;
                }
            }

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    float accumulated = 0.0F;
                    for (int k = -radius; k <= radius; ++k)
                    {
                        const int sampleY = y + k;
                        if (sampleY >= 0 && sampleY < height)
                        {
                            accumulated += kernel[static_cast<std::size_t>(k + radius)]
                                           * scratch[static_cast<std::size_t>(sampleY * width + x)];
                        }
                    }
                    field[static_cast<std::size_t>(y * width + x)] = accumulated;
                }
            }
        }

        /// Coverage of a single-channel image as a float field in 0..1.
        std::vector<float> readCoverage(const juce::Image& image, int width, int height)
        {
            std::vector<float> field(static_cast<std::size_t>(width * height), 0.0F);
            const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
            for (int y = 0; y < height; ++y)
            {
                const auto* row = pixels.getLinePointer(y);
                for (int x = 0; x < width; ++x)
                {
                    field[static_cast<std::size_t>(y * width + x)] =
                        static_cast<float>(row[x]) / CHANNEL_MAX;
                }
            }
            return field;
        }
    }

    VfdSegmentRenderer::VfdSegmentRenderer()
    {
        for (std::size_t index = 0; index < SEGMENT_LINES.size(); ++index)
        {
            const auto& line = SEGMENT_LINES[index];
            const auto dx = line.x1 - line.x0;
            const auto dy = line.y1 - line.y0;
            const auto length = std::sqrt(dx * dx + dy * dy);
            const auto unitX = dx / length;
            const auto unitY = dy / length;
            const auto gap = std::min(vfd::vfdSegGap, length / MIN_GAP_RATIO);

            const auto startX = line.x0 + unitX * gap;
            const auto startY = line.y0 + unitY * gap;
            const auto endX = line.x1 - unitX * gap;
            const auto endY = line.y1 - unitY * gap;

            _segments[index].addLineSegment({sheared(startX, startY), startY,
                                             sheared(endX, endY), endY},
                                            vfd::vfdSegStroke);
        }

        for (std::size_t index = 0; index < OVERRIDDEN_CHARACTERS.size(); ++index)
        {
            _overrides[index] = buildOverride(OVERRIDDEN_CHARACTERS[index]);
        }
    }

    bool VfdSegmentRenderer::hasOverride(int codePoint) noexcept
    {
        return std::find(OVERRIDDEN_CHARACTERS.begin(), OVERRIDDEN_CHARACTERS.end(),
                         codePoint)
               != OVERRIDDEN_CHARACTERS.end();
    }

    void VfdSegmentRenderer::paintGlyph(juce::Graphics& target,
                                        int codePoint,
                                        float originX,
                                        float originY,
                                        float cellWidth) const
    {
        // One uniform scale for both axes: the normalised cell keeps the 12:16
        // aspect precisely so this stays isotropic.
        const auto placement = juce::AffineTransform::scale(cellWidth)
                                   .translated(originX, originY);

        const auto overridden = std::find(OVERRIDDEN_CHARACTERS.begin(),
                                          OVERRIDDEN_CHARACTERS.end(), codePoint);
        if (overridden != OVERRIDDEN_CHARACTERS.end())
        {
            const auto index = static_cast<std::size_t>(
                std::distance(OVERRIDDEN_CHARACTERS.begin(), overridden));
            target.fillPath(_overrides[index], placement);
            return;
        }

        const auto mask = segmentMaskFor(codePoint);
        for (int bit = 0; bit < SEGMENT_COUNT; ++bit)
        {
            if (isLit(mask, static_cast<Segment>(bit)))
            {
                target.fillPath(_segments[static_cast<std::size_t>(bit)], placement);
            }
        }
    }

    void VfdSegmentRenderer::paintUnlitBed(juce::Graphics& target,
                                           float originX,
                                           float originY,
                                           float cellWidth) const
    {
        // The bed is the segments only. The off-model primitives are not part
        // of it: a real display's unlit state is its segments, and lighting a
        // ghost colon in every cell would show marks the hardware never has.
        const auto placement = juce::AffineTransform::scale(cellWidth)
                                   .translated(originX, originY);
        for (const auto& segment : _segments)
        {
            target.fillPath(segment, placement);
        }
    }

    juce::Image VfdSegmentRenderer::renderBlock(const juce::StringArray& lines,
                                                int columns,
                                                int rows,
                                                float scale) const
    {
        const auto cellWidth = static_cast<float>(CELL_WIDTH) * scale;
        const auto cellHeight = static_cast<float>(CELL_HEIGHT) * scale;
        const auto width = juce::roundToInt(static_cast<float>(columns) * cellWidth);
        const auto height = juce::roundToInt(static_cast<float>(rows) * cellHeight);
        if (width <= 0 || height <= 0)
        {
            return {};
        }

        // Two coverage passes: the lit segments, and every segment of every
        // cell for the always-drawn unlit bed. Both are plain masks; all the
        // light behaviour happens afterwards, on the scalar field.
        juce::Image litMask(juce::Image::SingleChannel, width, height, true);
        juce::Image bedMask(juce::Image::SingleChannel, width, height, true);
        {
            juce::Graphics litGraphics(litMask);
            juce::Graphics bedGraphics(bedMask);
            litGraphics.setColour(juce::Colours::white);
            bedGraphics.setColour(juce::Colours::white);

            for (int row = 0; row < rows; ++row)
            {
                const auto line = row < lines.size() ? lines[row] : juce::String();
                for (int column = 0; column < columns; ++column)
                {
                    const auto originX = static_cast<float>(column) * cellWidth;
                    const auto originY = static_cast<float>(row) * cellHeight;
                    const auto character = column < line.length()
                                               ? static_cast<int>(line[column])
                                               : static_cast<int>(' ');
                    paintGlyph(litGraphics, character, originX, originY, cellWidth);
                    paintUnlitBed(bedGraphics, originX, originY, cellWidth);
                }
            }
        }

        auto field = readCoverage(litMask, width, height);
        const auto bed = readCoverage(bedMask, width, height);

        // Glow: the radius is a fraction of the cell, so it must be taken to
        // device pixels through the same scale as the geometry. Skipping this
        // is the classic way a "resolution independent" glow silently shrinks
        // as the window grows. [DEC-JUC-053]
        auto glow = field;
        blur(glow, width, height, vfd::vfdGlowRadius * cellWidth);

        juce::Image out(juce::Image::ARGB, width, height, true);
        {
            const juce::Image::BitmapData pixels(out, juce::Image::BitmapData::writeOnly);
            const auto phosphor = vfd::vfdPhosphor;
            const auto baseRed = phosphor.getFloatRed();
            const auto baseGreen = phosphor.getFloatGreen();
            const auto baseBlue = phosphor.getFloatBlue();

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    const auto index = static_cast<std::size_t>(y * width + x);
                    // Core plus glow, floored by the unlit bed. The bed is a
                    // floor and not a summand: an unlit segment sits at a fixed
                    // low level, it does not brighten what crosses it.
                    auto intensity = field[index] + vfd::vfdGlowAmount * glow[index];
                    intensity = std::max(intensity, bed[index] * vfd::vfdUnlitLevel);

                    // One hue below full scale; above it the excess washes
                    // toward white, which is what the reference's red channel
                    // rising only at the core actually is. [DEC-JUC-054]
                    const auto below = std::min(intensity, FULLY_LIT);
                    const auto above = std::max(intensity - FULLY_LIT, 0.0F);
                    const auto lift = vfd::vfdWhiteLift * above;
                    const auto red = below * baseRed + lift * (FULLY_LIT - baseRed);
                    const auto green = below * baseGreen + lift * (FULLY_LIT - baseGreen);
                    const auto blue = below * baseBlue + lift * (FULLY_LIT - baseBlue);

                    pixels.setPixelColour(x, y,
                                          juce::Colour::fromFloatRGBA(std::min(red, FULLY_LIT),
                                                                      std::min(green, FULLY_LIT),
                                                                      std::min(blue, FULLY_LIT),
                                                                      FULLY_LIT));
                }
            }
        }
        return out;
    }
}
