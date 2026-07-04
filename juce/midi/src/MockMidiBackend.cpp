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
#include "xpl/midi/MockMidiBackend.hpp"

#include <algorithm>
#include <map>

namespace xpl::midi
{
    // Shared mutable state outliving the backend from the ports' viewpoint:
    // ports hold a shared_ptr so a port destroyed after the backend is safe.
    struct MockMidiBackend::State
    {
        std::mutex mutex;
        std::vector<std::string> inputNames;
        std::vector<std::string> outputNames;
        std::map<std::string, std::vector<MockInputPort*>> openInputs;
        std::map<std::string, std::vector<MockOutputPort*>> openOutputs;
        std::map<std::string, std::vector<MidiMessage>> captured;
        std::map<std::string, std::string> loopbacks; // output name -> input name

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

    void MockMidiBackend::connectLoopback(const std::string& outputDeviceName, const std::string& inputDeviceName)
    {
        const std::lock_guard lock(_state->mutex);
        _state->loopbacks[outputDeviceName] = inputDeviceName;
    }

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
