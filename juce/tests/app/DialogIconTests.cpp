#include <catch2/catch_test_macros.hpp>

#include "DesignTokens.hpp"
#include "DialogIcons.hpp"

// The OS title bar for Settings/About/Dependencies used to show a generic
// icon: useNativeTitleBar is native chrome, and nothing ever called
// DocumentWindow::setIcon(). A rendered icon has one failure mode a build log
// won't catch — it silently comes out blank, or Settings and About render the
// same badge with no glyph telling them apart — so this suite pins that the
// image is the requested size, is not blank, carries the badge colour, and
// that the two dialog icons are distinguishable from each other.
// [RQ-GUI-070, ADR-GUI-001 (DEC-GUI-001-A)]

using namespace xplorer::app;

namespace
{
    bool hasNonTransparentPixel(const juce::Image& image)
    {
        for (int y = 0; y < image.getHeight(); ++y)
        {
            for (int x = 0; x < image.getWidth(); ++x)
            {
                if (image.getPixelAt(x, y).getAlpha() > 0)
                {
                    return true;
                }
            }
        }
        return false;
    }
}

SCENARIO("Each dialog title-bar icon renders at the requested size, non-blank",
         "[RQ-GUI-070]")
{
    const int size = tokens::component::dialogIconSize;

    GIVEN("the Settings and About icons")
    {
        for (const auto icon : {DialogIcon::Settings, DialogIcon::About})
        {
            const auto image = dialogTitleBarIcon(icon, size);

            THEN("the image is exactly the requested square size")
            {
                REQUIRE(image.getWidth() == size);
                REQUIRE(image.getHeight() == size);
            }

            THEN("something was actually drawn")
            {
                REQUIRE(hasNonTransparentPixel(image));
            }
        }
    }

    GIVEN("a larger requested size")
    {
        THEN("the image scales to it, not just the token default")
        {
            const auto image = dialogTitleBarIcon(DialogIcon::Settings, size * 2);
            REQUIRE(image.getWidth() == size * 2);
            REQUIRE(image.getHeight() == size * 2);
        }
    }
}

SCENARIO("The badge is the design-system icon accent, not an arbitrary colour",
         "[RQ-GUI-070]")
{
    GIVEN("either dialog icon")
    {
        THEN("a corner of its circular badge is the exact accent colour")
        {
            const int size = tokens::component::dialogIconSize;
            for (const auto icon : {DialogIcon::Settings, DialogIcon::About})
            {
                const auto image = dialogTitleBarIcon(icon, size);
                // A corner of the square sits inside the circular badge fill
                // but outside both icons' glyph geometry (gear teeth and the
                // "i" are both centred, well clear of the corners).
                const auto corner = image.getPixelAt(2, 2);
                REQUIRE(corner.getARGB() == tokens::component::dialogIconAccent.getARGB());
            }
        }
    }
}

SCENARIO("Settings and About render different glyphs", "[RQ-GUI-070]")
{
    GIVEN("both icons rendered at the same size")
    {
        THEN("they are not pixel-identical")
        {
            const int size = tokens::component::dialogIconSize;
            const auto settingsIcon = dialogTitleBarIcon(DialogIcon::Settings, size);
            const auto aboutIcon = dialogTitleBarIcon(DialogIcon::About, size);

            bool foundDifference = false;
            for (int y = 0; y < size && !foundDifference; ++y)
            {
                for (int x = 0; x < size; ++x)
                {
                    if (settingsIcon.getPixelAt(x, y) != aboutIcon.getPixelAt(x, y))
                    {
                        foundDifference = true;
                        break;
                    }
                }
            }
            REQUIRE(foundDifference);
        }
    }
}
