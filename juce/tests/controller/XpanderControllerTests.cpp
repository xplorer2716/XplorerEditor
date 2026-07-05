#include <catch2/catch_test_macros.hpp>

#include "xplorer/controller/XpanderController.hpp"
#include "xpl/midi/MockMidiBackend.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

using namespace xplorer;
using namespace xpl::midi;
using controller::XpanderController;

namespace
{
    constexpr auto SYNTH_IN = "Synth In";
    constexpr auto SYNTH_OUT = "Synth Out";

    std::vector<std::uint8_t> readFixture()
    {
        std::ifstream stream(std::string(XPL_FIXTURES_DIR) + "/single_patch_oberheim.syx", std::ios::binary);
        REQUIRE(stream.good());
        return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(stream), {});
    }

    std::vector<std::uint8_t> fixturePatchDump()
    {
        auto bytes = readFixture();
        bytes.resize(399); // first frame only: the single-patch program dump
        return bytes;
    }

    struct Fixture
    {
        Fixture()
        {
            backend.addInputDevice(SYNTH_IN);
            backend.addOutputDevice(SYNTH_OUT);
            // deterministic sequences: no smart all-notes-off unless a test enables it
            auto settingsCopy = settingsService.allUsersSettings();
            settingsCopy.midiConfig.smartAllNotesOff = false;
            settingsService.saveSettings(settingsCopy);
            controller.setParameterTransmitDelay(1);
            REQUIRE(controller.setSynthOutputDevice(SYNTH_OUT));
            REQUIRE(controller.setSynthInputDevice(SYNTH_IN));
        }

        ~Fixture() { controller.stop(); }

        bool waitForSentCount(std::size_t count)
        {
            for (int i = 0; i < 1000; ++i)
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
        settings::InMemorySettingsService settingsService;
        XpanderController controller{backend, settingsService, nullptr, "XPLORER TEST 1.0"};
    };
}

SCENARIO("The worker sends a page select before a parameter of another page", "[RQ-CTL-020]")
{
    GIVEN("a started controller (synchronized on VCO_1_X after first start)")
    {
        Fixture f;
        f.controller.start();
        f.backend.clearSentMessages();

        WHEN("a VCF page parameter changes")
        {
            REQUIRE(f.controller.setParameter("VCF_FREQ", 42));

            THEN("a page-select frame precedes the parameter frame")
            {
                REQUIRE(f.waitForSentCount(2));
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                CHECK(sent[0][3] == 0x0B); // page select opcode
                CHECK(sent[0][4] == static_cast<int>(model::EnumPages::VCF_VCA_X));
                CHECK(sent[1][3] == 0x0A); // parameter edit opcode
                CHECK(sent[1][9] == 42);
            }
        }

        WHEN("a second parameter of the same page follows")
        {
            REQUIRE(f.controller.setParameter("VCF_FREQ", 10));
            REQUIRE(f.waitForSentCount(2));
            f.backend.clearSentMessages();
            REQUIRE(f.controller.setParameter("VCF_RES", 20));

            THEN("no extra page select is sent")
            {
                REQUIRE(f.waitForSentCount(1));
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                CHECK(sent[0][3] == 0x0A);
            }
        }
    }
}

SCENARIO("A single patch dump from the synth reloads the edited tone", "[RQ-CTL-021]")
{
    GIVEN("a started controller and the OBERHEIM dump")
    {
        Fixture f;
        f.controller.start();
        int fullToneEvents = 0;
        std::string receivedName;
        f.controller.setFullToneChangeHandler(
            [&](const controller::FullToneChangeEvent& event)
            {
                ++fullToneEvents;
                receivedName = std::string(); // name is not in the map; check via controller
                (void)event;
            });

        WHEN("the synth sends the dump")
        {
            f.backend.injectIncoming(SYNTH_IN, MidiMessage::sysEx(fixturePatchDump()));

            THEN("the tone is reloaded and the full-tone event raised")
            {
                CHECK(fullToneEvents == 1);
                CHECK(f.controller.toneName() == "OBERHEIM");
                CHECK(f.controller.isRunning());
            }
        }
    }
}

