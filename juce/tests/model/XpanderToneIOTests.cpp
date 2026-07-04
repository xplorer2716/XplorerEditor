#include <catch2/catch_test_macros.hpp>

#include "xplorer/model/XpanderToneIO.hpp"
#include "xplorer/model/XpanderTone.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

using namespace xplorer::model;
using Bytes = std::vector<std::uint8_t>;

namespace
{
    const std::string FIXTURE = std::string(XPL_FIXTURES_DIR) + "/single_patch_oberheim.syx";

    std::filesystem::path tempFile(const char* name)
    {
        return std::filesystem::temp_directory_path() / name;
    }

    void writeBytes(const std::filesystem::path& path, const Bytes& bytes)
    {
        std::ofstream stream(path, std::ios::binary | std::ios::trunc);
        stream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    }
}

SCENARIO("Sysex files are classified like the reference", "[RQ-MOD-043]")
{
    GIVEN("various .syx files")
    {
        THEN("the fixture (patch + page-select frame) classifies as a dump/bank")
        {
            // two frames in the file -> AllDataDump, per the reference rule
            CHECK(determineSysexFileType(FIXTURE) == SysexFileType::AllDataDump);
        }

        THEN("a lone single-patch frame classifies as SingleTone")
        {
            XpanderTone tone;
            const auto path = tempFile("xpl_lone_patch.syx");
            writeBytes(path, tone.toByteArray());
            CHECK(determineSysexFileType(path.string()) == SysexFileType::SingleTone);
        }

        THEN("a lone unrecognized frame classifies as AllDataDump")
        {
            const auto path = tempFile("xpl_unknown_frame.syx");
            writeBytes(path, Bytes{0xF0, 0x10, 0x02, 0x02, 0x00, 0xF7});
            CHECK(determineSysexFileType(path.string()) == SysexFileType::AllDataDump);
        }

        THEN("a missing or empty file classifies as Unknown")
        {
            CHECK(determineSysexFileType("/nonexistent/file.syx") == SysexFileType::Unknown);
            const auto path = tempFile("xpl_empty.syx");
            writeBytes(path, {});
            CHECK(determineSysexFileType(path.string()) == SysexFileType::Unknown);
        }
    }
}

SCENARIO("Tones load and save through the reader/writer", "[RQ-MOD-041][RQ-MOD-042][RQ-MOD-050]")
{
    GIVEN("the fixture file")
    {
        XpanderToneReader reader;
        XpanderToneWriter writer;

        WHEN("reading the first tone")
        {
            XpanderTone tone;
            reader.readTone(FIXTURE, tone);

            THEN("the OBERHEIM patch is loaded")
            {
                CHECK(tone.toneName() == "OBERHEIM");
            }

            AND_WHEN("it is written and read back")
            {
                const auto path = tempFile("xpl_roundtrip.syx");
                tone.setEditingProgramNumber(99);
                writer.writeTone(path.string(), tone);

                XpanderTone reloaded;
                reader.readTone(path.string(), reloaded);

                THEN("all parameter values survive the file round-trip")
                {
                    CHECK(reloaded.toneName() == "OBERHEIM");
                    CHECK(reloaded.toByteArray() == tone.toByteArray());
                }
            }
        }

        WHEN("enumerating all patches of the file")
        {
            const auto tones = reader.readTones(FIXTURE);

            THEN("the single patch is listed with its name")
            {
                REQUIRE(tones.size() == 1);
                CHECK(tones[0].first == "OBERHEIM");
            }
        }

        WHEN("reading a file without any patch")
        {
            const auto path = tempFile("xpl_no_patch.syx");
            writeBytes(path, Bytes{0xF0, 0x7E, 0x00, 0xF7});
            XpanderTone tone;

            THEN("a ToneException is raised")
            {
                CHECK_THROWS_AS(reader.readTone(path.string(), tone), midiapp::model::ToneException);
                CHECK_THROWS_AS(reader.readTones(path.string()), midiapp::model::ToneException);
            }
        }
    }
}
