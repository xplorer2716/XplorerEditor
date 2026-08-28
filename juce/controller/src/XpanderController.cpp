// XpanderController core: lifecycle, patch operations, randomize/morph,
// clipboard, synth utilities, display messages, transmit worker.
// Port of XpanderController.cs / .WorkerThread.cs / .Clipboard.cs /
// .ModulationMatrix.cs / Events.
//
// TWO RECURRING SHAPES IN THIS FILE, worth knowing before reading any single
// method, because almost every non-trivial operation uses one or both:
//
//  1. stop() / ... / start() brackets a bulk mutation of the tone.
//     stop() halts the transmit worker, so the scan loop cannot pick up the
//     dozens of parameters the mutation is about to touch and dribble them out
//     one frame at a time. The full tone is sent in one piece instead, and the
//     trailing start() resumes normal per-parameter transmission. Removing a
//     bracket does not break compilation -- it floods the synth.
//
//  2. setSetParameterEnabled(false) / ... / setSetParameterEnabled(true)
//     suppresses the UI-edit path while the tone is being written from an
//     external source (a file, the synth, the randomizer). Without it, every
//     value written would look like a user edit and be echoed straight back to
//     the instrument that just sent it.
//
// Ordering inside those brackets is load-bearing and reference-derived: the
// full-tone send happens BEFORE clearAllChangedFlags(), never after. Clearing
// first would mark the tone unchanged while the frames were still queued.
//
// THREAD AFFINITY: nothing here is thread-safe by itself. Menu-driven
// operations run on the JUCE message thread; restoreAllDataDumpToSynth runs on
// a ThreadWithProgressWindow worker; workerThreadProc runs on the transmit
// thread. The sleeps below are therefore reachable from the message thread and
// will freeze the UI for their duration -- see the note on
// storeSinglePatchToSynth.
#include "xplorer/controller/XpanderController.hpp"

