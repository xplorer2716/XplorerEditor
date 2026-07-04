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
#include "xpl/midi/SysexStreamIterator.hpp"

#include "xpl/midi/MidiMessage.hpp"

namespace xpl::midi
{
    SysexStreamIterator::SysexStreamIterator(std::span<const std::uint8_t> data)
        : _data(data)
    {
    }

    std::optional<std::vector<std::uint8_t>> SysexStreamIterator::next()
    {
        // Reference behavior (MidiApp SysexIterator.MoveNext): seek the next
        // 0xF0, then seek the closing 0xF7; yield [start..end] inclusive.
        std::size_t start = _position;
        while (start < _data.size() && _data[start] != SYSEX_START)
        {
            ++start;
        }
        std::size_t end = start;
        while (end < _data.size() && _data[end] != SYSEX_END)
        {
            ++end;
        }
        if (end >= _data.size() || end <= start)
        {
            return std::nullopt;
        }
        _position = end + 1;
        return std::vector<std::uint8_t>(_data.begin() + static_cast<std::ptrdiff_t>(start),
                                         _data.begin() + static_cast<std::ptrdiff_t>(end) + 1);
    }

    std::vector<std::vector<std::uint8_t>> SysexStreamIterator::allMessages(std::span<const std::uint8_t> data)
    {
        SysexStreamIterator iterator(data);
        std::vector<std::vector<std::uint8_t>> messages;
        while (auto message = iterator.next())
        {
            messages.push_back(std::move(*message));
        }
        return messages;
    }
}
