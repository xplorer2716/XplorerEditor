// AbstractController: MIDI device assignment and input handlers.
// Port of AbstractController.MIDIDevices.cs. [RQ-FMW-050..052, RQ-MID-002..006]
#include "midiapp/controller/AbstractController.hpp"

#include <cmath>

namespace midiapp::controller
{
    using xpl::midi::ChannelCommand;
    using xpl::midi::MidiInputCallbacks;
    using xpl::midi::MidiMessage;

    xpl::midi::MidiInputCallbacks AbstractController::makeAutomationCallbacks()
    {
        MidiInputCallbacks callbacks;
        callbacks.onChannelMessage = [this](const MidiMessage& m) { automationInputDeviceChannelMessageReceived(m); };
        callbacks.onSysExMessage = [this](const MidiMessage& m) { automationInputDeviceSysExMessageReceived(m); };
        callbacks.onSysCommonMessage = [this](const MidiMessage& m) { automationInputDeviceSysCommonMessageReceived(m); };
        callbacks.onSysRealtimeMessage = [this](const MidiMessage& m) { automationInputDeviceSysRealtimeMessageReceived(m); };
        callbacks.onError = [this](const std::string& e) { automationInputDeviceError(e); };
        return callbacks;
    }

    xpl::midi::MidiInputCallbacks AbstractController::makeSynthCallbacks()
    {
        MidiInputCallbacks callbacks;
        callbacks.onChannelMessage = [this](const MidiMessage& m) { synthInputDeviceChannelMessageReceived(m); };
        callbacks.onSysExMessage = [this](const MidiMessage& m) { synthInputDeviceSysExMessageReceived(m); };
        callbacks.onSysCommonMessage = [this](const MidiMessage& m) { synthInputDeviceSysCommonMessageReceived(m); };
        callbacks.onSysRealtimeMessage = [this](const MidiMessage& m) { synthInputDeviceSysRealtimeMessageReceived(m); };
        callbacks.onError = [this](const std::string& e) { synthInputDeviceError(e); };
        return callbacks;
    }

    bool AbstractController::assignInputDevice(std::unique_ptr<xpl::midi::MidiInputPort>& slot,
                                               const std::string& deviceName,
                                               xpl::midi::MidiInputCallbacks callbacks)
    {
        if (deviceName.empty())
        {
            return false;
        }
        // Reference: the previous device is deactivated and closed whether or
        // not the new name resolves (hot-swap). [RQ-MID-003]
        auto newPort = _backend.openInput(deviceName);
        slot.reset();
        if (newPort == nullptr)
        {
            return false;
        }
        newPort->setCallbacks(std::move(callbacks));
        newPort->start(); // reference StartRecording on assignment
        slot = std::move(newPort);
        return true;
    }

    bool AbstractController::setAutomationInputDevice(const std::string& deviceName)
    {
        return assignInputDevice(_automationInput, deviceName, makeAutomationCallbacks());
    }

    bool AbstractController::setSynthInputDevice(const std::string& deviceName)
    {
        return assignInputDevice(_synthInput, deviceName, makeSynthCallbacks());
    }

    bool AbstractController::setSynthOutputDevice(const std::string& deviceName)
    {
        if (deviceName.empty())
        {
            return false;
        }
        auto newPort = _backend.openOutput(deviceName);
        _synthOutput.reset();
        if (newPort == nullptr)
        {
            return false;
        }
        _synthOutput = std::move(newPort);
        return true;
    }

    void AbstractController::closeMidiDevices()
    {
        _synthOutput.reset();
        if (_synthInput != nullptr)
        {
            _synthInput->stop();
            _synthInput.reset();
        }
        if (_automationInput != nullptr)
        {
            _automationInput->stop();
            _automationInput.reset();
        }
    }

    // --- automation input, reference behavior ----------------------------

    void AbstractController::automationInputDeviceChannelMessageReceived(const MidiMessage& message)
    {
        if (message.command() == ChannelCommand::Controller)
        {
            if (message.data1() == disabledControlChangeNumber())
            {
                return; // temporarily disabled while the user edits it [RQ-FMW-033]
            }
            const auto parameterNames = _automationTable.parameterNamesFor(message.data1());
            if (!parameterNames.empty())
            {
                for (const auto& parameterName : parameterNames)
                {
                    // Autoscale the 0-127 CC value to the parameter range,
                    // reference formula included the mid-range special case
                    // for two-state parameters. [RQ-FMW-050]
                    const auto* parameter = getParameter(parameterName);
                    const double interleave = parameter->maxValue() + std::abs(parameter->minValue());
                    double controllerValue = message.data2();
                    if (interleave == 1 && controllerValue > 63)
                    {
                        controllerValue = 127;
                    }
                    const double controllerRatio = controllerValue / 127.0;
                    const int value = static_cast<int>(controllerRatio * interleave)
                                      - std::abs(parameter->minValue());
                    setParameter(parameterName, value);
                    notifyAutomationParameterChangeEvent(parameterName, value);
                }
                return;
            }
        }
        // Not an automated CC (or not a CC at all): forward to the synth
        // re-stamped on the tone's channel. [RQ-FMW-051]
        sendToSynthOutput(MidiMessage::channelMessage(message.command(), _tone->midiChannel(),
                                                      message.data1(), message.data2()));
    }

    void AbstractController::automationInputDeviceSysExMessageReceived(const MidiMessage& message)
    {
        sendToSynthOutput(message); // [RQ-FMW-052]
    }

    void AbstractController::automationInputDeviceSysCommonMessageReceived(const MidiMessage& message)
    {
        sendToSynthOutput(message); // [RQ-FMW-052]
    }

    void AbstractController::automationInputDeviceSysRealtimeMessageReceived(const MidiMessage& message)
    {
        sendToSynthOutput(message); // [RQ-FMW-052]
    }

    void AbstractController::automationInputDeviceError(const std::string&) {}

    // --- synth input defaults: do nothing, as the reference ---------------

    void AbstractController::synthInputDeviceChannelMessageReceived(const MidiMessage&) {}
    void AbstractController::synthInputDeviceSysExMessageReceived(const MidiMessage&) {}
    void AbstractController::synthInputDeviceSysCommonMessageReceived(const MidiMessage&) {}
    void AbstractController::synthInputDeviceSysRealtimeMessageReceived(const MidiMessage&) {}
    void AbstractController::synthInputDeviceError(const std::string&) {}
}
