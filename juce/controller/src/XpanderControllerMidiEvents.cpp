// XpanderController: MIDI event handling (bidirectional sync).
// Port of XpanderController.MIDIEvents.cs. [RQ-CTL-021..027]
#include "xplorer/controller/XpanderController.hpp"

#include "midiapp/service/FileUtils.hpp"
#include "midiapp/service/Logger.hpp"
#include "xplorer/model/XpanderSinglePatch.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>

namespace xplorer::controller
{
    using midiapp::service::Logger;
    using midiapp::service::TraceLevel;
    using model::EnumModulationEditCommands;
    using model::EnumModulationSourcesModMatrix;
    using model::EnumPages;
    using model::EnumRotaryEncoders;
    using model::ModulationMatrixEntry;
    using model::XpanderParameter;
    using model::XpanderTone;
    using xpl::midi::ChannelCommand;
    using xpl::midi::MidiMessage;

    namespace
    {
        bool startsWith(const MidiMessage& message, std::initializer_list<std::uint8_t> intro)
        {
            if (message.size() < intro.size())
            {
                return false;
            }
            std::size_t i = 0;
            for (const auto byte : intro)
            {
                if (message[i++] != byte)
                {
                    return false;
                }
            }
            return true;
        }

        void writeAllBytes(const std::filesystem::path& path, std::span<const std::uint8_t> bytes)
        {
            std::ofstream stream(path, std::ios::binary | std::ios::trunc);
            stream.write(reinterpret_cast<const char*>(bytes.data()),
                         static_cast<std::streamsize>(bytes.size()));
        }
    }

    // --- automation input overrides ------------------------------------------

    void XpanderController::automationInputDeviceSysExMessageReceived(const MidiMessage&)
    {
        // Reference override: notify activity only, no redirect to the synth.
        notifyMidiDataSendReceiveEvent(EnumMidiDevice::AutomationInputDevice);
    }

    void XpanderController::automationInputDeviceChannelMessageReceived(const MidiMessage& message)
    {
        if (message.command() == ChannelCommand::ProgramChange)
        {
            // Reference addition: an automation Program Change in range steers
            // the edited program. [RQ-CTL-024-adjacent behavior]
            if (message.data1() >= XpanderTone::MIN_PROGRAM_NUMBER
                && message.data1() <= XpanderTone::MAX_PROGRAM_NUMBER)
            {
                xpanderTone().setCurrentProgramNumber(message.data1());
                sendProgramChangeAndGetSinglePatchFromSynth(xpanderTone().currentProgramNumber());
            }
        }
        else
        {
            // CC mapping/forwarding behavior is the framework's. [RQ-FMW-050..051]
            AbstractController::automationInputDeviceChannelMessageReceived(message);
        }
        notifyMidiDataSendReceiveEvent(EnumMidiDevice::AutomationInputDevice); // [RQ-CTL-027]
    }

    // --- synth input overrides --------------------------------------------------

    void XpanderController::synthInputDeviceSysRealtimeMessageReceived(const MidiMessage&)
    {
        notifyMidiDataSendReceiveEvent(EnumMidiDevice::SynthInputDevice);
    }

    void XpanderController::synthInputDeviceSysCommonMessageReceived(const MidiMessage&)
    {
        notifyMidiDataSendReceiveEvent(EnumMidiDevice::SynthInputDevice);
    }

    void XpanderController::synthInputDeviceChannelMessageReceived(const MidiMessage& message)
    {
        if (message.command() == ChannelCommand::ProgramChange)
        {
            if (settings().midiConfig.smartAllNotesOff)
            {
                sendAllNotesOffToSynthOutput();
            }
            xpanderTone().setCurrentProgramNumber(message.data1());
            sendProgramDumpRequestToSynth(message.data1()); // [RQ-CTL-024]
        }
        notifyMidiDataSendReceiveEvent(EnumMidiDevice::SynthInputDevice);
    }

