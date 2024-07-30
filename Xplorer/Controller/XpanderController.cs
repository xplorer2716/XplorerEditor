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
using MidiApp.MidiController.Controller.Arguments;
using MidiApp.MidiController.Model;
using MidiApp.MidiController.Service;
using Sanford.Multimedia.Midi;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Threading;
using Xplorer.Common;
using Xplorer.Controller.Events;
using Xplorer.Controller.Service.Settings;
using Xplorer.Model;

namespace Xplorer.Controller
{
    /// <summary>
    /// Application controller
    /// </summary>
    internal sealed partial class XpanderController : AbstractController
    {
        /// <summary>
        /// time between each buffer sending for a ALL_DATA_DUMP
        /// </summary>
        private const int DELAY_BETWEEN_ALL_DATA_DUMP_SEND_SINGLE_PATCH = 150;

        /// <summary>
        /// Additional delay between messages needed in some cases
        /// </summary>
        private const int DELAY_BETWEEN_MESSAGES = 100;

        private bool _firstStart = true;

        /// <summary>
        /// Handle page /sub page logic
        /// </summary>
        private PageSubPageHelper _pageSubPageHelper = new PageSubPageHelper();

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderController"/> class.
        /// </summary>
        public XpanderController()
            : base()
        {
            ClearClipboard();
        }

        /// <summary>
        /// returns current program number
        /// </summary>
        public override int CurrentProgramNumber
        {
            get
            {
                return ((XpanderTone)Tone).CurrentProgramNumber;
            }
        }

        public override string ToneName
        {
            get
            {
                return base.ToneName;
            }
            set
            {
                base.ToneName = value;
                // the modified tone name  is not reflected into synth's edit buffer
                // Since we do not have a sysex message to set the name we need to resend the whole single patch
                XpanderTone clone = new XpanderTone();
                clone.FromByteArray(((XpanderTone)Tone).ToByteArray());
                SendDataToSynthOutputDevice(new SysExMessage(clone.ToByteArray()));
                // this is needd else the Xpander will not show the new name
                SendProgramChangeAndGetSinglePatchFromSynth(clone.CurrentProgramNumber);
            }
        }

        /// <summary>
        /// Extracts the tones from bank (all data dump) to a given directory.
        /// </summary>
        /// <param name="allDataDumpFilename">The bank filename.</param>
        /// <param name="directoryName">Name of the directory.</param>
        /// <returns> the tone collection</returns>
        public override IEnumerable<Tuple<string, AbstractTone>> ExtractSinglePatchesFromAllDataDumpFileToDirectory(string bankFilename, string directoryName)
        {
            // preconditions
            if (!File.Exists(bankFilename) || !Directory.Exists(directoryName))
            {
                throw new NonFatalException("File or directory does not exists.");
            }

            XPanderToneReader reader = new XPanderToneReader();
            ICollection<Tuple<string, AbstractTone>> tones;

            tones = reader.ReadTones(bankFilename);
            XPanderToneWriter writer = new XPanderToneWriter();

            foreach (Tuple<string, AbstractTone> t in tones)
            {
                string filename = FileUtils.MakeUniqueFilenameFromString(t.Item1, FileUtils.SYSEX_FILE_EXTENSION_WITH_DOT, directoryName);
                writer.WriteTone(Path.Combine(directoryName, filename), t.Item2);
            }

            return tones;
        }

        /// <summary>
        /// load the specified tone sysex file
        /// </summary>
        /// <param name="filename">The filename.</param>
        public void LoadTone(string filename)
        {
            IToneReader reader = new XPanderToneReader();
            LoadTone(filename, reader);
            ClearClipboard();
        }

