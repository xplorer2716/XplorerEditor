#include <catch2/catch_test_macros.hpp>

#include "midiapp/service/FileUtils.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

// File-name helpers behind the Save dialog default name and the bank
// extraction. Traceability: TASK-QLT-017 (formatStr for the counter) and
// TASK-QLT-020 (string_view parameter).
// [RQ-CTL-003, RQ-CTL-004, RQ-GUI-077, RQ-QLT-018, RQ-QLT-019]

using namespace midiapp::service;

namespace
{
    /// A throwaway directory, removed with the object.
    struct TempDirectory
    {
        TempDirectory() : path(std::filesystem::temp_directory_path() / "xpl_fileutils_test")
        {
            std::filesystem::remove_all(path);
            std::filesystem::create_directories(path);
        }

        ~TempDirectory() { std::filesystem::remove_all(path); }

        TempDirectory(const TempDirectory&) = delete;
        TempDirectory& operator=(const TempDirectory&) = delete;

        void touch(const std::string& fileName) const
        {
            std::ofstream stream(path / fileName);
            stream.put('x');
        }

        std::filesystem::path path;
    };
}

SCENARIO("Trailing spaces are trimmed, leading and interior ones are kept", "[RQ-GUI-077][RQ-QLT-019]")
{
    GIVEN("tone names as the model stores them, space-padded to a fixed width")
    {
        THEN("only the trailing run is removed")
        {
            CHECK(trimTrailingSpaces("BRASS   ") == "BRASS");
            CHECK(trimTrailingSpaces(" LEAD 1 ") == " LEAD 1");
            CHECK(trimTrailingSpaces("A  B") == "A  B");
        }

        THEN("an all-space or empty name becomes empty")
        {
            CHECK(trimTrailingSpaces("        ").empty());
            CHECK(trimTrailingSpaces("").empty());
        }

        THEN("a view into a longer string is trimmed on its own length")
        {
            const std::string longer = "ABC   tail";
            CHECK(trimTrailingSpaces(std::string_view(longer).substr(0, 6)) == "ABC");
        }
    }
}

SCENARIO("A file name is sanitized and made unique in its directory", "[RQ-CTL-003][RQ-GUI-077][RQ-QLT-018]")
{
    GIVEN("an empty directory")
    {
        const TempDirectory directory;
        const auto directoryName = directory.path.string();

        THEN("invalid characters are dropped and the extension is appended")
        {
            CHECK(makeUniqueFilenameFromString("A:B*C", ".syx", directoryName) == "ABC.syx");
        }

        WHEN("the name is already taken")
        {
            directory.touch("ABC.syx");

            THEN("a counter is appended, counting up while the name stays taken")
            {
                CHECK(makeUniqueFilenameFromString("ABC", ".syx", directoryName) == "ABC1.syx");
                directory.touch("ABC1.syx");
                CHECK(makeUniqueFilenameFromString("ABC", ".syx", directoryName) == "ABC2.syx");
            }
        }
    }
}
