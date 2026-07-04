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

#include "xpl/midi/JuceMidiBackend.hpp"

#include <atomic>
#include <chrono>
#include <thread>

using namespace xpl::midi;

SCENARIO("JUCE backend enumerates and opens devices safely headless", "[RQ-MID-001][RQ-MID-002]")
{
    GIVEN("a JUCE backend on this machine")
    {
        JuceMidiBackend backend;

        THEN("enumeration returns without crashing (possibly empty headless)")
        {
            CHECK_NOTHROW(backend.inputDeviceNames());
            CHECK_NOTHROW(backend.outputDeviceNames());
        }

        WHEN("opening a device name that does not exist")
        {
            THEN("nullptr is returned, no exception")
            {
                CHECK(backend.openInput("xpl-no-such-device") == nullptr);
                CHECK(backend.openOutput("xpl-no-such-device") == nullptr);
            }
        }
    }
}

SCENARIO("JUCE backend loops a message through a virtual MIDI cable when present", "[RQ-TST-005]")
{
    GIVEN("a machine with a loopback device pair (same name on input and output)")
    {
        JuceMidiBackend backend;
        std::string loopName;
        const auto inputs = backend.inputDeviceNames();
        for (const auto& outputName : backend.outputDeviceNames())
        {
            if (std::find(inputs.begin(), inputs.end(), outputName) != inputs.end())
            {
                loopName = outputName;
                break;
            }
        }
        if (loopName.empty())
        {
            SKIP("no virtual MIDI loopback device available on this machine");
        }

        WHEN("a CC message is sent through the cable")
        {
            auto input = backend.openInput(loopName);
            auto output = backend.openOutput(loopName);
            REQUIRE(input != nullptr);
            REQUIRE(output != nullptr);

            std::atomic<int> received{0};
            MidiInputCallbacks callbacks;
            callbacks.onChannelMessage = [&received](const MidiMessage&) { ++received; };
            input->setCallbacks(std::move(callbacks));
            input->start();

            output->send(MidiMessage::channelMessage(ChannelCommand::Controller, 0, 7, 64));

            THEN("it arrives within one second")
            {
                for (int i = 0; i < 100 && received == 0; ++i)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                CHECK(received == 1);
            }
        }
    }
}
