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

#include "xpl/midi/SysexStreamIterator.hpp"

#include <cstdint>
#include <fstream>
#include <vector>

using xpl::midi::SysexStreamIterator;
using Bytes = std::vector<std::uint8_t>;

SCENARIO("SysEx frames are split from a byte stream", "[RQ-MID-030]")
{
    GIVEN("two consecutive frames")
    {
        const Bytes data{0xF0, 0x10, 0x02, 0xF7, 0xF0, 0x11, 0xF7};

        THEN("both frames are yielded in order, framing included")
        {
            const auto messages = SysexStreamIterator::allMessages(data);
            REQUIRE(messages.size() == 2);
            CHECK(messages[0] == Bytes{0xF0, 0x10, 0x02, 0xF7});
            CHECK(messages[1] == Bytes{0xF0, 0x11, 0xF7});
        }
    }

    GIVEN("garbage before, between and after frames")
    {
        const Bytes data{0x00, 0x42, 0xF0, 0x01, 0xF7, 0xAA, 0xBB, 0xF0, 0x02, 0xF7, 0x55};

        THEN("only the framed content is yielded")
        {
            const auto messages = SysexStreamIterator::allMessages(data);
            REQUIRE(messages.size() == 2);
            CHECK(messages[0] == Bytes{0xF0, 0x01, 0xF7});
            CHECK(messages[1] == Bytes{0xF0, 0x02, 0xF7});
        }
    }

    GIVEN("an unterminated trailing frame")
    {
        const Bytes data{0xF0, 0x01, 0xF7, 0xF0, 0x02, 0x03};

        THEN("the trailing frame is dropped, as the reference iterator does")
        {
            const auto messages = SysexStreamIterator::allMessages(data);
            REQUIRE(messages.size() == 1);
            CHECK(messages[0] == Bytes{0xF0, 0x01, 0xF7});
        }
    }

    GIVEN("an empty stream and a stream with no SysEx at all")
    {
        THEN("no frame is yielded")
        {
            CHECK(SysexStreamIterator::allMessages(Bytes{}).empty());
            CHECK(SysexStreamIterator::allMessages(Bytes{0x01, 0x02, 0x03}).empty());
        }
    }
}

SCENARIO("The reference fixture splits into patch dump plus page select", "[RQ-MID-030][RQ-TST-002]")
{
    GIVEN("the OBERHEIM .syx fixture")
    {
        std::ifstream stream(std::string(XPL_FIXTURES_DIR) + "/single_patch_oberheim.syx", std::ios::binary);
        REQUIRE(stream.good());
        const Bytes data((std::istreambuf_iterator<char>(stream)), {});

        THEN("it contains the 399-byte dump and the 7-byte page-select frame")
        {
            const auto messages = SysexStreamIterator::allMessages(data);
            REQUIRE(messages.size() == 2);
            CHECK(messages[0].size() == 399);
            CHECK(messages[1].size() == 7);
        }
    }
}
