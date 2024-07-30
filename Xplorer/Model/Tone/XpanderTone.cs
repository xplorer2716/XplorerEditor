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

using MidiApp.MidiController.Model;
using MidiApp.MidiController.Service;
using Sanford.Multimedia.Midi;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using Xplorer.Common;
using Xplorer.Model.Tone;

namespace Xplorer.Model
{
    /// <summary>
    /// A Xpander Tone
    /// </summary>
    internal sealed partial class XpanderTone : AbstractTone
    {
        // parameter map of the tone
        private OrderedDictionary _parameterMap = new OrderedDictionary();

        // names of parameters in the parameters map than can be used for tone morphing
        // this is initialized right at the end of the initialization of the parameters map
        private HashSet<string> _parametersNamesForToneMorphing = null;

        /// <summary>
        /// The parameter map of the tone
        /// </summary>
        public override OrderedDictionary ParameterMap
        { get { return _parameterMap; } }

        private string _toneName = null;

        /// <summary>
        /// returns/set the name of the tone. Length is fixed/padded to ToneConstants.TONE_NAME_LENGTH
        /// </summary>
        public override string ToneName
        {
            get { return _toneName; }
            set
            {
                if (value.Length > XpanderConstants.TONE_NAME_LENGTH)
                {
                    _toneName = value.Substring(0, XpanderConstants.TONE_NAME_LENGTH);
                }
                else
                {
                    _toneName = value;
                    _toneName = _toneName.PadRight(XpanderConstants.TONE_NAME_LENGTH, ' ');
                }
            }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="XpanderTone"/> class.
        /// </summary>
        public XpanderTone()
            : base()
        {
            _toneName = "XPLORER";
            ClearModulationMatrix();
        }

        #region SYSEXTONE

        /// <summary>
        /// returns a byte array ready for serialization. Program number is forced to tone's CurrentProgramNumber
        /// </summary>
        /// <returns></returns>
        internal byte[] ToByteArray()
        {
            XPanderSinglePatch patch = this.ToSinglePatch();
            // force program number
            return patch.ToByteArray(this.EditingProgramNumber);
        }

        /// <summary>
        /// Initialize tone from the stream. Stream position must match first VCO byte
        /// </summary>
        /// <param name="stream">The stream.</param>
        internal void FromStream(System.IO.MemoryStream stream)
        {
            XPanderSinglePatch patch = new XPanderSinglePatch();
            patch.FromStream(stream);
            this.FromSinglePatch(patch);
        }

        /// <summary>
        /// Initialize tone from the byte array.
        /// </summary>
        /// <param name="data">The data.</param>
        internal void FromByteArray(byte[] data)
        {
            XPanderSinglePatch patch = new XPanderSinglePatch();
            patch.FromByteArray(data);
            this.FromSinglePatch(patch);
        }

        /// <summary>
        /// Gets the name from byte array.
        /// </summary>
        /// <param name="bytes">The bytes.</param>
        /// <returns></returns>
        internal static string GetNameFromByteArray(byte[] bytes)
        {
            return XPanderSinglePatch.GetNameFromByteArray(bytes);
        }

        /// <summary>
        /// updates tone parameters with
        /// </summary>
        /// <param name="singlePatch">The single patch.</param>
        private void FromSinglePatch(XPanderSinglePatch singlePatch)
        {
            //reset all parameters and mod matrix
            foreach (AbstractParameter parameter in ParameterMap.Values)
            {
                if ((parameter.MinValue < 0) && (0 < parameter.MaxValue))
                {
                    parameter.Value = 0;
                }
                else
                {
                    parameter.Value = parameter.MinValue;
                }
            }
            ClearModulationMatrix();

            string sParam = null;

            #region VCOs

            for (int i = 0; i < XpanderConstants.VCO_COUNT; i++)
            {
                sParam = string.Format("VCO{0}_FREQ", i + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._vcos[i].freq;
                sParam = string.Format("VCO{0}_DETUNE", i + 1);
                ((XpanderSignedParameter)_parameterMap[sParam]).Value = singlePatch._vcos[i].detune;
                sParam = string.Format("VCO{0}_PW", i + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._vcos[i].pw;

                sParam = string.Format("VCO{0}_VOLUME", i + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._vcos[i].vol;

                // waveshape masks
                sParam = string.Format("VCO{0}_WAVESHAPE_TRI", i + 1);
                if ((singlePatch._vcos[i].wave & (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_TRI) == (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_TRI)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                sParam = string.Format("VCO{0}_WAVESHAPE_SAW", i + 1);
                if ((singlePatch._vcos[i].wave & (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_SAW) == (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_SAW)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                sParam = string.Format("VCO{0}_WAVESHAPE_PULSE", i + 1);
                if ((singlePatch._vcos[i].wave & (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_PULSE) == (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_PULSE)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                // noise only for VCO2
                if ((i + 1) == 2)
                {
                    sParam = string.Format("VCO{0}_WAVESHAPE_NOISE", i + 1);
                    if ((singlePatch._vcos[i].wave & (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_NOISE) == (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_NOISE)
                    {
                        ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                    }
                }

                // sync only for VCO2 (shown as mod on the xpander, but is stored into wave)
                if ((i + 1) == 2)
                {
                    sParam = string.Format("VCO{0}_WAVE_SYNC", i + 1);
                    if ((singlePatch._vcos[i].wave & (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_SYNC) == (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_SYNC)
                    {
                        ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                    }
                    else
                    {
                        ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                    }
                }
                //VCO mods
                sParam = string.Format("VCO{0}_MOD_KEYB", i + 1);
                if ((singlePatch._vcos[i].mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_KEYBD) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_KEYBD)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("VCO{0}_MOD_LAG", i + 1);
                if ((singlePatch._vcos[i].mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LAG) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LAG)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("VCO{0}_MOD_LEV1", i + 1);
                if ((singlePatch._vcos[i].mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LEV_1) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LEV_1)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("VCO{0}_MOD_VIB", i + 1);
                if ((singlePatch._vcos[i].mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_VIB) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_VIB)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
            }

            #endregion VCOs

            #region VCF_VCA

            ((XpanderParameter)_parameterMap["VCF_FREQ"]).Value = singlePatch._vcf.freq;
            ((XpanderParameter)_parameterMap["VCF_RES"]).Value = singlePatch._vcf.res;
            ((XpanderParameter)_parameterMap["VCF_MODE"]).Value = singlePatch._vcf.fmode;
            ((XpanderParameter)_parameterMap["VCF_VCA1_VOLUME"]).Value = singlePatch._vcf.vca1;
            ((XpanderParameter)_parameterMap["VCF_VCA2_VOLUME"]).Value = singlePatch._vcf.vca2;

            sParam = "VCF_MOD_KEYB";
            if ((singlePatch._vcf.mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_KEYBD) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_KEYBD)
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 1;
            }
            else
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 0;
            }
            sParam = "VCF_MOD_LAG";
            if ((singlePatch._vcf.mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LAG) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LAG)
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 1;
            }
            else
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 0;
            }
            sParam = "VCF_MOD_LEV1";
            if ((singlePatch._vcf.mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LEV_1) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LEV_1)
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 1;
            }
            else
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 0;
            }
            sParam = "VCF_MOD_VIB";
            if ((singlePatch._vcf.mod & (byte)XpanderConstants.EnumModulationFlags.MODFLAG_VIB) == (byte)XpanderConstants.EnumModulationFlags.MODFLAG_VIB)
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 1;
            }
            else
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 0;
            }

            #endregion VCF_VCA

            #region FM_LAG

            ((XpanderParameter)_parameterMap["FM_AMP"]).Value = singlePatch._fmAndLag.f_amp;
            ((XpanderParameter)_parameterMap["FM_DESTINATION"]).Value = singlePatch._fmAndLag.f_dest;
            ((XpanderParameter)_parameterMap["LAG_IN"]).Value = singlePatch._fmAndLag.lag_in;
            ((XpanderParameter)_parameterMap["FMLAG_RATE"]).Value = singlePatch._fmAndLag.lag_rate;

            sParam = "LAG_MODE_LEGATO";
            if ((singlePatch._fmAndLag.lag_mode & (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_LEGATO) == (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_LEGATO)
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 1;
            }
            else
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 0;
            }
            sParam = "LAG_TIMING_LINEAR_EXPO";
            if ((singlePatch._fmAndLag.lag_mode & (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_EXPO) == (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_EXPO)
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 1;
            }
            else
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 0;
            }
            sParam = "LAG_LINEAR_EQUAL_TIME";
            if ((singlePatch._fmAndLag.lag_mode & (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_EQUAL_TIME) == (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_EQUAL_TIME)
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 1;
            }
            else
            {
                ((XpanderParameter)_parameterMap[sParam]).Value = 0;
            }

            #endregion FM_LAG

            #region TRACK_X

            for (int iTrackNumber = 0; iTrackNumber < XpanderConstants.TRACK_COUNT; iTrackNumber++)
            {
                sParam = string.Format("TRACK_{0}_IN", iTrackNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._tracks[iTrackNumber].input;

                for (int iPointNumber = 0; iPointNumber < XpanderConstants.TRACK_POINTS_COUNTS; iPointNumber++)
                {
                    sParam = string.Format("TRACK_{0}_POINT_{1}", iTrackNumber + 1, iPointNumber + 1);
                    ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._tracks[iTrackNumber].points[iPointNumber];
                }
            }

            #endregion TRACK_X

            #region ENV_X

            for (int iEnvNumber = 0; iEnvNumber < XpanderConstants.ENV_COUNT; iEnvNumber++)
            {
                sParam = string.Format("ENV_{0}_DELAY", iEnvNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._enveloppes[iEnvNumber].delay;
                sParam = string.Format("ENV_{0}_ATTACK", iEnvNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._enveloppes[iEnvNumber].attack;
                sParam = string.Format("ENV_{0}_DECAY", iEnvNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._enveloppes[iEnvNumber].decay;
                sParam = string.Format("ENV_{0}_SUSTAIN", iEnvNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._enveloppes[iEnvNumber].sustain;
                sParam = string.Format("ENV_{0}_RELEASE", iEnvNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._enveloppes[iEnvNumber].release;
                sParam = string.Format("ENV_{0}_VOLUME", iEnvNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._enveloppes[iEnvNumber].amp;

                //mod
                sParam = string.Format("ENV_{0}_MODE_RESET", iEnvNumber + 1);
                if ((singlePatch._enveloppes[iEnvNumber].flags & (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_RESET) == (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_RESET)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("ENV_{0}_MODE_FREERUN", iEnvNumber + 1);
                if ((singlePatch._enveloppes[iEnvNumber].flags & (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_FREERUN) == (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_FREERUN)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("ENV_{0}_MODE_DADR", iEnvNumber + 1);
                if ((singlePatch._enveloppes[iEnvNumber].flags & (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_DADR) == (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_DADR)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("ENV_{0}_TRIG_SINGLE_MULTI", iEnvNumber + 1);
                if ((singlePatch._enveloppes[iEnvNumber].flags & (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_MULTI) == (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_MULTI)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("ENV_{0}_TRIG_EXTRIG", iEnvNumber + 1);
                if ((singlePatch._enveloppes[iEnvNumber].flags & (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_EXTRIG) == (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_EXTRIG)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("ENV_{0}_TRIG_LFOTRIG", iEnvNumber + 1);
                if ((singlePatch._enveloppes[iEnvNumber].flags & (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_LFOTRIG) == (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_LFOTRIG)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }

                sParam = string.Format("ENV_{0}_TRIG_LFO_SOURCE", iEnvNumber + 1);
                // do not set lfotrig _value if flags do not set lfo_trig. some patch can have a dummy _value
                if (((XpanderParameter)_parameterMap[string.Format("ENV_{0}_TRIG_LFOTRIG", iEnvNumber + 1)]).Value == 0)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = (int)XpanderConstants.EnumLFOTriggerSources.LFO1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._enveloppes[iEnvNumber].lfotrig;
                }

                sParam = string.Format("ENV_{0}_TRIG_GATED", iEnvNumber + 1);
                if ((singlePatch._enveloppes[iEnvNumber].flags & (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_GATED) == (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_GATED)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
            }

            #endregion ENV_X

            #region LFO_X

            for (int iLFONumber = 0; iLFONumber < XpanderConstants.LFO_COUNT; iLFONumber++)
            {
                sParam = string.Format("LFO_{0}_SPEED", iLFONumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._lfos[iLFONumber].speed;
                sParam = string.Format("LFO_{0}_WAVESHAPE", iLFONumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._lfos[iLFONumber].wave;
                sParam = string.Format("LFO_{0}_SAMPLE_INPUT", iLFONumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._lfos[iLFONumber].sample;
                sParam = string.Format("LFO_{0}_RETRIG", iLFONumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._lfos[iLFONumber].retrig;
                sParam = string.Format("LFO_{0}_AMP", iLFONumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._lfos[iLFONumber].amp;
                //subpage
                sParam = string.Format("LFO_{0}_LAG", iLFONumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._lfos[iLFONumber].lag;

                sParam = string.Format("LFO_{0}_RETRIG_MODE", iLFONumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._lfos[iLFONumber].retrig_mode;
            }

            #endregion LFO_X

            #region RAMP_X

            for (int iRampNumber = 0; iRampNumber < XpanderConstants.RAMP_COUNT; iRampNumber++)
            {
                sParam = string.Format("RAMP_{0}_RATE", iRampNumber + 1);
                ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._ramps[iRampNumber].rate;

                //input
                sParam = string.Format("RAMP_{0}_TRIG_SINGLE_MULTI", iRampNumber + 1);
                if ((singlePatch._ramps[iRampNumber].flags & (byte)XpanderConstants.EnumRampFlags.RAMPF_MULTI) == (byte)XpanderConstants.EnumRampFlags.RAMPF_MULTI)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("RAMP_{0}_TRIG_EXTRIG", iRampNumber + 1);
                if ((singlePatch._ramps[iRampNumber].flags & (byte)XpanderConstants.EnumRampFlags.RAMPF_EXTRIG) == (byte)XpanderConstants.EnumRampFlags.RAMPF_EXTRIG)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
                sParam = string.Format("RAMP_{0}_TRIG_LFOTRIG", iRampNumber + 1);
                if ((singlePatch._ramps[iRampNumber].flags & (byte)XpanderConstants.EnumRampFlags.RAMPF_LFOTRIG) == (byte)XpanderConstants.EnumRampFlags.RAMPF_LFOTRIG)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }

                sParam = string.Format("RAMP_{0}_TRIG_LFO_SOURCE", iRampNumber + 1);
                // do not set lfotrig _value if flags do not set lfo_trig. some patch can have a dummy _value
                if (((XpanderParameter)_parameterMap[string.Format("ENV_{0}_TRIG_LFOTRIG", iRampNumber + 1)]).Value == 0)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = (int)XpanderConstants.EnumLFOTriggerSources.LFO1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = singlePatch._ramps[iRampNumber].lfotrig;
                }

                sParam = string.Format("RAMP_{0}_TRIG_GATED", iRampNumber + 1);
                if ((singlePatch._ramps[iRampNumber].flags & (byte)XpanderConstants.EnumRampFlags.RAMPF_GATED) == (byte)XpanderConstants.EnumRampFlags.RAMPF_GATED)
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 1;
                }
                else
                {
                    ((XpanderParameter)_parameterMap[sParam]).Value = 0;
                }
            }

            #endregion RAMP_X

            #region MODULATION_MATRIX

            // we don't know if the local matrix reflects the state of the mod matrix of the xpander.
            // so reset for all entries. and set the values from the sysexTone. Caller will have to send the
            // full sysextone to override the mod matrix into the xpander. that's ugly, but we won't
            // send here the 20x47 source deletions. it's useless.

            // and set the modulation matrix to have the same as in the sysex
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                if (
                    // avoid unused mod entries
                    (singlePatch._modulationEntries[i].source <= (int)XpanderConstants.EnumModulationSources.LEV2) &&
                    (singlePatch._modulationEntries[i].dest <= (int)XpanderConstants.EnumModulationDestinations.LAG_RATE))
                {
                    ChangeModulationDestination(
                        singlePatch._modulationEntries[i].source,
                        singlePatch._modulationEntries[i].amount,
                        singlePatch._modulationEntries[i].quantize ? 1 : 0,
                        (int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ,
                        singlePatch._modulationEntries[i].dest,
                        i + 1, null);
                }
            }

            // tonename
            ToneName = singlePatch._name.Name;

            #endregion MODULATION_MATRIX
        }

        /// <summary>
        /// converts the tone to single path data
        /// </summary>
        /// <returns></returns>
        private XPanderSinglePatch ToSinglePatch()
        {
            XPanderSinglePatch singlePatch = new XPanderSinglePatch();

            string param = null;

            #region VCOs

            for (int i = 0; i < XpanderConstants.VCO_COUNT; i++)
            {
                param = string.Format("VCO{0}_FREQ", i + 1);
                singlePatch._vcos[i].freq = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("VCO{0}_DETUNE", i + 1);
                singlePatch._vcos[i].detune = (sbyte)((XpanderSignedParameter)_parameterMap[param]).Value;
                param = string.Format("VCO{0}_PW", i + 1);
                singlePatch._vcos[i].pw = (byte)((XpanderParameter)_parameterMap[param]).Value;

                param = string.Format("VCO{0}_VOLUME", i + 1);
                singlePatch._vcos[i].vol = (byte)((XpanderParameter)_parameterMap[param]).Value;

                // WAVESHAPES MASK
                singlePatch._vcos[i].wave = 0;
                param = string.Format("VCO{0}_WAVESHAPE_TRI", i + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._vcos[i].wave |= (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_TRI;
                }
                param = string.Format("VCO{0}_WAVESHAPE_SAW", i + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._vcos[i].wave |= (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_SAW;
                }
                param = string.Format("VCO{0}_WAVESHAPE_PULSE", i + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._vcos[i].wave |= (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_PULSE;
                }

                // noise only for VCO2
                if ((i + 1) == 2)
                {
                    param = string.Format("VCO{0}_WAVESHAPE_NOISE", i + 1);
                    if (((XpanderParameter)_parameterMap[param]).Value == 1)
                    {
                        singlePatch._vcos[i].wave |= (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_NOISE;
                    }
                }
                // sync only for VCO2 (shown as mod on the xpander, but is stored into wave)
                if ((i + 1) == 2)
                {
                    param = string.Format("VCO{0}_WAVE_SYNC", i + 1);
                    if (((XpanderParameter)_parameterMap[param]).Value == 1)
                    {
                        singlePatch._vcos[i].wave |= (byte)XpanderConstants.EnumVCOWaveFlags.VCOWAVEFLAG_SYNC;
                    }
                }

                //VCO MASK
                singlePatch._vcos[i].mod = 0;
                param = string.Format("VCO{0}_MOD_KEYB", i + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._vcos[i].mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_KEYBD;
                }
                param = string.Format("VCO{0}_MOD_LAG", i + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._vcos[i].mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LAG;
                }
                param = string.Format("VCO{0}_MOD_LEV1", i + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._vcos[i].mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LEV_1;
                }
                param = string.Format("VCO{0}_MOD_VIB", i + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._vcos[i].mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_VIB;
                }
            }

            #endregion VCOs

            #region VCF_VCA

            singlePatch._vcf.freq = (byte)((XpanderParameter)_parameterMap["VCF_FREQ"]).Value;
            singlePatch._vcf.res = (byte)((XpanderParameter)_parameterMap["VCF_RES"]).Value;
            singlePatch._vcf.fmode = (byte)((XpanderParameter)_parameterMap["VCF_MODE"]).Value;
            singlePatch._vcf.vca1 = (byte)((XpanderParameter)_parameterMap["VCF_VCA1_VOLUME"]).Value;
            singlePatch._vcf.vca2 = (byte)((XpanderParameter)_parameterMap["VCF_VCA2_VOLUME"]).Value;

            param = "VCF_MOD_KEYB";
            singlePatch._vcf.mod = 0;
            if (((XpanderParameter)_parameterMap[param]).Value == 1)
            {
                singlePatch._vcf.mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_KEYBD;
            }
            param = "VCF_MOD_LAG";
            if (((XpanderParameter)_parameterMap[param]).Value == 1)
            {
                singlePatch._vcf.mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LAG;
            }
            param = "VCF_MOD_LEV1";
            if (((XpanderParameter)_parameterMap[param]).Value == 1)
            {
                singlePatch._vcf.mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_LEV_1;
            }
            param = "VCF_MOD_VIB";
            if (((XpanderParameter)_parameterMap[param]).Value == 1)
            {
                singlePatch._vcf.mod |= (byte)XpanderConstants.EnumModulationFlags.MODFLAG_VIB;
            }

            #endregion VCF_VCA

            #region FM_LAG

            singlePatch._fmAndLag.f_amp = (byte)((XpanderParameter)_parameterMap["FM_AMP"]).Value;
            singlePatch._fmAndLag.f_dest = (byte)((XpanderParameter)_parameterMap["FM_DESTINATION"]).Value;
            singlePatch._fmAndLag.lag_in = (byte)((XpanderParameter)_parameterMap["LAG_IN"]).Value;
            singlePatch._fmAndLag.lag_rate = (byte)((XpanderParameter)_parameterMap["FMLAG_RATE"]).Value;
            //flags

            param = "LAG_MODE_LEGATO";
            singlePatch._fmAndLag.lag_mode = 0;
            if (((XpanderParameter)_parameterMap[param]).Value == 1)
            {
                singlePatch._fmAndLag.lag_mode |= (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_LEGATO;
            }
            param = "LAG_TIMING_LINEAR_EXPO";
            if (((XpanderParameter)_parameterMap[param]).Value == 1)
            {
                singlePatch._fmAndLag.lag_mode |= (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_EXPO;
            }
            param = "LAG_LINEAR_EQUAL_TIME";
            if (((XpanderParameter)_parameterMap[param]).Value == 1)
            {
                singlePatch._fmAndLag.lag_mode |= (byte)XpanderConstants.EnumLagModeFlags.LAGMODE_EQUAL_TIME;
            }

            #endregion FM_LAG

            #region TRACK_X

            for (int iTrackNumber = 0; iTrackNumber < XpanderConstants.TRACK_COUNT; iTrackNumber++)
            {
                param = string.Format("TRACK_{0}_IN", iTrackNumber + 1);
                singlePatch._tracks[iTrackNumber].input = (byte)((XpanderParameter)_parameterMap[param]).Value;

                for (int iPointNumber = 0; iPointNumber < XpanderConstants.TRACK_POINTS_COUNTS; iPointNumber++)
                {
                    param = string.Format("TRACK_{0}_POINT_{1}", iTrackNumber + 1, iPointNumber + 1);
                    singlePatch._tracks[iTrackNumber].points[iPointNumber] = (byte)((XpanderParameter)_parameterMap[param]).Value;
                }
            }

            #endregion TRACK_X

            #region ENV_X

            for (int iEnvNumber = 0; iEnvNumber < XpanderConstants.ENV_COUNT; iEnvNumber++)
            {
                param = string.Format("ENV_{0}_DELAY", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].delay = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("ENV_{0}_ATTACK", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].attack = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("ENV_{0}_DECAY", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].decay = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("ENV_{0}_SUSTAIN", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].sustain = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("ENV_{0}_RELEASE", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].release = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("ENV_{0}_VOLUME", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].amp = (byte)((XpanderParameter)_parameterMap[param]).Value;

                //mod
                param = string.Format("ENV_{0}_MODE_RESET", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].flags = 0;
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._enveloppes[iEnvNumber].flags |= (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_RESET;
                }
                param = string.Format("ENV_{0}_MODE_FREERUN", iEnvNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._enveloppes[iEnvNumber].flags |= (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_FREERUN;
                }
                param = string.Format("ENV_{0}_MODE_DADR", iEnvNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._enveloppes[iEnvNumber].flags |= (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_DADR;
                }
                param = string.Format("ENV_{0}_TRIG_SINGLE_MULTI", iEnvNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._enveloppes[iEnvNumber].flags |= (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_MULTI;
                }
                param = string.Format("ENV_{0}_TRIG_EXTRIG", iEnvNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._enveloppes[iEnvNumber].flags |= (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_EXTRIG;
                }
                param = string.Format("ENV_{0}_TRIG_LFOTRIG", iEnvNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._enveloppes[iEnvNumber].flags |= (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_LFOTRIG;
                }

                param = string.Format("ENV_{0}_TRIG_LFO_SOURCE", iEnvNumber + 1);
                singlePatch._enveloppes[iEnvNumber].lfotrig = (byte)((XpanderParameter)_parameterMap[param]).Value;

                param = string.Format("ENV_{0}_TRIG_GATED", iEnvNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._enveloppes[iEnvNumber].flags |= (byte)XpanderConstants.EnumEnveloppeModeFlags.ENVMODE_GATED;
                }
            }

            #endregion ENV_X

            #region LFO_X

            for (int iLFONumber = 0; iLFONumber < XpanderConstants.LFO_COUNT; iLFONumber++)
            {
                param = string.Format("LFO_{0}_SPEED", iLFONumber + 1);
                singlePatch._lfos[iLFONumber].speed = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("LFO_{0}_WAVESHAPE", iLFONumber + 1);
                singlePatch._lfos[iLFONumber].wave = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("LFO_{0}_SAMPLE_INPUT", iLFONumber + 1);
                singlePatch._lfos[iLFONumber].sample = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("LFO_{0}_RETRIG", iLFONumber + 1);
                singlePatch._lfos[iLFONumber].retrig = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("LFO_{0}_AMP", iLFONumber + 1);
                singlePatch._lfos[iLFONumber].amp = (byte)((XpanderParameter)_parameterMap[param]).Value;
                //subpage
                param = string.Format("LFO_{0}_LAG", iLFONumber + 1);
                singlePatch._lfos[iLFONumber].lag = (byte)((XpanderParameter)_parameterMap[param]).Value;
                param = string.Format("LFO_{0}_RETRIG_MODE", iLFONumber + 1);
                singlePatch._lfos[iLFONumber].retrig_mode = (byte)((XpanderParameter)_parameterMap[param]).Value;
            }

            #endregion LFO_X

            #region TRACK_X

            for (int iRampNumber = 0; iRampNumber < XpanderConstants.RAMP_COUNT; iRampNumber++)
            {
                param = string.Format("RAMP_{0}_RATE", iRampNumber + 1);
                singlePatch._ramps[iRampNumber].rate = (byte)((XpanderParameter)_parameterMap[param]).Value;

                //input
                param = string.Format("RAMP_{0}_TRIG_SINGLE_MULTI", iRampNumber + 1);
                singlePatch._ramps[iRampNumber].flags = 0;
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._ramps[iRampNumber].flags |= (byte)XpanderConstants.EnumRampFlags.RAMPF_MULTI;
                }
                param = string.Format("RAMP_{0}_TRIG_EXTRIG", iRampNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._ramps[iRampNumber].flags |= (byte)XpanderConstants.EnumRampFlags.RAMPF_EXTRIG;
                }
                param = string.Format("RAMP_{0}_TRIG_LFOTRIG", iRampNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._ramps[iRampNumber].flags |= (byte)XpanderConstants.EnumRampFlags.RAMPF_LFOTRIG;
                }

                param = string.Format("RAMP_{0}_TRIG_LFO_SOURCE", iRampNumber + 1);
                singlePatch._ramps[iRampNumber].lfotrig = (byte)((XpanderParameter)_parameterMap[param]).Value;

                param = string.Format("RAMP_{0}_TRIG_GATED", iRampNumber + 1);
                if (((XpanderParameter)_parameterMap[param]).Value == 1)
                {
                    singlePatch._ramps[iRampNumber].flags |= (byte)XpanderConstants.EnumRampFlags.RAMPF_GATED;
                }
            }

            #endregion TRACK_X

            #region MODULATION_MATRIX

            // and set the modulation matrix to have the same as in the sysex
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                if (_modulationMatrix[i].Source == XpanderConstants.EnumModulationSourcesModMatrix.NONE)
                {
                    singlePatch._modulationEntries[i].source = XPanderSinglePatch.UNUSED_ENTRY_SOURCE_VALUE;
                    singlePatch._modulationEntries[i].dest = XPanderSinglePatch.UNUSED_ENTRY_DEST_VALUE;
                }
                else
                {
                    singlePatch._modulationEntries[i].source = (byte)_modulationMatrix[i].Source;
                    singlePatch._modulationEntries[i].dest = (byte)_modulationMatrix[i].Destination;
                }
                singlePatch._modulationEntries[i].amount = (sbyte)_modulationMatrix[i].Amount;
                singlePatch._modulationEntries[i].quantize = (_modulationMatrix[i].Quantize == 1) ? true : false;
            }

            #endregion MODULATION_MATRIX

            singlePatch._name.Name = ToneName;
            return singlePatch;
        }

        #endregion SYSEXTONE

        #region PARAMETERS

        /// <summary>
        /// Initializes the parameter map.
        /// </summary>
        protected override void InitializeParameterMap()
        {
            _parameterMap = new OrderedDictionary();

            #region INITIALIZEPARAMETERMAP_VCO

            //VCO1

            _parameterMap.Add("VCO1_FREQ",
                new XpanderParameter("VCO1_FREQ",
                   (int)XpanderConstants.EnumPages.VCO_1_X, 0x00,
                    0, 63, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x08,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Freq")
                    );
            _parameterMap.Add("VCO1_DETUNE",
                 new XpanderSignedParameter("VCO1_DETUNE",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x00,
                     -31, 31, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Detune")
                     );
            _parameterMap.Add("VCO1_PW",
                 new XpanderParameter("VCO1_PW",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x00,
                     0, 63, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "PW")
                     );
            _parameterMap.Add("VCO1_VOLUME",
                 new XpanderParameter("VCO1_VOLUME",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x00,
                     0, 63, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0D,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Vol")
                     );

            //SUBPAGE 2

            //waveshapes
            _parameterMap.Add("VCO1_WAVESHAPE_TRI",
                 new XpanderParameter("VCO1_WAVESHAPE_TRI",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Tri")
                     );
            _parameterMap.Add("VCO1_WAVESHAPE_SAW",
                 new XpanderParameter("VCO1_WAVESHAPE_SAW",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Saw")
                     );
            _parameterMap.Add("VCO1_WAVESHAPE_PULSE",
                 new XpanderParameter("VCO1_WAVESHAPE_PULSE",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Pulse")
                     );

            //mods
            _parameterMap.Add("VCO1_MOD_KEYB",
                 new XpanderParameter("VCO1_MOD_KEYB",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x01,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Keyb")
                     );
            _parameterMap.Add("VCO1_MOD_LAG",
                 new XpanderParameter("VCO1_MOD_LAG",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x02,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Lag")
                     );
            _parameterMap.Add("VCO1_MOD_LEV1",
                 new XpanderParameter("VCO1_MOD_LEV1",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x03,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Lev1")
                     );
            _parameterMap.Add("VCO1_MOD_VIB",
                 new XpanderParameter("VCO1_MOD_VIB",
                    (int)XpanderConstants.EnumPages.VCO_1_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x04,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Vib")
                     );

            //VCO2

            _parameterMap.Add("VCO2_FREQ",
                new XpanderParameter("VCO2_FREQ",
                   (int)XpanderConstants.EnumPages.VCO_2_X, 0x00,
                    0, 63, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x08,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Freq")
                    );
            _parameterMap.Add("VCO2_DETUNE",
                 new XpanderSignedParameter("VCO2_DETUNE",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x00,
                     -31, 31, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Detune")
                     );
            _parameterMap.Add("VCO2_PW",
                 new XpanderParameter("VCO2_PW",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x00,
                     0, 63, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "PW")
                     );
            _parameterMap.Add("VCO2_VOLUME",
                 new XpanderParameter("VCO2_VOLUME",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x00,
                     0, 63, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0D,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Vol")
                     );
            //SUBPAGE 2
            _parameterMap.Add("VCO2_WAVESHAPE_TRI",
                 new XpanderParameter("VCO2_WAVESHAPE_TRI",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Tri")
                     );
            _parameterMap.Add("VCO2_WAVESHAPE_SAW",
                 new XpanderParameter("VCO2_WAVESHAPE_SAW",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Saw")
                     );
            _parameterMap.Add("VCO2_WAVESHAPE_PULSE",
                 new XpanderParameter("VCO2_WAVESHAPE_PULSE",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Pulse")
                     );
            _parameterMap.Add("VCO2_WAVESHAPE_NOISE",
                 new XpanderParameter("VCO2_WAVESHAPE_NOISE",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0C,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Noise")
                     );

            //mods
            _parameterMap.Add("VCO2_MOD_KEYB",
                 new XpanderParameter("VCO2_MOD_KEYB",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x01,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Keyb")
                     );
            _parameterMap.Add("VCO2_MOD_LAG",
                 new XpanderParameter("VCO2_MOD_LAG",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x02,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Lag")
                     );
            _parameterMap.Add("VCO2_MOD_LEV1",
                 new XpanderParameter("VCO2_MOD_LEV1",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x03,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Lev1")
                     );
            _parameterMap.Add("VCO2_MOD_VIB",
                 new XpanderParameter("VCO2_MOD_VIB",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x04,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Vib")
                     );
            _parameterMap.Add("VCO2_WAVE_SYNC",
                 new XpanderParameter("VCO2_WAVE_SYNC",
                    (int)XpanderConstants.EnumPages.VCO_2_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x05,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Sync")
                     );

            #endregion INITIALIZEPARAMETERMAP_VCO

            #region INITIALIZEPARAMETERMAP_VCF_VCA

            //VCF/VCA

            _parameterMap.Add("VCF_FREQ",
                new XpanderParameter("VCF_FREQ",
                   (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x00,
                    0, 127, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x08,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Freq")
                    );
            _parameterMap.Add("VCF_RES",
                new XpanderParameter("VCF_RES",
                    (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x00,
                     0, 63, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Res")
                    );
            _parameterMap.Add("VCF_MODE",
                new XpanderParameter("VCF_MODE",
                   (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x00,
                    (int)(XpanderConstants.EnumVCFFilterModes._1_POLE_LOW), (int)(XpanderConstants.EnumVCFFilterModes._3_PHASE_1_LOW), 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Mode")
                    );
            _parameterMap.Add("VCF_VCA1_VOLUME",
                new XpanderParameter("VCF_VCA1_VOLUME",
                   (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x00,
                    0, 63, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0C,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "VCA1 Vol")
                    );
            _parameterMap.Add("VCF_VCA2_VOLUME",
                new XpanderParameter("VCF_VCA2_VOLUME",
                   (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x00,
                    0, 63, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0D,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "VCA2 Vol")
                    );
            //SUBPAGE
            //mods
            _parameterMap.Add("VCF_MOD_KEYB",
                 new XpanderParameter("VCF_MOD_KEYB",
                    (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x01,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Keyb")
                     );
            _parameterMap.Add("VCF_MOD_LAG",
                 new XpanderParameter("VCF_MOD_LAG",
                    (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x02,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Lag")
                     );
            _parameterMap.Add("VCF_MOD_LEV1",
                 new XpanderParameter("VCF_MOD_LEV1",
                    (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x03,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Lev1")
                     );
            _parameterMap.Add("VCF_MOD_VIB",
                 new XpanderParameter("VCF_MOD_VIB",
                    (int)XpanderConstants.EnumPages.VCF_VCA_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x04,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Vib")
                     );

            #endregion INITIALIZEPARAMETERMAP_VCF_VCA

            #region INITIALIZEPARAMETERMAP_FM_LAG

            //FM/LAG

            _parameterMap.Add("FM_AMP",
                new XpanderParameter("FM_AMP",
                   (int)XpanderConstants.EnumPages.FM_LAG_X, 0x00,
                    0, 63, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x08,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Amp")
                    );
            _parameterMap.Add("FM_DESTINATION",
                new XpanderParameter("FM_DESTINATION",
                   (int)XpanderConstants.EnumPages.FM_LAG_X, 0x00,
                    0, 1, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Dest")
                    );
            _parameterMap.Add("LAG_IN",
                new XpanderParameter("LAG_IN",
                   (int)XpanderConstants.EnumPages.FM_LAG_X, 0x00,
                    (int)XpanderConstants.EnumModulationSources.KBD, (int)XpanderConstants.EnumModulationSources.LEV2, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Input")
                    );
            _parameterMap.Add("FMLAG_RATE",
                new XpanderParameter("FMLAG_RATE",
                   (int)XpanderConstants.EnumPages.FM_LAG_X, 0x00,
                    0, 63, 1,
                    new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0C,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                    , 0, "Rate")
                    );

            //SUBPAGE
            _parameterMap.Add("LAG_MODE_LEGATO",
                new XpanderParameter("LAG_MODE_LEGATO",
                    (int)XpanderConstants.EnumPages.FM_LAG_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x03,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Legato")
                     );
            _parameterMap.Add("LAG_TIMING_LINEAR_EXPO",
                 new XpanderParameter("LAG_TIMING_LINEAR_EXPO",
                    (int)XpanderConstants.EnumPages.FM_LAG_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x08,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                     , 0, "Lin/Exp")
                     );
            _parameterMap.Add("LAG_LINEAR_EQUAL_TIME",
                new XpanderParameter("LAG_LINEAR_EQUAL_TIME",
                    (int)XpanderConstants.EnumPages.FM_LAG_X, 0x01,
                     0, 1, 1,
                     new SysExMessage(new byte[] {
                                    (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Equal time")
                     );

            #endregion INITIALIZEPARAMETERMAP_FM_LAG

            #region INITIALIZEPARAMETERMAP_TRACK_X

            //TRACK_X (3x)

            for (int trackNumber = 0; trackNumber < XpanderConstants.TRACK_COUNT; trackNumber++)
            {
                // TRACK IN
                string sParameterName = string.Format("TRACK_{0}_IN", trackNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.TRACK_1 + trackNumber), 0x00,
                       (int)XpanderConstants.EnumModulationSources.KBD, (int)XpanderConstants.EnumModulationSources.LEV2, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,(byte)(0x08),0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Input")
                        );

                //for each point (1 to 5)
                for (int pointNumber = 0; pointNumber < XpanderConstants.TRACK_POINTS_COUNTS; pointNumber++)
                {
                    sParameterName = string.Format("TRACK_{0}_POINT_{1}", trackNumber + 1, pointNumber + 1);
                    _parameterMap.Add(sParameterName,
                        new XpanderParameter(sParameterName,
                           (int)(XpanderConstants.EnumPages.TRACK_1 + trackNumber), 0x00,
                            0, 63, 1,
                            new SysExMessage(new byte[] {
                            (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,(byte)(0x09+pointNumber),0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                            , 0, "Point " + pointNumber.ToString())
                            );
                } //point
            } //track

            #endregion INITIALIZEPARAMETERMAP_TRACK_X

            #region INITIALIZEPARAMETERMAP_ENV_X

            //ENV_X (5x)

            for (int envNumber = 0; envNumber < XpanderConstants.ENV_COUNT; envNumber++)
            {
                string sParameterName;
                sParameterName = string.Format("ENV_{0}_DELAY", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x08,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Del")
                        );
                sParameterName = string.Format("ENV_{0}_ATTACK", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Atck")
                        );
                sParameterName = string.Format("ENV_{0}_DECAY", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Dec")
                        );
                sParameterName = string.Format("ENV_{0}_SUSTAIN", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Sust")
                        );
                sParameterName = string.Format("ENV_{0}_RELEASE", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0C,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Rel")
                        );
                sParameterName = string.Format("ENV_{0}_VOLUME", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0D,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Vol")
                        );

                //SUBPAGE
                sParameterName = string.Format("ENV_{0}_MODE_RESET", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x01,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Reset")
                        );
                sParameterName = string.Format("ENV_{0}_MODE_FREERUN", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x02,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Freerun")
                        );
                sParameterName = string.Format("ENV_{0}_MODE_DADR", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x03,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "DADR")
                        );
                // SUBPAGE, TRIGGER
                sParameterName = string.Format("ENV_{0}_TRIG_SINGLE_MULTI", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Single/Multi")
                        );
                sParameterName = string.Format("ENV_{0}_TRIG_EXTRIG", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Extern")
                        );
                sParameterName = string.Format("ENV_{0}_TRIG_LFOTRIG", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "LFO")
                        );
                //ENV_X_TRIG_LFO_SOURCE seems only setable from rotary
                sParameterName = string.Format("ENV_{0}_TRIG_LFO_SOURCE", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        (int)(XpanderConstants.EnumLFOTriggerSources.LFO1), (int)(XpanderConstants.EnumLFOTriggerSources.VIB), 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0C,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Source")
                        );

                sParameterName = string.Format("ENV_{0}_TRIG_GATED", envNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.ENV_1 + envNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0D,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Gated")
                        );
            }  //for iEnvNumber

            #endregion INITIALIZEPARAMETERMAP_ENV_X

            #region INITIALIZEPARAMETERMAP_LFO_X

            //LFO_X (5x)

            for (int iLFONumber = 0; iLFONumber < XpanderConstants.LFO_COUNT; iLFONumber++)
            {
                string sParameterName;
                sParameterName = string.Format("LFO_{0}_SPEED", iLFONumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.LFO_1 + iLFONumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x08,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Speed")
                        );
                sParameterName = string.Format("LFO_{0}_WAVESHAPE", iLFONumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.LFO_1 + iLFONumber), 0x00,
                        (int)(XpanderConstants.EnumLFOWaveShapes.TRIANGLE), (int)(XpanderConstants.EnumLFOWaveShapes.SAMPLE), 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Shape")
                        );
                sParameterName = string.Format("LFO_{0}_SAMPLE_INPUT", iLFONumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.LFO_1 + iLFONumber), 0x00,
                        (int)(XpanderConstants.EnumModulationSources.KBD), (int)(XpanderConstants.EnumModulationSources.LEV2), 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Sample input")
                        );

                sParameterName = string.Format("LFO_{0}_RETRIG", iLFONumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.LFO_1 + iLFONumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Retrig")
                        );

                sParameterName = string.Format("LFO_{0}_AMP", iLFONumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.LFO_1 + iLFONumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0D,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Amp")
                        );

                //SUBPAGE
                sParameterName = string.Format("LFO_{0}_LAG", iLFONumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.LFO_1 + iLFONumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Lag")
                        );

                sParameterName = string.Format("LFO_{0}_RETRIG_MODE", iLFONumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.LFO_1 + iLFONumber), 0x01,
                        (int)(XpanderConstants.EnumLFORetrigModes.OFF), (int)(XpanderConstants.EnumLFORetrigModes.EXTRIG), 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x1A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Retrig Mode")
                        );
            }  //for iLFONumber

            #endregion INITIALIZEPARAMETERMAP_LFO_X

            #region INITIALIZEPARAMETERMAP_RAMP_X

            //RAMP_X (4x)

            for (int iRampNumber = 0; iRampNumber < XpanderConstants.RAMP_COUNT; iRampNumber++)
            {
                string sParameterName;
                sParameterName = string.Format("RAMP_{0}_RATE", iRampNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.RAMP_1 + iRampNumber), 0x00,
                        0, 63, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Rate")
                        );

                // SUBPAGE, TRIGGER
                sParameterName = string.Format("RAMP_{0}_TRIG_SINGLE_MULTI", iRampNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.RAMP_1 + iRampNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x09,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Single/Multi")
                        );
                sParameterName = string.Format("RAMP_{0}_TRIG_EXTRIG", iRampNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.RAMP_1 + iRampNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0A,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Extern")
                        );
                sParameterName = string.Format("RAMP_{0}_TRIG_LFOTRIG", iRampNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.RAMP_1 + iRampNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0B,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "LFO")
                        );
                // RAMP_X_TRIG_LFO_SOURCE seems only setable from rotary encoder
                sParameterName = string.Format("RAMP_{0}_TRIG_LFO_SOURCE", iRampNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.RAMP_1 + iRampNumber), 0x01,
                        (int)(XpanderConstants.EnumLFOTriggerSources.LFO1), (int)(XpanderConstants.EnumLFOTriggerSources.VIB), 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0C,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Source")
                        );

                sParameterName = string.Format("RAMP_{0}_TRIG_GATED", iRampNumber + 1);
                _parameterMap.Add(sParameterName,
                    new XpanderParameter(sParameterName,
                       (int)(XpanderConstants.EnumPages.RAMP_1 + iRampNumber), 0x01,
                        0, 1, 1,
                        new SysExMessage(new byte[] {
                        (byte)SysExType.Start, 0x10,0x02,0x0A,0x00,0x0D,0x00,0x00,0x00,0x00,0x00, (byte)SysExType.Continuation})
                        , 0, "Gated")
                        );
            }  //for iRampNumber

            #endregion INITIALIZEPARAMETERMAP_RAMP_X

            #region MOD MATRIX (x20)

            for (int modMatrixEntry = 1; modMatrixEntry < XpanderConstants.MODENTRIES_COUNT + 1; modMatrixEntry++)
            {
                string parameterName = null;
                PageSubPage page = XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ];

                // set value
                parameterName = GetAmountSourceParameterNameForEntry(modMatrixEntry);
                _parameterMap.Add(parameterName,
                      new XpanderModMatrixParameter(parameterName,
                         (int)page.Page, page.SubPage, // page does not matter, will be changed dynamically when changing the dest
                         ModulationMatrixEntry.MIN_AMOUNT, ModulationMatrixEntry.MAX_AMOUNT, 1,
                          new SysExMessage(new byte[] {
                          (byte)SysExType.Start, 0x10,0x02,0x0F,0x00,/*id*/ 0x00,0x00,/*command*/ (int)XpanderConstants.EnumModulationEditCommands.SETUNSIGNEDVALUE,0x00,/*_value*/ 0x00,0x00, 0xF7})
                          , 0x00)
                          );

                // set quantize on/off
                parameterName = GetQuantizeSourceParameterNameForEntry(modMatrixEntry);
                _parameterMap.Add(parameterName,
                      new XpanderModMatrixParameter(parameterName,
                         (int)page.Page, page.SubPage, // page does not matter, will be changed dynamically when changing the dest
                          ModulationMatrixEntry.MIN_QUANTIZE, ModulationMatrixEntry.MAX_QUANTIZE, 1,
                          new SysExMessage(new byte[] {
                          (byte)SysExType.Start, 0x10,0x02,0x0F,0x00,/*id*/ 0x00,0x00,/*command*/ (int)XpanderConstants.EnumModulationEditCommands.SETQUANTIZE,0x00,/*_value*/ 0x00,0x00, 0xF7})
                          , 0)
                          );
            }

            #endregion MOD MATRIX (x20)

            #region PARAMETERS_FOR_TONE_MORPHING

            // return only parameters that are seens "as knobs" in the user interface
            // that is, all parameters with range 0..63, -31/+31, 0..127
            // do not return mod matrix parameters - morphing two differents matrix is nonsense
            IEnumerable<XpanderParameter> parameters = ParameterMap.Values.Cast<XpanderParameter>()
                .Where(p => ((p.MinValue <= 0) && (p.MaxValue >= 31)) && p.GetType() != typeof(XpanderModMatrixParameter));

            _parametersNamesForToneMorphing = new HashSet<string>(parameters.Select(p => p.Name));

            #endregion PARAMETERS_FOR_TONE_MORPHING
        } // InitializeParameterMap

        // modulation matrix command parameters that are not in the map
        /// <summary>
        /// DELETE SOURCE
        /// </summary>
        private static XpanderModMatrixParameter _DELETESourceParameter =
            new XpanderModMatrixParameter("DEL_MOD_SRC",
                         (int)XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ].Page,
                            XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ].SubPage, // page does not matter, will be changed dynamically when changing the dest
                          (int)XpanderConstants.EnumModulationSourcesModMatrix.KBD, (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE, 1, // not used
                          new SysExMessage(new byte[] {
                          (byte)SysExType.Start, 0x10,0x02,0x0F,0x00,/*id*/ 0x00,0x00,/*command*/ (int)XpanderConstants.EnumModulationEditCommands.DELETESOURCE,0x00,/*_value*/ 0x00,0x00, 0xF7})
                          , 0x00);

        /// <summary>
        /// ADD SOURCE
        /// </summary>
        private static XpanderModMatrixParameter _ADDSourceParameter =
          new XpanderModMatrixParameter("ADD_MOD_SRC",
              (int)XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ].Page,
                XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ].SubPage, // page does not matter, will be changed dynamically when changing the dest
              (int)XpanderConstants.EnumModulationSourcesModMatrix.KBD, (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE, 1,
              new SysExMessage(new byte[] {
                (byte)SysExType.Start, 0x10,0x02,0x0F,0x00/*id*/, 0x00,0x00,
                (int)XpanderConstants.EnumModulationEditCommands.ADDSOURCE,0x00, (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE,0x00, 0xF7})
              , (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE);

        /// <summary>
        /// CHANGE
        /// </summary>
        private static XpanderModMatrixParameter _CHANGESourceParameter =
              new XpanderModMatrixParameter("CHANGE_MOD_SRC",
                  (int)XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ].Page,
                  XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ].SubPage, // page does not matter, will be changed dynamically when changing the dest
                  (int)XpanderConstants.EnumModulationSourcesModMatrix.KBD, (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE, 1,
                  new SysExMessage(new byte[] {
                  (byte)SysExType.Start, 0x10,0x02,0x0F,0x00,/*id*/ 0x00,0x00,
                  /*command*/ (int)XpanderConstants.EnumModulationEditCommands.CHANGESOURCE,0x00,/*_value*/ 0x00,0x00, 0xF7})
                  , 0x00);

        private string GetAmountSourceParameterNameForEntry(int entryNumber)
        {
            return string.Format("MOD_AMNT_SRC_{0}", entryNumber);
        }

        private string GetQuantizeSourceParameterNameForEntry(int entryNumber)
        {
            return string.Format("MOD_QUANTIZE_{0}", entryNumber);
        }

        // gives the set sign bit message for the SETAMOUNTSourceParameter; and reset the value of the amount to a positive one
        private XpanderParameter AmountSetSignMessageForSetSourceAmountParameter(ref XpanderModMatrixParameter setSourceAmountParameter)
        {
            XpanderParameter toggleSignParameter =
            new XpanderModMatrixParameter(setSourceAmountParameter.Name + "_SETSIGN",
                     setSourceAmountParameter.Page, setSourceAmountParameter.SubPage, // page does not matter, will be changed dynamically when changing the dest
                     0, 1, 1,
                      new SysExMessage(new byte[] {
                          (byte)SysExType.Start, 0x10,0x02,0x0F,0x00,/*id*/ (byte)setSourceAmountParameter.IDSource,0x00,
                          /*command*/ (int)XpanderConstants.EnumModulationEditCommands.SETSIGN,0x00,/*_value*/ 0x00,0x00, 0xF7})
                      , 0x00);

            if (setSourceAmountParameter.Value < 0)
            {
                toggleSignParameter.Value = 0x01;
            }
            setSourceAmountParameter.Value = System.Math.Abs(setSourceAmountParameter.Value);
            setSourceAmountParameter.Changed = false;
            return toggleSignParameter;
        }

        #endregion PARAMETERS

        #region Tone randomization / tone morphing

        /// <summary>
        /// Returns the list of parameters that can be used for tone morphing
        /// </summary>
        /// <returns></returns>
        public override HashSet<string> GetEligibleParametersForToneMorhping()
        {
            Debug.Assert(_parametersNamesForToneMorphing != null, "_parametersNamesForToneMorphing undefined - InitializeParametersMap must be called before");
            return _parametersNamesForToneMorphing;
        }

        /// <summary>
        /// Detune the tone
        /// </summary>
        /// <param name="detuneAnalog">true for analog detune, else digital</param>
        internal void Detune(bool detuneAnalog = true)
        {
            int detuneValue = detuneAnalog ? 10 : 1;
            XpanderParameter vco1Detune = (XpanderParameter)ParameterMap["VCO1_DETUNE"];
            XpanderParameter vco2Detune = (XpanderParameter)ParameterMap["VCO2_DETUNE"];
            vco1Detune.Value = -detuneValue;
            vco2Detune.Value = detuneValue;
        }

        /// <summary>
        /// Define harmonic frequencies for VCO1/VCO2 Freq parameters
        /// </summary>
        /// <param name="tuning">the tuning</param>
        internal void DefineVCOFrequenciesTuning(EnumRandomVCOFreq tuning)
        {
            XpanderParameter vco1Freq = (XpanderParameter)ParameterMap["VCO1_FREQ"];
            XpanderParameter vco2Freq = (XpanderParameter)ParameterMap["VCO2_FREQ"];

            switch (tuning)
            {
                case EnumRandomVCOFreq.Free:
                    Debug.Fail("No tuning available for tuning " + tuning.ToString());
                    break;

                case EnumRandomVCOFreq.SameNote:
                    // value in semitones
                    vco1Freq.Value = 12;
                    vco2Freq.Value = vco1Freq.Value;
                    break;

                case EnumRandomVCOFreq.Third:
                    // value in semitones
                    vco1Freq.Value = 0;
                    vco2Freq.Value = 4;
                    break;

                case EnumRandomVCOFreq.Fifth:
                    // value in semitones
                    vco1Freq.Value = 0;
                    vco2Freq.Value = 7;
                    break;

                case EnumRandomVCOFreq.Seventh:
                    // value in semitones
                    vco1Freq.Value = 0;
                    vco2Freq.Value = 11;
                    break;

                case EnumRandomVCOFreq.Ninth:
                    // value in semitones
                    vco1Freq.Value = 0;
                    vco2Freq.Value = 12 + 2;
                    break;

                case EnumRandomVCOFreq.Eleventh:
                    // value in semitones
                    vco1Freq.Value = 0;
                    vco2Freq.Value = 12 + 5;
                    break;

                case EnumRandomVCOFreq.Thirteenth:
                    // value in semitones
                    vco1Freq.Value = 0;
                    vco2Freq.Value = 12 + 9;
                    break;
            }
        }

        /// <summary>
        /// Define ENV2 modulation and parameter so that is acts as a "classic" volume enveloppe
        /// </summary>
        /// <remarks></remarks>
        /// <param name="enveloppe">enveloppe type</param>
        internal void ForceEnv2ModVca2AfterRandomizeMatrix(EnumRandomVCAEnv enveloppe)
        {
            // Envelope type is DADSR + volume
            Dictionary<EnumRandomVCAEnv, int[]> enveloppes = new Dictionary<EnumRandomVCAEnv, int[]>()
            { {EnumRandomVCAEnv.Organ,new int [] {0,63,63,63,0,63}},
              {EnumRandomVCAEnv.String,new int [] {0,30,63,63,32,63}},
              {EnumRandomVCAEnv.Percusive,new int [] {0,0,63,10,0,63}},
              {EnumRandomVCAEnv.PercusiveWithRelease,new int [] {0,0,63,0,32,63}}
            };

            int[] values;
            if (enveloppes.TryGetValue(enveloppe, out values))
            {
                //DADSR+V
                ((XpanderParameter)ParameterMap["ENV_2_DELAY"]).Value = values[0];
                ((XpanderParameter)ParameterMap["ENV_2_ATTACK"]).Value = values[1];
                ((XpanderParameter)ParameterMap["ENV_2_DECAY"]).Value = values[2];
                ((XpanderParameter)ParameterMap["ENV_2_SUSTAIN"]).Value = values[3];
                ((XpanderParameter)ParameterMap["ENV_2_RELEASE"]).Value = values[4];
                ((XpanderParameter)ParameterMap["ENV_2_VOLUME"]).Value = values[5];
            }
            else
            {
                Debug.Fail("Envelope type is not supported");
                return;
            }

            // fix other ENV2 values
            ((XpanderParameter)ParameterMap["ENV_2_MODE_RESET"]).Value = 0;
            ((XpanderParameter)ParameterMap["ENV_2_MODE_FREERUN"]).Value = 0;
            ((XpanderParameter)ParameterMap["ENV_2_MODE_DADR"]).Value = 0;
            ((XpanderParameter)ParameterMap["ENV_2_TRIG_SINGLE_MULTI"]).Value = 0; // single if not multi
            ((XpanderParameter)ParameterMap["ENV_2_TRIG_EXTRIG"]).Value = 0;
            ((XpanderParameter)ParameterMap["ENV_2_TRIG_LFOTRIG"]).Value = 0;
            ((XpanderParameter)ParameterMap["ENV_2_TRIG_LFO_SOURCE"]).Value = 0;
            ((XpanderParameter)ParameterMap["ENV_2_TRIG_GATED"]).Value = 0;

            // set VCF/VCA2 volume to 0, so that only ENV2 modulates
            ((XpanderParameter)ParameterMap["VCF_VCA2_VOLUME"]).Value = 0;

            // now force ENV2 mod matrix
            // reset all modulations to VCA2_VOL and set ENV2 to it
            // we can do direct editing here, since whole patch will be resent
            this._modulationMatrix
            .Where(e => e.Destination == XpanderConstants.EnumModulationDestinations.VCA2_VOL)
            .ToList()
            .ForEach(
                me =>
                {
                    me.Destination = XpanderConstants.EnumModulationDestinations.VCO1_FRQ;
                    me.Source = XpanderConstants.EnumModulationSourcesModMatrix.NONE;
                    me.IdSource = UNDEFINED_MODULATION_SOURCE_NUMBER;
                    me.Amount = 0;
                    me.Quantize = 0;
                }
                );

            // get first available entry and set the modulation
            // since we removed everything, we should always get a valid id source, which should be the first one
            int nextIdSourceForVca2Vol = GetNextAvailableModIDSourceForDest(XpanderConstants.EnumModulationDestinations.VCA2_VOL);
            Debug.Assert(nextIdSourceForVca2Vol != UNDEFINED_MODULATION_SOURCE_NUMBER, "Unable to get a new mod ID source for VCA2_VOL");

            int nextAvailableModEntry = GetNextAvailableModEntry();
            if (nextAvailableModEntry == NO_AVAILABLE_MOD_ENTRY)
            {
                // this could happen; ex: all matrix filled without any modulation source set to NONE.
                // in this case, remove arbitrary modulation to get a free slot
                // since we randomized the full matrix, and we will resend the whole tone, there is no need to edit
                // an existing entry to keep the matrix synched with synth, just replace a whole entry, take the first one
                nextAvailableModEntry = 0;
            }

            ModulationMatrixEntry entry = new ModulationMatrixEntry()
            {
                IdSource = nextIdSourceForVca2Vol,
                Source = XpanderConstants.EnumModulationSourcesModMatrix.ENV2,
                Amount = 63,
                Destination = XpanderConstants.EnumModulationDestinations.VCA2_VOL,
                Quantize = 0
            };
            this._modulationMatrix[nextAvailableModEntry] = entry;

            // we still need to update the amount/quantize of the parameter map for this entry
            // for other entries this has been managed thru matrix randomization before
            PageSubPage pages = XpanderConstants.PageSubPageForModulationDestination[(int)XpanderConstants.EnumModulationDestinations.VCA2_VOL];
            ((XpanderModMatrixParameter)_parameterMap[GetAmountSourceParameterNameForEntry(nextAvailableModEntry + 1)]).Page = (int)pages.Page;
            ((XpanderModMatrixParameter)_parameterMap[GetAmountSourceParameterNameForEntry(nextAvailableModEntry + 1)]).SubPage = pages.SubPage;

            ((XpanderModMatrixParameter)_parameterMap[GetAmountSourceParameterNameForEntry(nextAvailableModEntry + 1)]).Value =
                _modulationMatrix[nextAvailableModEntry].Amount;
            ((XpanderModMatrixParameter)_parameterMap[GetQuantizeSourceParameterNameForEntry(nextAvailableModEntry + 1)]).Value =
                _modulationMatrix[nextAvailableModEntry].Quantize;
        }

        #endregion Tone randomization / tone morphing

        /// <summary>
        /// for debug purpose. dumps the amount and quantize parameters
        /// </summary>
        internal void DumpModulationParameters()
        {
            Logger.WriteLine(this, TraceLevel.Verbose, " -- Modulation Parameters (amount & quantize): --");

            StringBuilder sb = new StringBuilder("\r\n");
            for (int modMatrixEntry = 1; modMatrixEntry < XpanderConstants.MODENTRIES_COUNT + 1; modMatrixEntry++)
            {
                // amout parameter
                string parameterName = GetAmountSourceParameterNameForEntry(modMatrixEntry);
                XpanderModMatrixParameter parameter = (XpanderModMatrixParameter)_parameterMap[parameterName];
                sb = sb.AppendFormat("{0}: AMT: {1}\r\n", modMatrixEntry - 1, parameter.ToString());

                // quantize parameter
                parameterName = GetQuantizeSourceParameterNameForEntry(modMatrixEntry);
                parameter = (XpanderModMatrixParameter)_parameterMap[parameterName];
                sb = sb.AppendFormat("{0}: QTZ: {1}\r\n", modMatrixEntry - 1, parameter.ToString());
            }
            Logger.WriteLine(this, TraceLevel.Verbose, sb.ToString());
        }
    }
}