        /// <summary>
        /// Load the editing tone from the specified filename
        /// </summary>
        /// <param name="filename">the tone sysex filename</param>
        /// <param name="reader"></param>
        public override void LoadTone(string filename, IToneReader reader)
        {
            if (AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff)
            {
                SendAllNotesOffToSynthOutput();
            }

            Stop();
            // disable the change of parameter (automation input or change from UI)
            EnableSetParameter = false;
            reader.ReadTone(filename, Tone);

            ((XpanderTone)Tone).CurrentProgramNumber = ((XpanderTone)Tone).EditingProgramNumber;
            UpdateUIAndSendFullToneToSynth(((XpanderTone)Tone).EditingProgramNumber);

            // dot not resend parameter when workerthread is reactivated
            foreach (AbstractParameter parameter in Tone.ParameterMap.Values)
            {
                parameter.Changed = false;
            }

            //re-enable the change of parameter
            EnableSetParameter = true;
            Start();
        }

        /// <summary>
        /// Generates a new tone based on randomized values and send it to synth
        /// </summary>
        /// <param name="excludedParameters">The excluded parameters.</param>
        public override void RandomizeTone(RandomizeToneArgument argument)
        {
            if (AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff)
            {
                SendAllNotesOffToSynthOutput();
            }

            Stop();
            ClearClipboard();

            // disable the change of parameter (automation input or change from UI)
            EnableSetParameter = false;

            var randomizerConfig = AllUsersSettingsService.AllUsersSettings.RandomizerConfig;

            // EnumRandomVCO
            if (!randomizerConfig.VCO2FmNoiseSync.HasFlag(EnumRandomVCO2.EnableFM))
            {
                // add FM parameters to exclusion liste
                argument.ExcludedParameters.Add("FM_AMP");
                argument.ExcludedParameters.Add("FM_DESTINATION");
            }
            if (!randomizerConfig.VCO2FmNoiseSync.HasFlag(EnumRandomVCO2.EnableNoise))
            {
                argument.ExcludedParameters.Add("VCO2_WAVESHAPE_NOISE");
            }
            if (!randomizerConfig.VCO2FmNoiseSync.HasFlag(EnumRandomVCO2.EnableSync))
            {
                argument.ExcludedParameters.Add("VCO2_WAVE_SYNC");
            }
            if (randomizerConfig.VCODetune == EnumRandomVCODetune.Analog ||
                randomizerConfig.VCODetune == EnumRandomVCODetune.Digital)
            {
                ((XpanderTone)Tone).Detune(detuneAnalog: (randomizerConfig.VCODetune == EnumRandomVCODetune.Analog));
                argument.ExcludedParameters.Add("VCO1_DETUNE");
                argument.ExcludedParameters.Add("VCO2_DETUNE");

                // else detune will be fully randomized
            }

            if (randomizerConfig.VCOFreq != EnumRandomVCOFreq.Free)
            {
                ((XpanderTone)Tone).DefineVCOFrequenciesTuning(randomizerConfig.VCOFreq);
                argument.ExcludedParameters.Add("VCO1_FREQ");
                argument.ExcludedParameters.Add("VCO2_FREQ");
                // else fully randomized
            }

            // randomize / humanize other parameters
            Tone.RandomizeToneParameters(argument.ExcludedParameters, argument.HumanizeRatio);

            // mod matrix randomize updates amount and quantize parameters for new destinations
            // mod matrix "parameters" are not in the parameter map
            ((XpanderTone)Tone).RandomizeModulationMatrix(
               enableAmount: randomizerConfig.ModulationMatrix.HasFlag(EnumRandomModMatrix.EnableAmount),
               enableQuantize: randomizerConfig.ModulationMatrix.HasFlag(EnumRandomModMatrix.EnableQuantize),
               enableSourceAndDest: randomizerConfig.ModulationMatrix.HasFlag(EnumRandomModMatrix.EnableSourcesAndDestinations),
               humanizeRatio: argument.HumanizeRatio);

            // this is done after mod matrix random, see DefineEnveloppeTypeForEnv2ModVca2 impl
            if (randomizerConfig.VCA2Env != EnumRandomVCAEnv.Free)
            {
                ((XpanderTone)Tone).ForceEnv2ModVca2AfterRandomizeMatrix(randomizerConfig.VCA2Env);
            }

            // here we do not use controller's ToneName property since we will send the whole patch already
            Tone.ToneName = "RANDOM";
            ((XpanderTone)Tone).CurrentProgramNumber = ((XpanderTone)Tone).EditingProgramNumber;
            UpdateUIAndSendFullToneToSynth(((XpanderTone)Tone).EditingProgramNumber);

            // dot not resend parameters when worker thread is reactivated
            foreach (AbstractParameter parameter in Tone.ParameterMap.Values)
            {
                parameter.Changed = false;
            }

            //re-enable the change of parameter
            EnableSetParameter = true;
            Start();
        }

#warning UNTESTED_MorphTones

