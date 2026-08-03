#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"
#include "xplorer/app/MidiAutomationTable.hpp"
#include "xplorer/app/ModulationHighlight.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

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

SCENARIO("Parameter tags resolve to their reference display names", "[RQ-GUI-020]")
{
    GIVEN("the friendly-name table")
    {
        THEN("known parameters map to the reference resource strings")
        {
            CHECK(parameterDisplayName("VCF_MODE") == "VCF MODE");
            CHECK(parameterDisplayName("VCO1_FREQ") == "VCO1 FREQ");
            CHECK(parameterDisplayName("ENV_1_ATTACK") == "ENV1 ATTACK");
        }

        THEN("an unknown tag falls back to itself")
        {
            CHECK(parameterDisplayName("NOT_A_PARAM") == "NOT_A_PARAM");
        }
    }

    GIVEN("the modulation matrix label sets used by the VFD")
    {
        THEN("source and destination labels are indexable by enum value")
        {
            const auto sources = comboLabelsForControl("MOD_SRC_1");
            const auto destinations = comboLabelsForControl("MOD_DEST_1");
            REQUIRE(sources.size() > static_cast<std::size_t>(
                        xplorer::model::EnumModulationSourcesModMatrix::NONE));
            CHECK(sources.back() == "NONE");
            CHECK(destinations.at(static_cast<std::size_t>(
                      xplorer::model::EnumModulationDestinations::VCF_FRQ)) == "VCF FREQ");
        }
    }
}

SCENARIO("Knobs and selectors resolve to their modulation destination/source", "[RQ-GUI-018]")
{
    using xplorer::model::EnumModulationDestinations;
    using xplorer::model::EnumModulationSourcesModMatrix;

    GIVEN("fixed and paged knobs")
    {
        THEN("a fixed knob maps to its destination")
        {
            CHECK(modulationDestinationForParameter("VCO1_FREQ") == EnumModulationDestinations::VCO1_FRQ);
            CHECK(modulationDestinationForParameter("VCF_VCA1_VOLUME") == EnumModulationDestinations::VCA1_VOL);
            CHECK(modulationDestinationForParameter("FMLAG_RATE") == EnumModulationDestinations::LAG_RATE);
        }

        THEN("a paged ENV/LFO knob resolves against its instance (base + stride)")
        {
            CHECK(modulationDestinationForParameter("ENV_1_ATTACK") == EnumModulationDestinations::ENV1_ATK);
            CHECK(modulationDestinationForParameter("ENV_3_ATTACK") == EnumModulationDestinations::ENV3_ATK);
            CHECK(modulationDestinationForParameter("LFO_2_AMP") == EnumModulationDestinations::LFO2_AMP);
        }

        THEN("a non-destination parameter yields nothing")
        {
            CHECK_FALSE(modulationDestinationForParameter("VCO1_DETUNE").has_value());
            CHECK_FALSE(modulationDestinationForParameter("NOT_A_PARAM").has_value());
        }
    }

    GIVEN("page-family selectors")
    {
        THEN("a source selector maps to its modulation source")
        {
            CHECK(modulationSourceForSelector("ENV_1") == EnumModulationSourcesModMatrix::ENV1);
            CHECK(modulationSourceForSelector("LFO_5") == EnumModulationSourcesModMatrix::LFO5);
            CHECK(modulationSourceForSelector("TRACK_2") == EnumModulationSourcesModMatrix::TRK2);
            CHECK(modulationSourceForSelector("RAMP_4") == EnumModulationSourcesModMatrix::RMP4);
        }

        THEN("an unknown selector yields nothing")
        {
            CHECK_FALSE(modulationSourceForSelector("ENV_9").has_value());
        }
    }
}