SCENARIO("Panel edits (page edit follows) update the matching parameter", "[RQ-CTL-022][RQ-CTL-023]")
{
    GIVEN("a controller whose current page tracks the synth")
    {
        Fixture f;
        f.controller.start();
        std::vector<std::pair<std::string, int>> notified;
        f.controller.setAutomationParameterChangeHandler(
            [&](const std::string& name, int value) { notified.emplace_back(name, value); });

        // synth selects VCF_VCA page, sub-page 0
        const std::vector<std::uint8_t> pageSelect{
            0xF0, 0x10, 0x02, 0x0B, static_cast<std::uint8_t>(model::EnumPages::VCF_VCA_X), 0x00, 0xF7};
        f.backend.injectIncoming(SYNTH_IN, MidiMessage::sysEx(pageSelect));

        WHEN("the synth sends a button edit for VCF_FREQ")
        {
            const auto* parameter =
                dynamic_cast<const model::XpanderParameter*>(f.controller.getParameter("VCF_FREQ"));
            REQUIRE(parameter != nullptr);
            const auto buttonId = static_cast<std::uint8_t>(parameter->buttonId());
            const std::vector<std::uint8_t> pageEdit{
                0xF0, 0x10, 0x02, 0x0A, 0x00, buttonId, 0x00, 0x00, 0x00, 33, 0x00, 0xF7};
            f.backend.injectIncoming(SYNTH_IN, MidiMessage::sysEx(pageEdit));

            THEN("the parameter takes the value without echoing back (changed stays clear)")
            {
                CHECK(f.controller.getParameter("VCF_FREQ")->value() == 33);
                CHECK_FALSE(f.controller.getParameter("VCF_FREQ")->changed());
                REQUIRE(notified.size() == 1);
                CHECK(notified[0] == std::pair<std::string, int>{"VCF_FREQ", 33});
            }
        }
    }
}

SCENARIO("Program change up/down SysEx track the synth program", "[RQ-CTL-024]")
{
    GIVEN("a controller at program 99 (default editing program)")
    {
        Fixture f;
        f.controller.start();
        REQUIRE(f.controller.currentProgramNumber() == 99);
        f.backend.clearSentMessages();

        WHEN("the synth sends program-down")
        {
            const std::vector<std::uint8_t> down{0xF0, 0x10, 0x02, 0x0E, 0x08, 0xF7};
            f.backend.injectIncoming(SYNTH_IN, MidiMessage::sysEx(down));

            THEN("the program decrements and a dump request for it goes out")
            {
                CHECK(f.controller.currentProgramNumber() == 98);
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(sent.size() == 1);
                CHECK(sent[0][3] == 0x00);
                CHECK(sent[0][5] == 98);
            }
        }

        WHEN("the synth sends program-up (wraps 99 -> 0)")
        {
            const std::vector<std::uint8_t> up{0xF0, 0x10, 0x02, 0x0E, 0x04, 0xF7};
            f.backend.injectIncoming(SYNTH_IN, MidiMessage::sysEx(up));

            THEN("the program wraps to 0")
            {
                CHECK(f.controller.currentProgramNumber() == 0);
            }
        }
    }
}

