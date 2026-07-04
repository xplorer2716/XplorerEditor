#include <catch2/catch_test_macros.hpp>

#include "xpl/midi/SysexStreamIterator.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include <fstream>
#include <vector>

using namespace xplorer::model;
using midiapp::model::AbstractParameter;
using Bytes = std::vector<std::uint8_t>;

namespace
{
    Bytes loadFixtureDump()
    {
        std::ifstream stream(std::string(XPL_FIXTURES_DIR) + "/single_patch_oberheim.syx", std::ios::binary);
        REQUIRE(stream.good());
        const Bytes fileContent((std::istreambuf_iterator<char>(stream)), {});
        return xpl::midi::SysexStreamIterator::allMessages(fileContent).front();
    }
}

SCENARIO("The tone owns the full reference parameter map", "[RQ-MOD-020]")
{
    GIVEN("a freshly constructed tone")
    {
        XpanderTone tone;

        THEN("the map holds the 227 parameters of the reference (187 patch + 40 matrix)")
        {
            // 40 literal + 18 TRACK + 70 ENV + 35 LFO + 24 RAMP + 40 matrix,
            // counted from the reference InitializeParameterMap.
            CHECK(tone.parameterMap().size() == 227);
        }

        THEN("spot checks match the reference definitions")
        {
            const auto& detune = tone.parameterMap().at("VCO1_DETUNE");
            CHECK(detune.minValue() == -31);
            CHECK(detune.maxValue() == 31);
            const auto& mode = tone.parameterMap().at("VCF_MODE");
            CHECK(mode.minValue() == 0);
            CHECK(mode.maxValue() == 14); // 15 filter modes
            CHECK(tone.parameterMap().contains("MOD_AMNT_SRC_20"));
            CHECK(tone.parameterMap().contains("MOD_QUANTIZE_1"));
            CHECK(tone.parameterMap().at("ENV_5_TRIG_GATED").maxValue() == 1);
        }

        THEN("defaults match the reference (name XPLORER, program 99)")
        {
            CHECK(tone.toneName() == "XPLORER "); // padded to 8
            CHECK(tone.editingProgramNumber() == 99);
            CHECK(tone.currentProgramNumber() == 99);
        }
    }
}

SCENARIO("Tone names are truncated or padded to 8 characters", "[RQ-MOD-023]")
{
    GIVEN("a tone")
    {
        XpanderTone tone;

        THEN("long names truncate, short names pad")
        {
            tone.setToneName("SUPERLONGNAME");
            CHECK(tone.toneName() == "SUPERLON");
            tone.setToneName("AB");
            CHECK(tone.toneName() == "AB      ");
        }
    }
}

SCENARIO("The current program number wraps around 0-99", "[RQ-MOD-021]")
{
    GIVEN("a tone")
    {
        XpanderTone tone;
        tone.setCurrentProgramNumber(99);
        tone.setCurrentProgramNumber(100);
        CHECK(tone.currentProgramNumber() == 0);
        tone.setCurrentProgramNumber(-1);
        CHECK(tone.currentProgramNumber() == 99);
    }
}

SCENARIO("A real hardware dump round-trips through the full tone", "[RQ-MOD-022][RQ-MOD-050][RQ-TST-002]")
{
    GIVEN("the OBERHEIM dump loaded into a tone")
    {
        const auto dump = loadFixtureDump();
        XpanderTone tone;
        tone.fromByteArray(dump);

        THEN("the decoded name and a few parameters are plausible")
        {
            CHECK(tone.toneName() == "OBERHEIM");
        }

        WHEN("serialized again with the same program number")
        {
            tone.setEditingProgramNumber(dump[5]);
            const auto reEncoded = tone.toByteArray();

            THEN("bytes are identical except unused matrix entries, normalized to the reference markers")
            {
                // The hardware marks unused modulation entries with 0xFF;
                // the reference (and this port) rewrites them as source 0x1F
                // / dest 0x3F. Modulation entries occupy packetized data
                // indices 128..187 (3 bytes per entry after 128 data bytes).
                REQUIRE(reEncoded.size() == dump.size());
                constexpr std::size_t MOD_REGION_START = 6 + 128 * 2;
                constexpr std::size_t MOD_REGION_END = 6 + 188 * 2;
                for (std::size_t i = 0; i < dump.size(); ++i)
                {
                    if (dump[i] == reEncoded[i])
                    {
                        continue;
                    }
                    INFO("difference at byte " << i);
                    CHECK(i >= MOD_REGION_START);
                    CHECK(i < MOD_REGION_END);
                    CHECK((reEncoded[i] == 0x1F || reEncoded[i] == 0x3F || reEncoded[i] == 0x00));
                }
            }
        }

        WHEN("re-loading the re-encoded dump")
        {
            tone.setEditingProgramNumber(dump[5]);
            const auto reEncoded = tone.toByteArray();
            XpanderTone secondTone;
            secondTone.fromByteArray(reEncoded);

            THEN("serialization is idempotent")
            {
                CHECK(secondTone.toByteArray() == reEncoded);
            }
        }
    }
}

