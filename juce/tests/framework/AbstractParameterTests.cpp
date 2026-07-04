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

#include <stdexcept>

using midiapp::testing::TestParameter;

SCENARIO("Parameter values are quantized to step and clamped to bounds", "[RQ-FMW-002]")
{
    GIVEN("a parameter [0..63] step 1 value 0")
    {
        TestParameter parameter("VCO1_FREQ", 0, 63, 1, 0);

        WHEN("setting an in-range value")
        {
            parameter.setChanged(false);
            parameter.setValue(42);

            THEN("it stores, flags changed and regenerates the message")
            {
                CHECK(parameter.value() == 42);
                CHECK(parameter.changed());
                CHECK(parameter.message()[TestParameter::VALUE_INDEX] == 42);
            }
        }

        WHEN("setting values beyond the bounds")
        {
            THEN("they clamp to min/max")
            {
                parameter.setValue(1000);
                CHECK(parameter.value() == 63);
                parameter.setValue(-5);
                CHECK(parameter.value() == 0);
            }
        }

        WHEN("setting the same value again")
        {
            parameter.setValue(42);
            parameter.setChanged(false);
            parameter.setValue(42);

            THEN("the changed flag stays clear")
            {
                CHECK_FALSE(parameter.changed());
            }
        }
    }

    GIVEN("a parameter with step 2")
    {
        TestParameter parameter("COARSE", 0, 10, 2, 0);

        WHEN("setting 5")
        {
            parameter.setValue(5);

            THEN("integer division quantizes to 4 (reference semantics)")
            {
                CHECK(parameter.value() == 4);
            }
        }
    }

    GIVEN("a signed range [-31..31]")
    {
        TestParameter parameter("DETUNE", -31, 31, 1, 0);

        THEN("negative values pass quantization unchanged (truncation toward zero)")
        {
            parameter.setValue(-17);
            CHECK(parameter.value() == -17);
        }
    }

    GIVEN("step 0")
    {
        THEN("construction throws, as the reference does")
        {
            CHECK_THROWS_AS(TestParameter("BROKEN", 0, 10, 0, 0), std::invalid_argument);
        }
    }
}

SCENARIO("Parameters clone deeply", "[RQ-FMW-003]")
{
    GIVEN("a parameter with a value")
    {
        TestParameter parameter("SRC", 0, 63, 1, 33);
        parameter.setLabel("Source");

        WHEN("cloned then the original changes")
        {
            const auto copy = parameter.clone();
            parameter.setValue(7);

            THEN("the clone keeps the captured state, message included")
            {
                CHECK(copy->name() == "SRC");
                CHECK(copy->label() == "Source");
                CHECK(copy->value() == 33);
                CHECK(copy->message()[TestParameter::VALUE_INDEX] == 33);
                CHECK(parameter.message()[TestParameter::VALUE_INDEX] == 7);
            }
        }
    }
}
