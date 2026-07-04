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

#include "midiapp/LibraryInfo.hpp"
#include "xpl/midi/LibraryInfo.hpp"
#include "xplorer/controller/LibraryInfo.hpp"
#include "xplorer/model/LibraryInfo.hpp"
#include "xplorer/settings/LibraryInfo.hpp"

#include <cstdint>
#include <fstream>
#include <vector>

namespace
{
    std::vector<std::uint8_t> readFile(const std::string& path)
    {
        std::ifstream stream(path, std::ios::binary);
        REQUIRE(stream.good());
        return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(stream), {});
    }
}

SCENARIO("All layer libraries build and link bottom-up", "[RQ-BLD-005]")
{
    GIVEN("the five layer libraries")
    {
        THEN("each reports its identity")
        {
            CHECK(xpl::midi::libraryName() == "xpl_midi");
            CHECK(midiapp::libraryName() == "xpl_framework");
            CHECK(xplorer::model::libraryName() == "xpl_model");
            CHECK(xplorer::controller::libraryName() == "xpl_controller");
            CHECK(xplorer::settings::libraryName() == "xpl_settings");
        }
    }
}

SCENARIO("The JUCE backend adapter library links against JUCE 8", "[RQ-BLD-001]")
{
    GIVEN("the xpl_midi_juce adapter")
    {
        WHEN("the JUCE version is queried")
        {
            const auto version = xpl::midi::juceVersion();
            THEN("it reports JUCE 8.x")
            {
                INFO(version);
                CHECK(version.find("JUCE v8.") == 0);
            }
        }
    }
}

SCENARIO("Reference .syx fixture is available to tests", "[RQ-TST-002]")
{
    GIVEN("the OBERHEIM single-patch fixture")
    {
        const auto bytes = readFile(std::string(XPL_FIXTURES_DIR) + "/single_patch_oberheim.syx");

        THEN("it holds the 399-byte program dump plus the 7-byte page-select message")
        {
            REQUIRE(bytes.size() == 406);
            CHECK(bytes.front() == 0xF0);
            CHECK(bytes.back() == 0xF7);
            // Oberheim ID 0x10, Xpander device 0x02, single patch data opcode 0x01
            CHECK(bytes[1] == 0x10);
            CHECK(bytes[2] == 0x02);
            CHECK(bytes[3] == 0x01);
        }
    }
}
