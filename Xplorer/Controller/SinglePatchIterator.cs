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
using System.IO;
using Xplorer.Model.Tone;

namespace Xplorer.Controller
{
    /// <summary>
    /// A class to iterate single patches from a binary stream
    /// </summary>
    internal class SinglePatchIterator : SysexIterator
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="SinglePatchIterator"/> class.
        /// </summary>
        /// <param name="stream">The stream.</param>
        public SinglePatchIterator(Stream stream) :
            base(stream)
        {
        }

        /// <summary>
        /// Implements MoveNext()
        /// </summary>
        /// <returns></returns>
        public override bool MoveNext()
        {
            // call base implementation to get next sysex
            bool singlePatchFound = false;

            while (!singlePatchFound && base.MoveNext())
            {
                singlePatchFound = IsSinglePatch(Current);
            }
            return singlePatchFound;
        }

        /// <summary>
        /// Utility method. returns true if data contains at least one single patch
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static bool IsSinglePatch(byte[] data)
        {
            if (data.Length < XPanderSinglePatch.PATCH_INTRO.Length) return false;

            // compare to the single patch prologue
            for (int i = 0; i < XPanderSinglePatch.PATCH_INTRO.Length; i++)
            {
                if (data[i] != XPanderSinglePatch.PATCH_INTRO[i])
                {
                    return false;
                }
            }
            return true;
        }
    }
}