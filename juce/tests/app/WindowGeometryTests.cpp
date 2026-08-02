#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/ControlTable.hpp"

#include <cmath>
#include <iterator>

// Window sizing is pure arithmetic and lives in the headless layer precisely
// so it can be pinned here, with no JUCE window and no screenshot: the launch
// size and every View-menu preset come through one function, so one set of
// assertions covers both. [RQ-SCL-001, RQ-SCL-002, ADR-JUC-025 (DEC-JUC-063,
// DEC-JUC-066, DEC-JUC-068)]

using namespace xplorer::app;

SCENARIO("The window size for a display scale is derived from the 1x width", "[RQ-SCL-001]")
{
    GIVEN("the reference 1x width")
    {
        THEN("it is a window width, not the canvas coordinate grid")
        {
            // The distinction DEC-JUC-068 turns on: these are different
            // quantities and must not drift into being the same one.
            CHECK(WINDOW_WIDTH_AT_1X == 1440);
            CHECK(LOGICAL_CANVAS_WIDTH == 1260);
        }
    }

    GIVEN("the 1x scale")
    {
        const auto size = windowSizeForScale(1.0F);

        THEN("the window content is 1440x922")
        {
            CHECK(size.width == 1440);
            CHECK(size.height == 922);
        }

        THEN("the height carries the menu bar on top of the canvas area")
        {
            CHECK(size.height - MENU_BAR_HEIGHT == 898);
        }
    }

    GIVEN("each View-menu preset")
    {
        THEN("it yields the sizes RQ-SCL-002 states")
        {
            // Restated here rather than recomputed, so a change to the formula
            // has to face the numbers the requirement promises the user.
            CHECK(windowSizeForScale(1.0F).width == 1440);
            CHECK(windowSizeForScale(1.0F).height == 922);
            CHECK(windowSizeForScale(1.25F).width == 1800);
            CHECK(windowSizeForScale(1.25F).height == 1147);
            CHECK(windowSizeForScale(1.5F).width == 2160);
            CHECK(windowSizeForScale(1.5F).height == 1371);
            CHECK(windowSizeForScale(1.75F).width == 2520);
            CHECK(windowSizeForScale(1.75F).height == 1596);
            CHECK(windowSizeForScale(2.0F).width == 2880);
            CHECK(windowSizeForScale(2.0F).height == 1821);
        }
    }

    GIVEN("the 1.75x preset")
    {
        const auto size = windowSizeForScale(1.75F);

        THEN("its canvas area is exactly twice the logical canvas")
        {
            // 2520 = 2 x 1260, so this preset alone lands on an exact canvas
            // render scale. Pinned because it is a coincidence worth noticing
            // if it ever breaks — NOT a property the ratios were chosen for,
            // and not what makes the VFD crisp (that is ADR-JUC-026).
            CHECK(size.width == 2 * LOGICAL_CANVAS_WIDTH);
            CHECK(size.height - MENU_BAR_HEIGHT == 2 * LOGICAL_CANVAS_HEIGHT);
        }
    }

    GIVEN("any scale")
    {
        THEN("the canvas area keeps the canvas aspect ratio to within a pixel")
        {
            for (const auto scale : WINDOW_SCALE_PRESETS)
            {
                const auto size = windowSizeForScale(scale);
                const auto canvasHeight = size.height - MENU_BAR_HEIGHT;
                const auto expected = static_cast<double>(size.width) * LOGICAL_CANVAS_HEIGHT
                                      / LOGICAL_CANVAS_WIDTH;
                CHECK(std::abs(canvasHeight - expected) <= 0.5);
            }
        }
    }
}

SCENARIO("A preset is identified by the window's exact content width", "[RQ-SCL-002]")
{
    GIVEN("the preset widths")
    {
        THEN("each is distinct, so a width identifies at most one preset")
        {
            for (const auto outer : WINDOW_SCALE_PRESETS)
            {
                int matches = 0;
                for (const auto inner : WINDOW_SCALE_PRESETS)
                {
                    if (windowSizeForScale(outer).width == windowSizeForScale(inner).width)
                    {
                        ++matches;
                    }
                }
                CHECK(matches == 1);
            }
        }

        THEN("a width one pixel either side of a preset matches nothing")
        {
            // The checkmark is an integer comparison with no tolerance
            // (DEC-JUC-066): a dragged or OS-clamped window must not be
            // reported as sitting at a preset.
            for (const auto scale : WINDOW_SCALE_PRESETS)
            {
                const auto width = windowSizeForScale(scale).width;
                for (const auto probe : {width - 1, width + 1})
                {
                    bool matched = false;
                    for (const auto candidate : WINDOW_SCALE_PRESETS)
                    {
                        matched = matched || windowSizeForScale(candidate).width == probe;
                    }
                    CHECK_FALSE(matched);
                }
            }
        }
    }

    GIVEN("the presets in menu order")
    {
        THEN("they are the five ratios RQ-SCL-002 lists, ascending")
        {
            REQUIRE(std::size(WINDOW_SCALE_PRESETS) == 5);
            CHECK(WINDOW_SCALE_PRESETS[0] == 1.0F);
            CHECK(WINDOW_SCALE_PRESETS[1] == 1.25F);
            CHECK(WINDOW_SCALE_PRESETS[2] == 1.5F);
            CHECK(WINDOW_SCALE_PRESETS[3] == 1.75F);
            CHECK(WINDOW_SCALE_PRESETS[4] == 2.0F);
        }

        THEN("the launch size is the first of them")
        {
            // RQ-SCL-001: the window opens at 1x, so the View menu shows 1x
            // ticked at startup rather than nothing.
            CHECK(windowSizeForScale(WINDOW_SCALE_PRESETS[0]).width == WINDOW_WIDTH_AT_1X);
        }
    }
}
