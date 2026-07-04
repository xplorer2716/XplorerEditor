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

#include "midiapp/controller/DualDictionary.hpp"

using midiapp::controller::DualDictionary;

SCENARIO("The automation table maps names to CCs bidirectionally", "[RQ-FMW-032]")
{
    GIVEN("two parameters mapped to the same CC")
    {
        DualDictionary table;
        table.add("VCF_FREQ", 74);
        table.add("VCF_RES", 74);
        table.add("VCA_VOL", 7);

        THEN("lookups work both ways")
        {
            CHECK(table.ccNumberFor("VCF_FREQ") == 74);
            CHECK(table.ccNumberFor("VCA_VOL") == 7);
            CHECK(table.parameterNamesFor(74) == std::vector<std::string>{"VCF_FREQ", "VCF_RES"});
            CHECK(table.size() == 3);
        }

        THEN("unknown lookups are empty, not errors")
        {
            CHECK_FALSE(table.ccNumberFor("NOPE").has_value());
            CHECK(table.parameterNamesFor(99).empty());
        }

        WHEN("a name is reassigned to another CC")
        {
            table.add("VCF_RES", 71);

            THEN("it leaves the old CC list (reference Add removes first)")
            {
                CHECK(table.parameterNamesFor(74) == std::vector<std::string>{"VCF_FREQ"});
                CHECK(table.parameterNamesFor(71) == std::vector<std::string>{"VCF_RES"});
                CHECK(table.size() == 3);
            }
        }

        WHEN("a name is removed")
        {
            table.remove("VCF_FREQ");

            THEN("both directions forget it")
            {
                CHECK_FALSE(table.ccNumberFor("VCF_FREQ").has_value());
                CHECK(table.parameterNamesFor(74) == std::vector<std::string>{"VCF_RES"});
            }
        }

        WHEN("cleared")
        {
            table.clear();

            THEN("it is empty")
            {
                CHECK(table.size() == 0);
                CHECK(table.entries().empty());
            }
        }
    }
}