        /// <summary>
        /// Morphes two tones and load the result as the current Tone and send it to synth
        /// Modulation matrix of resulting tone is the same as first tone. Assumes files exist.
        /// </summary>
        /// <param name="firstToneFilename">The first tone filename.</param>
        /// <param name="secondtoneFilename">The secondtone filename.</param>
        /// <param name="morphingFactor">The morphing factor.</param>
        public void MorphTones(string firstToneFilename, string secondtoneFilename, float morphingFactor)
        {
            if (AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff)
            {
                SendAllNotesOffToSynthOutput();
            }
            Stop();

            ClearClipboard();
            // disable the change of parameter (automation input or change from UI)
            EnableSetParameter = false;

            try
            {
                IToneReader reader = new XPanderToneReader();

                XpanderTone firstTone = new XpanderTone();
                XpanderTone secondTone = new XpanderTone();
                AbstractTone resultTone = new XpanderTone();

                reader.ReadTone(firstToneFilename, firstTone);
                reader.ReadTone(secondtoneFilename, secondTone);

                // since we won't morph the modulation matrix we need to copy it
                // from first tone. We don't have a Clone() implementation
                // so use the instanciation from sysex instead.
                reader.ReadTone(firstToneFilename, resultTone);

                Tone.MorphTones(firstTone, secondTone, ref resultTone, morphingFactor);

                resultTone.ToneName = "MORPH-X";
                Tone = resultTone;
                ((XpanderTone)Tone).CurrentProgramNumber = ((XpanderTone)Tone).EditingProgramNumber;
                UpdateUIAndSendFullToneToSynth(((XpanderTone)Tone).EditingProgramNumber);
            }
            finally
            {
                // dot not resend parameter when workerthread is reactivated
                foreach (AbstractParameter parameter in Tone.ParameterMap.Values)
                {
                    parameter.Changed = false;
                }

                //re-enable the change of parameter
                EnableSetParameter = true;
                Start();
            }
        }

        /// <summary>
        /// Saves the tone.
        /// </summary>
        /// <param name="filename">The filename.</param>
        public void SaveTone(string filename)
        {
            IToneWriter writer = new XPanderToneWriter();
            SaveTone(filename, writer);
            ClearClipboard();
        }

        /// <summary>
        /// Sets the editing program number
        /// </summary>
        /// <param name="programNumber">The program number.</param>
        public void SetEditingProgramNumber(int programNumber)
        {
            ((XpanderTone)Tone).EditingProgramNumber = programNumber;
        }

        /// <summary>
        /// override : at the first start, ask the synth for editing single patch and update locally
        /// </summary>
        public override void Start()
        {
            if (_firstStart)
            {
                _firstStart = false;
                XpanderTone xTone = (XpanderTone)Tone;
                xTone.CurrentProgramNumber = xTone.EditingProgramNumber;

                // get single patch
                SendProgramChangeAndGetSinglePatchFromSynth(xTone.CurrentProgramNumber);
            }
            base.Start();
        }

