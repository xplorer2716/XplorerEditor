#include "midiapp/service/FileUtils.hpp"

#include <filesystem>
#include <string_view>

namespace midiapp::service
{
    namespace
    {
        // Windows-invalid filename characters (superset of POSIX), plus the
        // reference's extra ":.)&" set — identical output on both platforms.
        constexpr std::string_view INVALID_CHARS = "\"<>|\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C"
                                                   "\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"
                                                   "\x1A\x1B\x1C\x1D\x1E\x1F:*?\\/.)&";
    }

    std::string makeUniqueFilenameFromString(const std::string& name, const std::string& extension,
                                             const std::string& directoryName)
    {
        std::string sanitized;
        sanitized.reserve(name.size());
        for (const char c : name)
        {
            if (INVALID_CHARS.find(c) == std::string_view::npos && c != '\0')
            {
                sanitized += c;
            }
        }

        const std::filesystem::path directory(directoryName);
        std::string candidate = sanitized;
        int number = 1;
        while (std::filesystem::exists(directory / (candidate + extension)))
        {
            candidate = sanitized + std::to_string(number);
            ++number;
        }
        return candidate + extension;
    }
}
