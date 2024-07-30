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
using Sanford.Multimedia.Midi;
using System;
using System.Diagnostics;
using System.IO;
using Xplorer.Common;

namespace Xplorer.Model.Tone
{
    /// <summary>
    /// A class to model the tone data from a byte stream - allows load/save/transmit without handling separate parameters
    /// </summary>
    internal class XPanderSinglePatch
    {
        #region internal

        //VCO
        internal class VCO
        {
            internal byte freq = 0;/* Frequency */
            internal sbyte detune = 0;		/* Detune*/
            internal byte pw = 0;	/* Pulse Width*/
            internal byte vol = 0;	/* Volume*/
            internal byte mod = 0;	/* Standard Modulation flags */
            internal byte wave = 0;/* VCO Wave Shape flags */
        }

        // VCF
        internal class VCF
        {
            internal byte freq = 0;	/* Frequency */
            internal byte res = 0;		/* Resonance */
            internal byte fmode = 0;	/* Filter Mode Code */
            internal byte vca1 = 0;	 /* VCA 1 level*/
            internal byte vca2 = 0;	 /* VCA 2 level*/
            internal byte mod = 0;		/* Standard Modulation flags */
        }

        //FM LAG
        internal class FMAndLAG
        {
            internal byte f_amp = 0;	 /* FM Amplitude */
            internal byte f_dest = 0;	 /* FM destination code*/
            internal byte lag_in = 0;	 /* Lag Input code */
            internal byte lag_rate = 0;	 /* Lag Rate */
            internal byte lag_mode = 0; /* Lag Mode */
        }

        //LFO (x5)
        internal class LFO
        {
            internal byte speed = 0;		/* LFO Speed*/
            internal byte retrig_mode = 0;	/* Retrigger mode */
            internal byte lag = 0;			/* Lag flag*/
            internal byte wave = 0;		/* Wave Shape code */
            internal byte retrig = 0;		/* Retrigger Point */
            internal byte sample = 0;		/* LFO Sample Source */
            internal byte amp = 0;			/* LFO Amplitude */
        }

        internal class Enveloppe
        {
            internal byte flags = 0;	/* Miscellaneous flags */
            internal byte lfotrig = 0;	/* Triggering LFO code */
            internal byte delay = 0;	/* Delay Time*/
            internal byte attack = 0;	/* Attack Time*/
            internal byte decay = 0;	/* Decay Time*/
            internal byte sustain = 0;	/* Sustain Level*/
            internal byte release = 0;	/* Release Time */
            internal byte amp = 0;		/* Envelope Amplitude */
        }

        // TRACK (x3)
        internal class Track
        {
            internal byte input = 0;	/* Tracking Generator Source */
            internal byte[] points;	/* Mapped vals for Input Pts*/

            internal Track()
            {
                points = new byte[XpanderConstants.TRACK_POINTS_COUNTS];
            }
        }

        // RAMP (x4)
        internal class Ramp
        {
            internal byte rate = 0; /* Ramp Time*/
            internal byte flags = 0;/* Miscellaneous flags */
            internal byte lfotrig = 0;/* LFO Trigger Code*/
        }

        //MOD MATRIX (x20)
        internal class ModulationEntry
        {
            internal byte source = 0; /* Modulation Source code*/
            internal bool quantize = false;
            internal sbyte amount = 0;/* Modulation Amount */
            internal byte dest = 0;	/* Modulation Destination*/
        }

        internal class PatchName
        {
            internal const int NAME_LENTH = 8;
            internal byte[] name;

            internal PatchName()
            {
                name = new byte[NAME_LENTH];
            }

            internal string Name
            {
                get
                {
                    return System.Text.Encoding.ASCII.GetString(name);
                }
                set
                {
                    name = System.Text.Encoding.ASCII.GetBytes(value);
                }
            }
        }

        #endregion internal

        //sysex length 399 =  6 + 196*2 + 1(EOX=F7)
        public const int SINGLE_PATCH_LENGTH = 399;

        public const int SINGLE_PATCH_INTRO_LENGTH = 6;
        private const int SINGLE_PATCH_DATA_LENGTH = 196;

        //modulation amount mask
        private const byte VALUE_MASK = 0x3F;

        private const byte SIGN_MASK = 0x40;
        private const byte QUANTIZE_MASK = 0x80;

