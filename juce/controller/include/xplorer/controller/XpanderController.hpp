#pragma once

// Port of Xplorer.Controller.XpanderController (all partial classes).
// [RQ-CTL-001..008, RQ-CTL-020..027, RQ-CTL-030, RQ-CTL-040, RQ-CTL-050..051,
//  RQ-CTL-060..062, RQ-CTL-070]

#include "midiapp/controller/AbstractController.hpp"
#include "xplorer/controller/AllDataDumpRequestState.hpp"
#include "xplorer/controller/PageSubPageHelper.hpp"
#include "xplorer/controller/XpanderControllerEvents.hpp"
#include "xplorer/model/XpanderTone.hpp"
#include "xplorer/model/XpanderToneIO.hpp"
#include "xplorer/settings/SettingsService.hpp"

#include <functional>
#include <memory>
#include <string>

namespace xplorer::controller
{
    class XpanderController : public midiapp::controller::AbstractController
    {
    public:
        /// @param settingsService injected (reference kept). [RQ-SET-005]
        /// @param productNameAndVersion shown on the synth VFD greetings.
        XpanderController(xpl::midi::MidiBackend& backend,
                          settings::ISettingsService& settingsService,
                          std::shared_ptr<midiapp::controller::EventDispatcher> dispatcher = nullptr,
                          std::string productNameAndVersion = "XPLORER");

        // --- program / tone identity ---------------------------------------
        [[nodiscard]] int currentProgramNumber() const override;
        void setEditingProgramNumber(int programNumber);

        /// Renaming retransmits the full tone and resynchronizes the synth,
        /// as the reference ToneName setter does.
        void setToneName(const std::string& name) override;

        // --- lifecycle ------------------------------------------------------
        /// First start sends program change + patch request to sync with the
        /// synth; stop sends smart all-notes-off when enabled.
        void start() override;
        void stop() override;

        // --- patch operations [RQ-CTL-001..008] -----------------------------
        void loadTone(const std::string& filename);
        void loadTone(const std::string& filename, midiapp::model::IToneReader& reader) override;
        void saveTone(const std::string& filename);
        [[nodiscard]] model::SysexFileType determineSysexFileType(const std::string& fileName) const;
        [[nodiscard]] std::vector<std::pair<std::string, std::unique_ptr<midiapp::model::AbstractTone>>>
        extractSinglePatchesFromAllDataDumpFileToDirectory(const std::string& bankFilename,
                                                           const std::string& directoryName) override;
        void backupAllDataDumpToFile(const std::string& fileName);
        void restoreAllDataDumpToSynth(const std::string& fileName,
                                       const std::function<void(int, int)>& progressionAction);
        void getSingleTonesFromSynth(const std::string& destinationFolder);
        void storeSinglePatchToSynth(int programNumber);
        void sendProgramChangeAndGetSinglePatchFromSynth(int programNumber);
        void increaseCurrentProgramNumber();
        void decreaseCurrentProgramNumber();
        void sendProgrammerModeSinglePatch();

        // --- randomize / morph [RQ-CTL-050..051] ----------------------------
        void randomizeTone(const midiapp::controller::RandomizeToneArguments& arguments) override;
        void morphTones(const std::string& firstToneFilename, const std::string& secondToneFilename,
                        float morphingFactor);

        // --- modulation matrix [RQ-CTL-030] ---------------------------------
        void changeModulationSource(int newModulationSource, int modulationSourceAmount,
                                    int modulationQuantize, int modulationDestination, int entryNumber);
        void changeModulationSourceAmount(int modulationSource, int modulationSourceAmount,
                                          int modulationDestination, int entryNumber);
        void changeModulationSourceQuantize(int modulationSource, int modulationDestination,
                                            int modulationQuantize, int entryNumber);
        void changeModulationDestination(int modulationSource, int modulationSourceAmount,
                                         int modulationQuantize, int oldModulationDestination,
                                         int newModulationDestination, int entryNumber);
        [[nodiscard]] bool isMaxSourceCountForDestinationReached(model::EnumModulationDestinations destination) const;
        [[nodiscard]] bool sourceAvailabilityForEntry(int entryNumber) const;
        [[nodiscard]] std::vector<model::EnumModulationDestinations>
        getAvailableModulationDestinationsForEntry(int entryNumber) const;
        [[nodiscard]] const model::ModulationMatrixEntry& getModulationEntryByNumber(int number) const;

        // --- clipboard (page families) [RQ-CTL-040] --------------------------
        [[nodiscard]] const std::string& clipboardSource() const { return _clipboardSource; }
        void setClipboardSource(const std::string& source) { _clipboardSource = source; }
        [[nodiscard]] bool canClipboardPasteTo(const std::string& destination) const;
        void clearClipboard() { _clipboardSource.clear(); }
        [[nodiscard]] bool isClipboardEmpty() const { return _clipboardSource.empty(); }
        void pasteClipboardTo(const std::string& destination);

        // --- synth utilities [RQ-CTL-060..062] --------------------------------
        void sendAllNotesOffToSynthOutput();
        void sendTuneRequestToSynth();
        void sendPageUpdate(const std::string& pageName);
        void forceSendPageSubPage();
        void sendGreetingsToSynth();
        void sendTypeWriterMessageToSynth(const std::string& message);

