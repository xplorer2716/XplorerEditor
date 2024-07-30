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
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Xplorer.Controller;

namespace Xplorer.Model
{
    /// <summary>
    /// implementation of IToneReader for Xpander tone
    /// </summary>
    internal class XPanderToneReader : IToneReader
    {
        public void ReadTone(string fileName, AbstractTone tone)
        {
            if (!File.Exists(fileName)) throw new ToneException("File does not exist: " + fileName);

            try
            {
                using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open, FileAccess.Read)))
                {
                    SinglePatchIterator iterator = new SinglePatchIterator(reader.BaseStream);

                    if (!iterator.MoveNext())
                    {
                        throw new NonFatalException("No single patch data found");
                    }
                    byte[] data = iterator.Current;
                    ((XpanderTone)tone).FromByteArray(data);
                }
            }
            catch (Exception e)
            {
                throw new ToneException(string.Format("Unable to load sysex data from {0}.\r\n{1}", fileName, e.Message));
            }
        }

        /// <summary>
        /// Reads tones from a file containing several tones
        /// </summary>
        /// <param name="filename">The filename</param>
        /// <returns>pair of <tone name, tone>. Since tone name can duplicate,  a collection is returned instead of a dictionary</returns>
        public ICollection<Tuple<string, AbstractTone>> ReadTones(string fileName)
        {
            ICollection<Tuple<string, AbstractTone>> result = new List<Tuple<string, AbstractTone>>();

            if (!File.Exists(fileName)) throw new NonFatalException("File does not exist: " + fileName);
            try
            {
                using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open, FileAccess.Read)))
                {
                    SinglePatchIterator iterator = new SinglePatchIterator(reader.BaseStream);
                    foreach (byte[] data in iterator)
                    {
                        XpanderTone tone = new XpanderTone();
                        tone.FromByteArray(data);
                        result.Add(new Tuple<string, AbstractTone>(tone.ToneName, tone));
                    }
                }
                if (!result.Any())
                {
                    throw new NonFatalException("No single patch data found");
                }
            }
            catch (Exception e)
            {
                throw new NonFatalException(string.Format("Unable to load sysex data from {0}.\r\n{1}", fileName, e.Message));
            }

            return result;
        }
    }
}