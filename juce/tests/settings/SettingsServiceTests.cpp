#include <catch2/catch_test_macros.hpp>

#include "xplorer/settings/SettingsService.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>

using namespace xplorer::settings;
using namespace xplorer::model;

namespace
{
    std::filesystem::path freshTempDir(const char* name)
    {
        const auto dir = std::filesystem::temp_directory_path() / name;
        std::filesystem::remove_all(dir);
        std::filesystem::create_directories(dir);
        return dir;
    }
}

SCENARIO("Defaults match the reference values", "[RQ-SET-002][RQ-SET-003][RQ-SET-004]")
{
    GIVEN("the default settings")
    {
        const auto settings = defaultAllUsersSettings();

        THEN("MIDI defaults are the reference ones")
        {
            CHECK(settings.midiConfig.sysexTransmitDelay == 30);
            CHECK(settings.midiConfig.midiChannel == 1);
            CHECK(settings.midiConfig.editingProgramNumber == 99);
            CHECK(settings.midiConfig.smartAllNotesOff);
            CHECK_FALSE(settings.midiConfig.synthTypeIsMatrix12);
            CHECK(settings.midiConfig.automationTable.size() == 187);
            CHECK(settings.midiConfig.automationTable.front() == "VCO1_FREQ;128");
            CHECK(settings.midiConfig.automationTable.back() == "RAMP_4_TRIG_GATED;128");
        }

        THEN("UI and randomizer defaults are the reference ones")
        {
            CHECK(settings.uiConfig.knobLedBorderColor == static_cast<std::int32_t>(0xFF66B5E3));
            CHECK(settings.uiConfig.knobMovementIsLinear);
            CHECK(static_cast<int>(settings.randomizerConfig.vco2FmNoiseSync) == 7);
            CHECK(settings.randomizerConfig.vcoFreq == EnumRandomVCOFreq::Free);
            CHECK(static_cast<int>(settings.randomizerConfig.modulationMatrix) == 7);
        }
    }
}

SCENARIO("Missing or corrupted files fall back to persisted defaults", "[RQ-SET-001][RQ-SET-004]")
{
    GIVEN("an empty settings directory")
    {
        const auto dir = freshTempDir("xpl_settings_empty");
        XmlSettingsService service(dir.string());

        WHEN("settings are accessed")
        {
            const auto& settings = service.allUsersSettings();

            THEN("defaults are returned and the file now exists")
            {
                CHECK(settings.midiConfig.sysexTransmitDelay == 30);
                CHECK(std::filesystem::exists(service.settingsFilePath()));
            }
        }
    }

    GIVEN("a corrupted settings file")
    {
        const auto dir = freshTempDir("xpl_settings_corrupt");
        std::ofstream(dir / XmlSettingsService::SETTINGS_FILE_NAME) << "not xml at all <<<";
        XmlSettingsService service(dir.string());

        THEN("defaults are returned")
        {
            CHECK(service.allUsersSettings().midiConfig.midiChannel == 1);
        }
    }
}