        /// <summary>
        /// Backups all data dump to file.
        /// </summary>
        /// <param name="fileName">Name of the file.</param>
        internal void BackupAllDataDumpToFile(string fileName)
        {
            if (_allDataDumpRequestState.IsWaitingForAllDataDumpRequest)
            {
                throw new NonFatalException("An \"all data dump request\" is already in progress.");
            }
            // Reset the message queue.We can have GetSinglePatch messages in the queue, don't mix them
            // to avoid bad interpretation when receiving  single voice dumps, since messages are the same.
            // moreover, synth will not be able to handle messages while dumping the data, so don't
            // overhelm it with useless messages.
            Stop(); Start();

            // initialize AllDataDumpRequestState
            _allDataDumpRequestState.Initialize(fileName, AllDataDumpRequestState.EnumAllDataDumpRequestMode.All);
            try
            {
                SendAllDataDumpRequestToSynth();
                // all residual processing done when receiving events
            }
            catch (Exception e)
            {
                //avoid to get stuck in a IsWaitingForAllDataDumpRequest state
                _allDataDumpRequestState.IsWaitingForAllDataDumpRequest = false;
                throw new NonFatalException(e.Message);
            }
        }

        /// <summary>
        /// Decreases the current program number.
        /// </summary>
        internal void DecreaseCurrentProgramNumber()
        {
            XpanderTone xTone = (XpanderTone)Tone;
            xTone.CurrentProgramNumber -= 1;
            SendProgramChangeAndGetSinglePatchFromSynth(xTone.CurrentProgramNumber);
        }

        /// <summary>
        /// force a send of the current page, subpage
        /// </summary>
        internal void ForceSendPageSubPage()
        {
            int page, subPage;
            _pageSubPageHelper.GetPageSubPage(out page, out subPage);
            SendPageSubPageAndUpdatePageSubPage(page, subPage);
        }

        /// <summary>
        /// Gets the modulation entry by number.
        /// </summary>
        /// <param name="number">The number.</param>
        /// <returns></returns>
        internal ModulationMatrixEntry GetModulationEntryByNumber(int number)
        {
            XpanderTone tone = (XpanderTone)Tone;
            return tone.ModulationMatrix[number - 1];
        }

        /// <summary>
        /// Gets the single tones from synth and put them in the given destination folder
        /// </summary>
        /// <param name="destinationFolder">The destination folder.</param>
        internal void GetSingleTonesFromSynth(string destinationFolder)
        {
            if (!VerifySynthOutputDevice())
            {
                throw new NonFatalException("Unable to open MIDI device " + AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthOutputDeviceName);
            }
            if (!Directory.Exists(destinationFolder))
            {
                throw new NonFatalException(string.Format(CultureInfo.InvariantCulture, "Destionation folder {0} does not exists.", destinationFolder));
            }
            if (_allDataDumpRequestState.IsWaitingForAllDataDumpRequest)
            {
                throw new NonFatalException("An \"all data dump request\" is already in progress.");
            }
            // Reset the message queue.We can have GetSinglePatch messages in the queue, don't mix them
            // to avoid bad interpretation when receiving  single voice dumps, since messages are the same.
            // moreover, synth will not be able to handle messages while dumping the data, so don't
            // overhelmed it with useless messages.
            Stop(); Start();

            // initialize AllDataDumpRequestState
            _allDataDumpRequestState.Initialize(destinationFolder, AllDataDumpRequestState.EnumAllDataDumpRequestMode.SinglePatch);
            try
            {
                SendAllDataDumpRequestToSynth();
                // all residual processing done when receiving events
            }
            catch (Exception e)
            {
                //avoid to get stuck in a IsWaitingForAllDataDumpRequest state
                _allDataDumpRequestState.IsWaitingForAllDataDumpRequest = false;
                throw new NonFatalException(e.Message);
            }
        }

        /// <summary>
        /// Increases the current program number.
        /// </summary>
        internal void IncreaseCurrentProgramNumber()
        {
            XpanderTone xTone = (XpanderTone)Tone;
            xTone.CurrentProgramNumber += 1;
            SendProgramChangeAndGetSinglePatchFromSynth(xTone.CurrentProgramNumber);
        }

