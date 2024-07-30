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

namespace Xplorer.Model
{
    /// <summary>
    /// a Wrapper around BinaryReader to read packetized bytes
    /// </summary>
    internal class PacketizedBinaryReader
    {
        private readonly BinaryReader _reader;

        /// <summary>
        /// Prevents a default instance of the <see cref="PacketizedBinaryReader"/> class from being created.
        /// </summary>
        private PacketizedBinaryReader()
        {
            //do not use this
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="PacketizedBinaryReader"/> class.
        /// </summary>
        /// <param name="reader">The reader.</param>
        public PacketizedBinaryReader(BinaryReader reader)
        {
            _reader = reader;
        }

        /// <summary>
        /// Unpacks the specified packet.
        /// </summary>
        /// <param name="packet">The packet.</param>
        /// <returns></returns>
        private byte Unpack(ushort packet)
        {
            return (byte)(((packet & 0x100) >> 1) | (packet & 0xFF));
        }

        /// <summary>
        /// Reads the byte.
        /// </summary>
        /// <returns></returns>
        public virtual byte ReadByte()
        {
            ushort packet = _reader.ReadUInt16();
            return Unpack(packet);
        }

        /// <summary>
        /// Reads the signed byte.
        /// </summary>
        /// <returns></returns>
        public virtual sbyte ReadSByte()
        {
            ushort packet = _reader.ReadUInt16();
            return (sbyte)Unpack(packet);
        }

        /// <summary>
        /// Reads the bytes.
        /// </summary>
        /// <param name="count">The count.</param>
        /// <returns></returns>
        public virtual byte[] ReadBytes(int count)
        {
            var unpacked = new byte[count];
            for (int i = 0; i < count; i++)
            {
                ushort packet = _reader.ReadUInt16();
                unpacked[i] = Unpack(packet);
            }
            return unpacked;
        }

        /// <summary>
        /// Closes this instance.
        /// </summary>
        public virtual void Close()
        {
            _reader.Close();
        }
    }
}