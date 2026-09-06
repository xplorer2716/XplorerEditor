// Level- and domain-filtered diagnostic log with a pluggable sink. Wired at
// application startup by juce/app (MainComponent), which reads the persisted
// configuration (RQ-SET-008) and supplies a juce::FileLogger-backed sink --
// this library itself stays JUCE-free (ADR-FMW-001 DEC-FMW-001). Formerly
// built but never wired to anything outside juce/tests/; see GitHub issue
// #68 and ADR-FMW-001 for the history. [RQ-FMW-070, RQ-FMW-073, RQ-NFR-008]
#include "midiapp/service/Logger.hpp"

#include "xpl/util/EnumUtils.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>

namespace midiapp::service
{
    namespace
    {
        // Process-wide singleton state, deliberately file-local rather than a
        // class: logging is called from the message thread, the transmit
        // worker and the MIDI callback threads alike, so there is exactly one
        // sink and one mutex serialising writes to it.
        //
        // g_level and g_domainEnabled are atomic and read OUTSIDE the mutex,
        // so the common case -- a call below the current level or under a
        // disabled domain -- costs one atomic load and no lock contention.
        // Only calls that will actually be written take the mutex.
        std::mutex g_mutex;
        std::unique_ptr<ILogSink> g_sink;
        std::atomic<TraceLevel> g_level{TraceLevel::Off}; // silent until configured

        // One flag per LogDomain, indexed by its underlying value; all three
        // default enabled so raising the global threshold alone -- the
        // common case -- logs every domain, matching the pre-domain
        // behaviour this feature extends. [RQ-FMW-073, RQ-SET-008]
        constexpr std::size_t DOMAIN_COUNT = 3;
        std::array<std::atomic<bool>, DOMAIN_COUNT> g_domainEnabled{true, true, true};

        std::size_t domainIndex(LogDomain domain)
        {
            return static_cast<std::size_t>(xpl::util::toUnderlying(domain));
        }

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

        const char* domainName(LogDomain domain)
        {
            switch (domain)
            {
                case LogDomain::Midi: return "MIDI";
                case LogDomain::ControllerCalls: return "CTL";
                case LogDomain::UiEvents: return "UI";
            }
            return "?";
        }

        // __FILE__ is typically the full path the compiler was invoked with;
        // only the basename is useful in a log line meant for a human. Pure
        // pointer arithmetic on a string-literal lifetime -- no allocation.
        const char* fileBaseName(const char* path)
        {
            const char* base = path;
            for (const char* cursor = path; *cursor != '\0'; ++cursor)
            {
                if (*cursor == '/' || *cursor == '\\')
                {
                    base = cursor + 1;
                }
            }
            return base;
        }

        // UTC timestamp with millisecond precision, hand-rolled rather than
        // via std::format/chrono-io: both need a <format> the pinned Linux CI
        // toolchain does not ship (GCC 11; see PortableFormat.hpp).
        // [RQ-BLD-025, ADR-BLD-004 (DEC-BLD-021)]
        std::string formatTimestamp()
        {
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
            std::ostringstream stream;
            stream << std::put_time(&utc, "%F %T") << '.'
                   << std::setfill('0') << std::setw(3) << milliseconds.count();
            return stream.str();
        }

        // Default sink: appends to a std::ofstream. Backs the path-based
        // configure() overload -- the framework's own self-contained
        // default, used directly by tests and by any JUCE-free caller.
        // [ADR-FMW-001 (DEC-FMW-001)]
        class FileStreamSink final : public ILogSink
        {
        public:
            explicit FileStreamSink(const std::string& path) : _stream(path, std::ios::app) {}

            [[nodiscard]] bool good() const { return _stream.good(); }

            void write(const std::string& line) override
            {
                _stream << line << '\n';
                _stream.flush();
            }

        private:
            std::ofstream _stream;
        };
    }

    bool Logger::configure(const std::string& logFilePath)
    {
        auto sink = std::make_unique<FileStreamSink>(logFilePath);
        const bool opened = sink->good();
        const std::lock_guard lock(g_mutex);
        g_sink = std::move(sink);
        return opened;
    }

    void Logger::configure(std::unique_ptr<ILogSink> sink)
    {
        const std::lock_guard lock(g_mutex);
        g_sink = std::move(sink);
    }

    void Logger::setLevel(TraceLevel level)
    {
        g_level = level;
    }

    TraceLevel Logger::level()
    {
        return g_level;
    }

    void Logger::setDomainEnabled(LogDomain domain, bool enabled)
    {
        g_domainEnabled[domainIndex(domain)] = enabled;
    }

    bool Logger::isDomainEnabled(LogDomain domain)
    {
        return g_domainEnabled[domainIndex(domain)];
    }

    // Releases the sink's handle. Not merely tidy: on Windows an open handle
    // prevents its file from being deleted, which is why the tests call this
    // between scenarios that recreate their log file.
    void Logger::shutdown()
    {
        const std::lock_guard lock(g_mutex);
        g_sink.reset();
    }

    void Logger::writeLine(LogDomain domain, TraceLevel level, const char* file, int line,
                            const std::string& message)
    {
        if (!isDomainEnabled(domain) || level == TraceLevel::Off
            || xpl::util::toUnderlying(level) > xpl::util::toUnderlying(g_level.load()))
        {
            return;
        }
        const std::lock_guard lock(g_mutex);
        if (g_sink == nullptr)
        {
            return;
        }
        std::ostringstream formatted;
        formatted << formatTimestamp() << " [" << levelName(level) << "] [" << domainName(domain) << "] "
                  << fileBaseName(file) << ":" << line << ": " << message;
        g_sink->write(formatted.str());
    }
}
