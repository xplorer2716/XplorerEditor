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
#include "xpl/midi/JuceMidiBackend.hpp"

#include <juce_audio_devices/juce_audio_devices.h>

#include <atomic>

namespace xpl::midi
{
    namespace
    {
        // Resolves a display name to a JUCE device identifier; first match
        // wins, as the reference matches on device capability names.
        template <typename DeviceInfoArray>
        juce::String identifierForName(const DeviceInfoArray& devices, const std::string& deviceName)
        {
            for (const auto& device : devices)
            {
                if (device.name.toStdString() == deviceName)
                {
                    return device.identifier;
                }
            }
            return {};
        }

        MidiMessage toXplMessage(const juce::MidiMessage& message)
        {
            return MidiMessage::fromRawBytes(
                std::span<const std::uint8_t>(message.getRawData(),
                                              static_cast<std::size_t>(message.getRawDataSize())));
        }

        class JuceInputPort final : public MidiInputPort,
                                    private juce::MidiInputCallback
        {
        public:
            JuceInputPort(const juce::String& identifier, std::string displayName)
                : _displayName(std::move(displayName))
            {
                _device = juce::MidiInput::openDevice(identifier, this);
            }

            ~JuceInputPort() override
            {
                stop(); // [RQ-MID-006]
            }

            [[nodiscard]] bool isOpen() const { return _device != nullptr; }

            [[nodiscard]] std::string deviceName() const override { return _displayName; }

            void setCallbacks(MidiInputCallbacks callbacks) override
            {
                // Contract: only while stopped; JUCE never calls back before start().
                _callbacks = std::move(callbacks);
            }

            void start() override // [RQ-MID-005]
            {
                if (_device != nullptr && !_started.exchange(true))
                {
                    _device->start();
                }
            }

            void stop() override
            {
                if (_device != nullptr && _started.exchange(false))
                {
                    _device->stop();
                }
            }

            [[nodiscard]] bool isStarted() const override { return _started; }

        private:
            // JUCE delivers complete (reassembled) SysEx frames here, on one
            // dedicated thread per device: serialized order guaranteed.
            // [RQ-MID-021, RQ-MID-024]
            void handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage& message) override
            {
                if (!_started)
                {
                    return;
                }
                const auto xplMessage = toXplMessage(message);
                switch (xplMessage.type())
                {
                    case MessageType::Channel: // [RQ-MID-020]
                        if (_callbacks.onChannelMessage) _callbacks.onChannelMessage(xplMessage);
                        break;
                    case MessageType::SysEx:
                        if (_callbacks.onSysExMessage) _callbacks.onSysExMessage(xplMessage);
                        break;
                    case MessageType::SysCommon: // [RQ-MID-022]
                        if (_callbacks.onSysCommonMessage) _callbacks.onSysCommonMessage(xplMessage);
                        break;
                    case MessageType::SysRealtime:
                        if (_callbacks.onSysRealtimeMessage) _callbacks.onSysRealtimeMessage(xplMessage);
                        break;
                    case MessageType::Invalid:
                        break;
                }
            }

            std::string _displayName;
            std::unique_ptr<juce::MidiInput> _device;
            MidiInputCallbacks _callbacks;
            std::atomic<bool> _started{false};
        };

        class JuceOutputPort final : public MidiOutputPort
        {
        public:
            JuceOutputPort(const juce::String& identifier, std::string displayName)
                : _displayName(std::move(displayName))
            {
                _device = juce::MidiOutput::openDevice(identifier);
            }

            [[nodiscard]] bool isOpen() const { return _device != nullptr; }

            [[nodiscard]] std::string deviceName() const override { return _displayName; }

            void send(const MidiMessage& message) override // [RQ-MID-010..012]
            {
                const auto bytes = message.bytes();
                _device->sendMessageNow(juce::MidiMessage(bytes.data(), static_cast<int>(bytes.size())));
            }

        private:
            std::string _displayName;
            std::unique_ptr<juce::MidiOutput> _device;
        };
    }

    struct JuceMidiBackend::Impl
    {
        // Provides the JUCE runtime (MessageManager) required by device
        // enumeration in non-JUCE-app processes such as the test runners.
        juce::ScopedJuceInitialiser_GUI juceRuntime;
    };

    JuceMidiBackend::JuceMidiBackend()
        : _impl(std::make_unique<Impl>())
    {
    }

    JuceMidiBackend::~JuceMidiBackend() = default;

    std::vector<std::string> JuceMidiBackend::inputDeviceNames() const // [RQ-MID-001]
    {
        std::vector<std::string> names;
        for (const auto& device : juce::MidiInput::getAvailableDevices())
        {
            names.push_back(device.name.toStdString());
        }
        return names;
    }

    std::vector<std::string> JuceMidiBackend::outputDeviceNames() const // [RQ-MID-001]
    {
        std::vector<std::string> names;
        for (const auto& device : juce::MidiOutput::getAvailableDevices())
        {
            names.push_back(device.name.toStdString());
        }
        return names;
    }

    std::unique_ptr<MidiInputPort> JuceMidiBackend::openInput(const std::string& deviceName) // [RQ-MID-002]
    {
        const auto identifier = identifierForName(juce::MidiInput::getAvailableDevices(), deviceName);
        if (identifier.isEmpty())
        {
            return nullptr;
        }
        auto port = std::make_unique<JuceInputPort>(identifier, deviceName);
        return port->isOpen() ? std::move(port) : nullptr;
    }

    std::unique_ptr<MidiOutputPort> JuceMidiBackend::openOutput(const std::string& deviceName) // [RQ-MID-002]
    {
        const auto identifier = identifierForName(juce::MidiOutput::getAvailableDevices(), deviceName);
        if (identifier.isEmpty())
        {
            return nullptr;
        }
        auto port = std::make_unique<JuceOutputPort>(identifier, deviceName);
        return port->isOpen() ? std::move(port) : nullptr;
    }
}
