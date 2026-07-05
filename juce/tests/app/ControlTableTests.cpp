#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/ControlTable.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include <map>
#include <set>
#include <string>

using namespace xplorer::app;

namespace
{
    bool isSelectorTag(const std::string& tag)
    {
        for (const auto* prefix : {"ENV_", "LFO_", "RAMP_", "TRACK_"})
        {
            const std::string p(prefix);
            if (tag.size() == p.size() + 1 && tag.rfind(p, 0) == 0
                && tag.back() >= '1' && tag.back() <= '5')
            {
                return true;
            }
        }
        return false;
    }

    const ControlSpec* findById(const std::string& id)
    {
        for (const auto& spec : controlTable())
        {
            if (id == spec.id)
            {
                return &spec;
            }
        }
        return nullptr;
    }
}

SCENARIO("The extracted control table matches the reference main window", "[RQ-GUI-001]")
{
    GIVEN("the generated table")
    {
        const auto table = controlTable();

        THEN("it holds the expected population per control kind")
        {
            std::map<ControlKind, int> census;
            for (const auto& spec : table)
            {
                ++census[spec.kind];
            }
            CHECK(table.size() == 208);
            CHECK(census[ControlKind::KnobControl] == 49);
            CHECK(census[ControlKind::ComboBoxValuedControl] == 48);
            CHECK(census[ControlKind::CheckBoxValuedControl] == 52);
            CHECK(census[ControlKind::BackgroundImageButton] == 8); // shortcut buttons [RQ-GUI-021]
            CHECK(census[ControlKind::VacuumFluoDisplayControl] == 1);
            CHECK(census[ControlKind::LedPanelControl] == 1);
        }

        THEN("ids are unique and bounds fit the logical canvas")
        {
            std::set<std::string> ids;
            for (const auto& spec : table)
            {
                CHECK(ids.insert(spec.id).second);
                CHECK(spec.x >= 0);
                CHECK(spec.y >= 0);
                CHECK(spec.x + spec.width <= LOGICAL_CANVAS_WIDTH + 20);
                CHECK(spec.y + spec.height <= LOGICAL_CANVAS_HEIGHT + 40);
            }
        }
    }
}

SCENARIO("Control tags resolve against the tone parameter map", "[RQ-GUI-002]")
{
    GIVEN("the table and a real XpanderTone")
    {
        const xplorer::model::XpanderTone tone;

        WHEN("classifying every non-empty tag")
        {
            int direct = 0;
            int familyOrMatrix = 0;
            int navigationOrValue = 0;
            int unresolved = 0;
            for (const auto& spec : controlTable())
            {
                const std::string tag = spec.tag;
                if (tag.empty())
                {
                    continue;
                }
                if (tone.parameterMap().contains(tag))
                {
                    ++direct; // e.g. VCO1_FREQ
                }
                else if (tag.find("_X_") != std::string::npos || tag.rfind("MOD_", 0) == 0)
                {
                    ++familyOrMatrix; // shared family blocks / matrix helpers
                }
                else if (isSelectorTag(tag) || tag == "0" || tag == "1")
                {
                    ++navigationOrValue; // family selector buttons, radio value tags
                }
                else
                {
                    ++unresolved;
                    INFO("unresolved tag: " << tag);
                }
            }

            THEN("tags split between direct bindings and family/matrix placeholders")
            {
                CHECK(direct >= 60);
                CHECK(familyOrMatrix >= 50);
                CHECK(navigationOrValue >= 15); // 15 selectors + radio value tags
                CHECK(unresolved == 0);         // every tag is accounted for
            }
        }
    }
}

SCENARIO("Known anchor controls are present with their reference kinds", "[RQ-GUI-001][RQ-GUI-021]")
{
    GIVEN("well-known controls")
    {
        THEN("the 8 shortcut buttons exist as image buttons")
        {
            for (const auto* id : {"btPatchPlus", "btPatchMinus", "btPatchGoto", "btPatchRandom",
                                   "btPatchLoad", "btPatchSave", "btPatchStore", "btSettings"})
            {
                const auto* spec = findById(id);
                REQUIRE(spec != nullptr);
                CHECK(spec->kind == ControlKind::BackgroundImageButton);
            }
        }

        THEN("a knob is bound to VCO1_FREQ")
        {
            bool found = false;
            for (const auto& spec : controlTable())
            {
                if (spec.kind == ControlKind::KnobControl && std::string(spec.tag) == "VCO1_FREQ")
                {
                    found = true;
                }
            }
            CHECK(found);
        }
    }
}
