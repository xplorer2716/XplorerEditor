#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/LoggingConfigResolver.hpp"

#include <filesystem>

// Path/severity resolution and applying the result to the running Logger,
// behind wiring the diagnostic logger from persisted configuration
// (RQ-SET-008). JUCE-free and OS-free, so it stays headless-testable
// (RQ-BLD-025) without constructing a window.
// [RQ-FMW-070, ADR-FMW-001 (DEC-FMW-003)]

using namespace xplorer::app;
using midiapp::service::Logger;
using midiapp::service::LogDomain;
using midiapp::service::TraceLevel;

SCENARIO("The log file path defaults next to the settings file", "[RQ-SET-008]")
{
    GIVEN("no directory override")
    {
        WHEN("the log path is resolved")
        {
            const auto path = resolveLogFilePath("", "/home/user/.config/Xplorer/Xplorer/xplorer.users.config");

            THEN("it sits in the settings file's own directory, under xplorer.log")
            {
                // Built via std::filesystem::path, not a forward-slash string
                // literal: resolveLogFilePath joins with the platform's own
                // separator (backslash on Windows), which a hard-coded '/'
                // expectation does not match there. [CI: Windows job failure,
                // LoggingConfigResolverTests.cpp:23/37, run 33968166473]
                const auto expected = std::filesystem::path("/home/user/.config/Xplorer/Xplorer") / LOG_FILE_NAME;
                CHECK(path == expected.string());
            }
        }
    }

    GIVEN("a non-empty directory override")
    {
        WHEN("the log path is resolved")
        {
            const auto path = resolveLogFilePath("/custom/log/dir",
                                                   "/home/user/.config/Xplorer/Xplorer/xplorer.users.config");

            THEN("the override wins outright, still under xplorer.log")
            {
                const auto expected = std::filesystem::path("/custom/log/dir") / LOG_FILE_NAME;
                CHECK(path == expected.string());
            }
        }
    }
}

SCENARIO("A persisted severity value is clamped to a valid TraceLevel", "[RQ-SET-008]")
{
    GIVEN("values within range")
    {
        THEN("they resolve unchanged")
        {
            CHECK(resolveSeverityLevel(0) == TraceLevel::Off);
            CHECK(resolveSeverityLevel(1) == TraceLevel::Error);
            CHECK(resolveSeverityLevel(4) == TraceLevel::Verbose);
        }
    }

    GIVEN("a value below the valid range (e.g. a hand-edited settings file)")
    {
        THEN("it clamps to Off rather than producing an undefined enum value")
        {
            CHECK(resolveSeverityLevel(-1) == TraceLevel::Off);
        }
    }

    GIVEN("a value above the valid range")
    {
        THEN("it clamps to Verbose")
        {
            CHECK(resolveSeverityLevel(99) == TraceLevel::Verbose);
        }
    }
}

SCENARIO("Applying a logging configuration pushes it into the running Logger",
         "[RQ-FMW-070][RQ-FMW-073][RQ-SET-008][RQ-GUI-046]")
{
    GIVEN("a severity and three domain flags")
    {
        WHEN("severity is Off and only the UI domain is enabled")
        {
            applyLoggingConfiguration(0, false, false, true);

            THEN("the running Logger reflects exactly those values")
            {
                CHECK(Logger::level() == TraceLevel::Off);
                CHECK_FALSE(Logger::isDomainEnabled(LogDomain::Midi));
                CHECK_FALSE(Logger::isDomainEnabled(LogDomain::ControllerCalls));
                CHECK(Logger::isDomainEnabled(LogDomain::UiEvents));
            }
        }

        WHEN("the severity is out of range and all domains are enabled")
        {
            applyLoggingConfiguration(99, true, true, true);

            THEN("it clamps the same way resolveSeverityLevel does")
            {
                CHECK(Logger::level() == TraceLevel::Verbose);
                CHECK(Logger::isDomainEnabled(LogDomain::Midi));
                CHECK(Logger::isDomainEnabled(LogDomain::ControllerCalls));
                CHECK(Logger::isDomainEnabled(LogDomain::UiEvents));
            }
        }

        // Restore the defaults other suites rely on (Off, all domains
        // enabled) so this scenario cannot leak state into one that runs
        // after it -- same care ServicesTests.cpp takes around global
        // Logger state.
        applyLoggingConfiguration(0, true, true, true);
    }
}
