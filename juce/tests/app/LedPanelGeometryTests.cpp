#include <catch2/catch_test_macros.hpp>

#include "DesignTokens.hpp"

#include "xplorer/app/ControlTable.hpp"

// indicatorSize is explicitly a first version, invited to be retuned after
// visual review (ADR-JUC-031, DEC-JUC-096) -- but it is bound by THREE
// independent ceilings, none of which the type system enforces:
//   - strictly smaller than a check-box indicator (14 px), so a read-only
//     lamp can never be mistaken for a settable control;
//   - at most twice the 5 px .NET reference size (owner allowance);
//   - three lamps plus spacing must fit the extracted _ledPanelControl
//     bounds (32x8), the binding limit -- 10 px (the owner's ceiling) does
//     not actually fit.
// This guard reads the real token and the real control-table spec, so a
// future retune that breaks any ceiling fails here instead of shipping a
// clipped or oversized lamp. [RQ-GUI-056, ADR-JUC-031]

using namespace xplorer;

namespace
{
    constexpr int LED_COUNT = 3;
    constexpr int CHECK_BOX_INDICATOR_MAX = 14;
    constexpr int REFERENCE_LED_SIZE = 5;

    const app::ControlSpec* ledPanelSpec()
    {
        for (const auto& spec : app::controlTable())
        {
            if (spec.kind == app::ControlKind::LedPanelControl)
            {
                return &spec;
            }
        }
        return nullptr;
    }
}

SCENARIO("The MIDI LED diameter respects every ceiling that bounds it",
         "[RQ-GUI-056][ADR-JUC-031]")
{
    GIVEN("the indicatorSize token and the real _ledPanelControl spec")
    {
        const int size = app::tokens::component::indicatorSize;
        const auto* panel = ledPanelSpec();
        REQUIRE(panel != nullptr);

        THEN("it is strictly smaller than the check-box indicator")
        {
            CHECK(size < CHECK_BOX_INDICATOR_MAX);
        }

        THEN("it does not exceed twice the .NET reference size")
        {
            CHECK(size <= 2 * REFERENCE_LED_SIZE);
        }

        THEN("a single lamp fits the extracted panel height")
        {
            CHECK(size <= panel->height);
        }

        THEN("three lamps plus their spacing fit the extracted panel width")
        {
            const int horizontalSpace = (panel->width - LED_COUNT * size) / (LED_COUNT + 1);
            CHECK(horizontalSpace >= 0);
        }
    }
}
