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
using System.Collections.Generic;
using System.Globalization;

namespace Xplorer.Common
{
    /// <summary>
    /// page / sub page struct
    /// </summary>
    internal struct PageSubPage
    {
        public XpanderConstants.EnumPages Page;
        public int SubPage;
    }

    /// <summary>
    /// Common constants and enums, mainly concerning the tone
    /// </summary>
    internal static class XpanderConstants
    {
        // 100 single patches in synth's memory
        internal const int SINGLE_TONES_MAX_COUNT = 100;

        // 100 multi patches in synth's memory
        internal const int MULTI_PATCHES_MAX_COUNT = 100;

        internal const int TONE_NAME_LENGTH = 8;
        internal const int MODENTRIES_COUNT = 20;
        internal const int MAX_MODULATION_SOURCE = 6;

        // data counts
        internal const int VCO_COUNT = 2;

        internal const int LFO_COUNT = 5;
        internal const int ENV_COUNT = 5;
        internal const int TRACK_COUNT = 3;
        internal const int TRACK_POINTS_COUNTS = 5;
        internal const int RAMP_COUNT = 4;

        // Synthesizer type/display command, see MIDI spec.
        internal const int DISPLAY_CONTROL_COMMAND_XPANDER = 0x05;

        internal const int DISPLAY_CONTROL_COMMAND_MATRIX12 = 0x06;

        // EnumModulationFlags
        internal enum EnumModulationFlags
        { //bitfield
            MODFLAG_KEYBD = 0x01,
            MODFLAG_LAG = 0x02,
            MODFLAG_LEV_1 = 0x04,
            MODFLAG_VIB = 0x08
        }

        // EnumVCOWaveFlags
        internal enum EnumVCOWaveFlags
        { // bitfield
            VCOWAVEFLAG_TRI = 0x01,
            VCOWAVEFLAG_SAW = 0x02,
            VCOWAVEFLAG_PULSE = 0x04,
            VCOWAVEFLAG_SYNC = 0x08,
            VCOWAVEFLAG_NOISE = 0x10
        }

        // EnumFMDestinationTypes
        internal enum EnumFMDestinationTypes
        {
            FMDEST_VCO,
            FMDESTFM_VCF
        }

        // EnumLagModeFlags
        internal enum EnumLagModeFlags
        { //bitfield
            LAGMODE_LEGATO = 0x01,
            LAGMODE_EXPO = 0x02,
            LAGMODE_EQUAL_TIME = 0x04
        }

        // EnumTriggerTypes
        internal enum EnumTriggerTypes
        {
            TRIGGER_OFF,
            TRIGGER_SINGLE,
            TRIGGER_MULTI,
            TRIGGER_EXTRIG
        }

        // LagFlags
        internal enum LagFlags
        { //bitfield
            LAGF_LAG = 0x01
        }

        // EnumEnveloppeModeFlags
        internal enum EnumEnveloppeModeFlags
        { //bitfield
            ENVMODE_RESET = 0x01,
            ENVMODE_INVALID_VALUE = 0x02,	// original MIDI SPEC did not mention this unused bit
            ENVMODE_MULTI = 0x04,			// SINGLE if not MULTI
            ENVMODE_GATED = 0x08,
            ENVMODE_EXTRIG = 0x10,
            ENVMODE_LFOTRIG = 0x20,
            ENVMODE_DADR = 0x40,
            ENVMODE_FREERUN = 0x80
        }

        // EnumLFOTriggerCodes
        internal enum EnumLFOTriggerCodes
        {
            LFOTRIG_LFO1,
            LFOTRIG_LFO2,
            LFOTRIG_LFO3,
            LFOTRIG_LFO4,
            LFOTRIG_LFO5,
            LFOTRIG_VIB
        }

        // EnumRampFlags
        internal enum EnumRampFlags
        { //bitfield
            RAMPF_GATED = 0x01,
            RAMPF_LFOTRIG = 0x02,
            RAMPF_EXTRIG = 0x04,
            RAMPF_MULTI = 0x08		//SINGLE if not MULTI
        }

        //Modulation sources
        internal enum EnumModulationSources
        {
            KBD, LAG, VEL, RVEL, PRES,
            TRK1, TRK2, TRK3,
            RMP1, RMP2, RMP3, RMP4,
            ENV1, ENV2, ENV3, ENV4, ENV5,
            PED1, PED2,
            LFO1, LFO2, LFO3, LFO4, LFO5,
            VIB, LEV1, LEV2
        };

        //Modulation sources for mod matrix
        internal enum EnumModulationSourcesModMatrix
        {
            KBD, LAG, VEL, RVEL, PRES,
            TRK1, TRK2, TRK3,
            RMP1, RMP2, RMP3, RMP4,
            ENV1, ENV2, ENV3, ENV4, ENV5,
            PED1, PED2,
            LFO1, LFO2, LFO3, LFO4, LFO5,
            VIB, LEV1, LEV2, NONE = 0x1B,
        };

