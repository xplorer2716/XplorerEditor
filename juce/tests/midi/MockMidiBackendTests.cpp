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

#include "xpl/midi/MockMidiBackend.hpp"

#include <string>
#include <vector>

using namespace xpl::midi;

namespace
{
    MidiInputCallbacks captureAll(std::vector<MidiMessage>& sink, std::vector<std::string>* errors = nullptr)
    {
        MidiInputCallbacks callbacks;
        callbacks.onChannelMessage = [&sink](const MidiMessage& m) { sink.push_back(m); };
        callbacks.onSysExMessage = [&sink](const MidiMessage& m) { sink.push_back(m); };
        callbacks.onSysCommonMessage = [&sink](const MidiMessage& m) { sink.push_back(m); };
        callbacks.onSysRealtimeMessage = [&sink](const MidiMessage& m) { sink.push_back(m); };
        if (errors != nullptr)
        {
            callbacks.onError = [errors](const std::string& e) { errors->push_back(e); };
        }
        return callbacks;
    }
}

SCENARIO("Devices are enumerated and opened by name", "[RQ-MID-001][RQ-MID-002][RQ-MID-041]")
{
    GIVEN("a backend with two inputs and one output")
    {
        MockMidiBackend backend;
        backend.addInputDevice("Synth In");
        backend.addInputDevice("Automation In");
        backend.addOutputDevice("Synth Out");

        THEN("enumeration lists them")
        {
            CHECK(backend.inputDeviceNames() == std::vector<std::string>{"Synth In", "Automation In"});
            CHECK(backend.outputDeviceNames() == std::vector<std::string>{"Synth Out"});
        }

        WHEN("opening by exact name")
        {
            const auto input = backend.openInput("Synth In");
            const auto output = backend.openOutput("Synth Out");

            THEN("ports are returned and named")
            {
                REQUIRE(input != nullptr);
                REQUIRE(output != nullptr);
                CHECK(input->deviceName() == "Synth In");
                CHECK(output->deviceName() == "Synth Out");
            }
        }

        WHEN("opening an unknown name")
        {
            THEN("nullptr is returned, no exception")
            {
                CHECK(backend.openInput("Nope") == nullptr);
                CHECK(backend.openOutput("Nope") == nullptr);
            }
        }
    }
}

SCENARIO("Port lifetime tracks open/close for hot-swap", "[RQ-MID-003][RQ-MID-006]")
{
    GIVEN("an opened input port")
    {
        MockMidiBackend backend;
        backend.addInputDevice("Synth In");
        auto port = backend.openInput("Synth In");
        REQUIRE(backend.openInputPortCount("Synth In") == 1);

        WHEN("the port is released")
        {
            port.reset();

            THEN("the device is closed")
            {
                CHECK(backend.openInputPortCount("Synth In") == 0);
            }
        }
    }
}

SCENARIO("Injected messages reach started ports only", "[RQ-MID-005][RQ-MID-020][RQ-MID-021]")
{
    GIVEN("an input port with capturing callbacks")
    {
        MockMidiBackend backend;
        backend.addInputDevice("Synth In");
        auto port = backend.openInput("Synth In");
        std::vector<MidiMessage> received;
        port->setCallbacks(captureAll(received));

        const auto cc = MidiMessage::channelMessage(ChannelCommand::Controller, 0, 7, 64);

        WHEN("a message is injected before start()")
        {
            backend.injectIncoming("Synth In", cc);

            THEN("nothing is delivered")
            {
                CHECK(received.empty());
            }
        }

        WHEN("started, injected, then stopped and injected again")
        {
            port->start();
            backend.injectIncoming("Synth In", cc);
            const std::vector<std::uint8_t> frame{0xF0, 0x10, 0x02, 0xF7};
            backend.injectIncoming("Synth In", MidiMessage::sysEx(frame));
            port->stop();
            backend.injectIncoming("Synth In", cc);

            THEN("only messages injected while started are delivered, in order")
            {
                REQUIRE(received.size() == 2);
                CHECK(received[0].type() == MessageType::Channel);
                CHECK(received[1].type() == MessageType::SysEx);
            }
        }
    }
}

SCENARIO("Errors are delivered to the error callback", "[RQ-MID-023]")
{
    GIVEN("a started port with an error callback")
    {
        MockMidiBackend backend;
        backend.addInputDevice("Synth In");
        auto port = backend.openInput("Synth In");
        std::vector<MidiMessage> received;
        std::vector<std::string> errors;
        port->setCallbacks(captureAll(received, &errors));
        port->start();

        WHEN("an error is injected")
        {
            backend.injectError("Synth In", "device unplugged");

            THEN("the callback receives it")
            {
                REQUIRE(errors.size() == 1);
                CHECK(errors[0] == "device unplugged");
            }
        }
    }
}

SCENARIO("Output is captured and can loop back to an input", "[RQ-MID-041][RQ-TST-004]")
{
    GIVEN("an output wired to an input (virtual cable)")
    {
        MockMidiBackend backend;
        backend.addInputDevice("Synth In");
        backend.addOutputDevice("Synth Out");
        backend.connectLoopback("Synth Out", "Synth In");

        auto input = backend.openInput("Synth In");
        auto output = backend.openOutput("Synth Out");
        std::vector<MidiMessage> received;
        input->setCallbacks(captureAll(received));
        input->start();

        WHEN("messages are sent")
        {
            const auto note = MidiMessage::channelMessage(ChannelCommand::NoteOn, 0, 60, 100);
            output->send(note);
            output->send(MidiMessage::tuneRequest());

            THEN("they are captured on the output side")
            {
                const auto sent = backend.sentMessages("Synth Out");
                REQUIRE(sent.size() == 2);
                CHECK(sent[0] == note);
            }

            THEN("they arrive on the looped-back input")
            {
                REQUIRE(received.size() == 2);
                CHECK(received[0] == note);
                CHECK(received[1].type() == MessageType::SysCommon);
            }
        }
    }
}
