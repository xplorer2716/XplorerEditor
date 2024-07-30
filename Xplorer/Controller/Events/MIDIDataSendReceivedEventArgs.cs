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
using System;
using System.Diagnostics;

namespace Xplorer.Controller.Events
{
    /// <summary>
    /// Args for MIDIDataSendReceiveEvent. Indicates that MIDI data where received or sent
    /// </summary>
    internal class MidiDataSendReceivedEventArgs : EventArgs
    {
        /// <summary>
        /// All data dump request state
        /// </summary>
        private readonly EnumMIDIDevice _device = EnumMIDIDevice.None;

        /// <summary>
        /// Initializes a new instance of the <see cref="MIDIDataSendReceiveEvent"/> class.
        /// </summary>
        /// <param name="allDataDumpRequestState">State of all data dump request.</param>
        internal MidiDataSendReceivedEventArgs(EnumMIDIDevice device)
        {
            _device = device;
        }

        /// <summary>
        /// Prevents a default instance of the <see cref="MidiDataSendReceivedEventArgs"/> class from being created.
        /// </summary>
        private MidiDataSendReceivedEventArgs()
        {
            Debug.Fail("Do not use this ctor");
        }

        /// <summary>
        /// Specify which device who sent some data
        /// </summary>
        public enum EnumMIDIDevice
        {
            None = 0,
            AutomationInputDevice,
            SynthInputDevice,
            SynthOutputDevice
        }

        /// <summary>
        /// The Midi device
        /// </summary>
        public EnumMIDIDevice Device
        {
            get { return _device; }
        }
    }
}