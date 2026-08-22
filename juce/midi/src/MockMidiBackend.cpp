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
// In-memory MidiBackend used by every test that needs MIDI without hardware.
// It is the mock half of the port/adapter pair whose real half is
// JuceMidiBackend. [RQ-MID-002, ADR-JUC-004]
//
// Three behaviours make it usable as a test double, and each is a deliberate
// choice rather than a simplification:
//
//   * Delivery is SYNCHRONOUS, on whichever thread called injectIncoming() or
//     send(). A test can therefore assert immediately after injecting, with no
//     wait and no polling. The real backend delivers on a JUCE-owned callback
//     thread instead, so a test that depends on that asynchrony must use the
//     JUCE backend (see the virtual-cable scenarios in xpl_tests_midi_juce).
//   * Every message sent to an output is CAPTURED, so a test asserts on the
//     exact byte sequence the controller produced.
//   * An output can be LOOPED BACK to an input (connectLoopback), which is how
//     a full request/response exchange with a simulated synth is tested.
//
// Device names are the identity: openInput/openOutput fail for a name that was
// not registered with addInputDevice/addOutputDevice first, mirroring the real
// backend's behaviour when a device is absent.
#include "xpl/midi/MockMidiBackend.hpp"

#include <algorithm>
#include <map>

namespace xpl::midi
{
    // All mutable state lives here, behind a shared_ptr held by the backend AND
    // by every port it opened. That is what makes destruction order irrelevant:
    // a test may drop the backend while a port is still alive, and the port's
    // own destructor still finds the registry it must remove itself from.
    // Owning the state from the backend alone would leave those destructors
    // dereferencing freed memory.
    struct MockMidiBackend::State
    {
        std::mutex mutex;
        std::vector<std::string> inputNames;
        std::vector<std::string> outputNames;
        std::map<std::string, std::vector<MockInputPort*>> openInputs;
        std::map<std::string, std::vector<MockOutputPort*>> openOutputs;
        std::map<std::string, std::vector<MidiMessage>> captured;
        std::map<std::string, std::string> loopbacks; // output name -> input name

        // Both fan a message out to every port currently open on `inputName`.
        // "Every port", plural, on purpose: a test may open the same device
        // twice to check that the controller's hot-swap really closed the
        // previous handle (openInputPortCount is what asserts it).
        void deliver(const std::string& inputName, const MidiMessage& message);
        void deliverError(const std::string& inputName, const std::string& description);
    };

    class MockMidiBackend::MockInputPort final : public MidiInputPort
    {
    public:
        MockInputPort(std::shared_ptr<State> state, std::string name)
            : _state(std::move(state)), _name(std::move(name))
        {
            const std::lock_guard lock(_state->mutex);
            _state->openInputs[_name].push_back(this);
        }

        ~MockInputPort() override
        {
            const std::lock_guard lock(_state->mutex);
            auto& ports = _state->openInputs[_name];
            std::erase(ports, this);
        }

        [[nodiscard]] std::string deviceName() const override { return _name; }
        void setCallbacks(MidiInputCallbacks callbacks) override { _callbacks = std::move(callbacks); }
        void start() override { _started = true; }
        void stop() override { _started = false; }
        [[nodiscard]] bool isStarted() const override { return _started; }

        // Fans one message out to whichever callback matches its type. A port
        // that has been stopped drops the message entirely rather than queueing
        // it, which is what lets a test assert that stop() really silences a
        // device -- the controller relies on that when it stops the ports while
        // the settings dialog is open.
        void receive(const MidiMessage& message)
        {
            if (!_started)
            {
                return;
            }
            switch (message.type())
            {
                case MessageType::Channel:
                    if (_callbacks.onChannelMessage) _callbacks.onChannelMessage(message);
                    break;
                case MessageType::SysEx:
                    if (_callbacks.onSysExMessage) _callbacks.onSysExMessage(message);
                    break;
                case MessageType::SysCommon:
                    if (_callbacks.onSysCommonMessage) _callbacks.onSysCommonMessage(message);
                    break;
                case MessageType::SysRealtime:
                    if (_callbacks.onSysRealtimeMessage) _callbacks.onSysRealtimeMessage(message);
                    break;
                case MessageType::Invalid:
                    break;
            }
        }

        void receiveError(const std::string& description)
        {
            if (_started && _callbacks.onError)
            {
                _callbacks.onError(description);
            }
        }

    private:
        std::shared_ptr<State> _state;
        std::string _name;
        MidiInputCallbacks _callbacks;
        bool _started = false;
    };

    class MockMidiBackend::MockOutputPort final : public MidiOutputPort
    {
    public:
        MockOutputPort(std::shared_ptr<State> state, std::string name)
            : _state(std::move(state)), _name(std::move(name))
        {
            const std::lock_guard lock(_state->mutex);
            _state->openOutputs[_name].push_back(this);
        }

        ~MockOutputPort() override
        {
            const std::lock_guard lock(_state->mutex);
            auto& ports = _state->openOutputs[_name];
            std::erase(ports, this);
        }

        [[nodiscard]] std::string deviceName() const override { return _name; }

        // Capture first, then loop back -- and the loopback call happens with
        // the mutex RELEASED (note the scope around the lock). Delivery runs
        // arbitrary test callbacks that may re-enter this backend, so holding
        // the lock across them would deadlock the moment a callback sends
        // anything back.
        void send(const MidiMessage& message) override
        {
            std::string loopbackTarget;
            {
                const std::lock_guard lock(_state->mutex);
                _state->captured[_name].push_back(message);
                if (const auto found = _state->loopbacks.find(_name); found != _state->loopbacks.end())
                {
                    loopbackTarget = found->second;
                }
            }
            if (!loopbackTarget.empty())
            {
                _state->deliver(loopbackTarget, message);
            }
        }