    void XpanderController::synthInputDeviceSysExMessageReceived(const MidiMessage& message)
    {
        int page = static_cast<int>(EnumPages::UNKNOWN);
        int subPage = static_cast<int>(EnumPages::UNKNOWN);
        int buttonId = 0;
        int parameterValue = 0;
        bool isRotaryButton = false;
        auto modulationEditCommand = EnumModulationEditCommands::UNKNOWN;
        int modulationEditValue = 0;
        int modulationSourceNumber = 0;
        bool isMessageHandled = false;

        if (isSinglePatchProgramDumpSysex(message))
        {
            isMessageHandled = true;
            Logger::writeLine("XpanderController", TraceLevel::Info,
                              "RECV: SingleVoiceProgramDump " + message.toString());
            if (!handleAllDataDumpRequest(message, true))
            {
                // Live reload of the edited tone from the synth. [RQ-CTL-021]
                stop();
                setSetParameterEnabled(false);
                xpanderTone().fromByteArray(message.bytes());
                clearAllChangedFlags();
                notifyFullToneChangeEvent();
                setSetParameterEnabled(true);
                start();
            }
        }
        else if (isMultiPatchProgramDumpSysex(message))
        {
            Logger::writeLine("XpanderController", TraceLevel::Info,
                              "RECV: MultiPatchProgramDumpSysex " + message.toString());
            // Outside a dump request, multi patches are ignored. [RQ-CTL-026]
            isMessageHandled = handleAllDataDumpRequest(message, false);
        }
        else if (isProgramChangeDownSysex(message))
        {
            isMessageHandled = true;
            xpanderTone().setCurrentProgramNumber(xpanderTone().currentProgramNumber() - 1); // [RQ-CTL-024]
            if (settings().midiConfig.smartAllNotesOff)
            {
                sendAllNotesOffToSynthOutput();
            }
            sendProgramDumpRequestToSynth(xpanderTone().currentProgramNumber());
        }
        else if (isProgramChangeUpSysex(message))
        {
            isMessageHandled = true;
            xpanderTone().setCurrentProgramNumber(xpanderTone().currentProgramNumber() + 1);
            if (settings().midiConfig.smartAllNotesOff)
            {
                sendAllNotesOffToSynthOutput();
            }
            sendProgramDumpRequestToSynth(xpanderTone().currentProgramNumber());
        }
        else if (isPageSubPageSelectSysex(message, page, subPage))
        {
            isMessageHandled = true;
            _pageSubPageHelper.updatePageSubPage(page, subPage);
            if (_pageSubPageHelper.isPageEnvLfoRampTrack())
            {
                notifyPageChangeEvent(page, subPage); // [RQ-CTL-023]
            }
        }
        else if (isPageEditFollowsSysex(message, buttonId, parameterValue, isRotaryButton))
        {
            isMessageHandled = true;
            setSetParameterEnabled(false);
            int parameterPage = 0;
            int parameterSubPage = 0;
            _pageSubPageHelper.getPageSubPage(parameterPage, parameterSubPage);
            auto* parameter = getParameterForPageSubPageAndId(parameterPage, parameterSubPage, buttonId);
            if (parameter != nullptr)
            {
                // [RQ-CTL-022] rotaries are relative, buttons absolute.
                parameter->setValueUnchanged(isRotaryButton ? parameter->value() + parameterValue
                                                            : parameterValue);
                notifyAutomationParameterChangeEvent(parameter->name(), parameter->value());
            }
            setSetParameterEnabled(true);
        }
        else if (isModulationEditFollowsSysex(message, modulationSourceNumber, modulationEditCommand,
                                              modulationEditValue))
        {
            isMessageHandled = true;
            setSetParameterEnabled(false);
            handleModulationEditFromSynth(modulationSourceNumber, modulationEditCommand,
                                          modulationEditValue); // [RQ-CTL-025]
            setSetParameterEnabled(true);
        }

        if (isMessageHandled)
        {
            notifyMidiDataSendReceiveEvent(EnumMidiDevice::SynthInputDevice);
        }
    }

    // --- all-data dump reception [RQ-CTL-004, RQ-CTL-005] --------------------------

