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
#include <cctype>

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
        const auto& entry = getModulationEntryByNumber(entryEntryNumber);