        //Modulation dest
        internal enum EnumModulationDestinations
        {
            VCO1_FRQ, VCO1_PW, VCO1_VOL,
            VCO2_FRQ, VCO2_PW, VCO2_VOL,
            VCF_FRQ, VCF_RES, VCA1_VOL, VCA2_VOL,
            LFO1_SPD, LFO1_AMP,
            LFO2_SPD, LFO2_AMP,
            LFO3_SPD, LFO3_AMP,
            LFO4_SPD, LFO4_AMP,
            LFO5_SPD, LFO5_AMP,
            ENV1_DLY, ENV1_ATK, ENV1_DCY, ENV1_REL, ENV1_AMP,
            ENV2_DLY, ENV2_ATK, ENV2_DCY, ENV2_REL, ENV2_AMP,
            ENV3_DLY, ENV3_ATK, ENV3_DCY, ENV3_REL, ENV3_AMP,
            ENV4_DLY, ENV4_ATK, ENV4_DCY, ENV4_REL, ENV4_AMP,
            ENV5_DLY, ENV5_ATK, ENV5_DCY, ENV5_REL, ENV5_AMP,
            FM_AMP, LAG_RATE
        };

        // returns page/subpage for a given modulation dest
        internal static readonly PageSubPage[] PageSubPageForModulationDestination = new PageSubPage[(int)(EnumModulationDestinations.LAG_RATE) + 1] {
             new PageSubPage{Page=EnumPages.VCO_1_X,SubPage=2},new PageSubPage{Page=EnumPages.VCO_1_X,SubPage=5},new PageSubPage{Page=EnumPages.VCO_1_X,SubPage=7},
             new PageSubPage{Page=EnumPages.VCO_2_X,SubPage=2},new PageSubPage{Page=EnumPages.VCO_2_X,SubPage=5},new PageSubPage{Page=EnumPages.VCO_2_X,SubPage=7},
             new PageSubPage{Page=EnumPages.VCF_VCA_X,SubPage=2},new PageSubPage{Page=EnumPages.VCF_VCA_X,SubPage=3},new PageSubPage{Page=EnumPages.VCF_VCA_X,SubPage=6},new PageSubPage{Page=EnumPages.VCF_VCA_X,SubPage=7},
             new PageSubPage{Page=EnumPages.LFO_1,SubPage=2},new PageSubPage{Page=EnumPages.LFO_1,SubPage=7},
             new PageSubPage{Page=EnumPages.LFO_2,SubPage=2},new PageSubPage{Page=EnumPages.LFO_2,SubPage=7},
             new PageSubPage{Page=EnumPages.LFO_3,SubPage=2},new PageSubPage{Page=EnumPages.LFO_3,SubPage=7},
             new PageSubPage{Page=EnumPages.LFO_4,SubPage=2},new PageSubPage{Page=EnumPages.LFO_4,SubPage=7},
             new PageSubPage{Page=EnumPages.LFO_5,SubPage=2},new PageSubPage{Page=EnumPages.LFO_5,SubPage=7},
             new PageSubPage{Page=EnumPages.ENV_1,SubPage=2}, new PageSubPage{Page=EnumPages.ENV_1,SubPage=3}, new PageSubPage{Page=EnumPages.ENV_1,SubPage=4}, new PageSubPage{Page=EnumPages.ENV_1,SubPage=6},new PageSubPage{Page=EnumPages.ENV_1,SubPage=7},
             new PageSubPage{Page=EnumPages.ENV_2,SubPage=2}, new PageSubPage{Page=EnumPages.ENV_2,SubPage=3}, new PageSubPage{Page=EnumPages.ENV_2,SubPage=4}, new PageSubPage{Page=EnumPages.ENV_2,SubPage=6},new PageSubPage{Page=EnumPages.ENV_2,SubPage=7},
             new PageSubPage{Page=EnumPages.ENV_3,SubPage=2}, new PageSubPage{Page=EnumPages.ENV_3,SubPage=3}, new PageSubPage{Page=EnumPages.ENV_3,SubPage=4}, new PageSubPage{Page=EnumPages.ENV_3,SubPage=6},new PageSubPage{Page=EnumPages.ENV_3,SubPage=7},
             new PageSubPage{Page=EnumPages.ENV_4,SubPage=2}, new PageSubPage{Page=EnumPages.ENV_4,SubPage=3}, new PageSubPage{Page=EnumPages.ENV_4,SubPage=4}, new PageSubPage{Page=EnumPages.ENV_4,SubPage=6},new PageSubPage{Page=EnumPages.ENV_4,SubPage=7},
             new PageSubPage{Page=EnumPages.ENV_5,SubPage=2}, new PageSubPage{Page=EnumPages.ENV_5,SubPage=3}, new PageSubPage{Page=EnumPages.ENV_5,SubPage=4}, new PageSubPage{Page=EnumPages.ENV_5,SubPage=6},new PageSubPage{Page=EnumPages.ENV_5,SubPage=7},
             new PageSubPage{Page=EnumPages.FM_LAG_X,SubPage=2},new PageSubPage{Page=EnumPages.FM_LAG_X,SubPage=6}
        };