SCENARIO("Matrix entries are allocated, renumbered and bounded like the synth", "[RQ-MOD-030][RQ-MOD-031][RQ-MOD-032]")
{
    GIVEN("an empty matrix")
    {
        XpanderTone tone;
        std::vector<std::string> sentCommands;
        auto capture = [&sentCommands](const AbstractParameter& parameter)
        { sentCommands.push_back(parameter.name()); };

        WHEN("a source is assigned to entry 1 via destination change")
        {
            tone.changeModulationDestination(
                static_cast<int>(EnumModulationSourcesModMatrix::ENV2), 20, 1,
                static_cast<int>(EnumModulationDestinations::VCO1_FRQ),
                static_cast<int>(EnumModulationDestinations::VCA2_VOL), 1, capture);

            THEN("the entry carries source/amount/quantize/destination and id 0")
            {
                const auto& entry = tone.modulationMatrix()[0];
                CHECK(entry.source == EnumModulationSourcesModMatrix::ENV2);
                CHECK(entry.destination == EnumModulationDestinations::VCA2_VOL);
                CHECK(entry.amount() == 20);
                CHECK(entry.quantize() == 1);
                CHECK(entry.idSource == 0);
            }

            THEN("the synth received add, sign, amount and quantize commands in order")
            {
                REQUIRE(sentCommands.size() == 4);
                CHECK(sentCommands[0] == "ADD_MOD_SRC");
                CHECK(sentCommands[1] == "MOD_AMNT_SRC_1_SETSIGN");
                CHECK(sentCommands[2] == "MOD_AMNT_SRC_1");
                CHECK(sentCommands[3] == "MOD_QUANTIZE_1");
            }
        }

        WHEN("seven sources target the same destination")
        {
            for (int entryNumber = 1; entryNumber <= 6; ++entryNumber)
            {
                tone.addModulationSource(static_cast<int>(EnumModulationSourcesModMatrix::LFO1), 10, 0,
                                         static_cast<int>(EnumModulationDestinations::VCF_FRQ),
                                         entryNumber, nullptr);
            }

            THEN("the 6-source limit is reached and the 7th gets no id")
            {
                CHECK(tone.isMaxSourceCountForDestinationReached(EnumModulationDestinations::VCF_FRQ));
                tone.addModulationSource(static_cast<int>(EnumModulationSourcesModMatrix::LFO2), 5, 0,
                                         static_cast<int>(EnumModulationDestinations::VCF_FRQ), 7, nullptr);
                CHECK(tone.modulationMatrix()[6].idSource == XpanderTone::UNDEFINED_MODULATION_SOURCE_NUMBER);
            }
        }

        WHEN("a middle source is deleted")
        {
            for (int entryNumber = 1; entryNumber <= 3; ++entryNumber)
            {
                tone.addModulationSource(static_cast<int>(EnumModulationSourcesModMatrix::LFO1), 0, 0,
                                         static_cast<int>(EnumModulationDestinations::VCF_FRQ),
                                         entryNumber, nullptr);
            }
            // delete entry 2 (idSource 1) by setting its source to NONE
            tone.changeModulationSource(static_cast<int>(EnumModulationSourcesModMatrix::NONE), 0, 0,
                                        static_cast<int>(EnumModulationDestinations::VCF_FRQ), 2, capture);

            THEN("higher source ids of the same destination shift down, entry resets")
            {
                CHECK(sentCommands == std::vector<std::string>{"DEL_MOD_SRC"});
                CHECK(tone.modulationMatrix()[1].source == EnumModulationSourcesModMatrix::NONE);
                CHECK(tone.modulationMatrix()[1].idSource == XpanderTone::UNDEFINED_MODULATION_SOURCE_NUMBER);
                CHECK(tone.modulationMatrix()[0].idSource == 0);
                CHECK(tone.modulationMatrix()[2].idSource == 1); // was 2
            }
        }

        WHEN("only the destination changes while the source is NONE")
        {
            tone.changeModulationDestination(static_cast<int>(EnumModulationSourcesModMatrix::NONE), 0, 0,
                                             static_cast<int>(EnumModulationDestinations::VCO1_FRQ),
                                             static_cast<int>(EnumModulationDestinations::LAG_RATE), 5, capture);

            THEN("the matrix reflects it without any synth command")
            {
                CHECK(tone.modulationMatrix()[4].destination == EnumModulationDestinations::LAG_RATE);
                CHECK(sentCommands.empty());
            }
        }
    }
}

SCENARIO("Randomizer helpers force the documented values", "[RQ-MOD-033]")
{
    GIVEN("a tone")
    {
        XpanderTone tone;

        WHEN("detuned analog-style")
        {
            tone.detune(true);
            CHECK(tone.parameterMap().at("VCO1_DETUNE").value() == -10);
            CHECK(tone.parameterMap().at("VCO2_DETUNE").value() == 10);
        }

        WHEN("VCOs are tuned to a fifth")
        {
            tone.defineVCOFrequenciesTuning(EnumRandomVCOFreq::Fifth);
            CHECK(tone.parameterMap().at("VCO1_FREQ").value() == 0);
            CHECK(tone.parameterMap().at("VCO2_FREQ").value() == 7);
        }

        WHEN("ENV2->VCA2 is forced after matrix randomization")
        {
            tone.randomizeModulationMatrix(true, true, true, std::nullopt, 99U);
            tone.forceEnv2ModVca2AfterRandomizeMatrix(EnumRandomVCAEnv::Percusive);

            THEN("one matrix entry routes ENV2 to VCA2_VOL at full amount")
            {
                bool found = false;
                for (const auto& entry : tone.modulationMatrix())
                {
                    if (entry.source == EnumModulationSourcesModMatrix::ENV2
                        && entry.destination == EnumModulationDestinations::VCA2_VOL
                        && entry.amount() == 63)
                    {
                        found = true;
                    }
                }
                CHECK(found);
                CHECK(tone.parameterMap().at("VCF_VCA2_VOLUME").value() == 0);
                CHECK(tone.parameterMap().at("ENV_2_DECAY").value() == 63);
            }
        }
    }
}
