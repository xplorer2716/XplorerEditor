#include "midiapp/service/Logger.hpp"

#include <atomic>
#include <chrono>
#include <format>
#include <fstream>
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
        const auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
        g_sink << std::format("{:%F %T}", now) << " [" << levelName(level) << "] "
               << source << ": " << message << '\n';
        g_sink.flush();
    }
}
