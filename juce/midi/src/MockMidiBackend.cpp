/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2026 https://github.com/xplorer2716

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
        // Weak, not raw: deliver() must be able to prove a port body is still
        // alive before calling into it, and must keep it alive for the whole
        // callback. See the delivery note on State::deliver. [RQ-BUG-005]
        std::map<std::string, std::vector<std::weak_ptr<InputPortBody>>> openInputs;
        // Raw is safe here: these pointers are only ever COUNTED, under the
        // mutex, by openOutputPortCount. Nothing dereferences them outside it.
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

    // The state a delivery touches, split out of the port handle so its
    // lifetime can outlast the handle. openInput() hands the caller a
    // MockInputPort (owning this body); the registry keeps only a weak_ptr, and
    // deliver() upgrades that weak_ptr for the duration of the callback. A port
    // released mid-delivery therefore leaves the body alive until the callback
    // returns, instead of a dangling pointer. [RQ-BUG-005]
    //
    // `closed` distinguishes the two states a still-alive body can be in: the
    // handle is gone (closed, deliver nothing) versus merely stopped (start()
    // not called, also deliver nothing, but reversible).
    struct MockMidiBackend::InputPortBody
    {
        MidiInputCallbacks callbacks;
        bool started = false;
        bool closed = false;

        // Fans one message out to whichever callback matches its type. A port
        // that has been stopped or closed drops the message entirely rather
        // than queueing it, which is what lets a test assert that stop() really
        // silences a device -- the controller relies on that when it stops the
        // ports while the settings dialog is open. [RQ-MID-005]
        void receive(const MidiMessage& message)
        {
            if (!started || closed)
            {
                return;
            }
            switch (message.type())
            {
                case MessageType::Channel:
                    if (callbacks.onChannelMessage) callbacks.onChannelMessage(message);
                    break;
                case MessageType::SysEx:
                    if (callbacks.onSysExMessage) callbacks.onSysExMessage(message);
                    break;
                case MessageType::SysCommon:
                    if (callbacks.onSysCommonMessage) callbacks.onSysCommonMessage(message);
                    break;
                case MessageType::SysRealtime:
                    if (callbacks.onSysRealtimeMessage) callbacks.onSysRealtimeMessage(message);
                    break;
                case MessageType::Invalid:
                    break;
            }
        }

        void receiveError(const std::string& description)
        {
            if (started && !closed && callbacks.onError)
            {
                callbacks.onError(description);
            }
        }
    };

    // The handle the caller owns. Destroying it closes the port; the body it
    // owns may briefly outlive it if a delivery is in flight.
    class MockMidiBackend::MockInputPort final : public MidiInputPort
    {
    public:
        MockInputPort(std::shared_ptr<State> state, std::string name)
            : _state(std::move(state)), _name(std::move(name)),
              _body(std::make_shared<InputPortBody>())
        {
            const std::lock_guard lock(_state->mutex);
            _state->openInputs[_name].push_back(_body);
        }

        ~MockInputPort() override
        {
            const std::lock_guard lock(_state->mutex);
            _body->closed = true;
            auto& ports = _state->openInputs[_name];
            // Drop this body's entry, and take the opportunity to clear any
            // expired ones so the vector cannot grow without bound across a
            // long test that hot-swaps devices repeatedly.
            std::erase_if(ports, [this](const std::weak_ptr<InputPortBody>& candidate)
                          { return candidate.expired() || candidate.lock() == _body; });
        }

        [[nodiscard]] std::string deviceName() const override { return _name; }
        void setCallbacks(MidiInputCallbacks callbacks) override { _body->callbacks = std::move(callbacks); }
        void start() override { _body->started = true; }
        void stop() override { _body->started = false; }
        [[nodiscard]] bool isStarted() const override { return _body->started; }

    private:
        std::shared_ptr<State> _state;
        std::string _name;
        std::shared_ptr<InputPortBody> _body;
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

    // Snapshot the targets under the lock, then call out with the lock
    // RELEASED. Both halves are required and neither is negotiable:
    //
    //   * The lock must be released, because a callback may re-enter this
    //     backend. The controller's single-patch-dump handler, for instance,
    //     calls stop(), which joins the transmit worker -- and that worker may
    //     itself be inside MockOutputPort::send waiting for this very mutex.
    //     Holding the lock across delivery deadlocks that path.
    //   * The snapshot must own what it will call, because releasing the lock
    //     lets another thread destroy a port handle. Upgrading each weak_ptr
    //     into a shared_ptr here keeps the body alive for the whole callback;
    //     a body whose handle is already gone fails the lock() and is skipped.
    //     [RQ-BUG-005]
    void MockMidiBackend::State::deliver(const std::string& inputName, const MidiMessage& message)
    {
        std::vector<std::shared_ptr<InputPortBody>> targets;
        {
            const std::lock_guard lock(mutex);
            if (const auto found = openInputs.find(inputName); found != openInputs.end())
            {
                for (const auto& candidate : found->second)
                {
                    if (auto body = candidate.lock())
                    {
                        targets.push_back(std::move(body));
                    }
                }
            }
        }
        for (const auto& body : targets)
        {
            body->receive(message);
        }
    }

    // Same snapshot-then-call-out shape as deliver(), for the same two reasons.
    void MockMidiBackend::State::deliverError(const std::string& inputName, const std::string& description)
    {
        std::vector<std::shared_ptr<InputPortBody>> targets;
        {
            const std::lock_guard lock(mutex);
            if (const auto found = openInputs.find(inputName); found != openInputs.end())
            {
                for (const auto& candidate : found->second)
                {
                    if (auto body = candidate.lock())
                    {
                        targets.push_back(std::move(body));
                    }
                }
            }
        }
        for (const auto& body : targets)
        {
            body->receiveError(description);
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
        if (found == _state->openInputs.end())
        {
            return 0;
        }
        // Counts live handles, not registry slots: a body kept alive by an
        // in-flight delivery still has an expired weak_ptr here, and a closed
        // port must read as closed the instant its handle is destroyed.
        return static_cast<int>(std::count_if(found->second.begin(), found->second.end(),
                                              [](const std::weak_ptr<InputPortBody>& candidate)
                                              { return !candidate.expired(); }));
    }

    int MockMidiBackend::openOutputPortCount(const std::string& deviceName) const
    {
        const std::lock_guard lock(_state->mutex);
        const auto found = _state->openOutputs.find(deviceName);
        return found == _state->openOutputs.end() ? 0 : static_cast<int>(found->second.size());
    }
}
