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

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace xpl::midi
{
    /// Channel voice commands (status high nibble), values as on the wire.
    /// Mirrors the Sanford ChannelCommand surface used by the reference. [RQ-MID-010]
    enum class ChannelCommand : std::uint8_t
    {
        NoteOff = 0x80,
        NoteOn = 0x90,
        PolyPressure = 0xA0,
        Controller = 0xB0,
        ProgramChange = 0xC0,
        ChannelPressure = 0xD0,
        PitchWheel = 0xE0,
    };

    /// Broad message families, discriminated from the status byte.
    enum class MessageType
    {
        Channel,
        SysEx,
        SysCommon,
        SysRealtime,
        Invalid,
    };

    inline constexpr std::uint8_t SYSEX_START = 0xF0;    ///< [RQ-MID-011]
    inline constexpr std::uint8_t SYSEX_END = 0xF7;      ///< [RQ-MID-011]
    inline constexpr std::uint8_t TUNE_REQUEST = 0xF6;   ///< [RQ-MID-012]
    inline constexpr int MIDI_CHANNEL_MIN = 0;
    inline constexpr int MIDI_CHANNEL_MAX = 15;
    inline constexpr int MIDI_DATA_MAX = 127;

    /// Immutable MIDI message value type owning its raw bytes.
    /// Backend-agnostic by design: no JUCE type appears here. [RQ-MID-040]
    class MidiMessage
    {
    public:
        /// Builds a channel voice message. ProgramChange and ChannelPressure
        /// encode as two bytes; every other command as three. [RQ-MID-010]
        /// @throws std::out_of_range on invalid channel or data byte.
        static MidiMessage channelMessage(ChannelCommand command, int channel, int data1, int data2 = 0);

        /// Wraps a complete SysEx frame; bytes must start with 0xF0 and end
        /// with 0xF7. Arbitrary length is supported. [RQ-MID-011]
        /// @throws std::invalid_argument on missing framing bytes.
        static MidiMessage sysEx(std::span<const std::uint8_t> frame);

        /// Tune Request (0xF6). [RQ-MID-012]
        static MidiMessage tuneRequest();

        /// Wraps raw bytes without validation beyond non-emptiness; used by
        /// backends when forwarding received data verbatim.
        static MidiMessage fromRawBytes(std::span<const std::uint8_t> bytes);

        [[nodiscard]] MessageType type() const;
        [[nodiscard]] std::span<const std::uint8_t> bytes() const { return _bytes; }
        [[nodiscard]] std::size_t size() const { return _bytes.size(); }
        [[nodiscard]] std::uint8_t operator[](std::size_t index) const { return _bytes[index]; }

        /// Channel-message accessors; only valid when type() == Channel.
        [[nodiscard]] ChannelCommand command() const;
        [[nodiscard]] int channel() const;
        [[nodiscard]] int data1() const;
        [[nodiscard]] int data2() const;

        /// Returns a copy re-stamped on another channel (same command/data),
        /// as the reference does when forwarding automation input. [RQ-FMW-051]
        [[nodiscard]] MidiMessage withChannel(int channel) const;

        [[nodiscard]] std::string toString() const;

        friend bool operator==(const MidiMessage&, const MidiMessage&) = default;

    private:
        explicit MidiMessage(std::vector<std::uint8_t> bytes);

        std::vector<std::uint8_t> _bytes;
    };
}