        // reverse table from PageSubPageForModulationDestination
        // used by the ModulationDestinationForPageSubPage()
        private static Dictionary<string, EnumModulationDestinations> _ModulationDestinationForPageSubPage = new Dictionary<string, EnumModulationDestinations>();

        /// <summary>
        /// Initializes the modulation destination for page/ sub page.
        /// </summary>
        private static void InitializeModulationDestinationForPageSubPage()
        {
            int destination = (int)EnumModulationDestinations.VCO1_FRQ;
            foreach (PageSubPage psp in PageSubPageForModulationDestination)
            {
                string sKey = string.Format(CultureInfo.InvariantCulture, "{0}.{1}", (int)psp.Page, psp.SubPage);
                _ModulationDestinationForPageSubPage.Add(sKey, (EnumModulationDestinations)destination);
                destination++;
            }
        }

        /// <summary>
        /// Modulationdestination for page / sub page.
        /// </summary>
        /// <param name="page">The page.</param>
        /// <param name="subPage">The sub page.</param>
        /// <returns></returns>
        internal static EnumModulationDestinations ModulationDestinationForPageSubPage(int Page, int SubPage)
        {
            // build the key
            string sKey = string.Format(CultureInfo.InvariantCulture, "{0}.{1}", Page, SubPage);
            EnumModulationDestinations destination;
            _ModulationDestinationForPageSubPage.TryGetValue(sKey, out destination);
            return destination;
        }

        // LFO trigger sources (ENV_X, RAMP_X)
        internal enum EnumLFOTriggerSources
        {
            LFO1, LFO2, LFO3, LFO4, LFO5, VIB
        }

        // LFO Retrig modes
        internal enum EnumLFORetrigModes
        {
            OFF, SINGLE, MULTI, EXTRIG
        }

        //VCF Filter modes (x15)
        internal enum EnumVCFFilterModes
        {
            _1_POLE_LOW, _2_POLE_LOW, _3_POLE_LOW, _4_POLE_LOW,
            _1_POLE_HIGH, _2_POLE_HIGH, _3_POLE_HIGH,
            _2_POLE_BAND, _4_POLE_BAND,
            _2_POLE_NOTCH, _3_POLE_PHASE,
            _2_HIGH_1_LOW, _3_HIGH_1_LOW,
            _2_NOTCH_1_LOW, _3_PHASE_1_LOW
        }

        internal enum EnumLFOWaveShapes
        {
            TRIANGLE, UP_SAW, DOWN_SAW, SQUARE, RANDOM, NOISE, SAMPLE
        }

        // ModulationEditCommands
        internal enum EnumModulationEditCommands
        {
            UNKNOWN = -1,
            ADDSOURCE = 0x00,
            DELETESOURCE, //1
            CHANGESOURCE, //2
            SETUNSIGNEDVALUE, //3
            DIALVALUEAMOUNTOFCHANGE,
            SETQUANTIZE, //5
            TOGGLEQUANTIZE, //6
            SETSIGN //07
        }

        /// <summary>
        /// EnumPages constants
        /// </summary>
        internal enum EnumPages
        {
            UNKNOWN = -1,
            MASTER_X = 0x00,
            TUNE_X = 0x01,
            CHAIN = 0x02,
            MASTER_MISC_M12 = 0x03,
            GATE_XP_X = 0x03,
            VOICE_ON_OFF = 0x04,
            MIDI = 0x05,
            CASSETTE = 0x06,
            SEND_CASSETTE = 0x07,
            CHECK_CASSETTE = 0x08,
            FROM_CASSETTE = 0x09,
            LOAD_ALL = 0x0F,
            LOAD_ONE = 0x10,
            LOAD_SINGLE = 0x11,
            LOAD_MULTI = 0x12,
            LOAD_CHAIN = 0x13,
            START_LOAD = 0x14,
            LOAD_GLOBAL = 0x15,
            MIDI_BASIC_CHANNEL = 0x16,
            MIDI_CONTROLLERS = 0x17,
            MIDI_ENABLES = 0x18,
            MIDI_SEND_PATCH = 0x19,
            MIDI_RESET = 0x1A,
            MIDI_MUTE = 0x1B,
            VERSION_X = 0x1D,
            VCO_1_X = 0x20,
            VCO_2_X = 0x21,
            VCF_VCA_X = 0x22,
            FM_LAG_X = 0x23,