    private:
        std::shared_ptr<State> _state;
        std::string _name;
    };

    // Snapshot the target list under the lock, then call out with the lock
    // released -- same reason as MockOutputPort::send above: a callback is free
    // to re-enter the backend.
    //
    // TODO: the snapshot holds raw pointers, so a port destroyed on another
    // thread between the snapshot and the receive() call below leaves a
    // dangling pointer. The mutex makes this class look thread-safe and this
    // path is not. Harmless today because every test drives port lifetime from
    // its own thread, but nothing enforces that. A weak_ptr registry, or
    // holding the lock and forbidding re-entrant sends, would close it.
    void MockMidiBackend::State::deliver(const std::string& inputName, const MidiMessage& message)
    {
        std::vector<MockInputPort*> targets;
        {
            const std::lock_guard lock(mutex);
            if (const auto found = openInputs.find(inputName); found != openInputs.end())
            {
                targets = found->second;
            }
        }
        for (auto* port : targets)
        {
            port->receive(message);
        }
    }

    // Same snapshot-then-call-out shape as deliver(), and the same caveat.
    void MockMidiBackend::State::deliverError(const std::string& inputName, const std::string& description)
    {
        std::vector<MockInputPort*> targets;
        {
            const std::lock_guard lock(mutex);
            if (const auto found = openInputs.find(inputName); found != openInputs.end())
            {
                targets = found->second;
            }
        }
        for (auto* port : targets)
        {
            port->receiveError(description);
        }
    }

    MockMidiBackend::MockMidiBackend()
        : _state(std::make_shared<State>())
    {
    }

    MockMidiBackend::~MockMidiBackend() = default;

    void MockMidiBackend::addInputDevice(const std::string& deviceName)
    {
        const std::lock_guard lock(_state->mutex);
        _state->inputNames.push_back(deviceName);
    }

    void MockMidiBackend::addOutputDevice(const std::string& deviceName)
    {
        const std::lock_guard lock(_state->mutex);
        _state->outputNames.push_back(deviceName);
    }

    std::vector<std::string> MockMidiBackend::inputDeviceNames() const
    {
        const std::lock_guard lock(_state->mutex);
        return _state->inputNames;
    }

    std::vector<std::string> MockMidiBackend::outputDeviceNames() const
    {
        const std::lock_guard lock(_state->mutex);
        return _state->outputNames;
    }

    std::unique_ptr<MidiInputPort> MockMidiBackend::openInput(const std::string& deviceName)
    {
        {
            const std::lock_guard lock(_state->mutex);
            if (std::find(_state->inputNames.begin(), _state->inputNames.end(), deviceName)
                == _state->inputNames.end())
            {
                return nullptr; // [RQ-MID-002]
            }
        }
        return std::make_unique<MockInputPort>(_state, deviceName);
    }

    std::unique_ptr<MidiOutputPort> MockMidiBackend::openOutput(const std::string& deviceName)
    {
        {
            const std::lock_guard lock(_state->mutex);
            if (std::find(_state->outputNames.begin(), _state->outputNames.end(), deviceName)
                == _state->outputNames.end())
            {
                return nullptr; // [RQ-MID-002]
            }
        }
        return std::make_unique<MockOutputPort>(_state, deviceName);
    }

    // The test's way of playing the synth: a message appears on an input device
    // exactly as if hardware had sent it. Synchronous, so the assertion can
    // follow the call directly.
    void MockMidiBackend::injectIncoming(const std::string& inputDeviceName, const MidiMessage& message)
    {
        _state->deliver(inputDeviceName, message);
    }

    void MockMidiBackend::injectError(const std::string& inputDeviceName, const std::string& description)
    {
        _state->deliverError(inputDeviceName, description);
    }

    std::vector<MidiMessage> MockMidiBackend::sentMessages(const std::string& outputDeviceName) const
    {
        const std::lock_guard lock(_state->mutex);
        if (const auto found = _state->captured.find(outputDeviceName); found != _state->captured.end())
        {
            return found->second;
        }
        return {};
    }

    void MockMidiBackend::clearSentMessages()
    {
        const std::lock_guard lock(_state->mutex);
        _state->captured.clear();
    }

    // Wires an output straight back to an input, so a controller request
    // produces its own reply. Used to exercise round-trips (program change ->
    // dump request -> dump) without hardware. One target per output; calling
    // again re-points it.
    void MockMidiBackend::connectLoopback(const std::string& outputDeviceName, const std::string& inputDeviceName)
    {
        const std::lock_guard lock(_state->mutex);
        _state->loopbacks[outputDeviceName] = inputDeviceName;
    }

    // How many handles are open on a device right now. This is what proves the
    // controller's device hot-swap closed the old port instead of leaking it:
    // assign twice, expect 1. [RQ-MID-003]
    int MockMidiBackend::openInputPortCount(const std::string& deviceName) const
    {
        const std::lock_guard lock(_state->mutex);
        const auto found = _state->openInputs.find(deviceName);
        return found == _state->openInputs.end() ? 0 : static_cast<int>(found->second.size());
    }

    int MockMidiBackend::openOutputPortCount(const std::string& deviceName) const
    {
        const std::lock_guard lock(_state->mutex);
        const auto found = _state->openOutputs.find(deviceName);
        return found == _state->openOutputs.end() ? 0 : static_cast<int>(found->second.size());
    }
}