SCENARIO("Every declared radio panel resolves to its reference options",
         "[RQ-GUI-053][RQ-GUI-032]")
{
    // Regression guard for issue #31: ENV_X/RAMP_X_TRIG_SINGLE_MULTI were
    // declared in the control table but absent from the option metadata, so the
    // widget factories skipped them silently and a whole trigger mode was
    // uneditable. [RQ-GUI-053, ADR-JUC-016 (DEC-JUC-087)]
    const std::vector<std::pair<std::string, int>> singleMulti = {{"SINGLE", 0}, {"MULTI", 1}};

    GIVEN("the control table")
    {
        std::set<std::string> panelIds;
        for (const auto& spec : controlTable())
        {
            if (spec.kind == ControlKind::RadioButtonPanel)
            {
                panelIds.insert(spec.id);
            }
        }

        THEN("it declares exactly the four reference radio panels")
        {
            CHECK(panelIds
                  == std::set<std::string>{"ENV_X_TRIG_SINGLE_MULTI", "FM_DESTINATION",
                                           "LAG_TIMING_LINEAR_EXPO", "RAMP_X_TRIG_SINGLE_MULTI"});
        }

        THEN("every one of them resolves to a non-empty option set")
        {
            for (const auto& id : panelIds)
            {
                INFO("radio panel without options: " << id);
                CHECK_FALSE(radioPanelOptions(id).empty());
            }
        }
    }

    GIVEN("the page-family trigger panels")
    {
        THEN("both carry the reference SINGLE=0 / MULTI=1 pair, in reference order")
        {
            CHECK(radioPanelOptions("ENV_X_TRIG_SINGLE_MULTI") == singleMulti);
            CHECK(radioPanelOptions("RAMP_X_TRIG_SINGLE_MULTI") == singleMulti);
        }
    }

    GIVEN("the fixed-block panels")
    {
        THEN("their options are unchanged")
        {
            CHECK(radioPanelOptions("FM_DESTINATION")
                  == std::vector<std::pair<std::string, int>>{{"VCO1", 0}, {"VCF", 1}});
            CHECK(radioPanelOptions("LAG_TIMING_LINEAR_EXPO")
                  == std::vector<std::pair<std::string, int>>{{"LINEAR", 0}, {"EXPO", 1}});
        }
    }

    GIVEN("an id that is not a radio panel")
    {
        THEN("no options are returned")
        {
            CHECK(radioPanelOptions("VCO1_FREQ").empty());
        }
    }
}

SCENARIO("The MIDI CC automation table parses and names entries", "[RQ-GUI-036]")
{
    GIVEN("the reference CC-name list")
    {
        THEN("it has 129 entries with 'None' last")
        {
            CHECK(controlChangeNameCount() == 129);
            CHECK(unassignedControlChange() == 128);
            CHECK(controlChangeName(0) == "Bank Select");
            CHECK(controlChangeName(128) == "None");
            CHECK(controlChangeName(999) == "None"); // out of range → None
        }
    }

    GIVEN("persisted 'NAME;CC' entries")
    {
        THEN("a well-formed entry parses and clamps CC to 1..128")
        {
            const auto ok = parseAutomationEntry("VCO1_VOLUME;12");
            REQUIRE(ok.has_value());
            CHECK(ok->first == "VCO1_VOLUME");
            CHECK(ok->second == 12);
            CHECK(parseAutomationEntry("X;0")->second == 1);   // clamped up
            CHECK(parseAutomationEntry("X;200")->second == 128); // clamped down
        }

        THEN("a parameter name is split on the last ';'")
        {
            const auto ok = parseAutomationEntry("A;B;7");
            REQUIRE(ok.has_value());
            CHECK(ok->first == "A;B");
            CHECK(ok->second == 7);
        }

        THEN("malformed entries are rejected")
        {
            CHECK_FALSE(parseAutomationEntry("no-delimiter").has_value());
            CHECK_FALSE(parseAutomationEntry("name;notanumber").has_value());
            CHECK_FALSE(parseAutomationEntry(";5").has_value());
        }
    }

    GIVEN("the HTML export")
    {
        const auto html = buildMidiMappingHtml(
            {{"VCO1 FREQ", "None"}, {"VCF MODE", "Damper Pedal on/off"}}, "2026-07-13 10:00:00");

        THEN("it is a self-contained HTML document with a row per mapping")
        {
            CHECK(html.rfind("<!DOCTYPE html>", 0) == 0);
            CHECK(html.find("<style>") != std::string::npos); // inline CSS, self-contained
            CHECK(html.find("<td>VCO1 FREQ</td>") != std::string::npos);
            CHECK(html.find("<td>VCF MODE</td>") != std::string::npos);
            CHECK(html.find("Generated on 2026-07-13 10:00:00") != std::string::npos);
        }

        THEN("special characters are HTML-escaped")
        {
            const auto escaped = buildMidiMappingHtml({{"A & B", "x<y>z"}}, "t");
            CHECK(escaped.find("A &amp; B") != std::string::npos);
            CHECK(escaped.find("x&lt;y&gt;z") != std::string::npos);
        }
    }
}
