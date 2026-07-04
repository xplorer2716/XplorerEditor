/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2026 Pascal Schmitt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace xpl::midi
{
    /// Splits a byte stream (typically a .syx file content) into successive
    /// complete SysEx messages [0xF0 .. 0xF7], skipping any bytes outside
    /// frames. Port of the reference MidiApp SysexIterator semantics.
    /// [RQ-MID-030]
    class SysexStreamIterator
    {
    public:
        explicit SysexStreamIterator(std::span<const std::uint8_t> data);

        /// Returns the next complete SysEx frame, or nullopt when exhausted.
        /// An unterminated trailing frame (0xF0 without 0xF7) is not returned,
        /// as in the reference.
        std::optional<std::vector<std::uint8_t>> next();

        /// Convenience: all frames of the stream in order.
        static std::vector<std::vector<std::uint8_t>> allMessages(std::span<const std::uint8_t> data);

    private:
        std::span<const std::uint8_t> _data;
        std::size_t _position = 0;
    };
}
