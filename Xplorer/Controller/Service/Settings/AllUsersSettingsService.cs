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
using MidiApp.MidiController.Service;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net.NetworkInformation;
using System.Windows.Forms;
using System.Xml.Serialization;
using Xplorer.Common;

namespace Xplorer.Controller.Service.Settings
{
    /// <summary>
    /// A class to handle settings for all users without going into M$ settings customization nightmare
    /// </summary>
    public static class AllUsersSettingsService
    {
        /// <summary>
        /// All users settings file name
        /// </summary>
        private static readonly string _settingsFileName = "xplorer.users.config";

        private static System.Drawing.Color _defaultKnobColor = System.Drawing.Color.FromArgb(102, 181, 227);
        private static AllUsersSettings _allUserSettings = null;

        /// <summary>
        /// return an instance of defaut settings
        /// </summary>
        private static AllUsersSettings DefaultAllusersSettings
        {
            get
            {
                return new AllUsersSettings()
                {
                    MidiConfig = new AllUsersSettings.MidiConfiguration
                    {
                        // MIDI
                        EditingProgramNumber = XpanderConstants.SINGLE_TONES_MAX_COUNT - 1,
                        MidiChannel = 1,
                        SynthTypeIsMatrix12 = false,
                        SysexTransmitDelay = 30,
                        SmartAllNotesOff = true,
                        AutomationTable = new List<string>
                        {
                           "VCO1_FREQ;128",
                           "VCO1_DETUNE;128",
                           "VCO1_PW;128",
                           "VCO1_VOLUME;128",
                           "VCO1_WAVESHAPE_TRI;128",
                           "VCO1_WAVESHAPE_SAW;128",
                           "VCO1_WAVESHAPE_PULSE;128",
                           "VCO1_MOD_KEYB;128",
                           "VCO1_MOD_LAG;128",
                           "VCO1_MOD_LEV1;128",
                           "VCO1_MOD_VIB;128",
                           "VCO2_FREQ;128",
                           "VCO2_DETUNE;128",
                           "VCO2_PW;128",
                           "VCO2_VOLUME;128",
                           "VCO2_WAVESHAPE_TRI;128",
                           "VCO2_WAVESHAPE_SAW;128",
                           "VCO2_WAVESHAPE_PULSE;128",
                           "VCO2_WAVESHAPE_NOISE;128",
                           "VCO2_MOD_KEYB;128",
                           "VCO2_MOD_LAG;128",
                           "VCO2_MOD_LEV1;128",
                           "VCO2_MOD_VIB;128",
                           "VCO2_WAVE_SYNC;128",
                           "VCF_FREQ;128",
                           "VCF_RES;128",
                           "VCF_MODE;128",
                           "VCF_VCA1_VOLUME;128",
                           "VCF_VCA2_VOLUME;128",
                           "VCF_MOD_KEYB;128",
                           "VCF_MOD_LAG;128",
                           "VCF_MOD_LEV1;128",
                           "VCF_MOD_VIB;128",
                           "FM_AMP;128",
                           "FM_DESTINATION;128",
                           "LAG_IN;128",
                           "FMLAG_RATE;128",
                           "LAG_MODE_LEGATO;128",
                           "LAG_TIMING_LINEAR_EXPO;128",
                           "LAG_LINEAR_EQUAL_TIME;128",
                           "TRACK_1_IN;128",
                           "TRACK_1_POINT_1;128",
                           "TRACK_1_POINT_2;128",
                           "TRACK_1_POINT_3;128",
                           "TRACK_1_POINT_4;128",
                           "TRACK_1_POINT_5;128",
                           "TRACK_2_IN;128",
                           "TRACK_2_POINT_1;128",
                           "TRACK_2_POINT_2;128",
                           "TRACK_2_POINT_3;128",
                           "TRACK_2_POINT_4;128",
                           "TRACK_2_POINT_5;128",
                           "TRACK_3_IN;128",
                           "TRACK_3_POINT_1;128",
                           "TRACK_3_POINT_2;128",
                           "TRACK_3_POINT_3;128",
                           "TRACK_3_POINT_4;128",
                           "TRACK_3_POINT_5;128",
                           "ENV_1_DELAY;128",
                           "ENV_1_ATTACK;128",
                           "ENV_1_DECAY;128",
                           "ENV_1_SUSTAIN;128",
                           "ENV_1_RELEASE;128",
                           "ENV_1_VOLUME;128",
                           "ENV_1_MODE_RESET;128",
                           "ENV_1_MODE_FREERUN;128",
                           "ENV_1_MODE_DADR;128",
                           "ENV_1_TRIG_SINGLE_MULTI;128",
                           "ENV_1_TRIG_EXTRIG;128",
                           "ENV_1_TRIG_LFOTRIG;128",
                           "ENV_1_TRIG_LFO_SOURCE;128",
                           "ENV_1_TRIG_GATED;128",
                           "ENV_2_DELAY;128",
                           "ENV_2_ATTACK;128",
                           "ENV_2_DECAY;128",
                           "ENV_2_SUSTAIN;128",
                           "ENV_2_RELEASE;128",
                           "ENV_2_VOLUME;128",
                           "ENV_2_MODE_RESET;128",
                           "ENV_2_MODE_FREERUN;128",
                           "ENV_2_MODE_DADR;128",
                           "ENV_2_TRIG_SINGLE_MULTI;128",
                           "ENV_2_TRIG_EXTRIG;128",
                           "ENV_2_TRIG_LFOTRIG;128",
                           "ENV_2_TRIG_LFO_SOURCE;128",
                           "ENV_2_TRIG_GATED;128",
                           "ENV_3_DELAY;128",
                           "ENV_3_ATTACK;128",
                           "ENV_3_DECAY;128",
                           "ENV_3_SUSTAIN;128",
                           "ENV_3_RELEASE;128",
                           "ENV_3_VOLUME;128",
                           "ENV_3_MODE_RESET;128",
                           "ENV_3_MODE_FREERUN;128",
                           "ENV_3_MODE_DADR;128",
                           "ENV_3_TRIG_SINGLE_MULTI;128",
                           "ENV_3_TRIG_EXTRIG;128",
                           "ENV_3_TRIG_LFOTRIG;128",
                           "ENV_3_TRIG_LFO_SOURCE;128",
                           "ENV_3_TRIG_GATED;128",
                           "ENV_4_DELAY;128",
                           "ENV_4_ATTACK;128",
                           "ENV_4_DECAY;128",
                           "ENV_4_SUSTAIN;128",
                           "ENV_4_RELEASE;128",
                           "ENV_4_VOLUME;128",
                           "ENV_4_MODE_RESET;128",
                           "ENV_4_MODE_FREERUN;128",
                           "ENV_4_MODE_DADR;128",
                           "ENV_4_TRIG_SINGLE_MULTI;128",
                           "ENV_4_TRIG_EXTRIG;128",
                           "ENV_4_TRIG_LFOTRIG;128",
                           "ENV_4_TRIG_LFO_SOURCE;128",
                           "ENV_4_TRIG_GATED;128",
                           "ENV_5_DELAY;128",
                           "ENV_5_ATTACK;128",
                           "ENV_5_DECAY;128",
                           "ENV_5_SUSTAIN;128",
                           "ENV_5_RELEASE;128",
                           "ENV_5_VOLUME;128",
                           "ENV_5_MODE_RESET;128",
                           "ENV_5_MODE_FREERUN;128",
                           "ENV_5_MODE_DADR;128",
                           "ENV_5_TRIG_SINGLE_MULTI;128",
                           "ENV_5_TRIG_EXTRIG;128",
                           "ENV_5_TRIG_LFOTRIG;128",
                           "ENV_5_TRIG_LFO_SOURCE;128",
                           "ENV_5_TRIG_GATED;128",
                           "LFO_1_SPEED;128",
                           "LFO_1_WAVESHAPE;128",
                           "LFO_1_SAMPLE_INPUT;128",
                           "LFO_1_RETRIG;128",
                           "LFO_1_AMP;128",
                           "LFO_1_LAG;128",
                           "LFO_1_RETRIG_MODE;128",
                           "LFO_2_SPEED;128",
                           "LFO_2_WAVESHAPE;128",
                           "LFO_2_SAMPLE_INPUT;128",
                           "LFO_2_RETRIG;128",
                           "LFO_2_AMP;128",
                           "LFO_2_LAG;128",
                           "LFO_2_RETRIG_MODE;128",
                           "LFO_3_SPEED;128",
                           "LFO_3_WAVESHAPE;128",
                           "LFO_3_SAMPLE_INPUT;128",
                           "LFO_3_RETRIG;128",
                           "LFO_3_AMP;128",
                           "LFO_3_LAG;128",
                           "LFO_3_RETRIG_MODE;128",
                           "LFO_4_SPEED;128",
                           "LFO_4_WAVESHAPE;128",
                           "LFO_4_SAMPLE_INPUT;128",
                           "LFO_4_RETRIG;128",
                           "LFO_4_AMP;128",
                           "LFO_4_LAG;128",
                           "LFO_4_RETRIG_MODE;128",
                           "LFO_5_SPEED;128",
                           "LFO_5_WAVESHAPE;128",
                           "LFO_5_SAMPLE_INPUT;128",
                           "LFO_5_RETRIG;128",
                           "LFO_5_AMP;128",
                           "LFO_5_LAG;128",
                           "LFO_5_RETRIG_MODE;128",
                           "RAMP_1_RATE;128",
                           "RAMP_1_TRIG_SINGLE_MULTI;128",
                           "RAMP_1_TRIG_EXTRIG;128",
                           "RAMP_1_TRIG_LFOTRIG;128",
                           "RAMP_1_TRIG_LFO_SOURCE;128",
                           "RAMP_1_TRIG_GATED;128",
                           "RAMP_2_RATE;128",
                           "RAMP_2_TRIG_SINGLE_MULTI;128",
                           "RAMP_2_TRIG_EXTRIG;128",
                           "RAMP_2_TRIG_LFOTRIG;128",
                           "RAMP_2_TRIG_LFO_SOURCE;128",
                           "RAMP_2_TRIG_GATED;128",
                           "RAMP_3_RATE;128",
                           "RAMP_3_TRIG_SINGLE_MULTI;128",
                           "RAMP_3_TRIG_EXTRIG;128",
                           "RAMP_3_TRIG_LFOTRIG;128",
                           "RAMP_3_TRIG_LFO_SOURCE;128",
                           "RAMP_3_TRIG_GATED;128",
                           "RAMP_4_RATE;128",
                           "RAMP_4_TRIG_SINGLE_MULTI;128",
                           "RAMP_4_TRIG_EXTRIG;128",
                           "RAMP_4_TRIG_LFOTRIG;128",
                           "RAMP_4_TRIG_LFO_SOURCE;128",
                           "RAMP_4_TRIG_GATED;128"
                        }
                    },
                    UiConfig = new AllUsersSettings.UiConfiguration
                    {
                        KnobLedBorderColor = _defaultKnobColor.ToArgb(),
                        KnobMovementIsLinear = true
                    },

                    RandomizerConfig = GetRandomizerConfigurationDefault()
                }; //AllUsersSettings
            } //get
        }

