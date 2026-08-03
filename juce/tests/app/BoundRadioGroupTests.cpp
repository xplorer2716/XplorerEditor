#include <catch2/catch_test_macros.hpp>

#include "BoundControls.hpp"
#include "DesignTokens.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"
#include "xplorer/app/ParameterBindingRegistry.hpp"

#include "xpl/midi/MockMidiBackend.hpp"

#include <string>
#include <vector>

// The orientation rule of RQ-GUI-053: a radio panel lays its options out from
// its own extracted bounds, side by side when it is too short to stack them.
//
// This is a widget-geometry decision, so it is pinned against real
// juce::Component bounds rather than re-implemented arithmetic, and it is fed
// the REAL control-table geometry of the four shipped panels rather than
// literals — the whole point of DEC-JUC-086 is that the table is the input.
//
// The property that actually broke: under the unconditional stack, the second
// radio of a 24 px-high panel landed at y=12 with a 17 px height and so ran
// past the panel's own bottom edge. "Every button lies inside the panel" is
// therefore asserted in both orientations.
// [RQ-GUI-053, RQ-GUI-040, ADR-JUC-016 (DEC-JUC-086), ADR-JUC-014]

using namespace xplorer;
using namespace xpl::midi;

namespace
{
    struct Fixture
    {
        MockMidiBackend backend;
        settings::InMemorySettingsService settingsService;
        controller::XpanderController controller{backend, settingsService, nullptr, "TEST"};
        app::ParameterBindingRegistry registry{controller};
    };

    const app::ControlSpec& panelSpec(const std::string& id)
    {
        const app::ControlSpec* found = nullptr;
        for (const auto& spec : app::controlTable()) // span over a static array
        {
            if (id == spec.id)
            {
                found = &spec;
                break;
            }
        }
        REQUIRE(found != nullptr); // the caller's INFO names the panel

        REQUIRE(found->kind == app::ControlKind::RadioButtonPanel);
        return *found;
    }

    /// The widget laid out at the panel's own control-table bounds.
    struct LaidOutPanel
    {
        LaidOutPanel(app::ParameterBindingRegistry& registry, const std::string& id)
            : spec(panelSpec(id)), group(registry, id, app::radioPanelOptions(id))
        {
            REQUIRE(group.getNumChildComponents() == 2); // SINGLE/MULTI, VCO1/VCF, LINEAR/EXPO
            group.setBounds(spec.x, spec.y, spec.width, spec.height);
        }

        [[nodiscard]] juce::Rectangle<int> button(int index) const
        {
            return group.getChildComponent(index)->getBounds();
        }

        const app::ControlSpec& spec;
        app::BoundRadioGroup group;
    };
}

SCENARIO("A radio panel too short to stack its options lays them out side by side",
         "[RQ-GUI-053][RQ-GUI-040]")
{
    Fixture fixture;
    const int rowHeight = app::tokens::semantic::controlRowHeight;

    GIVEN("the two page-family trigger panels, at their control-table bounds")
    {
        for (const auto* id : {"ENV_X_TRIG_SINGLE_MULTI", "RAMP_X_TRIG_SINGLE_MULTI"})
        {
            INFO("panel: " << id);
            const LaidOutPanel panel(fixture.registry, id);
            const auto first = panel.button(0);
            const auto second = panel.button(1);

            THEN("the panel is indeed too short to stack them — the rule's input")
            {
                CHECK(panel.spec.height < 2 * rowHeight);
            }

            THEN("both radios sit on one control row")
            {
                CHECK(first.getY() == 0);
                CHECK(second.getY() == 0);
                CHECK(first.getHeight() == rowHeight);
                CHECK(second.getHeight() == rowHeight);
            }

            THEN("they are adjacent and do not overlap")
            {
                CHECK(first.getX() == 0);
                CHECK(second.getX() == first.getRight());
                CHECK(first.getWidth() == second.getWidth());
                CHECK(first.getWidth() == panel.spec.width / 2);
            }

            THEN("neither runs outside the panel — the defect of the stacked rule")
            {
                CHECK(panel.group.getLocalBounds().contains(first));
                CHECK(panel.group.getLocalBounds().contains(second));
            }
        }
    }
}

SCENARIO("A radio panel with room to stack its options keeps the vertical layout",
         "[RQ-GUI-053][RQ-GUI-038][RQ-GUI-040]")
{
    Fixture fixture;
    const int rowHeight = app::tokens::semantic::controlRowHeight;

    GIVEN("the two fixed-block panels, at their control-table bounds")
    {
        for (const auto* id : {"FM_DESTINATION", "LAG_TIMING_LINEAR_EXPO"})
        {
            INFO("panel: " << id);
            const LaidOutPanel panel(fixture.registry, id);
            const auto first = panel.button(0);
            const auto second = panel.button(1);

            THEN("the panel has room to stack them — the rule's input")
            {
                CHECK(panel.spec.height >= 2 * rowHeight);
            }

            THEN("the layout is the one shipped before RQ-GUI-053: full width, one slot each")
            {
                CHECK(first.getX() == 0);
                CHECK(second.getX() == 0);
                CHECK(first.getWidth() == panel.spec.width);
                CHECK(second.getWidth() == panel.spec.width);
                CHECK(first.getY() == 0);
                CHECK(second.getY() == panel.spec.height / 2);
                CHECK(first.getHeight() == rowHeight);
                CHECK(second.getHeight() == rowHeight);
            }

            THEN("neither runs outside the panel")
            {
                CHECK(panel.group.getLocalBounds().contains(first));
                CHECK(panel.group.getLocalBounds().contains(second));
            }
        }
    }
}
