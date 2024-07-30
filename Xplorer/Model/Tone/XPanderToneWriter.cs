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
using System;
using System.IO;

namespace Xplorer.Model
{
    /// <summary>
    /// implementation of IToneWriter for Xpander tone
    /// </summary>
    internal class XPanderToneWriter : IToneWriter
    {
        public void WriteTone(string filename, AbstractTone tone)
        {
            try
            {
                using (BinaryWriter writer = new BinaryWriter(File.Open(filename, FileMode.Create)))
                {
                    byte[] data = ((XpanderTone)tone).ToByteArray();
                    writer.Write(data);
                }
            }
            catch (Exception e)
            {
                throw new ToneException(string.Format("Unable to save tone {0} to file {1}:\r\n{2}", tone.ToneName, filename, e.Message));
            }
        }
    }
}