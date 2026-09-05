#pragma once

// Application-layer diagnostic-log sink: wraps juce::FileLogger behind the
// JUCE-free midiapp::service::ILogSink interface, so juce/framework never
// takes a JUCE dependency. [RQ-FMW-070, RQ-NFR-008, ADR-FMW-001 (DEC-FMW-001)]

#include "midiapp/service/Logger.hpp"

#include <juce_core/juce_core.h>

namespace xplorer::app
{
    class JuceFileLoggerSink final : public midiapp::service::ILogSink
    {
    public:
        /// Opens (creating, with parent directories, if needed) `file` for
        /// appending; `welcomeMessage` is written as a header the first time
        /// JUCE creates it. Uses juce::FileLogger's own default size-based
        /// trim rather than the .NET reference's truncate-per-launch —
        /// deliberate, see ADR-FMW-001.
        JuceFileLoggerSink(const juce::File& file, const juce::String& welcomeMessage);

        void write(const std::string& line) override;

    private:
        juce::FileLogger _fileLogger;
    };
}
