#include "midiapp/controller/AbstractController.hpp"

#include <cmath>
#include <filesystem>
#include <stdexcept>

namespace midiapp::controller
{
    using xpl::midi::ChannelCommand;
    using xpl::midi::MessageType;
    using xpl::midi::MidiMessage;

    AbstractController::AbstractController(xpl::midi::MidiBackend& backend,
                                           std::unique_ptr<model::AbstractTone> tone,
                                           std::shared_ptr<EventDispatcher> dispatcher)
        : _backend(backend),
          _tone(std::move(tone)),
          _dispatcher(dispatcher != nullptr ? std::move(dispatcher)
                                            : std::make_shared<SynchronousEventDispatcher>())
    {
        if (_tone == nullptr)
        {
            throw std::invalid_argument("tone cannot be null");
        }
    }

    AbstractController::~AbstractController()
    {
        stopWorkerThread();
        closeMidiDevices();
    }

    // --- lifecycle -------------------------------------------------------

    void AbstractController::start()
    {
        if (verifyAutomationInputDevice())
        {
            _automationInput->start();
        }
        if (verifySynthInputDevice())
        {
            _synthInput->start();
        }
        startWorkerThread();
        _isRunning = true;
    }

    void AbstractController::stop()
    {
        if (verifyAutomationInputDevice())
        {
            _automationInput->stop();
        }
        if (verifySynthInputDevice())
        {
            _synthInput->stop();
        }
        stopWorkerThread();
        _isRunning = false;
    }

    // --- parameters ------------------------------------------------------

    bool AbstractController::setParameter(const std::string& parameterName, int value)
    {
        if (!_tone->parameterMap().contains(parameterName) || !_isSetParameterEnabled)
        {
            return false; // [RQ-FMW-031]
        }
        _tone->parameterMap().at(parameterName).setValue(value);
        return true;
    }

    model::AbstractParameter* AbstractController::getParameter(const std::string& parameterName)
    {
        return _tone->parameterMap().find(parameterName);
    }

    std::string AbstractController::toneName() const
    {
        return _tone->toneName();
    }

    void AbstractController::setToneName(const std::string& name)
    {
        _tone->setToneName(name);
    }

    void AbstractController::setMidiChannel(int midiChannel)
    {
        _tone->setMidiChannel(midiChannel);
    }

    // --- automation table ------------------------------------------------

    int AbstractController::disabledControlChangeNumber() const
    {
        const std::lock_guard lock(_disabledCcMutex);
        return _disabledControlChangeNumber;
    }

    void AbstractController::setDisabledControlChangeNumber(int ccNumber)
    {
        const std::lock_guard lock(_disabledCcMutex);
        _disabledControlChangeNumber = ccNumber;
    }

    void AbstractController::setParameterTransmitDelay(int delayMilliseconds)
    {
        if (delayMilliseconds < 0)
        {
            throw std::out_of_range("delay can not be <0");
        }
        _parameterTransmitDelay = delayMilliseconds;
    }

    // --- tone I/O --------------------------------------------------------

    void AbstractController::loadTone(const std::string& filename, model::IToneReader& reader)
    {
        reader.readTone(filename, *_tone);
        // Reference: notify the UI for every parameter (no retransmission
        // at this level; synth transmission is a subclass concern).
        for (const auto& entry : _tone->parameterMap())
        {
            notifyAutomationParameterChangeEvent(entry.name, entry.parameter->value());
        }
    }

    void AbstractController::saveTone(const std::string& filename, model::IToneWriter& writer)
    {
        if (_tone->toneName().empty())
        {
            _tone->setToneName(std::filesystem::path(filename).stem().string());
        }
        writer.writeTone(filename, *_tone);
    }

    void AbstractController::randomizeTone(const RandomizeToneArguments& arguments)
    {
        stop();
        setSetParameterEnabled(false); // block automation/UI during the bulk change

        _tone->randomizeToneParameters(arguments.excludedParameters, arguments.humanizeRatio,
                                       arguments.seed);
        _tone->setToneName("RANDOM");
        // The full tone will be transmitted: flag everything changed.
        for (const auto& entry : _tone->parameterMap())
        {
            entry.parameter->setChanged(true);
            notifyAutomationParameterChangeEvent(entry.name, entry.parameter->value());
        }

        setSetParameterEnabled(true);
        start();
    }

    void AbstractController::playNote(bool isNoteOn, int noteId)
    {
        if (!verifySynthOutputDevice())
        {
            return;
        }
        const auto command = isNoteOn ? ChannelCommand::NoteOn : ChannelCommand::NoteOff;
        const int velocity = isNoteOn ? 127 : 0;
        sendToSynthOutput(MidiMessage::channelMessage(command, _tone->midiChannel(), noteId, velocity));
    }

    // --- events ----------------------------------------------------------

    void AbstractController::setAutomationParameterChangeHandler(
        std::function<void(const std::string&, int)> handler)
    {
        _automationParameterChangeHandler = std::move(handler);
    }

    void AbstractController::notifyAutomationParameterChangeEvent(const std::string& parameterName,
                                                                  int value)
    {
        if (_automationParameterChangeHandler)
        {
            _dispatcher->post([handler = _automationParameterChangeHandler, parameterName, value]
                              { handler(parameterName, value); });
        }
    }

    void AbstractController::postEvent(std::function<void()> action)
    {
        _dispatcher->post(std::move(action));
    }

    void AbstractController::sendToSynthOutput(const MidiMessage& message)
    {
        if (verifySynthOutputDevice())
        {
            _synthOutput->send(message);
        }
    }
}