SCENARIO("Modulation edit follows from the synth updates the local matrix", "[RQ-CTL-025]")
{
    GIVEN("a controller with the synth on the VCO1 frequency destination page")
    {
        Fixture f;
        f.controller.start();
        std::vector<controller::ModulationEntryChangeEvent> events;
        f.controller.setModulationEntryChangeHandler(
            [&](const controller::ModulationEntryChangeEvent& event) { events.push_back(event); });

        const auto pageSubPage = model::PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[
            static_cast<std::size_t>(model::EnumModulationDestinations::VCO1_FRQ)];
        const std::vector<std::uint8_t> pageSelect{
            0xF0, 0x10, 0x02, 0x0B, static_cast<std::uint8_t>(pageSubPage.page),
            static_cast<std::uint8_t>(pageSubPage.subPage), 0xF7};
        f.backend.injectIncoming(SYNTH_IN, MidiMessage::sysEx(pageSelect));

        WHEN("the synth adds LFO1 as source")
        {
            const auto lfo1 = static_cast<std::uint8_t>(model::EnumModulationSourcesModMatrix::LFO1);
            const std::vector<std::uint8_t> addSource{
                0xF0, 0x10, 0x02, 0x0F, 0x00, 0x00, 0x00,
                static_cast<std::uint8_t>(model::EnumModulationEditCommands::ADDSOURCE), 0x00,
                lfo1, 0x00, 0xF7};
            f.backend.injectIncoming(SYNTH_IN, MidiMessage::sysEx(addSource));

            THEN("entry 1 carries LFO1 -> VCO1_FRQ and the event is raised")
            {
                const auto& entry = f.controller.getModulationEntryByNumber(1);
                CHECK(entry.source == model::EnumModulationSourcesModMatrix::LFO1);
                CHECK(entry.destination == model::EnumModulationDestinations::VCO1_FRQ);
                REQUIRE(events.size() == 1);
                CHECK(events[0].entryNumber == 1);
                CHECK(events[0].parameter == controller::EnumModulationParameter::ALL);
            }
        }
    }
}

SCENARIO("Storing a patch sends tone, store command and resynchronizes", "[RQ-CTL-006]")
{
    GIVEN("a started controller")
    {
        Fixture f;
        f.controller.start();
        f.backend.clearSentMessages();

        WHEN("storing into program 12")
        {
            f.controller.storeSinglePatchToSynth(12);

            THEN("the sequence is: full tone (399 B), store 0x07, program change, page select, dump request")
            {
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(sent.size() >= 3);
                CHECK(sent[0].size() == 399);
                CHECK(sent[0][5] == 12); // program byte of the dump
                CHECK(sent[1][3] == 0x07);
                CHECK(sent[1][4] == 12);
                CHECK(f.controller.currentProgramNumber() == 12);
            }
        }
    }
}

SCENARIO("Clipboard pastes a page family instance onto another", "[RQ-CTL-040]")
{
    GIVEN("a controller with distinct ENV_1 values")
    {
        Fixture f;
        REQUIRE(f.controller.setParameter("ENV_1_ATTACK", 17));
        REQUIRE(f.controller.setParameter("ENV_1_DECAY", 23));
        f.controller.setClipboardSource("ENV_1");

        THEN("eligibility follows the reference rules")
        {
            CHECK(f.controller.canClipboardPasteTo("ENV_3"));
            CHECK_FALSE(f.controller.canClipboardPasteTo("ENV_1")); // same instance
            CHECK_FALSE(f.controller.canClipboardPasteTo("LFO_2")); // other family
        }

        WHEN("pasting to ENV_3")
        {
            f.controller.pasteClipboardTo("ENV_3");

            THEN("ENV_3 got ENV_1's values")
            {
                CHECK(f.controller.getParameter("ENV_3_ATTACK")->value() == 17);
                CHECK(f.controller.getParameter("ENV_3_DECAY")->value() == 23);
            }
        }
    }
}

