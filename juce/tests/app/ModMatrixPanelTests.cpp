#include <catch2/catch_test_macros.hpp>

#include "ModMatrixPanel.hpp"

#include "xplorer/app/ControlTable.hpp"
#include "xplorer/settings/SettingsService.hpp"

#include "xpl/midi/MockMidiBackend.hpp"
#include "xpl/util/EnumUtils.hpp"

#include <juce_gui_basics/juce_gui_basics.h>

// The item lists of a maxed-out row's source/destination combos SHALL never
// contain a choice that would push a destination past the RQ-GUI-016
// 6-source cap -- keeping the invalid choice out of the list is what makes
// it unreachable by both a pointer click and an arrow key (nudgeSelectedItem
// never opens the popup). [RQ-GUI-016, ADR-JUC-036 (DEC-JUC-122)]

using namespace xplorer;
using namespace xpl::midi;

namespace
{
    const app::ControlSpec* specFor(const std::string& id)
    {
        for (const auto& spec : app::controlTable())
        {
            if (id == spec.id)
            {
                return &spec;
            }
        }
        return nullptr;
    }

    juce::ComboBox* comboAt(const juce::Component& parent, const app::ControlSpec& spec)
    {
        const juce::Rectangle<int> bounds(spec.x, spec.y, spec.width, spec.height);
        for (int i = 0; i < parent.getNumChildComponents(); ++i)
        {
            auto* combo = dynamic_cast<juce::ComboBox*>(parent.getChildComponent(i));
            if (combo != nullptr && combo->getBounds() == bounds)
            {
                return combo;
            }
        }
        return nullptr;
    }

    bool comboOffersId(const juce::ComboBox& combo, int itemId)
    {
        for (int i = 0; i < combo.getNumItems(); ++i)
        {
            if (combo.getItemId(i) == itemId)
            {
                return true;
            }
        }
        return false;
    }

    struct Fixture
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;
        MockMidiBackend backend;
        settings::InMemorySettingsService settingsService;
        controller::XpanderController controller{backend, settingsService, nullptr, "TEST"};
        juce::Component parent;
        app::ModMatrixPanel panel{parent, controller};
    };
}

SCENARIO("A row with no source of its own offers only NONE once its destination is saturated",
         "[RQ-GUI-016][ADR-JUC-036]")
{
    GIVEN("six entries already sourced to the same destination")
    {
        Fixture f;
        for (int entryNumber = 1; entryNumber <= 6; ++entryNumber)
        {
            f.controller.changeModulationSource(
                xpl::util::toUnderlying(model::EnumModulationSourcesModMatrix::LFO1), 10, 0,
                xpl::util::toUnderlying(model::EnumModulationDestinations::VCF_FRQ), entryNumber);
        }
        // Entry 7 already targets the now-saturated destination but has no
        // source of its own -- sourceAvailabilityForEntry reads the ROW'S
        // OWN destination, so this is the real shape of "can't add a 7th",
        // not merely "something, somewhere, is saturated".
        f.controller.changeModulationDestination(
            xpl::util::toUnderlying(model::EnumModulationSourcesModMatrix::NONE), 0, 0,
            xpl::util::toUnderlying(model::EnumModulationDestinations::VCO1_FRQ),
            xpl::util::toUnderlying(model::EnumModulationDestinations::VCF_FRQ), 7);
        f.panel.refreshAll();

        const auto* sourceSpec = specFor("MOD_SRC_7");
        REQUIRE(sourceSpec != nullptr);
        auto* sourceCombo = comboAt(f.parent, *sourceSpec);
        REQUIRE(sourceCombo != nullptr);

        THEN("its source combo carries exactly one item: NONE")
        {
            CHECK(sourceCombo->getNumItems() == 1);
            CHECK(sourceCombo->getSelectedId() - 1
                  == xpl::util::toUnderlying(model::EnumModulationSourcesModMatrix::NONE));
        }

        // Entry 8 stays untouched (default destination, no source): the
        // control for "excluded from a row that is not itself pointed at
        // the saturated destination".
        const auto* destSpec = specFor("MOD_DEST_8");
        REQUIRE(destSpec != nullptr);
        auto* destCombo = comboAt(f.parent, *destSpec);
        REQUIRE(destCombo != nullptr);

        THEN("the saturated destination is absent from entry 8's destination list")
        {
            const int saturatedId = xpl::util::toUnderlying(model::EnumModulationDestinations::VCF_FRQ) + 1;
            CHECK_FALSE(comboOffersId(*destCombo, saturatedId));
        }

        const auto* sourceSpec1 = specFor("MOD_SRC_1");
        REQUIRE(sourceSpec1 != nullptr);
        auto* sourceCombo1 = comboAt(f.parent, *sourceSpec1);
        REQUIRE(sourceCombo1 != nullptr);

        THEN("one of the six entries already contributing a source keeps the full source list")
        {
            CHECK(sourceCombo1->getNumItems() > 1);
        }

        const auto* destSpec1 = specFor("MOD_DEST_1");
        REQUIRE(destSpec1 != nullptr);
        auto* destCombo1 = comboAt(f.parent, *destSpec1);
        REQUIRE(destCombo1 != nullptr);

        THEN("the saturated destination stays offered to one of the six entries that already targets it")
        {
            const int saturatedId = xpl::util::toUnderlying(model::EnumModulationDestinations::VCF_FRQ) + 1;
            CHECK(comboOffersId(*destCombo1, saturatedId));
        }
    }

    GIVEN("no destination anywhere near the cap")
    {
        Fixture f;

        const auto* sourceSpec = specFor("MOD_SRC_1");
        REQUIRE(sourceSpec != nullptr);
        auto* sourceCombo = comboAt(f.parent, *sourceSpec);
        REQUIRE(sourceCombo != nullptr);

        THEN("every row's source combo offers the full list")
        {
            CHECK(sourceCombo->getNumItems() > 1);
        }
    }
}