            ENV_1 = 0x28,
            ENV_2 = 0x29,
            ENV_3 = 0x2A,
            ENV_4 = 0x2B,
            ENV_5 = 0x2C,
            LFO_1 = 0x30,
            LFO_2 = 0x31,
            LFO_3 = 0x32,
            LFO_4 = 0x33,
            LFO_5 = 0x34,
            TRACK_1 = 0x38,
            TRACK_2 = 0x39,
            TRACK_3 = 0x3A,
            RAMP_1 = 0x40,
            RAMP_2 = 0x41,
            RAMP_3 = 0x42,
            RAMP_4 = 0x43,

            ZONE_1_M12_X = 0x50,
            ZONE_2_M12_X = 0x51,
            ZONE_3_M12_X = 0x52,
            ZONE_4_M12_X = 0x53,
            ZONE_5_M12_X = 0x54,
            MISC_NAME_M12 = 0x58,
            NAME_XP = 0x59,
            TRANSPOSE = 0x60,
            PAN = 0x61,
            VASSIGN_CVMIDI = 0x62,
            ZONE_XP = 0x63,
            ZONE_INPUT_XP = 0x64,
            ZONE_LIMITS_XP = 0x65,
            ZONE_MODE_XP = 0x66,
            VOLUME = 0x67,
            VIB = 0x68,
            NAME_M12 = 0x69,
            DETUNE = 0x6A,
            MOD_ROUTINGS = 0x6B
        }

        /// <summary>
        /// Rotary encoders
        /// </summary>
        internal enum EnumRotaryEncoders
        {
            FIRST = 0x18,
            SECOND = 0x19,
            THIRD = 0x1A,
            FOURTH = 0x1B,
            FIFTH = 0x1C,
            SIXTH = 0X1D
        }

        /// <summary>
        /// Initializes the <see cref="XpanderConstants"/> class.
        /// </summary>
        static XpanderConstants()
        {
            InitializeModulationDestinationForPageSubPage();
        }

        /// <summary>
        /// EnumPages enum type
        /// </summary>
        internal static Type PagesType
        {
            get
            {
                return typeof(XpanderConstants.EnumPages);
            }
        }

        internal static Type ModulationSourcesType
        {
            get
            {
                return typeof(XpanderConstants.EnumModulationSources);
            }
        }

        // Modulation source for mod matrix
        internal static Type ModulationSourcesModMatrixType
        {
            get
            {
                return typeof(XpanderConstants.EnumModulationSourcesModMatrix);
            }
        }

        internal static Type ModulationDestinationType
        {
            get
            {
                return typeof(XpanderConstants.EnumModulationDestinations);
            }
        }

        internal static Type ModulationEditCommandsType
        {
            get
            {
                return typeof(XpanderConstants.EnumModulationEditCommands);
            }
        }

        internal static Type LFOTriggerSourceType
        {
            get
            {
                return typeof(XpanderConstants.EnumLFOTriggerSources);
            }
        }

        internal static Type LFORetrigModesType
        {
            get
            {
                return typeof(XpanderConstants.EnumLFORetrigModes);
            }
        }

        internal static Type VCFFilterModesType
        {
            get
            {
                return typeof(XpanderConstants.EnumVCFFilterModes);
            }
        }

        internal static Type LFOWaveShapesType
        {
            get
            {
                return typeof(XpanderConstants.EnumLFOWaveShapes);
            }
        }
    }

    #region
    #region Enum for randomisation

    /// <summary>
    /// VCO additional randomization
    /// </summary>
    [Flags]
    public enum EnumRandomVCO2
    {
        EnableFM = 1,
        EnableNoise = 2,
        EnableSync = 4
    }

    /// <summary>
    /// VCO frequency randomization
    /// </summary>
    public enum EnumRandomVCOFreq
    {
        // undefined
        Free = 0,

        SameNote,

        // third..etc..
        Third,

        Fifth,
        Seventh,
        Octave,
        Ninth,
        Eleventh,
        Thirteenth
    }

    /// <summary>
    /// Detune randomization
    /// </summary>
    public enum EnumRandomVCODetune
    {
        Free = 0,

        // little detune
        Digital,

        // analog style
        Analog
    }

    public enum EnumRandomVCAEnv
    {
        //free
        Free = 0,

        // organ type |--|
        Organ,

        // string type /--\
        String,

        // percusive |-|
        Percusive,

        //percusive with release |-\
        PercusiveWithRelease
    }

    [Flags]
    public enum EnumRandomModMatrix
    {
        EnableAmount = 1,
        EnableSourcesAndDestinations = 2,
        EnableQuantize = 4
    }

    #endregion Enum for randomisation
    #endregion
}