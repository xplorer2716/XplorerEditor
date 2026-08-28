#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/PageFamilyModel.hpp"
#include "xplorer/model/XpanderConstants.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include "xpl/util/EnumUtils.hpp"

using namespace xplorer::app;
using xplorer::model::EnumPages;

SCENARIO("Page families match the reference descriptors", "[RQ-GUI-010]")
{
    GIVEN("the four families")
    {
        const auto& families = pageFamilies();

        THEN("counts and prefixes are the reference ones")
        {
            REQUIRE(families.size() == 4);
            CHECK(families[0].controlTagPrefix == "ENV_X");
            CHECK(families[0].count == 5);
            CHECK(families[1].controlTagPrefix == "LFO_X");
            CHECK(families[1].count == 5);
            CHECK(families[2].controlTagPrefix == "RAMP_X");
            CHECK(families[2].count == 4);
            CHECK(families[3].controlTagPrefix == "TRACK_X");
            CHECK(families[3].count == 3);
        }
    }
}

SCENARIO("Control tags resolve to concrete parameter names for an instance", "[RQ-GUI-011]")
{
    GIVEN("a fresh tone (to check the resolved names exist)")
    {
        const xplorer::model::XpanderTone tone;

        THEN("digit substitution matches the reference")
        {
            CHECK(resolveControlTag("ENV_X_ATTACK", 3) == "ENV_3_ATTACK");
            CHECK(resolveControlTag("LFO_X_SPEED", 5) == "LFO_5_SPEED");
            CHECK(resolveControlTag("RAMP_X_RATE", 2) == "RAMP_2_RATE");
            CHECK(resolveControlTag("TRACK_X_POINT_3", 1) == "TRACK_1_POINT_3");
            CHECK(resolveControlTag("VCO1_FREQ", 3) == "VCO1_FREQ"); // non-family unchanged
        }

        THEN("every resolved family tag exists in the tone map")
        {
            for (const auto& family : pageFamilies())
            {
                for (int instance = 1; instance <= family.count; ++instance)
                {
                    for (const auto& tag : family.controlTags)
                    {
                        const auto name = resolveControlTag(tag, instance);
                        INFO("resolved: " << name);
                        CHECK(tone.parameterMap().contains(name));
                    }
                }
            }
        }
    }
}

SCENARIO("Concrete parameter names map back to their family control tag", "[RQ-GUI-012]")
{
    GIVEN("concrete parameter names")
    {
        THEN("the reverse mapping yields tag, family and instance")
        {
            const auto env = familyParameterFor("ENV_3_ATTACK");
            REQUIRE(env.has_value());
            CHECK(env->controlTag == "ENV_X_ATTACK");
            CHECK(env->familyPrefix == "ENV_X");
            CHECK(env->instance == 3);

            const auto track = familyParameterFor("TRACK_2_POINT_5");
            REQUIRE(track.has_value());
            CHECK(track->controlTag == "TRACK_X_POINT_5");
            CHECK(track->instance == 2);
        }

        THEN("non-family names return nothing")
        {
            CHECK_FALSE(familyParameterFor("VCO1_FREQ").has_value());
            CHECK_FALSE(familyParameterFor("VCF_MODE").has_value());
        }
    }
}

SCENARIO("An instance resolves to its own concrete synth page", "[RQ-CTL-028]")
{
    GIVEN("the four families")
    {
        const auto& families = pageFamilies();

        THEN("pageForInstance matches the reference page numbering, not just instance 1")
        {
            CHECK(pageForInstance(families[0], 1) == xpl::util::toUnderlying(EnumPages::ENV_1));
            CHECK(pageForInstance(families[0], 3) == xpl::util::toUnderlying(EnumPages::ENV_3));
            CHECK(pageForInstance(families[1], 5) == xpl::util::toUnderlying(EnumPages::LFO_5));
            CHECK(pageForInstance(families[3], 2) == xpl::util::toUnderlying(EnumPages::TRACK_2));
        }
    }
}

SCENARIO("A concrete synth page maps back to its family and instance", "[RQ-GUI-012][RQ-CTL-028]")
{
    GIVEN("a page belonging to a family")
    {
        THEN("familyPageFor resolves the family prefix and 1-based instance")
        {
            const auto env = familyPageFor(xpl::util::toUnderlying(EnumPages::ENV_3));
            REQUIRE(env.has_value());
            CHECK(env->familyPrefix == "ENV_X");
            CHECK(env->instance == 3);
        }

        THEN("a page outside every family's range returns nothing")
        {
            CHECK_FALSE(familyPageFor(xpl::util::toUnderlying(EnumPages::VCO_1_X)).has_value());
        }
    }
}
