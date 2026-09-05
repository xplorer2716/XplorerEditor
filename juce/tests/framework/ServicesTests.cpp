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

SCENARIO("The logger filters by level and writes timestamped, domain-tagged lines", "[RQ-FMW-070]")
{
    GIVEN("a configured logger at Info level with every domain enabled")
    {
        const auto logPath = std::filesystem::temp_directory_path() / "xpl_logger_test.log";
        Logger::shutdown(); // release any handle left open by a previous section (Windows locks open files)
        std::filesystem::remove(logPath);
        REQUIRE(Logger::configure(logPath.string()));
        Logger::setLevel(TraceLevel::Info);
        Logger::setDomainEnabled(LogDomain::Midi, true);
        Logger::setDomainEnabled(LogDomain::ControllerCalls, true);
        Logger::setDomainEnabled(LogDomain::UiEvents, true);

        WHEN("writing at different levels")
        {
            XPL_LOG(LogDomain::Midi, TraceLevel::Error, "error-line");
            XPL_LOG(LogDomain::Midi, TraceLevel::Verbose, "verbose-line");

            THEN("only levels at or below the threshold are written")
            {
                const auto content = readAll(logPath);
                CHECK(content.find("error-line") != std::string::npos);
                CHECK(content.find("verbose-line") == std::string::npos);
            }

            THEN("the written line carries the level, the domain and the call site")
            {
                const auto content = readAll(logPath);
                CHECK(content.find("[ERROR]") != std::string::npos);
                CHECK(content.find("[MIDI]") != std::string::npos);
                CHECK(content.find("ServicesTests.cpp") != std::string::npos);
            }
        }

        WHEN("the level is Off")
        {
            Logger::setLevel(TraceLevel::Off);
            XPL_LOG(LogDomain::Midi, TraceLevel::Error, "muted-line");

            THEN("nothing is written")
            {
                CHECK(readAll(logPath).find("muted-line") == std::string::npos);
            }
        }
    }
}

SCENARIO("Each log domain can be enabled or disabled independently of the others", "[RQ-FMW-073]")
{
    GIVEN("a configured logger at Info level")
    {
        const auto logPath = std::filesystem::temp_directory_path() / "xpl_logger_domain_test.log";
        Logger::shutdown();
        std::filesystem::remove(logPath);
        REQUIRE(Logger::configure(logPath.string()));
        Logger::setLevel(TraceLevel::Info);

        WHEN("the MIDI domain is disabled and the other two are enabled")
        {
            Logger::setDomainEnabled(LogDomain::Midi, false);
            Logger::setDomainEnabled(LogDomain::ControllerCalls, true);
            Logger::setDomainEnabled(LogDomain::UiEvents, true);

            XPL_LOG(LogDomain::Midi, TraceLevel::Error, "midi-line");
            XPL_LOG(LogDomain::ControllerCalls, TraceLevel::Error, "controller-line");
            XPL_LOG(LogDomain::UiEvents, TraceLevel::Error, "ui-line");

            THEN("only the disabled domain's line is withheld, regardless of severity")
            {
                const auto content = readAll(logPath);
                CHECK(content.find("midi-line") == std::string::npos);
                CHECK(content.find("controller-line") != std::string::npos);
                CHECK(content.find("ui-line") != std::string::npos);
            }

            AND_WHEN("the MIDI domain is re-enabled")
            {
                Logger::setDomainEnabled(LogDomain::Midi, true);
                XPL_LOG(LogDomain::Midi, TraceLevel::Error, "midi-line-2");

                THEN("its lines are written again")
                {
                    CHECK(readAll(logPath).find("midi-line-2") != std::string::npos);
                }
            }
        }

        // Restore the default every test relies on (all domains enabled),
        // so this scenario cannot leak state into one that runs after it.
        Logger::setDomainEnabled(LogDomain::Midi, true);
        Logger::setDomainEnabled(LogDomain::ControllerCalls, true);
        Logger::setDomainEnabled(LogDomain::UiEvents, true);
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