#include "midiapp/service/FileUtils.hpp"
#include "midiapp/service/Logger.hpp"
#include "xpl/midi/SysexStreamIterator.hpp"
#include "xpl/util/EnumUtils.hpp"

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
        // Blocks the CALLING thread, which is the message thread for anything
        // reached from a menu. These delays are the synth's own pacing
        // requirement, not a convenience: the Xpander drops SysEx it receives
        // faster than it can parse. See the file header on thread affinity.
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

    const settings::AllUsersSettings& XpanderController::settings() const
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

    void XpanderController::loadXplorerTone(const std::string& filename)
    {
        model::XpanderToneReader reader;
        loadTone(filename, reader);
        clearClipboard();
    }

    // Reads a tone from disk into the live editing buffer and pushes it to the
    // synth. Both bracket shapes from the file header apply here, and the
    // sequence is the reference's: all-notes-off (if enabled) so nothing hangs
    // from the outgoing patch, worker stopped, UI-edit path suppressed, tone
    // replaced, full tone transmitted, THEN the changed flags cleared, then
    // both brackets released. [RQ-CTL-001]
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

    void XpanderController::saveXplorerTone(const std::string& filename)
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
            throw ToneException("File or directory does not exist."); // [RQ-BUG-006]
        }
        model::XpanderToneReader reader;
        auto tones = reader.readTones(bankFilename);
        model::XpanderToneWriter writer;
        for (const auto& [name, extractedTone] : tones)
        {
            // Trailing spaces trimmed first: readTones() returns the tone's
            // fixed-width, space-padded storage name, same padding the
            // synth-reception path (handleAllDataDumpRequest) already trims
            // via the same shared helper. [RQ-GUI-077]
            const auto filename = midiapp::service::makeUniqueFilenameFromString(
                midiapp::service::trimTrailingSpaces(name), midiapp::service::SYSEX_FILE_EXTENSION_WITH_DOT,
                directoryName);
            writer.writeTone((std::filesystem::path(directoryName) / filename).string(), *extractedTone);
        }
        return tones; // [RQ-CTL-003]
    }

    // Asks the synth for its entire memory and writes it to one file. The reply
    // does not arrive here: it lands asynchronously in the SysEx handler, which
    // accumulates frames into _allDataDumpRequestState until the dump is
    // complete. That is why this method returns immediately after sending the
    // request, and why a second call while one is in flight is refused rather
    // than queued -- the state machine holds exactly one dump.
    //
    // The bare stop()/start() pair below is not a no-op: it restarts the
    // transmit worker, which clears its pending queue, so no leftover parameter
    // frame interleaves with the dump exchange. [RQ-CTL-005]
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

    // Same all-data-dump request as backupAllDataDumpToFile, but the state
    // machine is told to split the reply into one .syx file per patch instead
    // of writing a single bank file. The only difference is the Mode passed to
    // initialize(). [RQ-CTL-004]
    void XpanderController::getSingleTonesFromSynth(const std::string& destinationFolder)
    {
        if (!verifySynthOutputDevice())
        {
            throw ToneException("Unable to open MIDI device "
                                + settings().midiConfig.synthOutputDeviceName);
        }
        if (!std::filesystem::is_directory(destinationFolder))
        {
            throw ToneException("Destination folder " + destinationFolder + " does not exist."); // [RQ-BUG-006]
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

    // Streams a previously backed-up bank file to the synth, frame by frame,
    // pacing each with DELAY_BETWEEN_ALL_DATA_DUMP_SEND_SINGLE_PATCH.
    //
    // This BLOCKS for the whole transfer -- hundreds of frames, so tens of
    // seconds -- and must therefore never be called on the message thread. The
    // UI runs it on a ThreadWithProgressWindow and receives `progressionAction`
    // callbacks to drive the progress bar. [RQ-CTL-005, RQ-GUI-026]
    //
    // The trailing resync matters: the synth has just had its memory rewritten
    // underneath the editor, so the editing buffer is stale until the current
    // patch is re-read.
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

    // Writes the edited patch into one of the synth's 100 memory slots -- the
    // only operation here that changes the instrument permanently.
    //
    // The clone is not defensive copying for its own sake: toByteArray() stamps
    // the tone's EDITING program number into the dump, so the destination slot
    // has to be set on a copy. Setting it on the live tone would move the
    // user's editing target as a side effect of saving.
    //
    // Blocks on the message thread for DELAY_BETWEEN_MESSAGES plus the resync
    // that follows (see the file header). [RQ-CTL-006]
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

    // THE synchronization primitive: select a patch on the synth, then ask it
    // to send that patch back. Every caller that needs the editor and the
    // instrument to agree goes through here -- Patch > Synchronize, Goto,
    // Store, previous/next, the first start() of the session, and the resync
    // after accepted MIDI settings.
    //
    // The reply is asynchronous: it arrives later in the SysEx handler, which
    // reloads the tone wholesale. This method only ASKS. The
    // setCurrentProgramNumber below therefore records intent, not a confirmed
    // state -- the tone is not yet the synth's until that reply lands.
    //
    // clearClipboard() is reference behaviour: a copied modulation entry
    // belongs to the patch it was taken from, so changing patch invalidates it.
    // [RQ-CTL-006]
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
        const auto vco2Flags = xpl::util::toUnderlying(randomizerConfig.vco2FmNoiseSync);
        if ((vco2Flags & xpl::util::toUnderlying(model::EnumRandomVCO2::EnableFM)) == 0)
        {
            excluded.insert("FM_AMP");
            excluded.insert("FM_DESTINATION");
        }
        if ((vco2Flags & xpl::util::toUnderlying(model::EnumRandomVCO2::EnableNoise)) == 0)
        {
            excluded.insert("VCO2_WAVESHAPE_NOISE");
        }
        if ((vco2Flags & xpl::util::toUnderlying(model::EnumRandomVCO2::EnableSync)) == 0)
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

        const auto matrixFlags = xpl::util::toUnderlying(randomizerConfig.modulationMatrix);
        xpanderTone().randomizeModulationMatrix(
            (matrixFlags & xpl::util::toUnderlying(model::EnumRandomModMatrix::EnableAmount)) != 0,
            (matrixFlags & xpl::util::toUnderlying(model::EnumRandomModMatrix::EnableQuantize)) != 0,
            (matrixFlags & xpl::util::toUnderlying(model::EnumRandomModMatrix::EnableSourcesAndDestinations)) != 0,
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
        constexpr int destinationCount = xpl::util::toUnderlying(model::EnumModulationDestinations::LAG_RATE) + 1;
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

    void XpanderController::sendPageUpdate(int page, int subPage)
    {
        // Unlike forceSendPageSubPage() (which re-sends whatever page/sub-page
        // is already tracked), this sends AND tracks the given page, so a
        // direct UI page selection is not lost behind a stale cached page. [RQ-CTL-028]
        sendPageSubPageAndUpdatePageSubPage(page, subPage);
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
        // padRight only grows a string, never shrinks it: a name+version that
        // does not fit the line must be shortened first, or it silently
        // overruns onto the fixed-width line2 that follows it. Dropping the
        // "-<stream>" suffix (canary/preprod/...) is the one part of the
        // string that is not the product name or the semantic version.
        if (line1.size() > static_cast<std::size_t>(PADDING_LENGTH))
        {
            const auto streamSuffix = line1.find('-');
            if (streamSuffix != std::string::npos)
            {
                line1 = line1.substr(0, streamSuffix);
            }
        }
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
            sendPageSubPageAndUpdatePageSubPage(xpl::util::toUnderlying(EnumPages::VCO_1_X), 0x00);
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

    // --- transmit worker [RQ-CTL-020, ADR-JUC-005] ---------------------------------------------

    // The transmit loop, and the only place in the application that sends a
    // parameter edit to the synth. Runs on its own std::jthread; every wait is
    // bound to `stopToken`, so stop() interrupts it immediately instead of
    // waiting out the current delay. [RQ-CTL-020, ADR-JUC-005]
    //
    // ONE parameter per tick, deliberately. The delay between ticks is the
    // user's configured SysEx pacing, and the Xpander needs it: sending a
    // burst at wire speed makes it drop frames or hang.
    //
    // The page-select branch is the Xpander's addressing model, not an
    // optimisation. A parameter frame carries only a parameter id, valid
    // within whatever page the synth is currently showing, so the page must be
    // selected first whenever it differs from the last one sent.
    // _pageSubPageHelper is what remembers that, which is why a page select is
    // skipped when consecutive parameters share a page -- and why a stale
    // helper would silently write parameters into the wrong page.
    //
    // The second waitForTransmitDelay() after a page select is not redundant:
    // the synth needs the same pacing between the page select and the
    // parameter frame as between any two frames.
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
