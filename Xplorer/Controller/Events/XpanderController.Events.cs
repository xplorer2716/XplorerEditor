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
using System;
using System.Collections;
using System.Collections.Generic;
using Xplorer.Common;
using Xplorer.Controller.Events;
using Xplorer.Model;

namespace Xplorer.Controller
{
    /// <summary>
    /// Application Controller - events definition
    /// </summary>
    internal partial class XpanderController : AbstractController
    {
        #region FullToneChangeEvent

        /// <summary>
        /// event fired when the full tone is changed (new tone loaded, randomization)
        /// </summary>
        public event EventHandler<FullToneChangeEventArgs> FullToneChangeEvent = null;

        /// <summary>
        /// update the whole user interface form tone values
        /// </summary>
        private void NotifyFullToneChangeEvent()
        {
            Dictionary<string, int> parameterMap = new Dictionary<string, int>(Tone.ParameterMap.Count);

            foreach (DictionaryEntry entry in Tone.ParameterMap)
            {
                AbstractParameter param = (AbstractParameter)entry.Value;
                parameterMap.Add((string)entry.Key, param.Value);
            }
            // build the parameter map for the event (optimization)
            // and notify for full update
            if (FullToneChangeEvent != null)
            {
                EventHandler<FullToneChangeEventArgs> theEvent = FullToneChangeEvent;
                theEvent(this, new FullToneChangeEventArgs(
                    parameterMap,
                    ((XpanderTone)Tone).ModulationMatrix));
            }
        }

        #endregion FullToneChangeEvent

        #region PageChangeEvent

        /// <summary>
        /// Occurs when [page change event].
        /// </summary>
        public event EventHandler<PageChangeEventArgs> PageChangeEvent = null;

        /// <summary>
        /// Notifies when  ENV_X, LFO_X,TRACK_X,RAMP_X page changes occurs from synth
        /// </summary>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        private void NotifyPageChangeEvent(int page, int subPage)
        {
            if (PageChangeEvent != null)
            {
                EventHandler<PageChangeEventArgs> theEvent = PageChangeEvent;
                theEvent(this, new PageChangeEventArgs((XpanderConstants.EnumPages)page, subPage));
            }
        }

        #endregion PageChangeEvent

        #region ModulationEntryChangeEvent

        /// <summary>
        /// Fired when a modulation entry is updated (received from synth)
        /// </summary>
        public event EventHandler<ModulationEntryChangeEventArgs> ModulationEntryChangeEvent = null;

        /// <summary>
        /// Notify a modulation entry change (from synth)
        /// </summary>
        /// <param name="entry">the entry</param>
        /// <param name="entryNumber">entry number</param>
        /// <param name="parameter">parameter change of the entry</param>
        private void NotifyModulationEntryChangeEvent(ModulationMatrixEntry entry, int entryNumber, EnumModulationParameter parameter)
        {
            if (ModulationEntryChangeEvent != null)
            {
                EventHandler<ModulationEntryChangeEventArgs> theEvent = ModulationEntryChangeEvent;
                theEvent(this, new ModulationEntryChangeEventArgs(entry, entryNumber, parameter));
            }
        }

        #endregion ModulationEntryChangeEvent

        #region AllDataDumpRequestProgressionEvent

        /// <summary>
        /// event fired when receiving data from a AllDataDumpRequest
        /// </summary>
        public event EventHandler<AllDataDumpRequestProgressionEventArgs> AllDataDumpRequestProgressionEvent = null;

        /// <summary>
        /// Notifies all data dump request progression event.
        /// </summary>
        private void NotifyAllDataDumpRequestProgressionEvent(AllDataDumpRequestState state)
        {
            if (AllDataDumpRequestProgressionEvent != null)
            {
                EventHandler<AllDataDumpRequestProgressionEventArgs> theEvent = AllDataDumpRequestProgressionEvent;
                theEvent(this, new AllDataDumpRequestProgressionEventArgs(state));
            }
        }

        #endregion AllDataDumpRequestProgressionEvent

        #region MIDIDataSendReceiveEvent

        /// <summary>
        /// Event fired when receiving data from a MIDIDataSendReceiveEvent
        /// WARNING: This can be fired from another thread !
        /// </summary>
        public event EventHandler<MidiDataSendReceivedEventArgs> MIDIDataSendReceiveEvent = null;

        /// <summary>
        /// Notifies MIDIDataSendReceiveEvent.
        /// </summary>
        private void NotifyMIDIDataSendReceiveEvent(MidiDataSendReceivedEventArgs.EnumMIDIDevice device)
        {
            if (MIDIDataSendReceiveEvent != null)
            {
                EventHandler<MidiDataSendReceivedEventArgs> theEvent = MIDIDataSendReceiveEvent;
                theEvent(this, new MidiDataSendReceivedEventArgs(device));
            }
        }

        #endregion MIDIDataSendReceiveEvent
    }
}