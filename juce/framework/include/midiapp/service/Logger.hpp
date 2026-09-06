#pragma once

#include <memory>
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

    /// The three independently-switchable log domains: MIDI I/O, calls the
    /// controller makes (and calls the UI makes on the controller), and UI
    /// events. Each has its own on/off gate, but they all share the single
    /// severity threshold above -- there is no per-domain level. [RQ-FMW-073]
    enum class LogDomain
    {
        Midi,
        ControllerCalls,
        UiEvents,
    };

    /// A destination for already-formatted log lines. `Logger`'s own default
    /// sink (used by `configure(std::string)`) implements this with a plain
    /// `std::ofstream`, keeping this library JUCE-free; the application layer
    /// injects a `juce::FileLogger`-backed implementation in production.
    /// [ADR-FMW-001 (DEC-FMW-001)]
    class ILogSink
    {
    public:
        virtual ~ILogSink() = default;
        virtual void write(const std::string& line) = 0;
    };

    /// Minimal diagnostic logger: level- and domain-filtered lines to a
    /// pluggable sink, timestamped. Thread safe -- called from the message
    /// thread, the transmit worker and MIDI callback threads alike.
    /// [RQ-FMW-070, RQ-FMW-073, RQ-NFR-008]
    class Logger
    {
    public:
        /// Opens the framework's own std::ofstream-backed sink at
        /// `logFilePath` (created/appended) and returns whether it opened
        /// successfully. Self-contained -- no JUCE dependency -- so tests and
        /// any JUCE-free caller can use it directly. [ADR-FMW-001 (DEC-FMW-001)]
        static bool configure(const std::string& logFilePath);

        /// Installs an externally-supplied sink (e.g. the application
        /// layer's `juce::FileLogger` adapter), taking ownership of it.
        /// [ADR-FMW-001 (DEC-FMW-001)]
        static void configure(std::unique_ptr<ILogSink> sink);

        static void setLevel(TraceLevel level);
        [[nodiscard]] static TraceLevel level();

        /// Enables or disables one log domain independently of the other
        /// two; all three default to enabled. [RQ-FMW-073, RQ-SET-008]
        static void setDomainEnabled(LogDomain domain, bool enabled);
        [[nodiscard]] static bool isDomainEnabled(LogDomain domain);

        /// Closes the current sink, releasing its handle (e.g. before
        /// deleting the file it points to, or before `configure()` opens a
        /// new one).
        static void shutdown();

        /// Writes one line under `domain` at `level`, prefixed with a UTC
        /// timestamp, the level, the domain and the call site -- but only
        /// when `domain` is enabled AND `level` passes the current
        /// threshold. Reached only through `XPL_LOG()` below, so `file`/
        /// `line` are always the true call site rather than a value a
        /// caller could mistype or leave stale. [RQ-FMW-073]
        static void writeLine(LogDomain domain, TraceLevel level, const char* file, int line,
                               const std::string& message);
    };
}

/// Logs one line under `domain` at `level`, capturing the call site
/// (`__FILE__`/`__LINE__`) automatically. The sole sanctioned way to reach
/// `Logger::writeLine`'s domain-aware overload. [RQ-FMW-073, RQ-FMW-074,
/// RQ-FMW-075, RQ-FMW-076, ADR-FMW-001 (DEC-FMW-002)]
#define XPL_LOG(domain, level, message) \
    ::midiapp::service::Logger::writeLine((domain), (level), __FILE__, __LINE__, (message))