        /// <summary>
        /// DefaultRandomizer configuration
        /// </summary>
        /// <returns></returns>
        public static AllUsersSettings.RandomizerConfiguration GetRandomizerConfigurationDefault()
        {
            return new AllUsersSettings.RandomizerConfiguration()
            {
                // random everything
                VCOFreq = EnumRandomVCOFreq.Free,
                VCODetune = EnumRandomVCODetune.Free,
                VCO2FmNoiseSync =
                    EnumRandomVCO2.EnableFM |
                    EnumRandomVCO2.EnableNoise |
                    EnumRandomVCO2.EnableSync,
                VCA2Env = EnumRandomVCAEnv.Free,
                ModulationMatrix =
                    EnumRandomModMatrix.EnableAmount |
                    EnumRandomModMatrix.EnableSourcesAndDestinations |
                    EnumRandomModMatrix.EnableQuantize,
            };
        }

        /// <summary>
        /// Returns the instance of AllUsersSettings
        /// If an error occurs while gettings the settings, default settings are returned
        /// </summary>
        public static AllUsersSettings AllUsersSettings
        {
            get
            {
                if (_allUserSettings == null)
                {
                    _allUserSettings = LoadSettings();
                    if (_allUserSettings == null ||
                        // Xplorer legacy version settings
                        _allUserSettings != null && (_allUserSettings.MidiConfig == null || _allUserSettings.RandomizerConfig == null || _allUserSettings.UiConfig == null))
                    {
                        Logger.WriteLine("AllUsersSettingsService.AllUsersSettings", TraceLevel.Warning, "Unable to load settings, creating a new default values");
                        ResetSettings();
                        _allUserSettings = LoadSettings();
                    }
                }
                return _allUserSettings;
            }
        }

