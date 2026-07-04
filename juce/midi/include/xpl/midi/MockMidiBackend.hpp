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

#include "xpl/midi/MidiPorts.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace xpl::midi
{
    /// In-memory MIDI backend for automated tests: scriptable input injection,
    /// captured output, no hardware. Callback delivery is synchronous on the
    /// injecting thread. [RQ-MID-041]
    class MockMidiBackend final : public MidiBackend
    {
    public:
        MockMidiBackend();
        ~MockMidiBackend() override;

        /// Devices visible to enumeration; open* only succeeds for these names.
        void addInputDevice(const std::string& deviceName);
        void addOutputDevice(const std::string& deviceName);

        [[nodiscard]] std::vector<std::string> inputDeviceNames() const override;
        [[nodiscard]] std::vector<std::string> outputDeviceNames() const override;
        [[nodiscard]] std::unique_ptr<MidiInputPort> openInput(const std::string& deviceName) override;
        [[nodiscard]] std::unique_ptr<MidiOutputPort> openOutput(const std::string& deviceName) override;

        /// Injects a message as if the named device received it. Delivered to
        /// every currently-open, started port of that device. No-op when the
        /// port is stopped ([RQ-MID-005]) or closed.
        void injectIncoming(const std::string& inputDeviceName, const MidiMessage& message);

        /// Injects an error notification on the named input device. [RQ-MID-023]
        void injectError(const std::string& inputDeviceName, const std::string& description);

        /// All messages sent so far through open output ports of that device,
        /// in send order. [RQ-TST-004]
        [[nodiscard]] std::vector<MidiMessage> sentMessages(const std::string& outputDeviceName) const;

        /// Clears the captured output of every device.
        void clearSentMessages();

        /// Wires an output device to an input device: every message sent to
        /// `outputDeviceName` is immediately injected into `inputDeviceName`
        /// (loopback cable). [RQ-MID-041]
        void connectLoopback(const std::string& outputDeviceName, const std::string& inputDeviceName);

        /// Number of currently open ports for a device name (0 after the
        /// unique_ptr is destroyed) — lets tests assert hot-swap release. [RQ-MID-003]
        [[nodiscard]] int openInputPortCount(const std::string& deviceName) const;
        [[nodiscard]] int openOutputPortCount(const std::string& deviceName) const;

    private:
        class MockInputPort;
        class MockOutputPort;
        struct State;

        std::shared_ptr<State> _state;
    };
}