    bool XpanderController::handleAllDataDumpRequest(const MidiMessage& message, bool isSinglePatchDataDump)
    {
        if (!_allDataDumpRequestState.isWaitingForAllDataDumpRequest())
        {
            return false;
        }
        const auto data = message.toBytes();
        if (isSinglePatchDataDump)
        {
            _allDataDumpRequestState.singlePatches().emplace_back(XpanderTone::getNameFromByteArray(data), data);
            if (_allDataDumpRequestState.receptionMode() == AllDataDumpRequestState::Mode::SinglePatch
                && _allDataDumpRequestState.singlePatches().size()
                       >= static_cast<std::size_t>(model::constants::SINGLE_TONES_MAX_COUNT))
            {
                _allDataDumpRequestState.setWaitingForAllDataDumpRequest(false);
                for (const auto& [name, bytes] : _allDataDumpRequestState.singlePatches())
                {
                    const auto filename = midiapp::service::makeUniqueFilenameFromString(
                        name, midiapp::service::SYSEX_FILE_EXTENSION_WITH_DOT,
                        _allDataDumpRequestState.destination());
                    writeAllBytes(std::filesystem::path(_allDataDumpRequestState.destination()) / filename,
                                  bytes);
                }
            }
            notifyAllDataDumpRequestProgressionEvent();
        }
        else if (_allDataDumpRequestState.receptionMode() == AllDataDumpRequestState::Mode::All)
        {
            _allDataDumpRequestState.multiPatches().push_back(data);
            if (_allDataDumpRequestState.multiPatches().size()
                >= static_cast<std::size_t>(model::constants::MULTI_PATCHES_MAX_COUNT))
            {
                _allDataDumpRequestState.setWaitingForAllDataDumpRequest(false);
                std::ofstream stream(_allDataDumpRequestState.destination(),
                                     std::ios::binary | std::ios::trunc);
                for (const auto& [name, bytes] : _allDataDumpRequestState.singlePatches())
                {
                    stream.write(reinterpret_cast<const char*>(bytes.data()),
                                 static_cast<std::streamsize>(bytes.size()));
                }
                for (const auto& bytes : _allDataDumpRequestState.multiPatches())
                {
                    stream.write(reinterpret_cast<const char*>(bytes.data()),
                                 static_cast<std::streamsize>(bytes.size()));
                }
            }
            notifyAllDataDumpRequestProgressionEvent();
        }
        return true;
    }

    // --- modulation edit follows [RQ-CTL-025] ------------------------------------------

