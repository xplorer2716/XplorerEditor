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
using System.Collections.Generic;
using Xplorer.Common;

namespace Xplorer.Controller.Service.Settings
{
    /// <summary>
    /// Simple class to have the same settings available for all users
    /// Don't want to mess around with M$ ClientSettingStore override stuff
    ///
    /// </summary>
    public sealed class AllUsersSettings
    {
        public sealed class MidiConfiguration
        {
            /// <summary>
            /// Automation MIDI intput device name
            /// </summary>
            /// <value>
            /// The name of the automation input device.
            /// </value>
            public string AutomationInputDeviceName { get; set; }

            /// <summary>
            /// Synth input device name
            /// </summary>
            /// <value>
            /// The name of the synth input device.
            /// </value>
            public string SynthInputDeviceName { get; set; }

            /// <summary>
            /// Gets or sets the name of the synth output device.
            /// </summary>
            /// <value>
            /// The name of the synth output device.
            /// </value>
            public string SynthOutputDeviceName { get; set; }

            /// <summary>
            /// Gets or sets the sysex transmit delay.
            /// </summary>
            /// <value>
            /// The sysex transmit delay.
            /// </value>
            public int SysexTransmitDelay { get; set; }

            /// <summary>
            /// Gets or sets the midi channel.
            /// </summary>
            /// <value>
            /// The midi channel.
            /// </value>
            public int MidiChannel { get; set; }

            /// <summary>
            /// Gets or sets the editing program number.
            /// </summary>
            /// <value>
            /// The editing program number.
            /// </value>
            public int EditingProgramNumber { get; set; }

            /// <summary>
            /// get or set if smart all note off mode is enabled
            /// </summary>
            public bool SmartAllNotesOff { get; set; }

            /// <summary>
            /// Gets or sets a value indicating whether [synth type is matrix12].
            /// </summary>
            /// <value>
            /// 	<c>true</c> if [synth type is matrix12]; otherwise, <c>false</c>.
            /// </value>
            public bool SynthTypeIsMatrix12 { get; set; }

            /// <summary>
            /// Gets or sets the automation table.
            /// </summary>
            /// <value>
            /// The automation table.
            /// </value>
            public List<string> AutomationTable { get; set; }
        }

        public sealed class UiConfiguration
        {
            /// <summary>
            /// Gets or sets the color of the knob led border (ARGB)
            /// </summary>
            /// <value>
            /// The color of the knob led border.
            /// </value>
            public int KnobLedBorderColor { get; set; }

            /// <summary>
            /// Gets or sets a value indicating whether [knob movement is linear].
            /// </summary>
            /// <value>
            /// 	<c>true</c> if [knob movement is linear]; otherwise, <c>false</c>.
            /// </value>
            public bool KnobMovementIsLinear { get; set; }

            /// <summary>
            /// Gets or sets a value indicating whether [knob style is standard].
            /// </summary>
            /// <value>
            /// <c>true</c> if [knob style is standard]; otherwise, <c>false</c>.
            /// </value>
            public bool KnobStyleIsStandard { get; set; }
        }

        public sealed class RandomizerConfiguration
        {
            /// <summary>
            /// VCO FM, Sync, etc.. randomization
            /// </summary>
            public EnumRandomVCO2 VCO2FmNoiseSync { get; set; }

            /// <summary>
            /// VCO Frequency predefined values
            /// </summary>
            public EnumRandomVCOFreq VCOFreq { get; set; }

            /// <summary>
            /// VCO Detune predefined values
            /// </summary>
            public EnumRandomVCODetune VCODetune { get; set; }

            /// <summary>
            /// VCA Env predefined values
            /// </summary>
            public EnumRandomVCAEnv VCA2Env { get; set; }

            /// <summary>
            /// Modulation matrix predefined
            /// </summary>
            public EnumRandomModMatrix ModulationMatrix { get; set; }
        }

        public MidiConfiguration MidiConfig { get; set; }
        public UiConfiguration UiConfig { get; set; }
        public RandomizerConfiguration RandomizerConfig { get; set; }
    }
}