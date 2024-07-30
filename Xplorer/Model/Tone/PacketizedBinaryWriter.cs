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
using System.IO;

namespace Xplorer.Model.Tone
{
    /// <summary>
    /// a Wrapper around BinaryWriter to write packetized bytes
    /// </summary>
    internal class PacketizedBinaryWriter
    {
        private readonly BinaryWriter _writer = null;

        /// <summary>
        /// Prevents a default instance of the <see cref="PacketizedBinaryWriter"/> class from being created.
        /// </summary>
        private PacketizedBinaryWriter()
        {
            //do not use this
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="PacketizedBinaryWriter"/> class.
        /// </summary>
        /// <param name="writer">The writer.</param>
        public PacketizedBinaryWriter(BinaryWriter writer)
        {
            _writer = writer;
        }

        /// <summary>
        /// Packs the specified data into a short
        /// </summary>
        /// <param name="data">The data.</param>
        /// <returns></returns>
        private ushort Pack(byte data)
        {
            return (ushort)(((data & 0x80) << 1) | (data & 0x7F));
        }

        /// <summary>
        /// Writes the byte.
        /// </summary>
        /// <param name="data">The data.</param>
        public virtual void WriteByte(byte data)
        {
            ushort packet = Pack(data);
            _writer.Write(packet);
        }

        /// <summary>
        /// Writes the signed byte.
        /// </summary>
        /// <param name="data">The data.</param>
        public virtual void WriteSByte(sbyte data)
        {
            WriteByte((byte)data);
        }

        /// <summary>
        /// Writes the bytes.
        /// </summary>
        /// <param name="bytes">The bytes.</param>
        public virtual void WriteBytes(byte[] bytes)
        {
            int length = bytes.Length;
            for (int i = 0; i < length; i++)
            {
                // not optimized, but used only for track points, who cares...
                WriteByte(bytes[i]);
            }
        }

        /// <summary>
        /// Closes this instance.
        /// </summary>
        public virtual void Close()
        {
            _writer.Close();
        }
    }
}