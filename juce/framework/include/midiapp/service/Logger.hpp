#pragma once

#include <string>

namespace midiapp::service
{
    /// Trace levels, matching System.Diagnostics.TraceLevel of the reference
    /// (xplorer.config switch: 0 = off .. 4 = verbose). [RQ-FMW-070]
    enum class TraceLevel
    {
        Off = 0,
        Error = 1,
        Warning = 2,
        Info = 3,
        Verbose = 4,
    };

    /// Minimal diagnostic logger: level-filtered lines to a per-user log
    /// file, timestamped. Thread safe. [RQ-FMW-070, RQ-NFR-008]
    class Logger
    {
    public:
        /// Sets the sink file (created/appended) and returns success.
        static bool configure(const std::string& logFilePath);

        static void setLevel(TraceLevel level);
        [[nodiscard]] static TraceLevel level();

        /// `source` mirrors the reference's caller argument.
        static void writeLine(const std::string& source, TraceLevel level, const std::string& message);
    };
}
