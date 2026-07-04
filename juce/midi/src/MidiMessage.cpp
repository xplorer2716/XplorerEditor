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
#include "xpl/midi/MidiMessage.hpp"

#include <sstream>
#include <stdexcept>

namespace xpl::midi
{
    namespace
    {
        void requireDataByte(int value, const char* what)
        {
            if (value < 0 || value > MIDI_DATA_MAX)
            {
                throw std::out_of_range(std::string(what) + " out of range [0,127]: " + std::to_string(value));
            }
        }

        bool isTwoByteCommand(ChannelCommand command)
        {
            return command == ChannelCommand::ProgramChange || command == ChannelCommand::ChannelPressure;
        }
    }

    MidiMessage::MidiMessage(std::vector<std::uint8_t> bytes)
        : _bytes(std::move(bytes))
    {
    }

    MidiMessage MidiMessage::channelMessage(ChannelCommand command, int channel, int data1, int data2)
    {
        if (channel < MIDI_CHANNEL_MIN || channel > MIDI_CHANNEL_MAX)
        {
            throw std::out_of_range("MIDI channel out of range [0,15]: " + std::to_string(channel));
        }
        requireDataByte(data1, "data1");

        const auto status = static_cast<std::uint8_t>(static_cast<std::uint8_t>(command)
                                                      | static_cast<std::uint8_t>(channel));
        if (isTwoByteCommand(command))
        {
            return MidiMessage({status, static_cast<std::uint8_t>(data1)});
        }
        requireDataByte(data2, "data2");
        return MidiMessage({status, static_cast<std::uint8_t>(data1), static_cast<std::uint8_t>(data2)});
    }

    MidiMessage MidiMessage::sysEx(std::span<const std::uint8_t> frame)
    {
        if (frame.size() < 2 || frame.front() != SYSEX_START || frame.back() != SYSEX_END)
        {
            throw std::invalid_argument("SysEx frame must start with 0xF0 and end with 0xF7");
        }
        return MidiMessage(std::vector<std::uint8_t>(frame.begin(), frame.end()));
    }

    MidiMessage MidiMessage::tuneRequest()
    {
        return MidiMessage({TUNE_REQUEST});
    }

    MidiMessage MidiMessage::fromRawBytes(std::span<const std::uint8_t> bytes)
    {
        if (bytes.empty())
        {
            throw std::invalid_argument("MIDI message cannot be empty");
        }
        return MidiMessage(std::vector<std::uint8_t>(bytes.begin(), bytes.end()));
    }

    MessageType MidiMessage::type() const
    {
        const std::uint8_t status = _bytes.front();
        if (status >= 0x80 && status <= 0xEF)
        {
            return MessageType::Channel;
        }
        if (status == SYSEX_START)
        {
            return MessageType::SysEx;
        }
        if (status >= 0xF1 && status <= 0xF7)
        {
            return MessageType::SysCommon;
        }
        if (status >= 0xF8)
        {
            return MessageType::SysRealtime;
        }
        return MessageType::Invalid;
    }

    ChannelCommand MidiMessage::command() const
    {
        return static_cast<ChannelCommand>(_bytes.front() & 0xF0U);
    }

    int MidiMessage::channel() const
    {
        return _bytes.front() & 0x0FU;
    }

    int MidiMessage::data1() const
    {
        return _bytes.size() > 1 ? _bytes[1] : 0;
    }

    int MidiMessage::data2() const
    {
        return _bytes.size() > 2 ? _bytes[2] : 0;
    }

    MidiMessage MidiMessage::withChannel(int newChannel) const
    {
        if (type() != MessageType::Channel)
        {
            throw std::logic_error("withChannel is only valid for channel messages");
        }
        if (newChannel < MIDI_CHANNEL_MIN || newChannel > MIDI_CHANNEL_MAX)
        {
            throw std::out_of_range("MIDI channel out of range [0,15]: " + std::to_string(newChannel));
        }
        auto copy = _bytes;
        copy.front() = static_cast<std::uint8_t>((copy.front() & 0xF0U) | static_cast<std::uint8_t>(newChannel));
        return MidiMessage(std::move(copy));
    }

    std::string MidiMessage::toString() const
    {
        std::ostringstream stream;
        stream << std::hex << std::uppercase;
        for (const auto byte : _bytes)
        {
            stream << (byte < 0x10 ? "0" : "") << static_cast<int>(byte) << ' ';
        }
        return stream.str();
    }
}
