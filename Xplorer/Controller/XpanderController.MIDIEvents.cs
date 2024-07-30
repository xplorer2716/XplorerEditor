/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2024 Pascal Schmitt

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

using MidiApp.MidiController.Controller;
using MidiApp.MidiController.Model;
using MidiApp.MidiController.Service;
using Sanford.Multimedia.Midi;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Xplorer.Common;
using Xplorer.Controller.Events;
using Xplorer.Controller.Service.Settings;
using Xplorer.Model;

namespace Xplorer.Controller
{
    /// <summary>
    /// Controller - manage MIDI events
    /// </summary>
    internal partial class XpanderController : AbstractController
    {
        /// <summary>
        /// internal state for AllDataDumpRequest
        /// </summary>
        private AllDataDumpRequestState _allDataDumpRequestState = new AllDataDumpRequestState();

        #region AUTOMATION_INPUT_OVERRIDE

        protected override void AutomationInputDeviceSysExMessageReceived(object sender, SysExMessageEventArgs e)
        {
            // do not redirect sysex to synth to avoid data mix between us and the automation input
            // notify only to update MIDI status to user interface
            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.AutomationInputDevice);
        }

        /// <summary>
        /// happens when we receive midi message from automation input (DAW, Midi controller)
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected override void AutomationInputDeviceChannelMessageReceived(object sender, ChannelMessageEventArgs e)
        {
            // if CC# message is registered as automation, update the corresponding parameter.
            // else redirect to synth as is
            if (e.Message.Command == ChannelCommand.Controller)
            {
                // check if this control change number is not temporarily disabled
                // (mouse vs automation input fight)
                if (e.Message.Data1 != DisabledControlChangeNumber)
                {
                    List<string> ParameterNames = ControlChangeAutomationTable[e.Message.Data1];
                    if (ParameterNames != null)
                    {
                        for (int i = 0; i < ParameterNames.Count; i++)
                        {
                            string parameterName = ParameterNames[i];
                            // change parameter, and autoscale
                            AbstractParameter param = GetParameter(parameterName);

                            double interleave = (param.MaxValue + Math.Abs(param.MinValue));
                            //special case handling for "1" interleave. fill the gap when we get more that 64 in order to have mid range switching
                            double value = (double)e.Message.Data2;
                            if (interleave == 1)
                            {
                                if (value > 63) value = 127;
                            }
                            double controllerRatio = (value / (double)127);
                            int intValue = ((int)(controllerRatio * interleave) - Math.Abs(param.MinValue));
                            SetParameter(parameterName, intValue);
                            // fire the parameter change event
                            NotifyAutomationParameterChangeEvent(new ParameterChangeEventArgs(parameterName, intValue));
                        }
                    }
                    else
                    {
                        //this  CC does not automate anything, forward
                        e.Message.MidiChannel = Tone.MIDIChannel;
                        if (VerifySynthOutputDevice())
                        {
                            SendDataToSynthOutputDevice(e.Message);
                        }
                    }
                }
            }
            else if (e.Message.Command == ChannelCommand.ProgramChange)
            {
                // align ourself to this program number if in a compatible range
                if ((e.Message.Data1 >= XpanderTone.MIN_PROGRAM_NUMBER) && (e.Message.Data1 <= XpanderTone.MAX_PROGRAM_NUMBER))
                {
                    XpanderTone xTone = (XpanderTone)Tone;
                    xTone.CurrentProgramNumber = e.Message.Data1;
                    SendProgramChangeAndGetSinglePatchFromSynth(xTone.CurrentProgramNumber);
                }
            }
            else
            {
                //overwrite message's channel
                if (VerifySynthOutputDevice())
                {
                    e.Message.MidiChannel = Tone.MIDIChannel;
                    SendDataToSynthOutputDevice(e.Message);
                }
            }

            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.AutomationInputDevice);
        }

        #endregion AUTOMATION_INPUT_OVERRIDE

        #region SYNTH_INPUT_OVERRIDE

        /// <summary>
        /// default behavior: do nothing
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected override void SynthInputDeviceSysRealtimeMessageReceived(object sender, SysRealtimeMessageEventArgs e)
        {
            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthInputDevice);
        }

        /// <summary>
        /// default behavior: do nothing
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected override void SynthInputDeviceChannelMessageReceived(object sender, ChannelMessageEventArgs e)
        {
            // this happens when a new program number is dialed on the synth from panel
            if (e.Message.Command == ChannelCommand.ProgramChange)
            {
                if (AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff)
                {
                    SendAllNotesOffToSynthOutput();
                }
                // set tone current program number to this _value and ask synth to dump the currently selected program
                ((XpanderTone)Tone).CurrentProgramNumber = e.Message.Data1;
                SendProgramDumpRequestToSynth(e.Message.Data1);
            }
            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthInputDevice);
        }

        /// <summary>
        /// default behavior: do nothing
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected override void SynthInputDeviceSysCommonMessageReceived(object sender, SysCommonMessageEventArgs e)
        {
            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthInputDevice);
        }

        /// <summary>
        /// Happens when the synth send sysex to us
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected override void SynthInputDeviceSysExMessageReceived(object sender, SysExMessageEventArgs e)
        {
            int page = (int)XpanderConstants.EnumPages.UNKNOWN;
            int subPage = (int)XpanderConstants.EnumPages.UNKNOWN;

            // page edit follows recognition
            int buttonID = 0;
            int parameterValue = 0;
            bool isRotaryButton = false;

            // for ModulationEdit recognition
            XpanderConstants.EnumModulationEditCommands modulationEditCommand = XpanderConstants.EnumModulationEditCommands.UNKNOWN;
            int modulationEditValue = 0;
            int modulationSourceNumber = 0;

            // do not update MIDI status for user interface if message is not handled.
            bool isMessageHandled = false;

            if (IsSinglePatchProgramDumpSysex(e.Message))
            {
                isMessageHandled = true;
                Logger.WriteLine(this, TraceLevel.Info, string.Format("RECV: SingleVoiceProgramDump {0}", e.Message.ToString()));

                // we have 2 cases here:
                // - because we asked for it (program change, xplorer/synth resync) or sent by a manual dump from synth
                // - because we are waiting for a AllDataDumpRequest response
                if (!HandleAllDataDumpRequest(e.Message, true))
                {
                    Stop();
                    // disable the change of parameter (automation input or change from UI)
                    EnableSetParameter = false;
                    // set current tone to new value
                    ((XpanderTone)Tone).FromByteArray(e.Message.GetBytes());
                    foreach (AbstractParameter param in Tone.ParameterMap.Values)
                    {
                        param.Changed = false;
                    }
                    NotifyFullToneChangeEvent();
                    EnableSetParameter = true;
                    Start();
                }
            }
            else if (IsMultiPatchProgramDumpSysex(e.Message))
            {
                // we have 2 cases here:
                // - because it is sent by a manual dump from synth
                // - because we are waiting for a AllDataDumpRequest response
                // do only handle the second case, since we don't support multi patch yet
                Logger.WriteLine(this, TraceLevel.Info, string.Format("RECV: MultiPatchProgramDumpSysex {0}", e.Message.ToString()));

                if (!HandleAllDataDumpRequest(e.Message, false))
                {
                    //TODO multi patch support
                    // do not show multi patch dump as received message since we don't support it yet
                }
                else
                {
                    isMessageHandled = true;
                }
            }

            // handle "-" or "+" input on dial panel (sysex, not program change)
            else if (IsProgramChangeDOWNSysEx(e.Message))
            {
                isMessageHandled = true;
                Logger.WriteLine(this, TraceLevel.Info, string.Format("RECV: ProgramChangeDOWN {0}", e.Message.ToString()));
                ((XpanderTone)Tone).CurrentProgramNumber -= 1;
                if (AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff)
                {
                    SendAllNotesOffToSynthOutput();
                }
                SendProgramDumpRequestToSynth(((XpanderTone)Tone).CurrentProgramNumber);
            }
            else if (IsProgramChangeUPSysEx(e.Message))
            {
                isMessageHandled = true;
                Logger.WriteLine(this, TraceLevel.Info, string.Format("RECV: ProgramChangeUP {0}", e.Message.ToString()));
                ((XpanderTone)Tone).CurrentProgramNumber += 1;
                if (AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff)
                {
                    SendAllNotesOffToSynthOutput();
                }
                SendProgramDumpRequestToSynth(((XpanderTone)Tone).CurrentProgramNumber);
            }
            else if (IsPageSubPageSelectSysEx(e.Message, out page, out subPage))
            {
                isMessageHandled = true;
                Logger.WriteLine(this, TraceLevel.Info, string.Format("RECV: PageSubPageSelect {0}", e.Message.ToString()));
                _pageSubPageHelper.UpdatePageSubPage(page, subPage);
                // update UI if ENV_X, LFO_X,TRACK_X,RAMP_X
                if (_pageSubPageHelper.IsPageEnvLfoRampTrack())
                {
                    NotifyPageChangeEvent(page, subPage);
                }
            }
            else if (IsPageEditFollowsSysEx(e.Message, out buttonID, out parameterValue, out isRotaryButton))
            {
                isMessageHandled = true;
                Logger.WriteLine(this, TraceLevel.Info, string.Format("RECV: PageEditFollows {0}", e.Message.ToString()));

                // disable the change of parameter (automation input or change from UI)
                EnableSetParameter = false;

                XpanderParameter parameter = null;
                int parameterPage, parameterSubPage;

                _pageSubPageHelper.GetPageSubPage(out parameterPage, out parameterSubPage);
                parameter = GetParameterForPageSubPageAndID(parameterPage, parameterSubPage, buttonID);
                if (parameter != null)
                {
                    //update value and notify as if it was an automation update
                    if (isRotaryButton)
                    {
                        // offset
                        parameter.SetValueUnchanged(parameter.Value + parameterValue);
                    }
                    else
                    {
                        // raw value
                        parameter.SetValueUnchanged(parameterValue);
                    }
                    NotifyAutomationParameterChangeEvent(new ParameterChangeEventArgs(parameter.Name, parameter.Value));
                }
                // re-enable the change of parameter
                EnableSetParameter = true;
            }
            else if (IsModulationEditFollowsSysEx(e.Message, out modulationSourceNumber, out modulationEditCommand, out modulationEditValue))
            {
                isMessageHandled = true;
                EnableSetParameter = false;
                HandleModulationEditFromSynth(modulationSourceNumber, modulationEditCommand, modulationEditValue);
                EnableSetParameter = true;
            }

            if (isMessageHandled)
            {
                NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthInputDevice);
            }
        }

        /// <summary>
        /// Handles all data dump request.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="isSinglePatchDataDump">if set to <c>true</c> [is single patch data dump].</param>
        /// <returns></returns>
        private bool HandleAllDataDumpRequest(SysExMessage message, bool isSinglePatchDataDump)
        {
            if (!_allDataDumpRequestState.IsWaitingForAllDataDumpRequest)
            {
                return false;
            }
            if (isSinglePatchDataDump)
            {
                // store single patch
                byte[] data = message.GetBytes();
                _allDataDumpRequestState.SinglePatches.Add(new Tuple<string, byte[]>(XpanderTone.GetNameFromByteArray(data), data));

                // save each individual single patches to disk
                if (_allDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.SinglePatch &&
                   (_allDataDumpRequestState.SinglePatches.Count() >= XpanderConstants.SINGLE_TONES_MAX_COUNT))
                {
                    _allDataDumpRequestState.IsWaitingForAllDataDumpRequest = false;
                    foreach (Tuple<string, byte[]> single in _allDataDumpRequestState.SinglePatches)
                    {
                        string filename = FileUtils.MakeUniqueFilenameFromString(single.Item1, FileUtils.SYSEX_FILE_EXTENSION_WITH_DOT, _allDataDumpRequestState.Destination);
                        // no need to instanciate a tone here
                        using (BinaryWriter w = new BinaryWriter(File.Open(Path.Combine(_allDataDumpRequestState.Destination, filename), FileMode.Create)))
                        {
                            w.Write(single.Item2);
                        }
                    }
                }
                NotifyAllDataDumpRequestProgressionEvent(_allDataDumpRequestState);
            }
            else
            {
                //  save all data dump request into a file
                if (_allDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.All)
                {
                    // store multi patch
                    _allDataDumpRequestState.MultiPatches.Add(message.GetBytes());

                    // test end of reception (we get all singles and multis)
                    if (_allDataDumpRequestState.MultiPatches.Count() >= XpanderConstants.MULTI_PATCHES_MAX_COUNT)
                    {
                        _allDataDumpRequestState.IsWaitingForAllDataDumpRequest = false;

                        using (BinaryWriter writer = new BinaryWriter(File.Open(_allDataDumpRequestState.Destination, FileMode.Create)))
                        {
                            // single patches
                            foreach (Tuple<string, byte[]> single in _allDataDumpRequestState.SinglePatches)
                            {
                                writer.Write(single.Item2);
                            }
                            // multi patches
                            foreach (byte[] multi in _allDataDumpRequestState.MultiPatches)
                            {
                                writer.Write(multi);
                            }
                        }
                    } // EnumAllDataDumpRequestMode.All
                    NotifyAllDataDumpRequestProgressionEvent(_allDataDumpRequestState);
                }

                // do nothing with multipatches if EnumAllDataDumpRequestMode.SinglePatch
            }

            return true;
        }

        /// <summary>
        /// Handles the modulation edit from synth.
        /// </summary>
        /// <param name="modulationSourceNumber">The modulation source number.</param>
        /// <param name="modulationEditCommand">The modulation edit command.</param>
        /// <param name="modulationEditValue">The modulation edit value.</param>
        private void HandleModulationEditFromSynth(int modulationSourceNumber, XpanderConstants.EnumModulationEditCommands modulationEditCommand, int modulationEditValue)
        {
            int page; int subPage;
            _pageSubPageHelper.GetPageSubPage(out page, out subPage);

            Logger.WriteLine(this, TraceLevel.Verbose, string.Format("RECV: ModulationEdit: page:{0}, subpage:{1}, cmd: {2} IdSource :{3} value:{4}",
                Enum.GetName(((XpanderConstants.EnumPages)page).GetType(), page),
                subPage,
                Enum.GetName(modulationEditCommand.GetType(), modulationEditCommand),
                modulationSourceNumber,
                modulationEditValue));

            XpanderTone xTone = (XpanderTone)Tone;
            XpanderConstants.EnumModulationDestinations destination = XpanderConstants.ModulationDestinationForPageSubPage(page, subPage);
            int entryNumber = 0;

            // if command is not "add source", retrieve the modulation entry from the matrix and perform edit command
            if (modulationEditCommand != XpanderConstants.EnumModulationEditCommands.ADDSOURCE)
            {
                foreach (ModulationMatrixEntry entry in xTone.ModulationMatrix)
                {
                    if ((entry.Destination == destination) && (entry.IdSource == modulationSourceNumber))
                    {
                        // simulate editing from UI
                        switch (modulationEditCommand)
                        {
                            case XpanderConstants.EnumModulationEditCommands.CHANGESOURCE:
                                xTone.ChangeModulationSource(modulationEditValue,
                                    entry.Amount, entry.Quantize, (int)entry.Destination, entryNumber + 1, null);
                                NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.MODULATIONSOURCE);
                                break;

                            case XpanderConstants.EnumModulationEditCommands.DELETESOURCE:
                                xTone.ChangeModulationSource((int)XpanderConstants.EnumModulationSourcesModMatrix.NONE, // setting to none will reset the entry
                                    entry.Amount, entry.Quantize, (int)entry.Destination, entryNumber + 1, null);
                                NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.MODULATIONSOURCE);
                                break;

                            case XpanderConstants.EnumModulationEditCommands.DIALVALUEAMOUNTOFCHANGE:
                                // modulationEditValue is signed amount of change
                                xTone.ChangeModulationSourceAmount((int)entry.Source,
                                   entry.Amount + modulationEditValue, (int)entry.Destination, entryNumber + 1, null);
                                NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.MODULATIONAMOUNT);
                                break;

                            case XpanderConstants.EnumModulationEditCommands.SETQUANTIZE:
                                xTone.ChangeModulationSourceQuantize((int)entry.Source, (int)entry.Destination,
                                     modulationEditValue, entryNumber + 1, null);
                                NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.MODULATIONQUANTIZE);
                                break;

                            case XpanderConstants.EnumModulationEditCommands.SETSIGN:
                                {
                                    int valueSign = modulationEditValue == 1 ? -1 : 1;
                                    int amountSign = entry.Amount < 0 ? -1 : 1;
                                    if (amountSign != valueSign)
                                    {
                                        // toggle sign if sign are different
                                        xTone.ChangeModulationSourceAmount((int)entry.Source,
                                       entry.Amount * -1, (int)entry.Destination, entryNumber + 1, null);
                                        NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.MODULATIONAMOUNT);
                                    }
                                }
                                break;

                            case XpanderConstants.EnumModulationEditCommands.SETUNSIGNEDVALUE:
                                {
                                    // keep the sign, we only change the raw value, whatever the sign is
                                    int amountSign = entry.Amount < 0 ? -1 : 1;
                                    xTone.ChangeModulationSourceAmount((int)entry.Source,
                                       /*raw _value*/modulationEditValue * amountSign, (int)entry.Destination, entryNumber + 1, null);
                                    NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.MODULATIONAMOUNT);
                                }
                                break;

                            case XpanderConstants.EnumModulationEditCommands.TOGGLEQUANTIZE:
                                int toggle = entry.Quantize == 1 ? 0 : 1;
                                xTone.ChangeModulationSourceQuantize((int)entry.Source,
                                    (int)entry.Destination, toggle, entryNumber + 1, null);

                                NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.MODULATIONQUANTIZE);
                                break;
                        }

                        // modulation updated, exit the loop
                        break;
                    }
                    entryNumber++;
                }
            }
            else //ADD source
            {
                // when the xpander sends an "add source", it is always the next available source id
                // (ex: not possible to add id source= 4 if 1,2,3 are not already used)
                // this is the same thing as getting the next available source in AddModulationSource
                entryNumber = xTone.GetNextAvailableModEntry();
                if (entryNumber != XpanderTone.NO_AVAILABLE_MOD_ENTRY)
                {
                    // amount and quantize are defined later with the appropriate message
                    xTone.AddModulationSource(modulationEditValue, 0, ModulationMatrixEntry.MIN_QUANTIZE, (int)destination, entryNumber + 1, null);
                    ModulationMatrixEntry entry = xTone.ModulationMatrix[entryNumber]; //0 based
                    NotifyModulationEntryChangeEvent(entry, entryNumber + 1, EnumModulationParameter.ALL);
                }
                // else this should not happen, it will mean our local matrix is desynchronized with the synth one.
                else
                {
                    Debug.Fail("received ADD source into mod matrix from synth, but none available locally");
                }
            }
        }

        /// <summary>
        /// Determines whether [is modulation edit follows sys ex] [the specified message].
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="modulationSourceNumber">The modulation source number.</param>
        /// <param name="modulationEditCommand">The modulation edit command.</param>
        /// <param name="modulationEditValue">The modulation edit value.</param>
        /// <returns>
        ///   <c>true</c> if [is modulation edit follows sys ex] [the specified message]; otherwise, <c>false</c>.
        /// </returns>
        private bool IsModulationEditFollowsSysEx(SysExMessage message,
            out int modulationSourceNumber,
            out XpanderConstants.EnumModulationEditCommands modulationEditCommand,
            out int modulationEditValue)
        {
            modulationSourceNumber = 0;
            modulationEditCommand = XpanderConstants.EnumModulationEditCommands.UNKNOWN;
            modulationEditValue = 0;
            const int MOD_EDIT_MESSAGE_LENGTH = 11;
            const int MOD_EDIT_SOURCE_NUMBER_INDEX = 5;
            const int MOD_EDIT_COMMAND_INDEX = 7; // 7+8
            const int MOD_EDIT_VALUE_LOW_BYTE_INDEX = MOD_EDIT_COMMAND_INDEX + 2; // 2 bytes
            const int MOD_EDIT_VALUE_HIGH_BYTE_INDEX = MOD_EDIT_VALUE_LOW_BYTE_INDEX + 1;

            byte[] ModulationEditIntro = new byte[] { (byte)SysExType.Start, 0x10, 0x02, 0x0F, 0x00 };

            if (message.Length < MOD_EDIT_MESSAGE_LENGTH) return false;
            for (int i = 0; i < ModulationEditIntro.Length; i++)
            {
                if (ModulationEditIntro[i] != message[i])
                {
                    return false;
                }
            }

            modulationSourceNumber = message[MOD_EDIT_SOURCE_NUMBER_INDEX];
            modulationEditCommand = (XpanderConstants.EnumModulationEditCommands)message[MOD_EDIT_COMMAND_INDEX];

            byte lowByte = message[MOD_EDIT_VALUE_LOW_BYTE_INDEX];
            byte highByte = message[MOD_EDIT_VALUE_HIGH_BYTE_INDEX];

            // values
            lowByte = message[MOD_EDIT_VALUE_LOW_BYTE_INDEX];
            highByte = message[MOD_EDIT_VALUE_HIGH_BYTE_INDEX];

            // transforms to 2's complement, 8 bits.
            if (highByte == 0x01) // sign bit
            {
                modulationEditValue = (0x80 - lowByte) * -1;
            }
            else
            {
                modulationEditValue = lowByte;
            }

            return true;
        }

        /// <summary>
        /// try to get the paramter from the parameter map that matches the page, subpage and button ID
        /// </summary>
        /// <param name="page"></param>
        /// <param name="_subPage"></param>
        /// <param name="buttonID"></param>
        /// <returns></returns>
        private XpanderParameter GetParameterForPageSubPageAndID(int page, int subPage, int buttonID)
        {
            XpanderParameter parameter = null;

            foreach (XpanderParameter param in Tone.ParameterMap.Values)
            {
                if ((param.Page == page) && (param.SubPage == subPage) && (param.ButtonID == buttonID))
                {
                    return param;
                }
            }
            Logger.WriteLine(
                this,
                TraceLevel.Error, string.Format("Unable to get param for: page={0}, subPage={1}, buttonID={2}",
                Enum.GetName(XpanderConstants.PagesType, page), subPage, buttonID));
            return parameter;
        }

        /// <summary>
        /// returns true if the specified sysex message is a single patch program dump data follows
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        private bool IsSinglePatchProgramDumpSysex(SysExMessage message)
        {
            return SinglePatchIterator.IsSinglePatch(message.GetBytes());
        }

        /// <summary>
        /// returns true if the specified sysex message is a multi patch program dump data follows
        /// </summary>
        /// <param name="message">The message.</param>
        /// <returns>
        ///   <c>true</c> if [is multi patch program dump sysex] [the specified message]; otherwise, <c>false</c>.
        /// </returns>
        private bool IsMultiPatchProgramDumpSysex(SysExMessage message)
        {
            // multi patch intro message depends on synthesizer type who send the data (see MIDI spec)
            // be able to recognize both of them
            int XpanderOrMatrix12Index = 2;
            byte[] multiPatchIntro = new byte[] {
                0xF0,
                0x10, 0x02 /* 0x04 on Matrix 12*/, 0x01, 0x01};

            if (message.Length < multiPatchIntro.Length) return false;

            for (int i = 0; i < multiPatchIntro.Length; i++)
            {
                if (i == XpanderOrMatrix12Index)
                {
                    if (message[i] != 0x02 && message[i] != 0x04)
                    {
                        return false;
                    }
                }
                else if (message[i] != multiPatchIntro[i])
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// Determines whether [is program change UP sys ex] [the specified message].
        /// </summary>
        /// <param name="message">The message.</param>
        /// <returns>
        ///   <c>true</c> if [is program change UP sys ex] [the specified message]; otherwise, <c>false</c>.
        /// </returns>
        private bool IsProgramChangeUPSysEx(SysExMessage message)
        {
            byte[] upMessage = new byte[] { (byte)SysExType.Start, 0x10, 0x02, 0x0E, 0x04, (byte)SysExType.Continuation };

            if (message.Length < upMessage.Length) return false;
            for (int i = 0; i < upMessage.Length; i++)
            {
                if (message[i] != upMessage[i])
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// Determines whether [is program change DOWN sys ex] [the specified message].
        /// </summary>
        /// <param name="message">The message.</param>
        /// <returns>
        ///   <c>true</c> if [is program change DOWN sys ex] [the specified message]; otherwise, <c>false</c>.
        /// </returns>
        private bool IsProgramChangeDOWNSysEx(SysExMessage message)
        {
            byte[] downMessage = new byte[] { (byte)SysExType.Start, 0x10, 0x02, 0x0E, 0x08, (byte)SysExType.Continuation };

            if (message.Length < downMessage.Length) return false;
            for (int i = 0; i < downMessage.Length; i++)
            {
                if (message[i] != downMessage[i])
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// Determines whether [is page sub page select sys ex] [the specified message].
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        /// <returns>
        ///   <c>true</c> if [is page sub page select sys ex] [the specified message]; otherwise, <c>false</c>.
        /// </returns>
        private bool IsPageSubPageSelectSysEx(SysExMessage message, out int page, out int subPage)
        {
            page = -1;
            subPage = -1;

            byte[] pageSubPageSelectMessage = new byte[] { (byte)SysExType.Start, 0x10, 0x02, 0x0B };
            const int PAGESUBPAGE_SELECTMESSAGE_LENGTH = 6;

            if (message.Length < PAGESUBPAGE_SELECTMESSAGE_LENGTH) return false;
            for (int i = 0; i < pageSubPageSelectMessage.Length; i++)
            {
                if (message[i] != pageSubPageSelectMessage[i])
                {
                    return false;
                }
            }

            //get page and subpage
            page = message[4];
            subPage = message[5];
            return true;
        }

        /// <summary>
        /// returns true if message is a page edit follows
        /// </summary>
        /// <param name="sysExMessage">The system ex message.</param>
        /// <param name="buttonID">The button identifier.</param>
        /// <param name="parameterValue">The parameter value.</param>
        /// <param name="isRotary">if set to <c>true</c> [is rotary].</param>
        /// <returns></returns>
        private bool IsPageEditFollowsSysEx(SysExMessage sysExMessage, out int buttonID, out int parameterValue, out bool isRotary)
        {
            byte[] pageEditFollowsMessageIntro = new byte[] { (byte)SysExType.Start, 0x10, 0x02, 0x0A, 0x00 };

            const int MAX_BUTTON_ID = 0x18;
            const int BUTTONTOROTARYOFFSET = 0x10;

            buttonID = int.MinValue;
            parameterValue = int.MinValue;
            isRotary = false;

            // Xpander does echo a PageEditFollowsSysEx without any data when we send a page edit follow
            // dot not take care about this echo if no data are provided
            if (sysExMessage.Length < XpanderParameter.SYSEX_MESSAGE_LENGTH) return false;

            // check message intro
            for (int i = 0; i < pageEditFollowsMessageIntro.Length; i++)
            {
                if (sysExMessage[i] != pageEditFollowsMessageIntro[i])
                {
                    return false;
                }
            }
            // else
            buttonID = sysExMessage[XpanderParameter.SYSEX_BUTTON_ID];
            byte highByte = 0;
            byte lowByte = 0;

            // rotary used as button, convert it to a button message
            if (buttonID >= MAX_BUTTON_ID)
            {
                // get current page/subpage (assuming it's the same between Xpander and local)
                int parameterPage, parameterSubPage;
                _pageSubPageHelper.GetPageSubPage(out parameterPage, out parameterSubPage);

                // depending on page and subpage, only few rotary encoders are used for editing; filter them
                // this does not apply for modulation edit
                if (!_pageSubPageHelper.IsAuthorizedRotary((XpanderConstants.EnumRotaryEncoders)buttonID))
                {
                    return false;
                }
                else
                {
                    // for LFO retrig changes, do not translate as a button value, since editing
                    // can only be done thru rotary
                    if (!_pageSubPageHelper.IsLfoRetrig(parameterPage, parameterSubPage, buttonID))
                    {
                        buttonID -= BUTTONTOROTARYOFFSET;
                    }
                }

                // rotary value
                isRotary = true;
                lowByte = sysExMessage[XpanderParameter.SYSEX_BUTTON_ID + 2];
                highByte = sysExMessage[XpanderParameter.SYSEX_BUTTON_ID + 3];
            }
            else
            {
                // button value
                lowByte = sysExMessage[XpanderParameter.SYSEX_BUTTON_ID + 4];
                highByte = sysExMessage[XpanderParameter.SYSEX_BUTTON_ID + 5];
            }
            // transforms to 2's complement, 8 bits.
            if (highByte == 0x01) // sign bit
            {
                parameterValue = (0x80 - lowByte) * -1;
            }
            else
            {
                parameterValue = lowByte;
            }

            return true;
        }

        #endregion SYNTH_INPUT_OVERRIDE
    }
}