        // --- event handlers (set by the view) [RQ-CTL-070] --------------------
        void setFullToneChangeHandler(std::function<void(const FullToneChangeEvent&)> handler);
        void setPageChangeHandler(std::function<void(const PageChangeEvent&)> handler);
        void setModulationEntryChangeHandler(std::function<void(const ModulationEntryChangeEvent&)> handler);
        void setAllDataDumpProgressionHandler(std::function<void(const AllDataDumpProgressionEvent&)> handler);
        void setMidiActivityHandler(std::function<void(EnumMidiDevice)> handler);

    protected:
        /// Page-select-before-edit transmit loop. [RQ-CTL-020, ADR-JUC-005]
        void workerThreadProc(std::stop_token stopToken) override;

        /// Automation overrides (reference XpanderController.MIDIEvents).
        void automationInputDeviceSysExMessageReceived(const xpl::midi::MidiMessage& message) override;
        void automationInputDeviceChannelMessageReceived(const xpl::midi::MidiMessage& message) override;

        /// Synth input overrides. [RQ-CTL-021..027]
        void synthInputDeviceChannelMessageReceived(const xpl::midi::MidiMessage& message) override;
        void synthInputDeviceSysExMessageReceived(const xpl::midi::MidiMessage& message) override;
        void synthInputDeviceSysCommonMessageReceived(const xpl::midi::MidiMessage& message) override;
        void synthInputDeviceSysRealtimeMessageReceived(const xpl::midi::MidiMessage& message) override;

    private:
        static constexpr int DELAY_BETWEEN_ALL_DATA_DUMP_SEND_SINGLE_PATCH = 150;
        static constexpr int DELAY_BETWEEN_MESSAGES = 100;
        static constexpr int MAX_DISPLAY_MESSAGE_LENGTH = 80;

        [[nodiscard]] model::XpanderTone& xpanderTone();
        [[nodiscard]] const model::XpanderTone& xpanderTone() const;
        [[nodiscard]] settings::AllUsersSettings& settings() const;
        [[nodiscard]] std::uint8_t displayControlCommand() const;

        // guarded send + MIDI-activity notification (reference SendDataToSynthOutputDevice)
        void sendDataToSynthOutputDevice(const xpl::midi::MidiMessage& message);

        void sendAllDataDumpRequestToSynth();
        void sendProgramChangeToSynthOutput(int programNumber);
        void sendProgramDumpRequestToSynth(int programNumber);
        void sendPageSubPageAndUpdatePageSubPage(int page, int subPage);
        void sendFullToneToSynthIntoProgram(int programNumber);
        void updateUIAndSendFullToneToSynth(int programNumber);
        void sendDisplayMessageToSynth(const std::string& message);
        void sendDisplayOffOnToSynth();
        void clearAllChangedFlags();

        // synth-input SysEx decoding (reference Is*SysEx helpers)
        bool handleAllDataDumpRequest(const xpl::midi::MidiMessage& message, bool isSinglePatchDataDump);
        void handleModulationEditFromSynth(int modulationSourceNumber,
                                           model::EnumModulationEditCommands command, int value);
        [[nodiscard]] model::XpanderParameter* getParameterForPageSubPageAndId(int page, int subPage, int buttonId);
        [[nodiscard]] static bool isSinglePatchProgramDumpSysex(const xpl::midi::MidiMessage& message);
        [[nodiscard]] static bool isMultiPatchProgramDumpSysex(const xpl::midi::MidiMessage& message);
        [[nodiscard]] static bool isProgramChangeUpSysex(const xpl::midi::MidiMessage& message);
        [[nodiscard]] static bool isProgramChangeDownSysex(const xpl::midi::MidiMessage& message);
        [[nodiscard]] static bool isPageSubPageSelectSysex(const xpl::midi::MidiMessage& message,
                                                           int& page, int& subPage);
        [[nodiscard]] bool isPageEditFollowsSysex(const xpl::midi::MidiMessage& message, int& buttonId,
                                                  int& parameterValue, bool& isRotary) const;
        [[nodiscard]] static bool isModulationEditFollowsSysex(const xpl::midi::MidiMessage& message,
                                                               int& modulationSourceNumber,
                                                               model::EnumModulationEditCommands& command,
                                                               int& value);

        // event notification (posted through the dispatcher)
        void notifyFullToneChangeEvent();
        void notifyPageChangeEvent(int page, int subPage);
        void notifyModulationEntryChangeEvent(const model::ModulationMatrixEntry& entry, int entryNumber,
                                              EnumModulationParameter parameter);
        void notifyAllDataDumpRequestProgressionEvent();
        void notifyMidiDataSendReceiveEvent(EnumMidiDevice device);

        settings::ISettingsService& _settingsService;
        std::string _productNameAndVersion;
        PageSubPageHelper _pageSubPageHelper;
        AllDataDumpRequestState _allDataDumpRequestState;
        std::string _clipboardSource;
        bool _firstStart = true;

        std::function<void(const FullToneChangeEvent&)> _fullToneChangeHandler;
        std::function<void(const PageChangeEvent&)> _pageChangeHandler;
        std::function<void(const ModulationEntryChangeEvent&)> _modulationEntryChangeHandler;
        std::function<void(const AllDataDumpProgressionEvent&)> _allDataDumpProgressionHandler;
        std::function<void(EnumMidiDevice)> _midiActivityHandler;
    };
}
