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
        std::mutex g_mutex;
        std::ofstream g_sink;
        std::atomic<TraceLevel> g_level{TraceLevel::Off};

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
