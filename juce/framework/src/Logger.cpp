// Level-filtered diagnostic log with a file sink. [RQ-FMW-070, RQ-NFR-008]
//
// TODO: THIS LOGGER IS NEVER ENABLED IN THE APPLICATION. Neither configure()
// nor setLevel() is called anywhere outside juce/tests/ -- so g_sink is never
// opened, g_level stays TraceLevel::Off, and writeLine() returns at its first
// two guards for every call site in the product. No log file is written and no
// message at any severity is ever emitted, including the Error-level ones in
// XpanderControllerMidiEvents. A user's field problem therefore cannot be
// diagnosed from a log, which is what RQ-FMW-070 and RQ-NFR-008 require.
// Wiring it needs two decisions: WHERE the file goes (the settings directory
// resolution in XmlSettingsService already handles the non-writable
// per-machine case) and HOW the level is configured -- the reference used a
// .NET .config TraceSwitch, which has no C++ equivalent, so the level needs a
// new home (settings file, environment variable or command line).
// Tracked as GitHub issue #68.
#include "midiapp/service/Logger.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>

namespace midiapp::service
{
    namespace
    {
        // Process-wide singleton state, deliberately file-local rather than a
        // class: logging is called from the message thread, the transmit
        // worker and the MIDI callback threads alike, so there is exactly one
        // sink and one mutex serialising writes to it.
        //
        // g_level is atomic and read OUTSIDE the mutex, so the common case --
        // a call below the current level -- costs one atomic load and no lock
        // contention. Only calls that will actually be written take the mutex.
        std::mutex g_mutex;
        std::ofstream g_sink;
        std::atomic<TraceLevel> g_level{TraceLevel::Off}; // silent until configured — see the TODO above

        const char* levelName(TraceLevel level)
        {
            switch (level)
            {
                case TraceLevel::Error: return "ERROR";
                case TraceLevel::Warning: return "WARNING";
                case TraceLevel::Info: return "INFO";
                case TraceLevel::Verbose: return "VERBOSE";
                case TraceLevel::Off: break;
            }
            return "OFF";
        }
    }

    bool Logger::configure(const std::string& logFilePath)
    {
        const std::lock_guard lock(g_mutex);
        g_sink.close();
        g_sink.clear();
        g_sink.open(logFilePath, std::ios::app);
        return g_sink.good();
    }

    void Logger::setLevel(TraceLevel level)
    {
        g_level = level;
    }

    TraceLevel Logger::level()
    {
        return g_level;
    }

    // Releases the file handle. Not merely tidy: on Windows an open handle
    // prevents the file from being deleted, which is why the tests call this
    // between scenarios that recreate their log file.
    void Logger::shutdown()
    {
        const std::lock_guard lock(g_mutex);
        g_sink.close();
    }

    void Logger::writeLine(const std::string& source, TraceLevel level, const std::string& message)
    {
        if (level == TraceLevel::Off || static_cast<int>(level) > static_cast<int>(g_level.load()))
        {
            return;
        }
        const std::lock_guard lock(g_mutex);
        if (!g_sink.is_open())
        {
            return;
        }
        // UTC timestamp with millisecond precision, hand-rolled rather than
        // via std::format/chrono-io: both need a <format> the pinned Linux CI
        // toolchain does not ship (GCC 11; see PortableFormat.hpp).
        // [RQ-BLD-025, ADR-BLD-004 (DEC-BLD-021)]
        const auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
        const auto sinceEpoch = now.time_since_epoch();
        const auto seconds = std::chrono::floor<std::chrono::seconds>(sinceEpoch);
        const auto milliseconds = sinceEpoch - seconds;
        const auto secondsCount = static_cast<std::time_t>(seconds.count());
        std::tm utc{};
#if defined(_WIN32)
        gmtime_s(&utc, &secondsCount);
#else
        gmtime_r(&secondsCount, &utc);
#endif
        g_sink << std::put_time(&utc, "%F %T") << '.'
               << std::setfill('0') << std::setw(3) << milliseconds.count()
               << " [" << levelName(level) << "] " << source << ": " << message << '\n';
        g_sink.flush();
    }
}
