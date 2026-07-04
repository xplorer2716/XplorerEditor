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

#include "xpl/midi/MidiMessage.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace xpl::midi
{
    /// Callbacks a client registers on an input port. Unset members are
    /// simply not invoked. Delivery is serialized per port. [RQ-MID-020..024]
    struct MidiInputCallbacks
    {
        std::function<void(const MidiMessage&)> onChannelMessage;
        std::function<void(const MidiMessage&)> onSysExMessage;
        std::function<void(const MidiMessage&)> onSysCommonMessage;
        std::function<void(const MidiMessage&)> onSysRealtimeMessage;
        std::function<void(const std::string& description)> onError; ///< [RQ-MID-023]
    };

    /// An opened MIDI input device. Destroying the port closes the device.
    class MidiInputPort
    {
    public:
        virtual ~MidiInputPort() = default;

        [[nodiscard]] virtual std::string deviceName() const = 0;

        /// Replaces the registered callbacks. Must be called while stopped.
        virtual void setCallbacks(MidiInputCallbacks callbacks) = 0;

        /// Start/stop message delivery without closing the device. [RQ-MID-005]
        virtual void start() = 0;
        virtual void stop() = 0;
        [[nodiscard]] virtual bool isStarted() const = 0;
    };

    /// An opened MIDI output device. Destroying the port closes the device.
    class MidiOutputPort
    {
    public:
        virtual ~MidiOutputPort() = default;

        [[nodiscard]] virtual std::string deviceName() const = 0;

        /// Sends one message (channel, SysEx of arbitrary length, or system
        /// common/realtime). [RQ-MID-010..012]
        virtual void send(const MidiMessage& message) = 0;
    };

    /// Device enumeration and factory. The single seam between the framework
    /// and any concrete MIDI implementation (JUCE or mock). [RQ-MID-040]
    class MidiBackend
    {
    public:
        virtual ~MidiBackend() = default;

        [[nodiscard]] virtual std::vector<std::string> inputDeviceNames() const = 0;  ///< [RQ-MID-001]
        [[nodiscard]] virtual std::vector<std::string> outputDeviceNames() const = 0; ///< [RQ-MID-001]

        /// Opens a device by display name; returns nullptr when the name does
        /// not match any device (no exception). [RQ-MID-002]
        [[nodiscard]] virtual std::unique_ptr<MidiInputPort> openInput(const std::string& deviceName) = 0;
        [[nodiscard]] virtual std::unique_ptr<MidiOutputPort> openOutput(const std::string& deviceName) = 0;
    };
}
