#include <catch2/catch_test_macros.hpp>

#include "midiapp/service/Logger.hpp"
#include "midiapp/service/SingleInstanceGuard.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace midiapp::service;

namespace
{
    std::string readAll(const std::filesystem::path& path)
    {
        std::ifstream stream(path);
        std::ostringstream content;
        content << stream.rdbuf();
        return content.str();
    }
}

SCENARIO("The logger filters by level and writes timestamped lines", "[RQ-FMW-070]")
{
    GIVEN("a configured logger at Info level")
    {
        const auto logPath = std::filesystem::temp_directory_path() / "xpl_logger_test.log";
        std::filesystem::remove(logPath);
        REQUIRE(Logger::configure(logPath.string()));
        Logger::setLevel(TraceLevel::Info);

        WHEN("writing at different levels")
        {
            Logger::writeLine("test", TraceLevel::Error, "error-line");
            Logger::writeLine("test", TraceLevel::Verbose, "verbose-line");

            THEN("only levels at or below the threshold are written")
            {
                const auto content = readAll(logPath);
                CHECK(content.find("error-line") != std::string::npos);
                CHECK(content.find("verbose-line") == std::string::npos);
            }
        }

        WHEN("the level is Off")
        {
            Logger::setLevel(TraceLevel::Off);
            Logger::writeLine("test", TraceLevel::Error, "muted-line");

            THEN("nothing is written")
            {
                CHECK(readAll(logPath).find("muted-line") == std::string::npos);
            }
        }
    }
}

SCENARIO("Only one instance can hold the single-instance lock", "[RQ-FMW-072]")
{
    GIVEN("a lock file path")
    {
        const auto lockPath = std::filesystem::temp_directory_path() / "xpl_instance_test.lock";

        WHEN("a first guard acquires it")
        {
            SingleInstanceGuard first(lockPath.string());
            REQUIRE(first.acquired());

            THEN("a second guard fails while the first lives")
            {
                SingleInstanceGuard second(lockPath.string());
                CHECK_FALSE(second.acquired());
            }
        }

        WHEN("the first guard is gone")
        {
            {
                SingleInstanceGuard first(lockPath.string());
                REQUIRE(first.acquired());
            }

            THEN("a new guard can acquire")
            {
                SingleInstanceGuard next(lockPath.string());
                CHECK(next.acquired());
            }
        }
    }
}
