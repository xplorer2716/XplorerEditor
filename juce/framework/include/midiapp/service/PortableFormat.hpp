#pragma once

// formatStr() is std::format wherever the standard library ships <format>, and a
// portable fallback where it does not: GCC's libstdc++ implements <format>
// only from GCC 13, and the Linux CI runner is pinned to ubuntu-22.04 (GCC 11)
// for AppImage glibc/libstdc++ portability rather than upgraded, because a
// PPA-installed compiler would risk depending on a libstdc++ ABI newer than
// the pinned image ships, which is exactly the failure the pin exists to
// avoid. [RQ-BLD-025, ADR-BLD-004 (DEC-BLD-021)]
//
// formatFallbackInto()/formatFallback() are compiled unconditionally (not
// behind the #if below) so their substitution logic stays covered by unit
// tests on every toolchain, including ones where formatStr() never selects them.
// They support only the bare "{}" placeholder, which is the sole form this
// codebase's call sites use — not the full std::format grammar.

#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <version>

#if defined(__cpp_lib_format)
#include <format>
#endif

namespace midiapp::service
{
    namespace detail
    {
        inline void formatFallbackInto(std::ostringstream& out, std::string_view rest)
        {
            out << rest;
        }

        template <typename T, typename... Rest>
        void formatFallbackInto(std::ostringstream& out, std::string_view rest, T&& value, Rest&&... more)
        {
            const auto pos = rest.find("{}");
            out << rest.substr(0, pos == std::string_view::npos ? rest.size() : pos);
            if (pos == std::string_view::npos)
            {
                return;
            }
            out << std::forward<T>(value);
            formatFallbackInto(out, rest.substr(pos + 2), std::forward<Rest>(more)...);
        }
    }

    template <typename... Args>
    [[nodiscard]] std::string formatFallback(std::string_view fmt, Args&&... args)
    {
        std::ostringstream out;
        detail::formatFallbackInto(out, fmt, std::forward<Args>(args)...);
        return out.str();
    }

#if defined(__cpp_lib_format)
    template <typename... Args>
    [[nodiscard]] std::string formatStr(std::string_view fmt, Args&&... args)
    {
        return std::vformat(fmt, std::make_format_args(args...));
    }
#else
    template <typename... Args>
    [[nodiscard]] std::string formatStr(std::string_view fmt, Args&&... args)
    {
        return formatFallback(fmt, std::forward<Args>(args)...);
    }
#endif
}
