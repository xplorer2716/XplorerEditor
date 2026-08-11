#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "DesignTokens.hpp"
#include "ShortcutIcons.hpp"

#include <array>
#include <juce_graphics/juce_graphics.h>

// The eight shortcut icons replaced 24 bitmaps whose only failure mode was being
// unreadable. Vector geometry has a different one: an icon can silently come out
// empty, escape its key, or end up drawn like its neighbour, and none of that
// shows up in a build log.
//
// So this suite pins the three properties that make an icon set work at 29 px —
// each icon draws something, stays inside its key, and is distinguishable from
// the other seven — plus the stroke type, which is load-bearing rather than
// cosmetic (JUCE's default mitred joint leaves a burr at this size).
// [RQ-GUI-063, RQ-GUI-064, ADR-GUI-001 (DEC-GUI-001-A, DEC-GUI-001-C)]

using namespace xplorer::app;

namespace
{
    constexpr std::array<ShortcutIcon, 8> ALL_ICONS{
        ShortcutIcon::PreviousProgram, ShortcutIcon::NextProgram,
        ShortcutIcon::GoToProgram,     ShortcutIcon::Randomise,
        ShortcutIcon::LoadFile,        ShortcutIcon::SaveFile,
        ShortcutIcon::StoreToSynth,    ShortcutIcon::MidiSettings};

    [[nodiscard]] juce::Rectangle<float> inkBounds(ShortcutIcon icon, float box)
    {
        const auto paths = shortcutIconPaths(icon, box);
        auto bounds = paths.stroked.getBounds();
        if (!paths.filled.isEmpty())
        {
            bounds = bounds.isEmpty() ? paths.filled.getBounds()
                                      : bounds.getUnion(paths.filled.getBounds());
        }
        return bounds;
    }
}

SCENARIO("Every shortcut icon draws something inside its key",
         "[RQ-GUI-064][TASK-GUI-001]")
{
    const float box = static_cast<float>(tokens::component::shortcutButtonSize);

    GIVEN("the eight icons built at the key's own size")
    {
        THEN("none of them is empty")
        {
            for (const auto icon : ALL_ICONS)
            {
                const auto paths = shortcutIconPaths(icon, box);
                REQUIRE_FALSE((paths.stroked.isEmpty() && paths.filled.isEmpty()));
            }
        }

        THEN("each stays within the key, stroke width included")
        {
            // Geometry is a centre line: half the stroke spills outside it, so the
            // check has to allow for that or it passes on artwork that is actually
            // clipped by the key's edge.
            const float halfStroke = shortcutIconStroke(box).getStrokeThickness() * 0.5F;
            for (const auto icon : ALL_ICONS)
            {
                const auto ink = inkBounds(icon, box).expanded(halfStroke);
                REQUIRE(ink.getX() >= 0.0F);
                REQUIRE(ink.getY() >= 0.0F);
                REQUIRE(ink.getRight() <= box);
                REQUIRE(ink.getBottom() <= box);
            }
        }

        THEN("and each leaves a margin, so eight keys in a row do not read as one band")
        {
            for (const auto icon : ALL_ICONS)
            {
                REQUIRE(inkBounds(icon, box).getWidth() < box * 0.90F);
            }
        }
    }
}

SCENARIO("No two shortcut icons are the same drawing", "[RQ-GUI-064][TASK-GUI-001]")
{
    const float box = static_cast<float>(tokens::component::shortcutButtonSize);

    GIVEN("the eight icons")
    {
        // Save and Store share a floppy on purpose — same act, different target —
        // and are told apart by colour, not shape, so this compares the six that
        // must differ geometrically and asserts the pair explicitly.
        THEN("the six distinct shapes all differ in their path data")
        {
            const std::array<ShortcutIcon, 6> distinct{
                ShortcutIcon::PreviousProgram, ShortcutIcon::NextProgram,
                ShortcutIcon::GoToProgram,     ShortcutIcon::Randomise,
                ShortcutIcon::LoadFile,        ShortcutIcon::MidiSettings};

            for (std::size_t i = 0; i < distinct.size(); ++i)
            {
                for (std::size_t j = i + 1; j < distinct.size(); ++j)
                {
                    const auto a = shortcutIconPaths(distinct[i], box);
                    const auto b = shortcutIconPaths(distinct[j], box);
                    REQUIRE(a.stroked.toString() != b.stroked.toString());
                }
            }
        }

        THEN("save and store are deliberately the same shape")
        {
            REQUIRE(shortcutIconPaths(ShortcutIcon::SaveFile, box).stroked.toString()
                    == shortcutIconPaths(ShortcutIcon::StoreToSynth, box).stroked.toString());
        }

        THEN("prev and next are mirror images, not the same triangle twice")
        {
            const auto prev = shortcutIconPaths(ShortcutIcon::PreviousProgram, box).stroked;
            const auto next = shortcutIconPaths(ShortcutIcon::NextProgram, box).stroked;
            REQUIRE(prev.toString() != next.toString());
            REQUIRE(prev.getBounds().getWidth() == next.getBounds().getWidth());
        }
    }
}

