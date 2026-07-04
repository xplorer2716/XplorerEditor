#include <catch2/catch_test_macros.hpp>

#include "TestParameter.hpp"
#include "midiapp/controller/AbstractController.hpp"
#include "xpl/midi/MockMidiBackend.hpp"

#include <chrono>
#include <memory>
#include <thread>

using namespace midiapp;
using namespace xpl::midi;

namespace
{
    constexpr auto SYNTH_IN = "Synth In";
    constexpr auto SYNTH_OUT = "Synth Out";
    constexpr auto AUTO_IN = "Automation In";

    class TestController final : public controller::AbstractController
    {
    public:
        explicit TestController(MidiBackend& backend)
            : AbstractController(backend, makeTone())
        {
        }

        [[nodiscard]] int currentProgramNumber() const override { return 0; }

        [[nodiscard]] std::vector<std::pair<std::string, std::unique_ptr<model::AbstractTone>>>
        extractSinglePatchesFromAllDataDumpFileToDirectory(const std::string&, const std::string&) override
        {
            return {};
        }

    private:
        static std::unique_ptr<model::AbstractTone> makeTone()
        {
            auto tone = std::make_unique<testing::TestTone>();
            tone->addParameter("FREQ", 0, 63, 1, 0);
            tone->addParameter("DETUNE", -31, 31, 1, 0);
            tone->addParameter("SWITCH", 0, 1, 1, 0);
            return tone;
        }
    };

    struct Fixture
    {
        Fixture()
        {
            backend.addInputDevice(SYNTH_IN);
            backend.addInputDevice(AUTO_IN);
            backend.addOutputDevice(SYNTH_OUT);
            controller.setParameterTransmitDelay(1); // fast ticks for tests
        }

        ~Fixture() { controller.stop(); }

        /// Polls the mock output until `count` messages were sent (or 500 ms).
        bool waitForSentCount(std::size_t count)
        {
            for (int i = 0; i < 500; ++i)
            {
                if (backend.sentMessages(SYNTH_OUT).size() >= count)
                {
                    return true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return false;
        }

        MockMidiBackend backend;
        TestController controller{backend};
    };
}

SCENARIO("SetParameter validates names and the enable gate", "[RQ-FMW-031]")
{
    GIVEN("a controller")
    {
        Fixture f;

        THEN("known names are set, unknown rejected without side effect")
        {
            CHECK(f.controller.setParameter("FREQ", 42));
            CHECK(f.controller.getParameter("FREQ")->value() == 42);
            CHECK_FALSE(f.controller.setParameter("NOPE", 1));
            CHECK(f.controller.getParameter("NOPE") == nullptr);
        }
    }
}

SCENARIO("The transmit delay is validated", "[RQ-FMW-034]")
{
    GIVEN("a controller (reference default 20 ms)")
    {
        MockMidiBackend backend;
        TestController controller(backend);
        CHECK(controller.parameterTransmitDelay() == 20);
        controller.setParameterTransmitDelay(0);
        CHECK_THROWS_AS(controller.setParameterTransmitDelay(-1), std::out_of_range);
    }
}

SCENARIO("Devices are assigned, replaced and released by name", "[RQ-MID-002][RQ-MID-003][RQ-MID-004]")
{
    GIVEN("a controller and a mock backend")
    {
        Fixture f;

        THEN("assignment succeeds for existing names, fails for unknown/empty")
        {
            CHECK(f.controller.setSynthOutputDevice(SYNTH_OUT));
            CHECK(f.controller.setSynthInputDevice(SYNTH_IN));
            CHECK(f.controller.setAutomationInputDevice(AUTO_IN));
            CHECK_FALSE(f.controller.setSynthOutputDevice("Unknown"));
            CHECK_FALSE(f.controller.setSynthInputDevice(""));
        }

        WHEN("a device is re-assigned")
        {
            REQUIRE(f.controller.setSynthInputDevice(SYNTH_IN));
            REQUIRE(f.backend.openInputPortCount(SYNTH_IN) == 1);
            REQUIRE(f.controller.setSynthInputDevice(SYNTH_IN));

            THEN("the previous port was closed first (hot-swap)")
            {
                CHECK(f.backend.openInputPortCount(SYNTH_IN) == 1);
            }
        }

        WHEN("closeMidiDevices is called")
        {
            REQUIRE(f.controller.setSynthInputDevice(SYNTH_IN));
            REQUIRE(f.controller.setSynthOutputDevice(SYNTH_OUT));
            f.controller.closeMidiDevices();

            THEN("all ports are released")
            {
                CHECK(f.backend.openInputPortCount(SYNTH_IN) == 0);
                CHECK(f.backend.openOutputPortCount(SYNTH_OUT) == 0);
            }
        }
    }
}

SCENARIO("The worker transmits changed parameters, paced and in FIFO order", "[RQ-FMW-040][RQ-FMW-041][RQ-FMW-042]")
{
    GIVEN("a running controller with a synth output")
    {
        Fixture f;
        REQUIRE(f.controller.setSynthOutputDevice(SYNTH_OUT));
        f.controller.start();
        REQUIRE(f.controller.isRunning());

        WHEN("two parameters change")
        {
            f.controller.setParameter("FREQ", 10);
            f.controller.setParameter("DETUNE", -5);

            THEN("both SysEx go out, FREQ first (map order)")
            {
                REQUIRE(f.waitForSentCount(2));
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                CHECK(sent[0][testing::TestParameter::VALUE_INDEX] == 10);
                CHECK(sent[1][testing::TestParameter::VALUE_INDEX] == (-5 & 0x7F));
            }
        }

        WHEN("the controller stops")
        {
            f.controller.stop();

            THEN("it stops promptly and later changes are not transmitted")
            {
                CHECK_FALSE(f.controller.isRunning());
                f.backend.clearSentMessages();
                f.controller.setParameter("FREQ", 33);
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                CHECK(f.backend.sentMessages(SYNTH_OUT).empty());
            }
        }
    }
}

SCENARIO("Automation CC input scales to the parameter range", "[RQ-FMW-050]")
{
    GIVEN("a running controller with mapped CCs")
    {
        Fixture f;
        REQUIRE(f.controller.setAutomationInputDevice(AUTO_IN));
        REQUIRE(f.controller.setSynthOutputDevice(SYNTH_OUT));
        f.controller.controlChangeAutomationTable().add("FREQ", 74);
        f.controller.controlChangeAutomationTable().add("DETUNE", 75);
        f.controller.controlChangeAutomationTable().add("SWITCH", 76);

        std::vector<std::pair<std::string, int>> notified;
        f.controller.setAutomationParameterChangeHandler(
            [&notified](const std::string& name, int value) { notified.emplace_back(name, value); });

        WHEN("CC 74 = 127 arrives (range 0..63)")
        {
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::Controller, 0, 74, 127));

            THEN("FREQ scales to 63 and the UI is notified")
            {
                CHECK(f.controller.getParameter("FREQ")->value() == 63);
                REQUIRE(notified.size() == 1);
                CHECK(notified[0] == std::pair<std::string, int>{"FREQ", 63});
            }
        }

