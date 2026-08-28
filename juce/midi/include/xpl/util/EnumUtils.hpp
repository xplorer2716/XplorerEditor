/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2026 Pascal Schmitt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

// Local equivalent of C++23's std::to_underlying. The project targets C++20
// (RQ-BLD-003; a standard bump was rejected in ADR-BLD-004 for CI/Linux
// toolchain reasons), so this is hand-rolled instead. Lives in xpl_midi, the
// one library every other layer depends on transitively, so it has a single
// definition reachable everywhere — not because it is MIDI-specific.
// [cpp:S7035]

#include <type_traits>

namespace xpl::util
{
    template <typename Enum>
    [[nodiscard]] constexpr std::underlying_type_t<Enum> toUnderlying(Enum value) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(value);
    }
}
