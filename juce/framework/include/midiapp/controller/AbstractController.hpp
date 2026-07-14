#pragma once

#include "midiapp/controller/DualDictionary.hpp"
#include "midiapp/controller/EventDispatcher.hpp"
#include "midiapp/model/AbstractTone.hpp"
#include "midiapp/model/ToneIO.hpp"
#include "xpl/midi/MidiPorts.hpp"

#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace midiapp::controller
{
    /// Port of MidiApp RandomizeToneArgument. `seed` added for deterministic
    /// tests (the reference seeds from the clock).
    struct RandomizeToneArguments
    {
        std::set<std::string> excludedParameters;
        std::optional<float> humanizeRatio;
        std::optional<unsigned int> seed;
    };

    /// Port of MidiApp.MidiController.Controller.AbstractController:
    /// owns the edited tone, three MIDI devices resolved by display name
    /// through an injected backend (ADR-JUC-004), the CC automation table, and
    /// the paced transmit worker (ADR-JUC-005). [RQ-FMW-030..034]
    class AbstractController
    {
    public:
        /// Reference ControlChangeType.None.
        static constexpr int NO_CONTROL_CHANGE = -1;

        virtual ~AbstractController();

        AbstractController(const AbstractController&) = delete;
        AbstractController& operator=(const AbstractController&) = delete;

        // --- lifecycle [RQ-FMW-030] ---
        virtual void start();
        virtual void stop();
        [[nodiscard]] bool isRunning() const { return _isRunning; }

        // --- parameters ---
        /// Rejects unknown names and no-ops while parameter setting is
        /// disabled; returns false in both cases. [RQ-FMW-031]
        bool setParameter(const std::string& parameterName, int value);

        /// nullptr when unknown ("used for init only" in the reference).
        [[nodiscard]] model::AbstractParameter* getParameter(const std::string& parameterName);

        [[nodiscard]] virtual std::string toneName() const;
        virtual void setToneName(const std::string& name);
        [[nodiscard]] virtual int currentProgramNumber() const = 0;
        virtual void setMidiChannel(int midiChannel);

        // --- automation [RQ-FMW-032, RQ-FMW-033] ---
        [[nodiscard]] DualDictionary& controlChangeAutomationTable() { return _automationTable; }
        [[nodiscard]] int disabledControlChangeNumber() const;
        void setDisabledControlChangeNumber(int ccNumber);

        /// Delay in ms between two transmitted SysEx; throws when < 0
        /// (reference default 20 ms). [RQ-FMW-034]
        [[nodiscard]] int parameterTransmitDelay() const { return _parameterTransmitDelay; }
        void setParameterTransmitDelay(int delayMilliseconds);

        // --- MIDI devices, resolved by display name [RQ-MID-002..004] ---
        virtual bool setAutomationInputDevice(const std::string& deviceName);
        virtual bool setSynthInputDevice(const std::string& deviceName);
        virtual bool setSynthOutputDevice(const std::string& deviceName);
        virtual void closeMidiDevices(); // [RQ-MID-006]

        // --- tone I/O [RQ-FMW-020] ---
        virtual void loadTone(const std::string& filename, model::IToneReader& reader);
        virtual void saveTone(const std::string& filename, model::IToneWriter& writer);
        virtual void randomizeTone(const RandomizeToneArguments& arguments);
        [[nodiscard]] virtual std::vector<std::pair<std::string, std::unique_ptr<model::AbstractTone>>>
        extractSinglePatchesFromAllDataDumpFileToDirectory(const std::string& bankFilename,
                                                           const std::string& directoryName) = 0;

        /// Sends Note On (velocity 127) / Note Off (velocity 0) on the tone's
        /// channel, as the reference PlayNote does.
        virtual void playNote(bool isNoteOn, int noteId);

        /// Parameter-change notification toward the UI, delivered through the
        /// event dispatcher. [RQ-FMW-060, RQ-FMW-061]
        void setAutomationParameterChangeHandler(std::function<void(const std::string&, int)> handler);

    protected:
        AbstractController(xpl::midi::MidiBackend& backend,
                           std::unique_ptr<model::AbstractTone> tone,
                           std::shared_ptr<EventDispatcher> dispatcher = nullptr);

        [[nodiscard]] model::AbstractTone& tone() { return *_tone; }
        [[nodiscard]] const model::AbstractTone& tone() const { return *_tone; }

        /// Reference EnableSetParameter: gates SetParameter during bulk
        /// operations (randomize, full-tone reload).
        [[nodiscard]] bool isSetParameterEnabled() const { return _isSetParameterEnabled; }
        void setSetParameterEnabled(bool enabled) { _isSetParameterEnabled = enabled; }

        void notifyAutomationParameterChangeEvent(const std::string& parameterName, int value);

        /// Posts through the injected dispatcher (UI-thread marshalling).
        void postEvent(std::function<void()> action);

        // --- device guards (reference Verify*Device) ---
        [[nodiscard]] bool verifySynthOutputDevice() const { return _synthOutput != nullptr; }
        [[nodiscard]] bool verifySynthInputDevice() const { return _synthInput != nullptr; }
        [[nodiscard]] bool verifyAutomationInputDevice() const { return _automationInput != nullptr; }

        /// Guarded send to the synth output; silently drops when no device.
        void sendToSynthOutput(const xpl::midi::MidiMessage& message);

        // --- automation input handlers (defaults = reference behavior) ---
        virtual void automationInputDeviceChannelMessageReceived(const xpl::midi::MidiMessage& message); // [RQ-FMW-050/051]
        virtual void automationInputDeviceSysExMessageReceived(const xpl::midi::MidiMessage& message);   // [RQ-FMW-052]
        virtual void automationInputDeviceSysCommonMessageReceived(const xpl::midi::MidiMessage& message);
        virtual void automationInputDeviceSysRealtimeMessageReceived(const xpl::midi::MidiMessage& message);
        virtual void automationInputDeviceError(const std::string& description);

        // --- synth input handlers (defaults: do nothing, like the reference) ---
        virtual void synthInputDeviceChannelMessageReceived(const xpl::midi::MidiMessage& message);
        virtual void synthInputDeviceSysExMessageReceived(const xpl::midi::MidiMessage& message);
        virtual void synthInputDeviceSysCommonMessageReceived(const xpl::midi::MidiMessage& message);
        virtual void synthInputDeviceSysRealtimeMessageReceived(const xpl::midi::MidiMessage& message);
        virtual void synthInputDeviceError(const std::string& description);

        // --- transmit worker [RQ-FMW-040..042, ADR-JUC-005] ---
        /// One iteration = wait one transmit-delay tick, scan changed
        /// parameters into the queue (cloned), send at most one. Subclasses
        /// override to add page-select sequencing (RQ-CTL-020).
        virtual void workerThreadProc(std::stop_token stopToken);

        /// Interruptible wait honoring both the transmit delay and stop
        /// requests; returns false when stopping.
        bool waitForTransmitDelay(std::stop_token stopToken);

        void scanChangedParametersIntoQueue();
        void enqueueParameter(std::unique_ptr<model::AbstractParameter> parameter);
        bool dequeueParameter(std::unique_ptr<model::AbstractParameter>& parameter);

    private:
        void startWorkerThread();
        void stopWorkerThread();
        [[nodiscard]] xpl::midi::MidiInputCallbacks makeAutomationCallbacks();
        [[nodiscard]] xpl::midi::MidiInputCallbacks makeSynthCallbacks();
        bool assignInputDevice(std::unique_ptr<xpl::midi::MidiInputPort>& slot,
                               const std::string& deviceName,
                               xpl::midi::MidiInputCallbacks callbacks);

        xpl::midi::MidiBackend& _backend;
        std::unique_ptr<model::AbstractTone> _tone;
        std::shared_ptr<EventDispatcher> _dispatcher;
        DualDictionary _automationTable;

        std::unique_ptr<xpl::midi::MidiOutputPort> _synthOutput;
        std::unique_ptr<xpl::midi::MidiInputPort> _synthInput;
        std::unique_ptr<xpl::midi::MidiInputPort> _automationInput;

        std::function<void(const std::string&, int)> _automationParameterChangeHandler;

        mutable std::mutex _disabledCcMutex;
        int _disabledControlChangeNumber = NO_CONTROL_CHANGE;

        int _parameterTransmitDelay = 20; // reference default, ms
        bool _isRunning = false;
        bool _isSetParameterEnabled = true;

        std::mutex _queueMutex;
        std::deque<std::unique_ptr<model::AbstractParameter>> _parameterQueue;

        std::jthread _workerThread;
        std::mutex _workerWakeMutex;
        std::condition_variable_any _workerWake;
    };
}
