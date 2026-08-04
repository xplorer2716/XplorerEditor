#include <catch2/catch_test_macros.hpp>

#include "BoundControls.hpp"
#include "DesignTokens.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"
#include "xplorer/app/ParameterBindingRegistry.hpp"

#include "xpl/midi/MockMidiBackend.hpp"

#include <algorithm>
#include <cmath>
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
//
// DEC-JUC-094 (2026-08-04): the side-by-side scenario used to assert
// `getHeight() == controlRowHeight`, which is an implementation detail and was
// in fact the defect -- it holds with the radios anywhere vertically, so it
// passed while they sat 3 px above their row. It is replaced by the property the
// owner actually reported: the radio INDICATOR shares a vertical centre with the
// check-box indicators of the same row. Both panels and both check boxes are
// read from the real control table, and the indicator geometry is computed with
// the rule XplorerLookAndFeel::drawToggleButton uses, so the guard fails if
// either the layout or that centring rule moves.
// [RQ-GUI-053, RQ-GUI-040, ADR-JUC-016 (DEC-JUC-086, DEC-JUC-094), ADR-JUC-014]

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

    const app::ControlSpec& specById(const std::string& id)
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
        REQUIRE(found != nullptr); // the caller's INFO names the control
        return *found;
    }

    const app::ControlSpec& panelSpec(const std::string& id)
    {
        const auto& spec = specById(id);
        REQUIRE(spec.kind == app::ControlKind::RadioButtonPanel);
        return spec;
    }

    /// Vertical centre of the indicator XplorerLookAndFeel::drawToggleButton
    /// paints for a toggle of height `height` whose top edge is at `y`: a box of
    /// min(INDICATOR_MAX, height) centred in the control's own bounds. Duplicated
    /// from the LookAndFeel deliberately -- the test lives in the JUCE-linked
    /// binary but must not depend on a paint call to state where the dot lands,
    /// and a second expression of the rule is what makes a change to it visible
    /// here. [RQ-GUI-053, ADR-JUC-016 (DEC-JUC-094)]
    constexpr int INDICATOR_MAX = 14;

    [[nodiscard]] double indicatorCentreY(int y, int height)
    {
        const int box = std::min(INDICATOR_MAX, height);
        return y + (height - box) / 2.0 + box / 2.0;
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
        // Each trigger panel and the GATED check box of its own row: the
        // alignment guard needs both, and both come from the table.
        for (const auto& [id, siblingId] :
             {std::pair{"ENV_X_TRIG_SINGLE_MULTI", "ENV_X_TRIG_GATED"},
              std::pair{"RAMP_X_TRIG_SINGLE_MULTI", "RAMP_X_TRIG_GATED"}})
        {
            INFO("panel: " << id);
            const LaidOutPanel panel(fixture.registry, id);
            const auto first = panel.button(0);
            const auto second = panel.button(1);

            THEN("the panel is indeed too short to stack them — the rule's input")
            {
                CHECK(panel.spec.height < 2 * rowHeight);
            }

            THEN("both radios sit on one row, filling the panel's own height")
            {
                // NOT controlRowHeight: the panel IS the row here, and its
                // height is extracted reference geometry. Pinning the button to
                // a design-system row height is what put the radios 3 px high.
                // [DEC-JUC-094]
                CHECK(first.getY() == 0);
                CHECK(second.getY() == 0);
                CHECK(first.getHeight() == panel.spec.height);
                CHECK(second.getHeight() == panel.spec.height);
            }

            THEN("the radio indicators share the row's vertical centre with the check boxes")
            {
                // The reported defect, in the terms it was reported: on screen
                // the radio dot sat above the tick boxes of the same row. The
                // panel straddles the 17 px row (ENV X: y=313 h=24 against
                // y=316 h=17), so taking its full height puts the indicator
                // back on the row's centre. [RQ-GUI-053, DEC-JUC-094]
                const auto& sibling = specById(siblingId);
                INFO("sibling check box: " << siblingId);

                const double checkBoxCentre = indicatorCentreY(sibling.y, sibling.height);
                const double firstCentre =
                    indicatorCentreY(panel.spec.y + first.getY(), first.getHeight());
                const double secondCentre =
                    indicatorCentreY(panel.spec.y + second.getY(), second.getHeight());

                CHECK(std::abs(firstCentre - checkBoxCentre) <= 1.0);
                CHECK(std::abs(secondCentre - checkBoxCentre) <= 1.0);
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