        /// <summary>
        /// Restores all data dump to synth.
        /// </summary>
        /// <remarks>
        /// We don't check if the file is really a data dump, since content can be different depending the way it was created (with Xplorer or by hand).
        /// We only send the contained sysex to the synth
        /// </remarks>
        /// <param name="fileName">Name of the file.</param>
        internal void RestoreAllDataDumpToSynth(string fileName, Action<int, int> progressionAction)
        {
            if (!File.Exists(fileName)) throw new NonFatalException("File does not exist: " + fileName);
            try
            {
                using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open, FileAccess.Read)))
                {
                    SysexIterator iterator = new SysexIterator(reader.BaseStream);

                    // first iterates to get the total count
                    List<byte[]> allSysexes = new List<byte[]>(iterator);

                    int count = allSysexes.Count;
                    int i = 0;
                    foreach (byte[] sysex in allSysexes)
                    {
                        SendDataToSynthOutputDevice(new SysExMessage(sysex));
                        progressionAction(i, count);
                        Thread.Sleep(DELAY_BETWEEN_ALL_DATA_DUMP_SEND_SINGLE_PATCH);
                        i++;
                    }

                    // update the current patch
                    SendProgramChangeAndGetSinglePatchFromSynth(this.CurrentProgramNumber);
                }
            }
            catch (Exception e)
            {
                throw new NonFatalException(string.Format(CultureInfo.InvariantCulture, "An error occured while sending file {0}: {1}", fileName, e.Message), e);
            }
        }

        /// <summary>
        /// Sends all notes off to synth output.
        /// </summary>
        internal void SendAllNotesOffToSynthOutput()
        {
            if (VerifySynthOutputDevice())
            {
                const int ALL_NOTES_OFF = 123;
                ChannelMessage allNotesOff = new ChannelMessage(ChannelCommand.Controller, AllUsersSettingsService.AllUsersSettings.MidiConfig.MidiChannel, ALL_NOTES_OFF);
                SendDataToSynthOutputDevice(allNotesOff);
            }
        }

        /// <summary>
        /// send a page update to synth and update local page and subpage accordingly
        /// </summary>
        /// <param name="pageName">Name of the s page.</param>
        internal void SendPageUpdate(string pageName)
        {
            XpanderConstants.EnumPages page = XpanderConstants.EnumPages.CASSETTE;
            try
            {
                page = (XpanderConstants.EnumPages)Enum.Parse(typeof(XpanderConstants.EnumPages), pageName);
            }
            catch (Exception e)
            {
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(e));
            }
            if (page != XpanderConstants.EnumPages.CASSETTE)
            {
                ForceSendPageSubPage();
            }
        }

        /// <summary>
        /// Sends the program change and get single patch from synth. page is set to XpanderConstants.EnumPages.VCO_1_X
        /// </summary>
        /// <param name="programNumber">The program number.</param>
        internal void SendProgramChangeAndGetSinglePatchFromSynth(int programNumber)
        {
            if (AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff)
            {
                SendAllNotesOffToSynthOutput();
            }

            ClearClipboard();

            SendProgramChangeToSynthOutput(programNumber);

            SendProgramDumpRequestToSynth(programNumber);
            Thread.Sleep(DELAY_BETWEEN_MESSAGES);

            ((XpanderTone)Tone).CurrentProgramNumber = programNumber;
        }

        /// <summary>
        /// Sends a program change to synth; page and Subpage are set to XpanderConstants.EnumPages.VCO_1_X
        /// </summary>
        /// <param name="programNumber">The program number.</param>
        internal void SendProgramChangeToSynthOutput(int programNumber)
        {
            if (VerifySynthOutputDevice())
            {
                SendDataToSynthOutputDevice(new ChannelMessage(ChannelCommand.ProgramChange, Tone.MIDIChannel, programNumber));
                Thread.Sleep(DELAY_BETWEEN_MESSAGES);

                int page = (int)XpanderConstants.EnumPages.VCO_1_X; int subpage = 0x00;
                SendPageSubPageAndUpdatePageSubPage(page, subpage);
                _pageSubPageHelper.UpdatePageSubPage(page, subpage);
            }
        }

        /// <summary>
        /// Set programmer mode to single patch (see midi spec, "programmer mode swtiches")
        /// </summary>
        internal void SendProgrammerModeSinglePatch()
        {
            if (VerifySynthOutputDevice())
            {
                SysExMessage programmerModeSinglePatch = new SysExMessage(new byte[] {
                (byte)SysExType.Start,
                0xF0,0x10,0x02,0x0D,0x01,0x00,
                (byte)SysExType.Continuation });

                SendDataToSynthOutputDevice(programmerModeSinglePatch);
            }
        }

        /// <summary>
        /// Sends a tune request to synth
        /// </summary>
        internal void SendTuneRequestToSynth()
        {
            if (VerifySynthOutputDevice())
            {
                // the Xpander MIDI spec says that only "F6h" is required, and this works.
                // however, in respect to the MIDI standard,  this should be at least F0h F6h F7h.
                // Since both work on the Xpander, let's use the MIDI standard
                SysExMessage tuneRequest = new SysExMessage(new byte[] { (byte)SysExType.Start, 0xF6, (byte)SysExType.Continuation });
                SendDataToSynthOutputDevice(tuneRequest);
            }
        }

        /// <summary>
        /// Stores the single patch to synth.
        /// </summary>
        /// <param name="programNumber">The program number.</param>
        internal void StoreSinglePatchToSynth(int programNumber)
        {
            SysExMessage storePatchMessage = new SysExMessage(new byte[] {
                        0xF0,0x10,0x02,0x07,(byte)programNumber,(byte)SysExType.Continuation});
            if (VerifySynthOutputDevice())
            {
                // The program number destination can be different from the current edit buffer
                // we need to update the program number of the patch we send but without modifying our current patch
                XpanderTone clone = new XpanderTone();
                clone.FromByteArray(((XpanderTone)Tone).ToByteArray());
                clone.EditingProgramNumber = programNumber;
                SendDataToSynthOutputDevice(new SysExMessage(clone.ToByteArray()));
                Thread.Sleep(DELAY_BETWEEN_MESSAGES);

                SendDataToSynthOutputDevice(storePatchMessage);
                SendProgramChangeAndGetSinglePatchFromSynth(programNumber);
            }
        }

        /// <summary>
        /// Instanciates the tone instance
        /// </summary>
        /// <returns></returns>
        protected override AbstractTone CreateToneInstance()
        {
            return new XpanderTone();
        }

        //save tone is identical to base class

        #region Product information

        /// <summary>
        /// Returns the software version as a string. Prefers this to AssemblyService.GetVersionAsString()
        /// </summary>
        /// <returns></returns>
        private string GetVersionAsString()
        {
            AssemblyName name = Assembly.GetEntryAssembly().GetName();
            int major = name.Version.Major;
            if (major <= 0)
            {
                Debug.Assert(false, "major <= 0");
                major = 0;
            }
            string version = string.Format("{0}.{1}.{2}.{3}", major, name.Version.Minor, name.Version.Build, name.Version.Revision);
            return version;
        }

        public string GetProductNameAndVersionAsString()
        {
            return string.Format(@"{0} Version {1}", AssemblyService.GetProductName(), GetVersionAsString());
        }

        #endregion Product information

        #region DISPLAY_MESSAGE

        /// <summary>
        /// max length for message to show on xpander's display
        /// </summary>
        private const int MAX_DISPLAY_MESSAGE_LENGTH = 80;

        /// <summary>
        /// Display OFF message
        /// </summary>
        private static readonly SysExMessage displayOFFMessage = new SysExMessage(new byte[] {
                        0xF0,0x10,0x02,
                        AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthTypeIsMatrix12 ? (byte)XpanderConstants.DISPLAY_CONTROL_COMMAND_MATRIX12
                        : (byte)XpanderConstants.DISPLAY_CONTROL_COMMAND_XPANDER,
                        0x00,(byte)Sanford.Multimedia.Midi.SysExType.Continuation});

        /// <summary>
        /// Display ON message
        /// </summary>
        private static readonly SysExMessage displayONMessage = new SysExMessage(new byte[] {
                        0xF0,0x10,0x02,
                        AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthTypeIsMatrix12 ? (byte)XpanderConstants.DISPLAY_CONTROL_COMMAND_MATRIX12
                        : (byte)XpanderConstants.DISPLAY_CONTROL_COMMAND_XPANDER,
                        0x02,(byte)Sanford.Multimedia.Midi.SysExType.Continuation});

        /// <summary>
        /// Sends a message on synth's display
        /// </summary>
        /// <param name="additionalMessage">The additional message (null if unused)</param>
        public void SendGreetingsToSynth(string additionalMessage)
        {
            const int PADDING_LENGTH = MAX_DISPLAY_MESSAGE_LENGTH / 2;

            if (VerifySynthOutputDevice())
            {
                // build the message
                string greetingsLine1 = GetProductNameAndVersionAsString().ToUpperInvariant().PadRight(PADDING_LENGTH);

                string greetingsLine2 = "HTTPS://GITHUB.COM/XPLORER2716/XPLOREREDITOR";
                if (!string.IsNullOrEmpty(additionalMessage))
                {
                    greetingsLine2 += " " + additionalMessage.ToUpperInvariant();
                }
                greetingsLine2 = greetingsLine2.PadRight(PADDING_LENGTH);
                greetingsLine2 = greetingsLine2.Substring(0, PADDING_LENGTH);

                SendDataToSynthOutputDevice(displayOFFMessage);
                Thread.Sleep(ParameterTransmitDelay);
                SendDataToSynthOutputDevice(displayONMessage);
                Thread.Sleep(ParameterTransmitDelay);
                SendDisplayMessageToSynth(greetingsLine1 + greetingsLine2);
            }
        }

        /// <summary>
        /// Sends a display message to synth.
        /// Message can not be more than 80 character length
        /// displayOFFMessage/displayONMessage is managed by caller
        /// </summary>
        /// <param name="message">The message.</param>
        private void SendDisplayMessageToSynth(string message)
        {
            Debug.Assert(message.Length <= MAX_DISPLAY_MESSAGE_LENGTH);

            const int DISPLAY_INTRO_LENGTH = 5;
            const int DISPLAY_MESSAGE_LENGTH = DISPLAY_INTRO_LENGTH + MAX_DISPLAY_MESSAGE_LENGTH + 1;

            byte[] displayMessage = new byte[DISPLAY_MESSAGE_LENGTH];
            byte[] intro = new byte[DISPLAY_INTRO_LENGTH] { (byte)SysExType.Start,0x10, 0x02,
               AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthTypeIsMatrix12 ? (byte)XpanderConstants.DISPLAY_CONTROL_COMMAND_MATRIX12
                : (byte)XpanderConstants.DISPLAY_CONTROL_COMMAND_XPANDER,
                0x01 };
            intro.CopyTo(displayMessage, 0);

            System.Text.Encoding.ASCII.GetBytes(message.ToUpperInvariant()).CopyTo(displayMessage, DISPLAY_INTRO_LENGTH);

            displayMessage[DISPLAY_MESSAGE_LENGTH - 1] = (byte)Sanford.Multimedia.Midi.SysExType.Continuation;
            SysExMessage sysexMessage = new SysExMessage(displayMessage);

            SendDataToSynthOutputDevice(sysexMessage);
            Thread.Sleep(ParameterTransmitDelay);
        }

        /// <summary>
        /// Sends a "typewriter" style message to synth.
        /// </summary>
        /// <param name="message">The message.</param>
        public void SendTypeWriterMessageToSynth(string message)
        {
            // empirical value
            const int SCROLL_DELAY = 50;
            if (VerifySynthOutputDevice())
            {
                SendDataToSynthOutputDevice(displayOFFMessage);
                Thread.Sleep(ParameterTransmitDelay);
                SendDataToSynthOutputDevice(displayONMessage);
                Thread.Sleep(ParameterTransmitDelay);
                for (int i = 0; i <= message.Length; i++)
                {
                    string scrollingMessage = message.Substring(0, i).PadRight(MAX_DISPLAY_MESSAGE_LENGTH);
                    SendDisplayMessageToSynth(scrollingMessage);
                    Thread.Sleep(SCROLL_DELAY);
                }
            }
        }

        #endregion DISPLAY_MESSAGE

        /// <summary>
        /// Sends all data dump request to synth output and enter the "AllDataDumpRequest" state
        /// </summary>
        private void SendAllDataDumpRequestToSynth()
        {
            // depending on the synth type, message is not the same
            SysExMessage allDataDumpRequest = new SysExMessage(new byte[] {
                            (byte)SysExType.Start,0x10,0x02,0x02,
                            AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthTypeIsMatrix12 ? (byte)0x01 : (byte)0x00,
                            (byte)SysExType.Continuation});

            SendDataToSynthOutputDevice(allDataDumpRequest);
        }

        /// <summary>
        /// Sends the data to synth output device.
        /// </summary>
        /// <param name="message">The message.</param>
        private void SendDataToSynthOutputDevice(SysExMessage message)
        {
            _synthOutputDevice.Send(message);
            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthOutputDevice);
        }

        /// <summary>
        /// Sends the data to synth output device.
        /// </summary>
        /// <param name="message">The message.</param>
        private void SendDataToSynthOutputDevice(SysCommonMessage message)
        {
            _synthOutputDevice.Send(message);
            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthOutputDevice);
        }

        /// <summary>
        /// Sends the data to synth output device.
        /// </summary>
        /// <param name="message">The message.</param>
        private void SendDataToSynthOutputDevice(ChannelMessage message)
        {
            _synthOutputDevice.Send(message);
            NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthOutputDevice);
        }

        /// <summary>
        /// sends the full tone to the synth as the specified program number
        /// </summary>
        /// <param name="programNumber">The program number.</param>
        private void SendFullToneToSynthIntoProgram(int programNumber)
        {
            if (VerifySynthOutputDevice())
            {
                SendDataToSynthOutputDevice(new SysExMessage(((XpanderTone)Tone).ToByteArray()));
                Thread.Sleep(ParameterTransmitDelay);
                SendProgramChangeToSynthOutput(programNumber);
                // align tone's current program number
                ((XpanderTone)Tone).CurrentProgramNumber = programNumber;
            }
        }

        /// <summary>
        /// Sends the page sub page and update page sub page.
        /// </summary>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        private void SendPageSubPageAndUpdatePageSubPage(int page, int subPage)
        {
            SysExMessage pageSelectMessage = new SysExMessage(new byte[] {
                        0xF0,0x10,0x02,0x0B,(byte)page,(byte)subPage,(byte)SysExType.Continuation});
            if (VerifySynthOutputDevice())
            {
                SendDataToSynthOutputDevice(pageSelectMessage);
            }
            _pageSubPageHelper.UpdatePageSubPage(page, subPage);
        }

        /// <summary>
        /// sends a program dump request to synth
        /// </summary>
        /// <param name="programNumber">The program number</param>
        private void SendProgramDumpRequestToSynth(int programNumber)
        {
            if (VerifySynthOutputDevice())
            {
                // F0 10 02 00 00 <program number> F7
                SysExMessage programDumpRequest = new SysExMessage(new byte[] {
                            (byte)SysExType.Start,0x10,0x02,0x00,0x00,(byte)programNumber,(byte)SysExType.Continuation});
                SendDataToSynthOutputDevice(programDumpRequest);
            }
        }

        /// <summary>
        /// internal: send the current tone to synth as a full tone sysex
        /// </summary>
        /// <param name="programNumber">The program number.</param>
        private void UpdateUIAndSendFullToneToSynth(int programNumber)
        {
            SendFullToneToSynthIntoProgram(programNumber);
            NotifyFullToneChangeEvent();
        }
    }
}