    void XpanderController::handleModulationEditFromSynth(int modulationSourceNumber,
                                                          EnumModulationEditCommands command, int value)
    {
        int page = 0;
        int subPage = 0;
        _pageSubPageHelper.getPageSubPage(page, subPage);
        auto& xTone = xpanderTone();
        const auto destination = model::modulationDestinationForPageSubPage(page, subPage);

        if (command == EnumModulationEditCommands::ADDSOURCE)
        {
            const int entryNumber = xTone.getNextAvailableModEntry();
            if (entryNumber != XpanderTone::NO_AVAILABLE_MOD_ENTRY)
            {
                xTone.addModulationSource(value, 0, ModulationMatrixEntry::MIN_QUANTIZE,
                                          static_cast<int>(destination), entryNumber + 1, nullptr);
                notifyModulationEntryChangeEvent(xTone.modulationMatrix()[static_cast<std::size_t>(entryNumber)],
                                                 entryNumber + 1, EnumModulationParameter::ALL);
            }
            return;
        }

        int entryNumber = 0;
        for (const auto& entry : xTone.modulationMatrix())
        {
            if (entry.destination == destination && entry.idSource == modulationSourceNumber)
            {
                switch (command)
                {
                    case EnumModulationEditCommands::CHANGESOURCE:
                        xTone.changeModulationSource(value, entry.amount(), entry.quantize(),
                                                     static_cast<int>(entry.destination), entryNumber + 1,
                                                     nullptr);
                        notifyModulationEntryChangeEvent(entry, entryNumber + 1,
                                                         EnumModulationParameter::MODULATIONSOURCE);
                        break;
                    case EnumModulationEditCommands::DELETESOURCE:
                        // setting NONE resets the entry
                        xTone.changeModulationSource(static_cast<int>(EnumModulationSourcesModMatrix::NONE),
                                                     entry.amount(), entry.quantize(),
                                                     static_cast<int>(entry.destination), entryNumber + 1,
                                                     nullptr);
                        notifyModulationEntryChangeEvent(entry, entryNumber + 1,
                                                         EnumModulationParameter::MODULATIONSOURCE);
                        break;
                    case EnumModulationEditCommands::DIALVALUEAMOUNTOFCHANGE:
                        xTone.changeModulationSourceAmount(static_cast<int>(entry.source),
                                                           entry.amount() + value,
                                                           static_cast<int>(entry.destination),
                                                           entryNumber + 1, nullptr);
                        notifyModulationEntryChangeEvent(entry, entryNumber + 1,
                                                         EnumModulationParameter::MODULATIONAMOUNT);
                        break;
                    case EnumModulationEditCommands::SETQUANTIZE:
                        xTone.changeModulationSourceQuantize(static_cast<int>(entry.source),
                                                             static_cast<int>(entry.destination), value,
                                                             entryNumber + 1, nullptr);
                        notifyModulationEntryChangeEvent(entry, entryNumber + 1,
                                                         EnumModulationParameter::MODULATIONQUANTIZE);
                        break;
                    case EnumModulationEditCommands::SETSIGN:
                    {
                        const int valueSign = value == 1 ? -1 : 1;
                        const int amountSign = entry.amount() < 0 ? -1 : 1;
                        if (amountSign != valueSign)
                        {
                            xTone.changeModulationSourceAmount(static_cast<int>(entry.source),
                                                               entry.amount() * -1,
                                                               static_cast<int>(entry.destination),
                                                               entryNumber + 1, nullptr);
                            notifyModulationEntryChangeEvent(entry, entryNumber + 1,
                                                             EnumModulationParameter::MODULATIONAMOUNT);
                        }
                        break;
                    }
                    case EnumModulationEditCommands::SETUNSIGNEDVALUE:
                    {
                        const int amountSign = entry.amount() < 0 ? -1 : 1;
                        xTone.changeModulationSourceAmount(static_cast<int>(entry.source),
                                                           value * amountSign,
                                                           static_cast<int>(entry.destination),
                                                           entryNumber + 1, nullptr);
                        notifyModulationEntryChangeEvent(entry, entryNumber + 1,
                                                         EnumModulationParameter::MODULATIONAMOUNT);
                        break;
                    }
                    case EnumModulationEditCommands::TOGGLEQUANTIZE:
                    {
                        const int toggle = entry.quantize() == 1 ? 0 : 1;
                        xTone.changeModulationSourceQuantize(static_cast<int>(entry.source),
                                                             static_cast<int>(entry.destination), toggle,
                                                             entryNumber + 1, nullptr);
                        notifyModulationEntryChangeEvent(entry, entryNumber + 1,
                                                         EnumModulationParameter::MODULATIONQUANTIZE);
                        break;
                    }
                    case EnumModulationEditCommands::ADDSOURCE:
                    case EnumModulationEditCommands::UNKNOWN:
                        break;
                }
                break;
            }
            ++entryNumber;
        }
    }

    // --- SysEx decoders (reference Is* helpers) -------------------------------------------

    XpanderParameter* XpanderController::getParameterForPageSubPageAndId(int page, int subPage, int buttonId)
    {
        for (const auto& entry : tone().parameterMap())
        {
            auto* parameter = dynamic_cast<XpanderParameter*>(entry.parameter.get());
            if (parameter != nullptr && parameter->page() == page && parameter->subPage() == subPage
                && parameter->buttonId() == buttonId)
            {
                return parameter;
            }
        }
        Logger::writeLine("XpanderController", TraceLevel::Error,
                          "Unable to get param for: page=" + std::to_string(page)
                              + ", subPage=" + std::to_string(subPage)
                              + ", buttonID=" + std::to_string(buttonId));
        return nullptr;
    }

    bool XpanderController::isSinglePatchProgramDumpSysex(const MidiMessage& message)
    {
        return model::SinglePatchIterator::isSinglePatch(message.bytes());
    }

    bool XpanderController::isMultiPatchProgramDumpSysex(const MidiMessage& message)
    {
        // 3rd byte is 0x02 (Xpander) or 0x04 (Matrix-12).
        if (message.size() < 5 || message[0] != 0xF0 || message[1] != 0x10
            || (message[2] != 0x02 && message[2] != 0x04) || message[3] != 0x01 || message[4] != 0x01)
        {
            return false;
        }
        return true;
    }

    bool XpanderController::isProgramChangeUpSysex(const MidiMessage& message)
    {
        return startsWith(message, {0xF0, 0x10, 0x02, 0x0E, 0x04, 0xF7});
    }