        public const byte UNUSED_ENTRY_SOURCE_VALUE = 0x1F;
        public const byte UNUSED_ENTRY_DEST_VALUE = 0x3F;

        public static readonly byte[] PATCH_INTRO = new byte[(SINGLE_PATCH_INTRO_LENGTH - 1)] { (byte)Sanford.Multimedia.Midi.SysExType.Start, 0x10, 0x02, 0x01, 0x00 };

        //patch data
        internal VCO[] _vcos = null;

        internal VCF _vcf = null;
        internal FMAndLAG _fmAndLag = null;
        internal LFO[] _lfos = null;
        internal Enveloppe[] _enveloppes = null;
        internal Track[] _tracks = null;
        internal Ramp[] _ramps = null;
        internal ModulationEntry[] _modulationEntries = null;
        internal PatchName _name = null;

        /// <summary>
        /// Initializes a new instance of the <see cref="XPanderSinglePatch"/> class.
        /// </summary>
        public XPanderSinglePatch()
        {
            _vcos = new VCO[XpanderConstants.VCO_COUNT];
            for (int i = 0; i < XpanderConstants.VCO_COUNT; i++)
            {
                _vcos[i] = new VCO();
            }
            _vcf = new VCF();
            _fmAndLag = new FMAndLAG();
            _lfos = new LFO[XpanderConstants.LFO_COUNT];
            for (int i = 0; i < XpanderConstants.LFO_COUNT; i++)
            {
                _lfos[i] = new LFO();
            }
            _enveloppes = new Enveloppe[XpanderConstants.ENV_COUNT];
            for (int i = 0; i < XpanderConstants.ENV_COUNT; i++)
            {
                _enveloppes[i] = new Enveloppe();
            }
            _tracks = new Track[XpanderConstants.TRACK_COUNT];
            for (int i = 0; i < XpanderConstants.TRACK_COUNT; i++)
            {
                _tracks[i] = new Track();
            }

            _ramps = new Ramp[XpanderConstants.RAMP_COUNT];
            for (int i = 0; i < XpanderConstants.RAMP_COUNT; i++)
            {
                _ramps[i] = new Ramp();
            }

            _modulationEntries = new ModulationEntry[XpanderConstants.MODENTRIES_COUNT];
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                _modulationEntries[i] = new ModulationEntry();
            }
            _name = new PatchName();
        }

        /// <summary>
        /// "deserialization" from stream  - caller must handle exceptions
        /// The curent stream position must match first VCO data
        /// Caller must close the stream itself !
        /// </summary>
        /// <param name="stream">The stream.</param>
        /// <param name="position"></param>
        internal void FromStream(MemoryStream stream)
        {
            BinaryReader br = new BinaryReader(stream);
            PacketizedBinaryReader reader = new PacketizedBinaryReader(br);
            try
            {
                //read VCOs
                foreach (VCO vco in _vcos)
                {
                    vco.freq = reader.ReadByte();
                    vco.detune = reader.ReadSByte();
                    vco.pw = reader.ReadByte();
                    vco.vol = reader.ReadByte();
                    vco.mod = reader.ReadByte();
                    vco.wave = reader.ReadByte();
                }
                // VCF
                _vcf.freq = reader.ReadByte();
                _vcf.res = reader.ReadByte();
                _vcf.fmode = reader.ReadByte();
                _vcf.vca1 = reader.ReadByte();
                _vcf.vca2 = reader.ReadByte();
                _vcf.mod = reader.ReadByte();
                // FM and LAG
                _fmAndLag.f_amp = reader.ReadByte();
                _fmAndLag.f_dest = reader.ReadByte();
                _fmAndLag.lag_in = reader.ReadByte();
                _fmAndLag.lag_rate = reader.ReadByte();
                _fmAndLag.lag_mode = reader.ReadByte();
                //LFOs
                foreach (LFO lfo in _lfos)
                {
                    lfo.speed = reader.ReadByte();
                    lfo.retrig_mode = reader.ReadByte();
                    lfo.lag = reader.ReadByte();
                    lfo.wave = reader.ReadByte();
                    lfo.retrig = reader.ReadByte();
                    lfo.sample = reader.ReadByte();
                    lfo.amp = reader.ReadByte();
                }
                //ENV
                foreach (Enveloppe env in _enveloppes)
                {
                    env.flags = reader.ReadByte();
                    env.lfotrig = reader.ReadByte();
                    env.delay = reader.ReadByte();
                    env.attack = reader.ReadByte();
                    env.decay = reader.ReadByte();
                    env.sustain = reader.ReadByte();
                    env.release = reader.ReadByte();
                    env.amp = reader.ReadByte();
                }
                // TRACK
                foreach (Track track in _tracks)
                {
                    track.input = reader.ReadByte();
                    track.points = reader.ReadBytes(XpanderConstants.TRACK_POINTS_COUNTS);
                }
                //RAMPS
                foreach (Ramp ramp in _ramps)
                {
                    ramp.rate = reader.ReadByte();
                    ramp.flags = reader.ReadByte();
                    ramp.lfotrig = reader.ReadByte();
                }
                //MODENTRIES
                foreach (ModulationEntry modentry in _modulationEntries)
                {
                    modentry.source = reader.ReadByte();
                    byte bAmountSignQuantize = reader.ReadByte();
                    //get the 6 bits absolute value
                    modentry.amount = (sbyte)(bAmountSignQuantize & VALUE_MASK);
                    // extract sign and qtz
                    if ((bAmountSignQuantize & SIGN_MASK) == SIGN_MASK)
                    {
                        modentry.amount *= -1;
                    }
                    if ((bAmountSignQuantize & QUANTIZE_MASK) == QUANTIZE_MASK)
                    {
                        modentry.quantize = true;
                    }
                    else
                    {
                        modentry.quantize = false;
                    }

                    modentry.dest = reader.ReadByte();
                }
                //NAME
                _name.name = reader.ReadBytes(PatchName.NAME_LENTH);
            }
            finally
            {
                // keep stream open
                reader = null;
            }
        }