SCENARIO("Icon strokes are round-capped and curve-jointed", "[RQ-GUI-064][TASK-GUI-001]")
{
    GIVEN("the shared stroke type")
    {
        const auto stroke = shortcutIconStroke(
            static_cast<float>(tokens::component::shortcutButtonSize));

        // Stated, not defaulted: JUCE's mitred joint and butt cap leave a visible
        // burr on every corner at this size. [DEC-GUI-001-C]
        THEN("its joints are curved and its caps rounded")
        {
            REQUIRE(stroke.getJointStyle() == juce::PathStrokeType::curved);
            REQUIRE(stroke.getEndStyle() == juce::PathStrokeType::rounded);
        }

        THEN("its width is the design-system value at the design-system size")
        {
            REQUIRE(stroke.getStrokeThickness()
                    == Catch::Approx(tokens::component::shortcutIconStroke));
        }
    }

    GIVEN("a key rendered at twice the size")
    {
        // The row scales with the window (RQ-SCL-001), so the stroke has to scale
        // with the box rather than staying a fixed number of pixels.
        THEN("the stroke scales with it")
        {
            const float box = static_cast<float>(tokens::component::shortcutButtonSize);
            REQUIRE(shortcutIconStroke(box * 2.0F).getStrokeThickness()
                    == Catch::Approx(shortcutIconStroke(box).getStrokeThickness() * 2.0F));
        }
    }
}

SCENARIO("A hovered key lights in the panel LED blue", "[RQ-GUI-067][TASK-GUI-004]")
{
    // Colour choice is the whole requirement here, so this renders the key and
    // reads the pixels back rather than re-deriving the decision in the test.
    const int box = tokens::component::shortcutButtonSize;

    const auto render = [box](ShortcutIcon icon, bool hovered, bool down)
    {
        juce::Image image{juce::Image::ARGB, box, box, true};
        juce::Graphics g{image};
        paintShortcutButton(g, juce::Rectangle<float>{0.0F, 0.0F, static_cast<float>(box),
                                                      static_cast<float>(box)},
                            icon, hovered, down);
        return image;
    };
    // Mid-height on the left edge: always on the key's outline, never on an icon.
    const auto outlineOf = [box](const juce::Image& i) { return i.getPixelAt(0, box / 2); };

    GIVEN("a key that is not the destructive one")
    {
        THEN("hovering turns its outline to the hover ink")
        {
            const auto hovered = outlineOf(render(ShortcutIcon::Randomise, true, false));
            const auto resting = outlineOf(render(ShortcutIcon::Randomise, false, false));
            REQUIRE(hovered != resting);
            REQUIRE(hovered.getHue()
                    == Catch::Approx(tokens::component::shortcutButtonHoverInk.getHue())
                           .margin(0.02));
        }
    }

    GIVEN("the store key, which writes to the synth")
    {
        THEN("hovering keeps it in the red family, not the hover blue")
        {
            const auto hovered = render(ShortcutIcon::StoreToSynth, true, false);
            // Sample the icon itself: the outline is the shared hover colour on
            // every key, but the INK is what must stay red here. [RQ-GUI-067]
            bool foundRedInk = false;
            for (int y = 0; y < box && !foundRedInk; ++y)
            {
                for (int x = 0; x < box; ++x)
                {
                    const auto p = hovered.getPixelAt(x, y);
                    if (p.getSaturation() > 0.5F && p.getHue() < 0.08F)
                    {
                        foundRedInk = true;
                        break;
                    }
                }
            }
            REQUIRE(foundRedInk);
        }
    }
}