SCENARIO("Synth utilities emit the reference byte frames", "[RQ-CTL-060][RQ-CTL-061][RQ-CTL-062]")
{
    GIVEN("a controller")
    {
        Fixture f;

        WHEN("sending a tune request")
        {
            f.controller.sendTuneRequestToSynth();

            THEN("the reference SysEx-wrapped frame goes out")
            {
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(sent.size() == 1);
                CHECK(sent[0].toBytes() == std::vector<std::uint8_t>{0xF0, 0xF6, 0xF7});
            }
        }

        WHEN("smart all-notes-off is enabled and the controller stops")
        {
            auto settingsCopy = f.settingsService.allUsersSettings();
            settingsCopy.midiConfig.smartAllNotesOff = true;
            settingsCopy.midiConfig.midiChannel = 3;
            f.settingsService.saveSettings(settingsCopy);
            f.backend.clearSentMessages();
            f.controller.stop();

            THEN("CC 123 is sent on the configured channel")
            {
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(sent.size() == 1);
                CHECK(sent[0][0] == 0xB3);
                CHECK(sent[0].data1() == 123);
            }
        }

        WHEN("greeting an Xpander, then a Matrix-12")
        {
            f.controller.sendGreetingsToSynth();
            const auto xpanderFrames = f.backend.sentMessages(SYNTH_OUT);
            REQUIRE(xpanderFrames.size() == 3); // display off, on, text
            CHECK(xpanderFrames[0][3] == 0x05);

            auto settingsCopy = f.settingsService.allUsersSettings();
            settingsCopy.midiConfig.synthTypeIsMatrix12 = true;
            f.settingsService.saveSettings(settingsCopy);
            f.backend.clearSentMessages();
            f.controller.sendGreetingsToSynth();

            THEN("the display command switches from 0x05 to 0x06 and the text is uppercased")
            {
                const auto m12Frames = f.backend.sentMessages(SYNTH_OUT);
                REQUIRE(m12Frames.size() == 3);
                CHECK(m12Frames[0][3] == 0x06);
                CHECK(m12Frames[2][4] == 0x01); // display text opcode
                CHECK(m12Frames[2].size() == 5 + 80 + 1);
                CHECK(m12Frames[2][5] == 'X'); // "XPLORER TEST 1.0"
            }
        }
    }
}

SCENARIO("Restore all-data dump paces frames and reports progression", "[RQ-CTL-005]")
{
    GIVEN("a .syx file with two frames (the fixture)")
    {
        Fixture f;
        const auto dir = std::filesystem::temp_directory_path() / "xpl_ctl_restore";
        std::filesystem::create_directories(dir);
        const auto file = dir / "backup.syx";
        const auto bytes = readFixture();
        std::ofstream(file, std::ios::binary)
            .write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));

        WHEN("restoring it")
        {
            std::vector<std::pair<int, int>> progression;
            f.controller.restoreAllDataDumpToSynth(file.string(),
                                                   [&](int index, int count)
                                                   { progression.emplace_back(index, count); });

            THEN("both frames were sent and progression reported 0/2 then 1/2")
            {
                REQUIRE(progression.size() == 2);
                CHECK(progression[0] == std::pair<int, int>{0, 2});
                CHECK(progression[1] == std::pair<int, int>{1, 2});
                const auto sent = f.backend.sentMessages(SYNTH_OUT);
                CHECK(sent.size() >= 2);
                CHECK(sent[0].size() == 399);
                CHECK(sent[1].size() == 7);
            }
        }
    }
}

SCENARIO("Get-all-patches writes one file per received dump", "[RQ-CTL-004]")
{
    GIVEN("a controller asked to fetch all single tones")
    {
        Fixture f;
        f.controller.start();
        const auto dir = std::filesystem::temp_directory_path() / "xpl_ctl_getall";
        std::filesystem::remove_all(dir);
        std::filesystem::create_directories(dir);
        int progressionEvents = 0;
        f.controller.setAllDataDumpProgressionHandler(
            [&](const controller::AllDataDumpProgressionEvent&) { ++progressionEvents; });

        WHEN("the synth answers with 100 single patch dumps")
        {
            f.controller.getSingleTonesFromSynth(dir.string());
            const auto dump = MidiMessage::sysEx(fixturePatchDump());
            for (int i = 0; i < 100; ++i)
            {
                f.backend.injectIncoming(SYNTH_IN, dump);
            }

            THEN("100 uniquely-named .syx files exist and progression fired each time")
            {
                CHECK(progressionEvents == 100);
                const auto fileCount = std::distance(std::filesystem::directory_iterator(dir),
                                                     std::filesystem::directory_iterator{});
                CHECK(fileCount == 100);
            }
        }
    }
}