    bool XpanderController::isProgramChangeDownSysex(const MidiMessage& message)
    {
        return startsWith(message, {0xF0, 0x10, 0x02, 0x0E, 0x08, 0xF7});
    }

    bool XpanderController::isPageSubPageSelectSysex(const MidiMessage& message, int& page, int& subPage)
    {
        page = -1;
        subPage = -1;
        constexpr std::size_t PAGESUBPAGE_SELECTMESSAGE_LENGTH = 6;
        if (message.size() < PAGESUBPAGE_SELECTMESSAGE_LENGTH
            || !startsWith(message, {0xF0, 0x10, 0x02, 0x0B}))
        {
            return false;
        }
        page = message[4];
        subPage = message[5];
        return true;
    }

    bool XpanderController::isPageEditFollowsSysex(const MidiMessage& message, int& buttonId,
                                                   int& parameterValue, bool& isRotary) const
    {
        constexpr int MAX_BUTTON_ID = 0x18;
        constexpr int BUTTON_TO_ROTARY_OFFSET = 0x10;
        buttonId = std::numeric_limits<int>::min();
        parameterValue = std::numeric_limits<int>::min();
        isRotary = false;

        if (message.size() < XpanderParameter::SYSEX_MESSAGE_LENGTH
            || !startsWith(message, {0xF0, 0x10, 0x02, 0x0A, 0x00}))
        {
            return false;
        }
        buttonId = message[XpanderParameter::SYSEX_BUTTON_ID];
        std::uint8_t lowByte = 0;
        std::uint8_t highByte = 0;
        if (buttonId >= MAX_BUTTON_ID)
        {
            if (!_pageSubPageHelper.isAuthorizedRotary(static_cast<EnumRotaryEncoders>(buttonId)))
            {
                return false;
            }
            int parameterPage = 0;
            int parameterSubPage = 0;
            _pageSubPageHelper.getPageSubPage(parameterPage, parameterSubPage);
            if (!_pageSubPageHelper.isLfoRetrig(parameterPage, parameterSubPage, buttonId))
            {
                buttonId -= BUTTON_TO_ROTARY_OFFSET;
            }
            isRotary = true;
            lowByte = message[XpanderParameter::SYSEX_BUTTON_ID + 2];
            highByte = message[XpanderParameter::SYSEX_BUTTON_ID + 3];
        }
        else
        {
            lowByte = message[XpanderParameter::SYSEX_BUTTON_ID + 4];
            highByte = message[XpanderParameter::SYSEX_BUTTON_ID + 5];
        }
        parameterValue = highByte == 0x01 ? (0x80 - lowByte) * -1 : lowByte; // sign bit
        return true;
    }

    bool XpanderController::isModulationEditFollowsSysex(const MidiMessage& message,
                                                         int& modulationSourceNumber,
                                                         EnumModulationEditCommands& command, int& value)
    {
        constexpr std::size_t MOD_EDIT_MESSAGE_LENGTH = 11;
        constexpr std::size_t MOD_EDIT_SOURCE_NUMBER_INDEX = 5;
        constexpr std::size_t MOD_EDIT_COMMAND_INDEX = 7;
        constexpr std::size_t MOD_EDIT_VALUE_LOW_BYTE_INDEX = MOD_EDIT_COMMAND_INDEX + 2;
        constexpr std::size_t MOD_EDIT_VALUE_HIGH_BYTE_INDEX = MOD_EDIT_VALUE_LOW_BYTE_INDEX + 1;

        modulationSourceNumber = 0;
        command = EnumModulationEditCommands::UNKNOWN;
        value = 0;
        if (message.size() < MOD_EDIT_MESSAGE_LENGTH
            || !startsWith(message, {0xF0, 0x10, 0x02, 0x0F, 0x00}))
        {
            return false;
        }
        modulationSourceNumber = message[MOD_EDIT_SOURCE_NUMBER_INDEX];
        command = static_cast<EnumModulationEditCommands>(message[MOD_EDIT_COMMAND_INDEX]);
        const std::uint8_t lowByte = message[MOD_EDIT_VALUE_LOW_BYTE_INDEX];
        const std::uint8_t highByte = message[MOD_EDIT_VALUE_HIGH_BYTE_INDEX];
        value = highByte == 0x01 ? (0x80 - lowByte) * -1 : lowByte; // sign bit
        return true;
    }
}
