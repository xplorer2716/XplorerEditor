#include "xplorer/app/LoggingConfigResolver.hpp"

#include "xpl/util/EnumUtils.hpp"

#include <algorithm>
#include <filesystem>

namespace xplorer::app
{
    using midiapp::service::TraceLevel;

    std::string resolveLogFilePath(const std::string& logDirectoryOverride,
                                    const std::string& settingsFilePath)
    {
        const std::filesystem::path directory = logDirectoryOverride.empty()
            ? std::filesystem::path(settingsFilePath).parent_path()
            : std::filesystem::path(logDirectoryOverride);
        return (directory / LOG_FILE_NAME).string();
    }

    TraceLevel resolveSeverityLevel(int rawSeverityLevel)
    {
        constexpr auto MIN_LEVEL = xpl::util::toUnderlying(TraceLevel::Off);
        constexpr auto MAX_LEVEL = xpl::util::toUnderlying(TraceLevel::Verbose);
        const auto clamped = std::clamp(rawSeverityLevel, MIN_LEVEL, MAX_LEVEL);
        return static_cast<TraceLevel>(clamped);
    }
}
