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

#include "xpl/midi/MidiMessage.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

using xpl::midi::ChannelCommand;
using xpl::midi::MessageType;
using xpl::midi::MidiMessage;

SCENARIO("Channel voice messages encode as on the wire", "[RQ-MID-010]")
{
    GIVEN("a Control Change on channel 3")
    {
        const auto message = MidiMessage::channelMessage(ChannelCommand::Controller, 3, 74, 100);

        THEN("bytes, type and accessors match")
        {
            REQUIRE(message.size() == 3);
            CHECK(message[0] == 0xB3);
            CHECK(message[1] == 74);
            CHECK(message[2] == 100);
            CHECK(message.type() == MessageType::Channel);
            CHECK(message.command() == ChannelCommand::Controller);
            CHECK(message.channel() == 3);
            CHECK(message.data1() == 74);
            CHECK(message.data2() == 100);
        }
    }

    GIVEN("a Program Change on channel 0")
    {
        const auto message = MidiMessage::channelMessage(ChannelCommand::ProgramChange, 0, 42);

        THEN("it encodes as two bytes")
        {
            REQUIRE(message.size() == 2);
            CHECK(message[0] == 0xC0);
            CHECK(message[1] == 42);
        }
    }

    GIVEN("Note On / Note Off round-trips")
    {
        const auto noteOn = MidiMessage::channelMessage(ChannelCommand::NoteOn, 15, 60, 127);
        const auto noteOff = MidiMessage::channelMessage(ChannelCommand::NoteOff, 15, 60, 0);

        THEN("status bytes carry command and channel")
        {
            CHECK(noteOn[0] == 0x9F);
            CHECK(noteOff[0] == 0x8F);
        }
    }

    GIVEN("out-of-range arguments")
    {
        THEN("construction throws")
        {
            CHECK_THROWS_AS(MidiMessage::channelMessage(ChannelCommand::Controller, 16, 0, 0), std::out_of_range);
            CHECK_THROWS_AS(MidiMessage::channelMessage(ChannelCommand::Controller, -1, 0, 0), std::out_of_range);
            CHECK_THROWS_AS(MidiMessage::channelMessage(ChannelCommand::Controller, 0, 128, 0), std::out_of_range);
            CHECK_THROWS_AS(MidiMessage::channelMessage(ChannelCommand::Controller, 0, 0, 128), std::out_of_range);
        }
    }
}

SCENARIO("Channel messages can be re-stamped on another channel", "[RQ-FMW-051]")
{
    GIVEN("a CC message on channel 0")
    {
        const auto original = MidiMessage::channelMessage(ChannelCommand::Controller, 0, 7, 99);

        WHEN("re-stamped on channel 9")
        {
            const auto forwarded = original.withChannel(9);

            THEN("only the channel nibble changes")
            {
                CHECK(forwarded[0] == 0xB9);
                CHECK(forwarded.data1() == 7);
                CHECK(forwarded.data2() == 99);
                CHECK(original[0] == 0xB0);
            }
        }
    }

    GIVEN("a SysEx message")
    {
        const std::vector<std::uint8_t> frame{0xF0, 0x10, 0x02, 0xF7};

        THEN("re-stamping is rejected")
        {
            CHECK_THROWS_AS(MidiMessage::sysEx(frame).withChannel(1), std::logic_error);
        }
    }
}

SCENARIO("SysEx frames of arbitrary length are supported", "[RQ-MID-011]")
{
    GIVEN("a valid short frame")
    {
        const std::vector<std::uint8_t> frame{0xF0, 0x10, 0x02, 0x01, 0xF7};
        const auto message = MidiMessage::sysEx(frame);

        THEN("bytes are kept verbatim and type is SysEx")
        {
            CHECK(message.size() == 5);
            CHECK(message.type() == MessageType::SysEx);
            CHECK(std::vector<std::uint8_t>(message.bytes().begin(), message.bytes().end()) == frame);
        }
    }

    GIVEN("an all-data-dump-sized frame (> 64 KB)")
    {
        std::vector<std::uint8_t> frame(70000, 0x00);
        frame.front() = 0xF0;
        frame.back() = 0xF7;

        THEN("it is accepted unchanged")
        {
            CHECK(MidiMessage::sysEx(frame).size() == 70000);
        }
    }

    GIVEN("frames with missing framing bytes")
    {
        THEN("construction throws")
        {
            const std::vector<std::uint8_t> noStart{0x10, 0xF7};
            const std::vector<std::uint8_t> noEnd{0xF0, 0x10};
            CHECK_THROWS_AS(MidiMessage::sysEx(noStart), std::invalid_argument);
            CHECK_THROWS_AS(MidiMessage::sysEx(noEnd), std::invalid_argument);
        }
    }
}

SCENARIO("System common and realtime messages are classified", "[RQ-MID-012]")
{
    GIVEN("a Tune Request")
    {
        const auto message = MidiMessage::tuneRequest();

        THEN("it is the single byte 0xF6, classified SysCommon")
        {
            REQUIRE(message.size() == 1);
            CHECK(message[0] == 0xF6);
            CHECK(message.type() == MessageType::SysCommon);
        }
    }

    GIVEN("raw realtime bytes")
    {
        const std::vector<std::uint8_t> clock{0xF8};

        THEN("classification is SysRealtime")
        {
            CHECK(MidiMessage::fromRawBytes(clock).type() == MessageType::SysRealtime);
        }
    }
}