SCENARIO("Settings round-trip through the XML file", "[RQ-SET-001][RQ-SET-002]")
{
    GIVEN("modified settings saved to disk")
    {
        const auto dir = freshTempDir("xpl_settings_roundtrip");
        {
            XmlSettingsService service(dir.string());
            auto settings = service.allUsersSettings();
            settings.midiConfig.synthInputDeviceName = "MIDI <In> & \"Out\"";
            settings.midiConfig.sysexTransmitDelay = 55;
            settings.midiConfig.synthTypeIsMatrix12 = true;
            settings.midiConfig.automationTable = {"VCF_FREQ;74"};
            settings.uiConfig.knobStyleIsStandard = true;
            settings.randomizerConfig.vca2Env = EnumRandomVCAEnv::Percusive;
            service.saveSettings(settings);
        }

        WHEN("a new service instance reads the same directory")
        {
            XmlSettingsService service(dir.string());
            const auto& settings = service.allUsersSettings();

            THEN("all values persisted, XML-escaped characters included")
            {
                CHECK(settings.midiConfig.synthInputDeviceName == "MIDI <In> & \"Out\"");
                CHECK(settings.midiConfig.sysexTransmitDelay == 55);
                CHECK(settings.midiConfig.synthTypeIsMatrix12);
                CHECK(settings.midiConfig.automationTable == std::vector<std::string>{"VCF_FREQ;74"});
                CHECK(settings.uiConfig.knobStyleIsStandard);
                CHECK(settings.randomizerConfig.vca2Env == EnumRandomVCAEnv::Percusive);
            }
        }

        WHEN("reset is requested")
        {
            XmlSettingsService service(dir.string());
            service.resetSettings();

            THEN("defaults are back")
            {
                CHECK(service.allUsersSettings().midiConfig.sysexTransmitDelay == 30);
            }
        }
    }
}

SCENARIO("A .NET XmlSerializer file imports unchanged", "[RQ-SET-006]")
{
    GIVEN("a settings file as the reference application writes it")
    {
        const auto dir = freshTempDir("xpl_settings_dotnet");
        std::ofstream(dir / XmlSettingsService::SETTINGS_FILE_NAME) <<
            R"(<?xml version="1.0" encoding="utf-8"?>
<AllUsersSettings xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <MidiConfig>
    <AutomationInputDeviceName>LoopBe Internal MIDI</AutomationInputDeviceName>
    <SynthInputDeviceName>MIDISPORT 2x2 A</SynthInputDeviceName>
    <SynthOutputDeviceName>MIDISPORT 2x2 Out A</SynthOutputDeviceName>
    <SysexTransmitDelay>30</SysexTransmitDelay>
    <MidiChannel>1</MidiChannel>
    <EditingProgramNumber>99</EditingProgramNumber>
    <SmartAllNotesOff>true</SmartAllNotesOff>
    <SynthTypeIsMatrix12>false</SynthTypeIsMatrix12>
    <AutomationTable>
      <string>VCO1_FREQ;128</string>
      <string>VCF_FREQ;74</string>
    </AutomationTable>
  </MidiConfig>
  <UiConfig>
    <KnobLedBorderColor>-10111517</KnobLedBorderColor>
    <KnobMovementIsLinear>true</KnobMovementIsLinear>
    <KnobStyleIsStandard>false</KnobStyleIsStandard>
  </UiConfig>
  <RandomizerConfig>
    <VCO2FmNoiseSync>EnableFM EnableNoise EnableSync</VCO2FmNoiseSync>
    <VCOFreq>Octave</VCOFreq>
    <VCODetune>Analog</VCODetune>
    <VCA2Env>Free</VCA2Env>
    <ModulationMatrix>EnableAmount EnableQuantize</ModulationMatrix>
  </RandomizerConfig>
</AllUsersSettings>)";

        WHEN("the C++ service loads it")
        {
            XmlSettingsService service(dir.string());
            const auto& settings = service.allUsersSettings();

            THEN("every value imports")
            {
                CHECK(settings.midiConfig.automationInputDeviceName == "LoopBe Internal MIDI");
                CHECK(settings.midiConfig.synthInputDeviceName == "MIDISPORT 2x2 A");
                CHECK(settings.midiConfig.synthOutputDeviceName == "MIDISPORT 2x2 Out A");
                CHECK(settings.midiConfig.automationTable.size() == 2);
                CHECK(settings.uiConfig.knobLedBorderColor == -10111517);
                CHECK(settings.randomizerConfig.vcoFreq == EnumRandomVCOFreq::Octave);
                CHECK(settings.randomizerConfig.vcoDetune == EnumRandomVCODetune::Analog);
                CHECK(static_cast<int>(settings.randomizerConfig.modulationMatrix) == 5);
            }
        }
    }
}