        /// <summary>
        /// Reset (overwrite) the settings to their default values
        /// </summary>
        public static void ResetSettings()
        {
            SaveSettings(DefaultAllusersSettings);
            _allUserSettings = null;
        }

        /// <summary>
        /// Load the settings from the configuration file
        /// </summary>
        /// <returns></returns>
        private static AllUsersSettings LoadSettings()
        {
            AllUsersSettings settings = null;
            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(AllUsersSettings));
                using (FileStream fs = new FileStream(Path.Combine(Application.CommonAppDataPath, _settingsFileName), FileMode.Open))
                {
                    settings = (AllUsersSettings)serializer.Deserialize(fs);
                }
            }
            catch (Exception e)
            {
                // whatever the error, return null
                Logger.WriteLine("AllUsersSettingsService.LoadSettings", TraceLevel.Error, e.Message);
            }
            return settings;
        }

        /// <summary>
        /// Save the settings
        /// </summary>
        /// <param name="settings">The settings.</param>
        public static void SaveSettings(AllUsersSettings settings)
        {
            Debug.Assert(settings != null);
            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(AllUsersSettings));
                using (StreamWriter writer = new StreamWriter(Path.Combine(Application.CommonAppDataPath, _settingsFileName)))
                {
                    serializer.Serialize(writer, settings);
                }
            }
            catch (Exception e)
            {
                // whatever the error, return null
                Logger.WriteLine("AllUsersSettingsService.SaveSettings", TraceLevel.Error, e.Message);
            }
        }
    }
}