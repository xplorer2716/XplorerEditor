#include <catch2/catch_test_macros.hpp>

#include "LoggingSink.hpp"

#include <juce_core/juce_core.h>

// JuceFileLoggerSink: the application-layer ILogSink implementation wrapping
// juce::FileLogger (ADR-FMW-001 DEC-FMW-001). Needs real JUCE file I/O, hence
// this JUCE-linked target rather than the headless one.
// [RQ-FMW-070, RQ-NFR-008]

using namespace xplorer::app;

SCENARIO("JuceFileLoggerSink writes lines to a real file via juce::FileLogger", "[RQ-FMW-070]")
{
    GIVEN("a sink pointed at a fresh temp file")
    {
        const auto file = juce::File::getSpecialLocation(juce::File::tempDirectory)
                               .getChildFile("xpl_logging_sink_test.log");
        file.deleteFile();
        JuceFileLoggerSink sink(file, "Xplorer test");

        WHEN("a line is written")
        {
            sink.write("hello from the test");

            THEN("the file exists and contains the line")
            {
                CHECK(file.existsAsFile());
                CHECK(file.loadFileAsString().contains("hello from the test"));
            }
        }

        file.deleteFile();
    }
}
