// XpanderController core: lifecycle, patch operations, randomize/morph,
// clipboard, synth utilities, display messages, transmit worker.
// Port of XpanderController.cs / .WorkerThread.cs / .Clipboard.cs /
// .ModulationMatrix.cs / Events.
#include "xplorer/controller/XpanderController.hpp"

#include "midiapp/service/FileUtils.hpp"
#include "midiapp/service/Logger.hpp"
#include "xpl/midi/SysexStreamIterator.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

namespace xplorer::controller
{
    using midiapp::model::AbstractTone;
    using midiapp::model::ToneException;
    using model::EnumPages;
    using model::XpanderParameter;
    using model::XpanderTone;
    using xpl::midi::ChannelCommand;
    using xpl::midi::MidiMessage;

    namespace
    {
        void sleepMs(int milliseconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }

        std::vector<std::uint8_t> readAllBytes(const std::string& filename)
        {
            std::ifstream stream(filename, std::ios::binary);
            if (!stream.good())
            {
                throw ToneException("File does not exist: " + filename);
            }
            return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(stream), {});
        }
    }

    XpanderController::XpanderController(xpl::midi::MidiBackend& backend,
                                         settings::ISettingsService& settingsService,
                                         std::shared_ptr<midiapp::controller::EventDispatcher> dispatcher,
                                         std::string productNameAndVersion)
        : AbstractController(backend, std::make_unique<XpanderTone>(), std::move(dispatcher)),
          _settingsService(settingsService),
          _productNameAndVersion(std::move(productNameAndVersion))
    {
        clearClipboard();
    }

    XpanderTone& XpanderController::xpanderTone()
    {
        return static_cast<XpanderTone&>(tone());
    }

    const XpanderTone& XpanderController::xpanderTone() const
    {
        return static_cast<const XpanderTone&>(tone());
    }

    settings::AllUsersSettings& XpanderController::settings() const
    {
        return _settingsService.allUsersSettings();
    }

    std::uint8_t XpanderController::displayControlCommand() const
    {
        // Unlike the reference (frozen at static init), evaluated per call so
        // a synth-type change applies without restart. [RQ-CTL-061..062]
        return settings().midiConfig.synthTypeIsMatrix12
                   ? static_cast<std::uint8_t>(model::constants::DISPLAY_CONTROL_COMMAND_MATRIX12)
                   : static_cast<std::uint8_t>(model::constants::DISPLAY_CONTROL_COMMAND_XPANDER);
    }

    // --- program / tone identity -------------------------------------------

    int XpanderController::currentProgramNumber() const
    {
        return xpanderTone().currentProgramNumber();
    }

    void XpanderController::setEditingProgramNumber(int programNumber)
    {
        xpanderTone().setEditingProgramNumber(programNumber);
    }

    void XpanderController::setToneName(const std::string& name)
    {
        AbstractController::setToneName(name);
        // Reference: round-trip through the byte form, retransmit and resync.
        XpanderTone clone;
        clone.fromByteArray(xpanderTone().toByteArray());
        sendDataToSynthOutputDevice(MidiMessage::sysEx(clone.toByteArray()));
        sendProgramChangeAndGetSinglePatchFromSynth(clone.currentProgramNumber());
    }

    // --- lifecycle -----------------------------------------------------------

    void XpanderController::start()
    {
        if (_firstStart)
        {
            _firstStart = false;
            xpanderTone().setCurrentProgramNumber(xpanderTone().editingProgramNumber());
            sendProgramChangeAndGetSinglePatchFromSynth(xpanderTone().currentProgramNumber());
        }
        AbstractController::start();
    }

    void XpanderController::stop()
    {
        if (settings().midiConfig.smartAllNotesOff)
        {
            sendAllNotesOffToSynthOutput(); // [RQ-CTL-060]
        }
        AbstractController::stop();
    }

    // --- patch operations ----------------------------------------------------

    model::SysexFileType XpanderController::determineSysexFileType(const std::string& fileName) const
    {
        return model::determineSysexFileType(fileName);
    }

    void XpanderController::loadTone(const std::string& filename)
    {
        model::XpanderToneReader reader;
        loadTone(filename, reader);
        clearClipboard();
    }

    void XpanderController::loadTone(const std::string& filename, midiapp::model::IToneReader& reader)
    {
        if (settings().midiConfig.smartAllNotesOff)
        {
            sendAllNotesOffToSynthOutput();
        }
        stop();
        setSetParameterEnabled(false);
        reader.readTone(filename, tone());
        xpanderTone().setCurrentProgramNumber(xpanderTone().editingProgramNumber());
        updateUIAndSendFullToneToSynth(xpanderTone().editingProgramNumber()); // [RQ-CTL-001]
        clearAllChangedFlags();
        setSetParameterEnabled(true);
        start();
    }

    void XpanderController::saveTone(const std::string& filename)
    {
        model::XpanderToneWriter writer;
        AbstractController::saveTone(filename, writer); // [RQ-CTL-002]
        clearClipboard();
    }

    std::vector<std::pair<std::string, std::unique_ptr<AbstractTone>>>
    XpanderController::extractSinglePatchesFromAllDataDumpFileToDirectory(const std::string& bankFilename,
                                                                          const std::string& directoryName)
    {
        if (!std::filesystem::exists(bankFilename) || !std::filesystem::is_directory(directoryName))
        {
            throw ToneException("File or directory does not exists.");
        }
        model::XpanderToneReader reader;
        auto tones = reader.readTones(bankFilename);
        model::XpanderToneWriter writer;
        for (const auto& [name, extractedTone] : tones)
        {
            const auto filename = midiapp::service::makeUniqueFilenameFromString(
                name, midiapp::service::SYSEX_FILE_EXTENSION_WITH_DOT, directoryName);
            writer.writeTone((std::filesystem::path(directoryName) / filename).string(), *extractedTone);
        }
        return tones; // [RQ-CTL-003]
    }

    void XpanderController::backupAllDataDumpToFile(const std::string& fileName)
    {
        if (_allDataDumpRequestState.isWaitingForAllDataDumpRequest())
        {
            throw ToneException("An \"all data dump request\" is already in progress.");
        }
        stop();
        start();
        _allDataDumpRequestState.initialize(fileName, AllDataDumpRequestState::Mode::All);
        try
        {
            sendAllDataDumpRequestToSynth(); // [RQ-CTL-005]
        }
        catch (const std::exception& e)
        {
            _allDataDumpRequestState.setWaitingForAllDataDumpRequest(false);
            throw ToneException(e.what());
        }
    }

    void XpanderController::getSingleTonesFromSynth(const std::string& destinationFolder)
    {
        if (!verifySynthOutputDevice())
        {
            throw ToneException("Unable to open MIDI device "
                                + settings().midiConfig.synthOutputDeviceName);
        }
        if (!std::filesystem::is_directory(destinationFolder))
        {
            throw ToneException("Destionation folder " + destinationFolder + " does not exists.");
        }
        if (_allDataDumpRequestState.isWaitingForAllDataDumpRequest())
        {
            throw ToneException("An \"all data dump request\" is already in progress.");
        }
        stop();
        start();
        _allDataDumpRequestState.initialize(destinationFolder, AllDataDumpRequestState::Mode::SinglePatch);
        try
        {
            sendAllDataDumpRequestToSynth(); // [RQ-CTL-004]
        }
        catch (const std::exception& e)
        {
            _allDataDumpRequestState.setWaitingForAllDataDumpRequest(false);
            throw ToneException(e.what());
        }
    }

    void XpanderController::restoreAllDataDumpToSynth(const std::string& fileName,
                                                      const std::function<void(int, int)>& progressionAction)
    {
        const auto data = readAllBytes(fileName);
        const auto allSysexes = xpl::midi::SysexStreamIterator::allMessages(data);
        const int count = static_cast<int>(allSysexes.size());
        int index = 0;
        for (const auto& sysex : allSysexes)
        {
            sendDataToSynthOutputDevice(MidiMessage::sysEx(sysex));
            progressionAction(index, count); // [RQ-CTL-005]
            sleepMs(DELAY_BETWEEN_ALL_DATA_DUMP_SEND_SINGLE_PATCH);
            ++index;
        }
        sendProgramChangeAndGetSinglePatchFromSynth(currentProgramNumber());
    }

    void XpanderController::storeSinglePatchToSynth(int programNumber)
    {
        if (!verifySynthOutputDevice())
        {
            return;
        }
        XpanderTone clone;
        clone.fromByteArray(xpanderTone().toByteArray());
        clone.setEditingProgramNumber(programNumber);
        sendDataToSynthOutputDevice(MidiMessage::sysEx(clone.toByteArray()));
        sleepMs(DELAY_BETWEEN_MESSAGES);
        const std::vector<std::uint8_t> storePatchMessage{
            0xF0, 0x10, 0x02, 0x07, static_cast<std::uint8_t>(programNumber), xpl::midi::SYSEX_END};
        sendDataToSynthOutputDevice(MidiMessage::sysEx(storePatchMessage)); // [RQ-CTL-006]
        sendProgramChangeAndGetSinglePatchFromSynth(programNumber);
    }

    void XpanderController::sendProgramChangeAndGetSinglePatchFromSynth(int programNumber)
    {
        if (settings().midiConfig.smartAllNotesOff)
        {
            sendAllNotesOffToSynthOutput();
        }
        clearClipboard();
        sendProgramChangeToSynthOutput(programNumber);
        sendProgramDumpRequestToSynth(programNumber);
        sleepMs(DELAY_BETWEEN_MESSAGES);
        xpanderTone().setCurrentProgramNumber(programNumber); // [RQ-CTL-006]
    }

    void XpanderController::increaseCurrentProgramNumber()
    {
        xpanderTone().setCurrentProgramNumber(xpanderTone().currentProgramNumber() + 1); // wraps [RQ-CTL-007]
        sendProgramChangeAndGetSinglePatchFromSynth(xpanderTone().currentProgramNumber());
    }

    void XpanderController::decreaseCurrentProgramNumber()
    {
        xpanderTone().setCurrentProgramNumber(xpanderTone().currentProgramNumber() - 1);
        sendProgramChangeAndGetSinglePatchFromSynth(xpanderTone().currentProgramNumber());
    }

    void XpanderController::sendProgrammerModeSinglePatch()
    {
        if (verifySynthOutputDevice())
        {
            // Byte-for-byte the reference frame, including its duplicated
            // leading 0xF0 (latent quirk preserved for wire fidelity). [RQ-CTL-008]
            const std::vector<std::uint8_t> programmerModeSinglePatch{
                xpl::midi::SYSEX_START, 0xF0, 0x10, 0x02, 0x0D, 0x01, 0x00, xpl::midi::SYSEX_END};
            sendDataToSynthOutputDevice(MidiMessage::fromRawBytes(programmerModeSinglePatch));
        }
    }

    // --- randomize / morph -----------------------------------------------------

    void XpanderController::randomizeTone(const midiapp::controller::RandomizeToneArguments& arguments)
    {
        if (settings().midiConfig.smartAllNotesOff)
        {
            sendAllNotesOffToSynthOutput();
        }
        stop();
        clearClipboard();
        setSetParameterEnabled(false);

        auto excluded = arguments.excludedParameters;
        const auto& randomizerConfig = settings().randomizerConfig;
        const auto vco2Flags = static_cast<unsigned>(randomizerConfig.vco2FmNoiseSync);
        if ((vco2Flags & static_cast<unsigned>(model::EnumRandomVCO2::EnableFM)) == 0)
        {
            excluded.insert("FM_AMP");
            excluded.insert("FM_DESTINATION");
        }
        if ((vco2Flags & static_cast<unsigned>(model::EnumRandomVCO2::EnableNoise)) == 0)
        {
            excluded.insert("VCO2_WAVESHAPE_NOISE");
        }
        if ((vco2Flags & static_cast<unsigned>(model::EnumRandomVCO2::EnableSync)) == 0)
        {
            excluded.insert("VCO2_WAVE_SYNC");
        }
        if (randomizerConfig.vcoDetune == model::EnumRandomVCODetune::Analog
            || randomizerConfig.vcoDetune == model::EnumRandomVCODetune::Digital)
        {
            xpanderTone().detune(randomizerConfig.vcoDetune == model::EnumRandomVCODetune::Analog);
            excluded.insert("VCO1_DETUNE");
            excluded.insert("VCO2_DETUNE");
        }
        if (randomizerConfig.vcoFreq != model::EnumRandomVCOFreq::Free)
        {
            xpanderTone().defineVCOFrequenciesTuning(randomizerConfig.vcoFreq);
            excluded.insert("VCO1_FREQ");
            excluded.insert("VCO2_FREQ");
        }

        tone().randomizeToneParameters(excluded, arguments.humanizeRatio, arguments.seed);

        const auto matrixFlags = static_cast<unsigned>(randomizerConfig.modulationMatrix);
        xpanderTone().randomizeModulationMatrix(
            (matrixFlags & static_cast<unsigned>(model::EnumRandomModMatrix::EnableAmount)) != 0,
            (matrixFlags & static_cast<unsigned>(model::EnumRandomModMatrix::EnableQuantize)) != 0,
            (matrixFlags & static_cast<unsigned>(model::EnumRandomModMatrix::EnableSourcesAndDestinations)) != 0,
            arguments.humanizeRatio, arguments.seed);
        if (randomizerConfig.vca2Env != model::EnumRandomVCAEnv::Free)
        {
            xpanderTone().forceEnv2ModVca2AfterRandomizeMatrix(randomizerConfig.vca2Env);
        }

        tone().setToneName("RANDOM"); // [RQ-CTL-050]
        xpanderTone().setCurrentProgramNumber(xpanderTone().editingProgramNumber());
        updateUIAndSendFullToneToSynth(xpanderTone().editingProgramNumber());
        clearAllChangedFlags();
        setSetParameterEnabled(true);
        start();
    }

    void XpanderController::morphTones(const std::string& firstToneFilename,
                                       const std::string& secondToneFilename, float morphingFactor)
    {
        if (settings().midiConfig.smartAllNotesOff)
        {
            sendAllNotesOffToSynthOutput();
        }
        stop();
        clearClipboard();
        setSetParameterEnabled(false);
        try
        {
            model::XpanderToneReader reader;
            XpanderTone firstTone;
            XpanderTone secondTone;
            reader.readTone(firstToneFilename, firstTone);
            reader.readTone(secondToneFilename, secondTone);
            // Result starts as a copy of the first tone, as the reference.
            reader.readTone(firstToneFilename, tone());
            AbstractTone::morphTones(firstTone, secondTone, tone(), morphingFactor); // [RQ-CTL-051]
            tone().setToneName("MORPH-X");
            xpanderTone().setCurrentProgramNumber(xpanderTone().editingProgramNumber());
            updateUIAndSendFullToneToSynth(xpanderTone().editingProgramNumber());
        }
        catch (...)
        {
            clearAllChangedFlags();
            setSetParameterEnabled(true);
            start();
            throw;
        }
        clearAllChangedFlags();
        setSetParameterEnabled(true);
        start();
    }

    // --- modulation matrix delegations [RQ-CTL-030] ------------------------------

    void XpanderController::changeModulationSource(int newModulationSource, int modulationSourceAmount,
                                                   int modulationQuantize, int modulationDestination,
                                                   int entryNumber)
    {
        if (isSetParameterEnabled())
        {
            xpanderTone().changeModulationSource(newModulationSource, modulationSourceAmount,
                                                 modulationQuantize, modulationDestination, entryNumber,
                                                 [this](const midiapp::model::AbstractParameter& parameter)
                                                 { enqueueParameter(parameter.clone()); });
        }
    }

    void XpanderController::changeModulationSourceAmount(int modulationSource, int modulationSourceAmount,
                                                         int modulationDestination, int entryNumber)
    {
        if (isSetParameterEnabled())
        {
            xpanderTone().changeModulationSourceAmount(modulationSource, modulationSourceAmount,
                                                       modulationDestination, entryNumber,
                                                       [this](const midiapp::model::AbstractParameter& parameter)
                                                       { enqueueParameter(parameter.clone()); });
        }
    }

    void XpanderController::changeModulationSourceQuantize(int modulationSource, int modulationDestination,
                                                           int modulationQuantize, int entryNumber)
    {
        if (isSetParameterEnabled())
        {
            xpanderTone().changeModulationSourceQuantize(modulationSource, modulationDestination,
                                                         modulationQuantize, entryNumber,
                                                         [this](const midiapp::model::AbstractParameter& parameter)
                                                         { enqueueParameter(parameter.clone()); });
        }
    }

    void XpanderController::changeModulationDestination(int modulationSource, int modulationSourceAmount,
                                                        int modulationQuantize, int oldModulationDestination,
                                                        int newModulationDestination, int entryNumber)
    {
        if (isSetParameterEnabled())
        {
            xpanderTone().changeModulationDestination(modulationSource, modulationSourceAmount,
                                                      modulationQuantize, oldModulationDestination,
                                                      newModulationDestination, entryNumber,
                                                      [this](const midiapp::model::AbstractParameter& parameter)
                                                      { enqueueParameter(parameter.clone()); });
        }
    }

    bool XpanderController::isMaxSourceCountForDestinationReached(
        model::EnumModulationDestinations destination) const
    {
        return xpanderTone().isMaxSourceCountForDestinationReached(destination);
    }

    const model::ModulationMatrixEntry& XpanderController::getModulationEntryByNumber(int number) const
    {
        return xpanderTone().modulationMatrix().at(static_cast<std::size_t>(number - 1));
    }

    bool XpanderController::sourceAvailabilityForEntry(int entryNumber) const
    {
        const auto& entry = getModulationEntryByNumber(entryNumber);
        const bool maxReached = isMaxSourceCountForDestinationReached(entry.destination);
        return !maxReached || entry.source != model::EnumModulationSourcesModMatrix::NONE;
    }

    std::vector<model::EnumModulationDestinations>
    XpanderController::getAvailableModulationDestinationsForEntry(int entryNumber) const
    {
        std::vector<model::EnumModulationDestinations> destinations;
        const auto& entry = getModulationEntryByNumber(entryNumber);
        constexpr int destinationCount = static_cast<int>(model::EnumModulationDestinations::LAG_RATE) + 1;
        for (int i = 0; i < destinationCount; ++i)
        {
            const auto destination = static_cast<model::EnumModulationDestinations>(i);
            if (!isMaxSourceCountForDestinationReached(destination) || destination == entry.destination)
            {
                destinations.push_back(destination);
            }
        }
        return destinations;
    }

    // --- clipboard [RQ-CTL-040] ---------------------------------------------------

    bool XpanderController::canClipboardPasteTo(const std::string& destination) const
    {
        constexpr std::size_t COMPARE_LENGTH = 4;
        if (_clipboardSource.empty() || destination.size() < COMPARE_LENGTH)
        {
            return false;
        }
        return _clipboardSource.compare(0, COMPARE_LENGTH, destination, 0, COMPARE_LENGTH) == 0
               && destination != _clipboardSource;
    }

    void XpanderController::pasteClipboardTo(const std::string& destination)
    {
        if (!canClipboardPasteTo(destination))
        {
            return;
        }
        std::size_t numberPosition = 0;
        if (destination.starts_with("ENV_") || destination.starts_with("LFO_"))
        {
            numberPosition = 4;
        }
        else if (destination.starts_with("TRACK_"))
        {
            numberPosition = 6;
        }
        else if (destination.starts_with("RAMP_"))
        {
            numberPosition = 5;
        }
        else
        {
            return;
        }
        const std::string sourceNumber = _clipboardSource.substr(numberPosition, 1);

        for (const auto& entry : tone().parameterMap())
        {
            if (!entry.name.starts_with(destination))
            {
                continue;
            }
            const auto sourceParameterName = entry.name.substr(0, numberPosition) + sourceNumber
                                             + entry.name.substr(numberPosition + 1);
            const auto& sourceParameter = tone().parameterMap().at(sourceParameterName);
            entry.parameter->setValue(sourceParameter.value());
            notifyAutomationParameterChangeEvent(entry.name, entry.parameter->value());
        }
    }

    // --- synth utilities ------------------------------------------------------------

    void XpanderController::sendAllNotesOffToSynthOutput()
    {
        if (verifySynthOutputDevice())
        {
            constexpr int ALL_NOTES_OFF = 123;
            // Reference sends on the *configured* channel (settings), not the
            // tone's channel — preserved. [RQ-CTL-060]
            sendDataToSynthOutputDevice(MidiMessage::channelMessage(
                ChannelCommand::Controller, settings().midiConfig.midiChannel, ALL_NOTES_OFF, 0));
        }
    }

    void XpanderController::sendTuneRequestToSynth()
    {
        if (verifySynthOutputDevice())
        {
            // Reference wraps Tune Request (0xF6) inside a SysEx frame —
            // preserved byte-for-byte. [RQ-CTL-060]
            const std::vector<std::uint8_t> tuneRequest{xpl::midi::SYSEX_START, 0xF6, xpl::midi::SYSEX_END};
            sendDataToSynthOutputDevice(MidiMessage::fromRawBytes(tuneRequest));
        }
    }

    void XpanderController::sendPageUpdate(const std::string& pageName)
    {
        // Reference parses the page name and force-sends unless it is the
        // (dummy) CASSETTE fallback; only the side effect matters here.
        if (!pageName.empty() && pageName != "CASSETTE")
        {
            forceSendPageSubPage();
        }
    }

    void XpanderController::forceSendPageSubPage()
    {
        int page = 0;
        int subPage = 0;
        _pageSubPageHelper.getPageSubPage(page, subPage);
        sendPageSubPageAndUpdatePageSubPage(page, subPage); // [RQ-CTL-020]
    }

    void XpanderController::sendGreetingsToSynth()
    {
        constexpr int PADDING_LENGTH = MAX_DISPLAY_MESSAGE_LENGTH / 2;
        if (!verifySynthOutputDevice())
        {
            return;
        }
        auto padRight = [](std::string text, std::size_t width)
        {
            text.resize(std::max(text.size(), width), ' ');
            return text;
        };
        std::string line1 = _productNameAndVersion;
        std::transform(line1.begin(), line1.end(), line1.begin(),
            [](char c) { return static_cast<char>(::toupper(static_cast<unsigned char>(c))); });
        line1 = padRight(line1, PADDING_LENGTH);
        const auto line2 = padRight("GITHUB.COM/XPLORER2716/XPLOREREDITOR", PADDING_LENGTH);
        sendDisplayOffOnToSynth();
        sendDisplayMessageToSynth(line1 + line2); // [RQ-CTL-061]
    }

    void XpanderController::sendTypeWriterMessageToSynth(const std::string& message)
    {
        constexpr int SCROLL_DELAY = 50;
        if (!verifySynthOutputDevice())
        {
            return;
        }
        sendDisplayOffOnToSynth();
        for (std::size_t i = 0; i <= message.size(); ++i)
        {
            std::string scrollingMessage = message.substr(0, i);
            scrollingMessage.resize(MAX_DISPLAY_MESSAGE_LENGTH, ' ');
            sendDisplayMessageToSynth(scrollingMessage);
            sleepMs(SCROLL_DELAY); // [RQ-CTL-061]
        }
    }

    void XpanderController::sendDisplayOffOnToSynth()
    {
        const std::vector<std::uint8_t> displayOff{0xF0, 0x10, 0x02, displayControlCommand(), 0x00, xpl::midi::SYSEX_END};
        const std::vector<std::uint8_t> displayOn{0xF0, 0x10, 0x02, displayControlCommand(), 0x02, xpl::midi::SYSEX_END};
        sendDataToSynthOutputDevice(MidiMessage::sysEx(displayOff));
        sleepMs(parameterTransmitDelay());
        sendDataToSynthOutputDevice(MidiMessage::sysEx(displayOn));
        sleepMs(parameterTransmitDelay());
    }

    void XpanderController::sendDisplayMessageToSynth(const std::string& message)
    {
        constexpr int DISPLAY_INTRO_LENGTH = 5;
        std::vector<std::uint8_t> displayMessage(DISPLAY_INTRO_LENGTH + MAX_DISPLAY_MESSAGE_LENGTH + 1, ' ');
        displayMessage[0] = xpl::midi::SYSEX_START;
        displayMessage[1] = 0x10;
        displayMessage[2] = 0x02;
        displayMessage[3] = displayControlCommand();
        displayMessage[4] = 0x01;
        for (std::size_t i = 0; i < message.size() && i < MAX_DISPLAY_MESSAGE_LENGTH; ++i)
        {
            displayMessage[DISPLAY_INTRO_LENGTH + i] =
                static_cast<std::uint8_t>(::toupper(static_cast<unsigned char>(message[i])));
        }
        displayMessage.back() = xpl::midi::SYSEX_END;
        sendDataToSynthOutputDevice(MidiMessage::sysEx(displayMessage));
        sleepMs(parameterTransmitDelay());
    }

    // --- send helpers ------------------------------------------------------------------

    void XpanderController::sendDataToSynthOutputDevice(const MidiMessage& message)
    {
        sendToSynthOutput(message);
        notifyMidiDataSendReceiveEvent(EnumMidiDevice::SynthOutputDevice); // [RQ-CTL-027]
    }

    void XpanderController::sendAllDataDumpRequestToSynth()
    {
        const std::vector<std::uint8_t> allDataDumpRequest{
            xpl::midi::SYSEX_START, 0x10, 0x02, 0x02,
            settings().midiConfig.synthTypeIsMatrix12 ? std::uint8_t{0x01} : std::uint8_t{0x00},
            xpl::midi::SYSEX_END};
        sendDataToSynthOutputDevice(MidiMessage::sysEx(allDataDumpRequest)); // [RQ-CTL-062]
    }

    void XpanderController::sendProgramChangeToSynthOutput(int programNumber)
    {
        if (verifySynthOutputDevice())
        {
            sendDataToSynthOutputDevice(MidiMessage::channelMessage(
                ChannelCommand::ProgramChange, tone().midiChannel(), programNumber));
            sleepMs(DELAY_BETWEEN_MESSAGES);
            sendPageSubPageAndUpdatePageSubPage(static_cast<int>(EnumPages::VCO_1_X), 0x00);
        }
    }

    void XpanderController::sendProgramDumpRequestToSynth(int programNumber)
    {
        if (verifySynthOutputDevice())
        {
            const std::vector<std::uint8_t> programDumpRequest{
                xpl::midi::SYSEX_START, 0x10, 0x02, 0x00, 0x00,
                static_cast<std::uint8_t>(programNumber), xpl::midi::SYSEX_END};
            sendDataToSynthOutputDevice(MidiMessage::sysEx(programDumpRequest));
        }
    }

    void XpanderController::sendPageSubPageAndUpdatePageSubPage(int page, int subPage)
    {
        const std::vector<std::uint8_t> pageSelectMessage{
            0xF0, 0x10, 0x02, 0x0B, static_cast<std::uint8_t>(page),
            static_cast<std::uint8_t>(subPage), xpl::midi::SYSEX_END};
        if (verifySynthOutputDevice())
        {
            sendDataToSynthOutputDevice(MidiMessage::sysEx(pageSelectMessage));
        }
        _pageSubPageHelper.updatePageSubPage(page, subPage);
    }

    void XpanderController::sendFullToneToSynthIntoProgram(int programNumber)
    {
        if (verifySynthOutputDevice())
        {
            sendDataToSynthOutputDevice(MidiMessage::sysEx(xpanderTone().toByteArray()));
            sleepMs(parameterTransmitDelay());
            sendProgramChangeToSynthOutput(programNumber);
            xpanderTone().setCurrentProgramNumber(programNumber);
        }
    }

    void XpanderController::updateUIAndSendFullToneToSynth(int programNumber)
    {
        sendFullToneToSynthIntoProgram(programNumber);
        notifyFullToneChangeEvent();
    }

    void XpanderController::clearAllChangedFlags()
    {
        for (const auto& entry : tone().parameterMap())
        {
            entry.parameter->setChanged(false);
        }
    }

    // --- transmit worker [RQ-CTL-020, ADR-005] ---------------------------------------------

    void XpanderController::workerThreadProc(std::stop_token stopToken)
    {
        while (waitForTransmitDelay(stopToken))
        {
            scanChangedParametersIntoQueue();

            std::unique_ptr<midiapp::model::AbstractParameter> parameterToSend;
            if (!dequeueParameter(parameterToSend) || !verifySynthOutputDevice())
            {
                continue;
            }
            auto* xpanderParameter = dynamic_cast<XpanderParameter*>(parameterToSend.get());
            if (xpanderParameter != nullptr)
            {
                int lastPage = 0;
                int lastSubPage = 0;
                _pageSubPageHelper.getPageSubPage(lastPage, lastSubPage);
                if (lastPage != xpanderParameter->page() || lastSubPage != xpanderParameter->subPage())
                {
                    sendDataToSynthOutputDevice(xpanderParameter->pageSelectMessage());
                    _pageSubPageHelper.updatePageSubPage(xpanderParameter->page(),
                                                         xpanderParameter->subPage());
                    if (!waitForTransmitDelay(stopToken))
                    {
                        break;
                    }
                }
            }
            sendDataToSynthOutputDevice(parameterToSend->message());
        }
    }

    // --- event notification ------------------------------------------------------------

    void XpanderController::setFullToneChangeHandler(std::function<void(const FullToneChangeEvent&)> handler)
    {
        _fullToneChangeHandler = std::move(handler);
    }

    void XpanderController::setPageChangeHandler(std::function<void(const PageChangeEvent&)> handler)
    {
        _pageChangeHandler = std::move(handler);
    }

    void XpanderController::setModulationEntryChangeHandler(
        std::function<void(const ModulationEntryChangeEvent&)> handler)
    {
        _modulationEntryChangeHandler = std::move(handler);
    }

    void XpanderController::setAllDataDumpProgressionHandler(
        std::function<void(const AllDataDumpProgressionEvent&)> handler)
    {
        _allDataDumpProgressionHandler = std::move(handler);
    }

    void XpanderController::setMidiActivityHandler(std::function<void(EnumMidiDevice)> handler)
    {
        _midiActivityHandler = std::move(handler);
    }

    void XpanderController::notifyFullToneChangeEvent()
    {
        if (!_fullToneChangeHandler)
        {
            return;
        }
        FullToneChangeEvent event;
        for (const auto& entry : tone().parameterMap())
        {
            event.parameterMap.emplace(entry.name, entry.parameter->value());
        }
        event.modulationMatrix = xpanderTone().modulationMatrix();
        postEvent([handler = _fullToneChangeHandler, event = std::move(event)] { handler(event); });
    }

    void XpanderController::notifyPageChangeEvent(int page, int subPage)
    {
        if (_pageChangeHandler)
        {
            const PageChangeEvent event{static_cast<EnumPages>(page), subPage};
            postEvent([handler = _pageChangeHandler, event] { handler(event); });
        }
    }

    void XpanderController::notifyModulationEntryChangeEvent(const model::ModulationMatrixEntry& entry,
                                                             int entryNumber,
                                                             EnumModulationParameter parameter)
    {
        if (_modulationEntryChangeHandler)
        {
            const ModulationEntryChangeEvent event{entry, entryNumber, parameter};
            postEvent([handler = _modulationEntryChangeHandler, event] { handler(event); });
        }
    }

    void XpanderController::notifyAllDataDumpRequestProgressionEvent()
    {
        if (_allDataDumpProgressionHandler)
        {
            const AllDataDumpProgressionEvent event{
                _allDataDumpRequestState.isWaitingForAllDataDumpRequest(),
                static_cast<int>(_allDataDumpRequestState.singlePatches().size()),
                static_cast<int>(_allDataDumpRequestState.multiPatches().size())};
            postEvent([handler = _allDataDumpProgressionHandler, event] { handler(event); });
        }
    }

    void XpanderController::notifyMidiDataSendReceiveEvent(EnumMidiDevice device)
    {
        if (_midiActivityHandler)
        {
            postEvent([handler = _midiActivityHandler, device] { handler(device); });
        }
    }
}