        WHEN("CC 75 = 127 then 0 arrives (signed range -31..31, interleave 62)")
        {
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::Controller, 0, 75, 127));
            CHECK(f.controller.getParameter("DETUNE")->value() == 31);
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::Controller, 0, 75, 0));
            CHECK(f.controller.getParameter("DETUNE")->value() == -31);
        }

        WHEN("a two-state parameter receives mid-range values")
        {
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::Controller, 0, 76, 64));
            THEN("values above 63 snap to max (reference special case)")
            {
                CHECK(f.controller.getParameter("SWITCH")->value() == 1);
            }
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::Controller, 0, 76, 63));
            CHECK(f.controller.getParameter("SWITCH")->value() == 0);
        }

        WHEN("the CC is temporarily disabled")
        {
            f.controller.setDisabledControlChangeNumber(74);
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::Controller, 0, 74, 127));

            THEN("the parameter is untouched and nothing is forwarded")
            {
                CHECK(f.controller.getParameter("FREQ")->value() == 0);
                CHECK(f.backend.sentMessages(SYNTH_OUT).empty());
            }
        }
    }
}

SCENARIO("Non-automated automation input is forwarded to the synth", "[RQ-FMW-051][RQ-FMW-052]")
{
    GIVEN("a running controller on MIDI channel 4 with no mapping")
    {
        Fixture f;
        REQUIRE(f.controller.setAutomationInputDevice(AUTO_IN));
        REQUIRE(f.controller.setSynthOutputDevice(SYNTH_OUT));
        f.controller.setMidiChannel(4);

        WHEN("an unmapped CC arrives on channel 0")
        {
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::Controller, 0, 11, 99));

            THEN("it is forwarded re-stamped on channel 4")
            {
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(sent.size() == 1);
                CHECK(sent[0][0] == 0xB4);
                CHECK(sent[0].data1() == 11);
                CHECK(sent[0].data2() == 99);
            }
        }

        WHEN("a note and a SysEx arrive")
        {
            f.backend.injectIncoming(AUTO_IN, MidiMessage::channelMessage(ChannelCommand::NoteOn, 2, 60, 100));
            const std::vector<std::uint8_t> frame{0xF0, 0x7E, 0x00, 0xF7};
            f.backend.injectIncoming(AUTO_IN, MidiMessage::sysEx(frame));

            THEN("the note is re-stamped, the SysEx forwarded verbatim")
            {
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(sent.size() == 2);
                CHECK(sent[0][0] == 0x94);
                CHECK(sent[1].type() == MessageType::SysEx);
            }
        }
    }
}

SCENARIO("Randomize renames the tone and retransmits everything", "[RQ-FMW-011][RQ-FMW-060]")
{
    GIVEN("a running controller")
    {
        Fixture f;
        REQUIRE(f.controller.setSynthOutputDevice(SYNTH_OUT));
        f.controller.start();
        int notifications = 0;
        f.controller.setAutomationParameterChangeHandler([&notifications](const std::string&, int)
                                                         { ++notifications; });

        WHEN("randomizing with a fixed seed")
        {
            controller::RandomizeToneArguments arguments;
            arguments.seed = 7U;
            f.controller.randomizeTone(arguments);

            THEN("the tone is RANDOM, every parameter notified and flagged")
            {
                CHECK(f.controller.toneName() == "RANDOM");
                CHECK(notifications == 3);
                CHECK(f.controller.isRunning());
                // all three flagged changed -> transmitted by the worker
                CHECK(f.waitForSentCount(3));
            }
        }
    }
}

SCENARIO("PlayNote sends note on/off on the tone channel", "[RQ-FMW-030]")
{
    GIVEN("a controller with a synth output on channel 2")
    {
        Fixture f;
        REQUIRE(f.controller.setSynthOutputDevice(SYNTH_OUT));
        f.controller.setMidiChannel(2);

        WHEN("playing and releasing note 60")
        {
            f.controller.playNote(true, 60);
            f.controller.playNote(false, 60);

            THEN("Note On v127 then Note Off v0 are sent")
            {
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(sent.size() == 2);
                CHECK(sent[0][0] == 0x92);
                CHECK(sent[0].data2() == 127);
                CHECK(sent[1][0] == 0x82);
                CHECK(sent[1].data2() == 0);
            }
        }
    }
}