        /// <summary>
        /// "deserialization" from byte array - caller must handle exceptions
        /// </summary>
        /// <param name="data">The data.</param>
        public void FromByteArray(byte[] data)
        {
            // preconditions
            if ((data.Length < SINGLE_PATCH_LENGTH) || (data.Length < SINGLE_PATCH_INTRO_LENGTH))
            {
                string messsage = string.Format("Not enough sysex data. Waiting for {0} bytes (SINGLE_PATCH_LENGTH), but get only {1} bytes:{2}",
                    SINGLE_PATCH_LENGTH, data.Length, SysExMessage.ToString(data));
                throw new NonFatalException(messsage);
            }

            //try to get the patch intro from the data
            int position = 0;
            bool singlePatchDataFound = false;
            bool endOfBuffer = position + SINGLE_PATCH_INTRO_LENGTH >= SINGLE_PATCH_LENGTH;
            while (!singlePatchDataFound && !(endOfBuffer))
            {
                if (!((data[position] == PATCH_INTRO[0]) &&
                    (data[position + 1] == PATCH_INTRO[1]) &&
                    (data[position + 2] == PATCH_INTRO[2]) &&
                    (data[position + 3] == PATCH_INTRO[3]) &&
                    (data[position + 4] == PATCH_INTRO[4])))
                {
                    position++;
                    endOfBuffer = (position + SINGLE_PATCH_LENGTH > data.Length);
                }
                else
                {
                    singlePatchDataFound = true;
                    // skip program number
                    position += SINGLE_PATCH_INTRO_LENGTH;
                    break;
                }
            }

            // do not read program number
            if (!singlePatchDataFound)
            {
                throw new NonFatalException("No single patch found in sysex data");
            }
            // get a stream at the position, easier to handle
            using (MemoryStream ms = new MemoryStream(data))
            {
                ms.Seek(position, SeekOrigin.Begin);
                this.FromStream(ms);
            }
        }

