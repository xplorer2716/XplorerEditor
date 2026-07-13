#include "xplorer/app/MidiAutomationTable.hpp"

#include <array>
#include <charconv>
#include <string_view>

namespace xplorer::app
{
    namespace
    {
        const std::array<std::string, 129>& ccNames()
        {
            static const std::array<std::string, 129> names = {
#include "GeneratedControlChangeNames.inc"
            };
            return names;
        }
    }

    int controlChangeNameCount() { return static_cast<int>(ccNames().size()); }

    int unassignedControlChange() { return controlChangeNameCount() - 1; }

    const std::string& controlChangeName(int ccNumber)
    {
        const auto& names = ccNames();
        if (ccNumber < 0 || ccNumber >= static_cast<int>(names.size()))
        {
            return names.back(); // "None"
        }
        return names[static_cast<std::size_t>(ccNumber)];
    }

    std::optional<std::pair<std::string, int>> parseAutomationEntry(const std::string& entry)
    {
        // Split on the last ';' (parameter names never contain one).
        const auto delimiter = entry.find_last_of(';');
        if (delimiter == std::string::npos || delimiter == 0 || delimiter + 1 >= entry.size())
        {
            return std::nullopt;
        }
        const std::string name = entry.substr(0, delimiter);
        const std::string_view number(entry.data() + delimiter + 1, entry.size() - delimiter - 1);
        int cc = 0;
        const auto result = std::from_chars(number.data(), number.data() + number.size(), cc);
        if (result.ec != std::errc{} || result.ptr != number.data() + number.size())
        {
            return std::nullopt;
        }
        // Reference clamps stored values to 1..128.
        cc = cc < 1 ? 1 : (cc > 128 ? 128 : cc);
        return std::make_pair(name, cc);
    }
}
