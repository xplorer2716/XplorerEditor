/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2026 Pascal Schmitt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see https://www.gnu.org/licenses/.
*/

namespace Xplorer.Common
{
    /// <summary>
    /// Describes the type of content found in a sysex file.
    /// </summary>
    internal enum SysexFileType
    {
        /// <summary>
        /// File contains a single tone (one single patch sysex message).
        /// </summary>
        SingleTone,

        /// <summary>
        /// File contains multiple sysex messages (bank / all data dump).
        /// </summary>
        AllDataDump,

        /// <summary>
        /// File content could not be identified.
        /// </summary>
        Unknown
    }
}
    