        /// <summary>
        /// "serialization" to byte array
        /// </summary>
        /// <param name="programNumber">The program number.</param>
        /// <returns></returns>
        public byte[] ToByteArray(int programNumber)
        {
            byte[] data = new byte[SINGLE_PATCH_LENGTH];
            // define patch intro
            int pos;
            for (pos = 0; pos < SINGLE_PATCH_INTRO_LENGTH - 1; pos++)
            {
                data[pos] = PATCH_INTRO[pos];
            }
            data[pos++] = (byte)programNumber;
            // get a stream at the position, easier to handle
            MemoryStream ms = new MemoryStream(data);
            ms.Seek(pos, SeekOrigin.Begin);
            BinaryWriter bw = new BinaryWriter(ms);
            PacketizedBinaryWriter writer = new PacketizedBinaryWriter(bw);

            try
            {
                //read VCOs
                foreach (VCO vco in _vcos)
                {
                    writer.WriteByte(vco.freq);
                    writer.WriteSByte(vco.detune);
                    writer.WriteByte(vco.pw);
                    writer.WriteByte(vco.vol);
                    writer.WriteByte(vco.mod);
                    writer.WriteByte(vco.wave);
                }
                // VCF
                writer.WriteByte(_vcf.freq);
                writer.WriteByte(_vcf.res);
                writer.WriteByte(_vcf.fmode);
                writer.WriteByte(_vcf.vca1);
                writer.WriteByte(_vcf.vca2);
                writer.WriteByte(_vcf.mod);
                // FM and LAG
                writer.WriteByte(_fmAndLag.f_amp);
                writer.WriteByte(_fmAndLag.f_dest);
                writer.WriteByte(_fmAndLag.lag_in);
                writer.WriteByte(_fmAndLag.lag_rate);
                writer.WriteByte(_fmAndLag.lag_mode);
                //LFOs
                foreach (LFO lfo in _lfos)
                {
                    writer.WriteByte(lfo.speed);
                    writer.WriteByte(lfo.retrig_mode);
                    writer.WriteByte(lfo.lag);
                    writer.WriteByte(lfo.wave);
                    writer.WriteByte(lfo.retrig);
                    writer.WriteByte(lfo.sample);
                    writer.WriteByte(lfo.amp);
                }
                //ENV
                foreach (Enveloppe env in _enveloppes)
                {
                    writer.WriteByte(env.flags);
                    writer.WriteByte(env.lfotrig);
                    writer.WriteByte(env.delay);
                    writer.WriteByte(env.attack);
                    writer.WriteByte(env.decay);
                    writer.WriteByte(env.sustain);
                    writer.WriteByte(env.release);
                    writer.WriteByte(env.amp);
                }
                // TRACK
                foreach (Track track in _tracks)
                {
                    writer.WriteByte(track.input);
                    writer.WriteBytes(track.points);
                }
                //RAMPS
                foreach (Ramp ramp in _ramps)
                {
                    writer.WriteByte(ramp.rate);
                    writer.WriteByte(ramp.flags);
                    writer.WriteByte(ramp.lfotrig);
                }
                //MODENTRIES
                foreach (ModulationEntry modentry in _modulationEntries)
                {
                    writer.WriteByte(modentry.source);

                    byte Amount = (byte)Math.Abs(modentry.amount);
                    if (modentry.amount < 0)
                    {
                        Amount = (byte)(Amount | SIGN_MASK);
                    }
                    if (modentry.quantize)
                    {
                        Amount = (byte)(Amount | QUANTIZE_MASK);
                    }
                    writer.WriteByte(Amount);
                    writer.WriteByte(modentry.dest);
                }
                //NAME
                writer.WriteBytes(_name.name);
                // and... the EOX (this one is not packed !)
                bw.Write((byte)Sanford.Multimedia.Midi.SysExType.Continuation);
            }
            finally
            {
                writer?.Close();
                ms?.Close();
            }
            return data;
        }

        /// <summary>
        /// Utility fonction : Get single patch name from single patch byte array
        /// </summary>
        /// <param name="bytes">the byte array</param>
        /// <returns>the name, string.empty if any error</returns>
        internal static string GetNameFromByteArray(byte[] bytes)
        {
            Debug.Assert(bytes != null);

            if (bytes.Length != SINGLE_PATCH_LENGTH)
            {
                return string.Empty;
            }

            PatchName patchName = new PatchName();
            using (MemoryStream ms = new MemoryStream(bytes))
            {
                // name is located at the top end of the array
                ms.Seek(-PatchName.NAME_LENTH * 2 - 1, SeekOrigin.End);

                using (BinaryReader br = new BinaryReader(ms))
                {
                    PacketizedBinaryReader reader = new PacketizedBinaryReader(br);
                    patchName.name = reader.ReadBytes(PatchName.NAME_LENTH);
                }
            }
            return patchName.Name;
        }
    }
}