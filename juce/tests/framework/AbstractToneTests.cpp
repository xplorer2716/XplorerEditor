/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2026 Pascal Schmitt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <catch2/catch_test_macros.hpp>

#include "TestParameter.hpp"

#include <set>
#include <stdexcept>

using midiapp::model::AbstractTone;
using midiapp::testing::TestTone;

namespace
{
    TestTone makeTone()
    {
        TestTone tone;
        tone.addParameter("A", 0, 63, 1, 10);
        tone.addParameter("B", -31, 31, 1, -5);
        tone.addParameter("SWITCH", 0, 1, 1, 0);
        return tone;
    }
}

SCENARIO("The parameter map preserves insertion order and unique names", "[RQ-FMW-010]")
{
    GIVEN("a tone with three parameters")
    {
        auto tone = makeTone();

        THEN("iteration follows insertion order")
        {
            std::vector<std::string> names;
            for (const auto& entry : tone.parameterMap())
            {
                names.push_back(entry.name);
            }
            CHECK(names == std::vector<std::string>{"A", "B", "SWITCH"});
        }

        THEN("lookups behave")
        {
            CHECK(tone.parameterMap().contains("B"));
            CHECK_FALSE(tone.parameterMap().contains("Z"));
            CHECK(tone.parameterMap().at("A").value() == 10);
            CHECK(tone.parameterMap().find("Z") == nullptr);
        }

        THEN("adding a duplicate name throws")
        {
            CHECK_THROWS_AS(tone.addParameter("A", 0, 1, 1, 0), std::invalid_argument);
        }
    }
}

SCENARIO("The MIDI channel is validated to 0-15", "[RQ-FMW-010]")
{
    GIVEN("a tone")
    {
        TestTone tone;
        CHECK(tone.midiChannel() == AbstractTone::DEFAULT_MIDI_CHANNEL);

        THEN("valid channels are stored, invalid ones throw")
        {
            tone.setMidiChannel(15);
            CHECK(tone.midiChannel() == 15);
            CHECK_THROWS_AS(tone.setMidiChannel(16), std::out_of_range);
            CHECK_THROWS_AS(tone.setMidiChannel(-1), std::out_of_range);
        }
    }
}

SCENARIO("Randomization respects exclusions, bounds and two-state coin flip", "[RQ-FMW-011]")
{
    GIVEN("a tone with an excluded parameter")
    {
        auto tone = makeTone();

        WHEN("randomizing everything except A, with a fixed seed")
        {
            tone.randomizeToneParameters({"A"}, std::nullopt, 12345U);

            THEN("A is untouched and the others stay within bounds")
            {
                CHECK(tone.parameterMap().at("A").value() == 10);
                const int b = tone.parameterMap().at("B").value();
                CHECK(b >= -31);
                CHECK(b <= 31);
                const int s = tone.parameterMap().at("SWITCH").value();
                CHECK((s == 0 || s == 1));
            }
        }

        WHEN("randomizing with humanize 0.5 around the current values")
        {
            tone.randomizeToneParameters({}, 0.5F, 42U);

            THEN("values stay within bounds")
            {
                const int a = tone.parameterMap().at("A").value();
                CHECK(a >= 0);
                CHECK(a <= 63);
            }
        }
    }
}

SCENARIO("Morphing interpolates eligible parameters", "[RQ-FMW-012]")
{
    GIVEN("two tones with distinct values")
    {
        auto toneA = makeTone(); // A=10, B=-5
        auto toneB = makeTone();
        toneB.parameterMap().at("A").setValue(50);
        toneB.parameterMap().at("B").setValue(25);
        auto result = makeTone();

        WHEN("morphing at factor 0.5")
        {
            AbstractTone::morphTones(toneA, toneB, result, 0.5F);

            THEN("each parameter is the truncated linear blend")
            {
                CHECK(result.parameterMap().at("A").value() == 30); // (10+50)/2
                CHECK(result.parameterMap().at("B").value() == 10); // (-5+25)/2
            }
        }

        WHEN("morphing at the extremes")
        {
            AbstractTone::morphTones(toneA, toneB, result, 0.0F);
            CHECK(result.parameterMap().at("A").value() == 10);
            AbstractTone::morphTones(toneA, toneB, result, 1.0F);
            CHECK(result.parameterMap().at("A").value() == 50);
        }
    }
}
