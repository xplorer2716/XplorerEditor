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
using Xplorer.Common;

namespace Xplorer.Model
{
    /// <summary>
    /// additional properties for XpanderTone
    /// </summary>
    internal partial class XpanderTone : AbstractTone
    {
        public const int MIN_PROGRAM_NUMBER = 0;
        public const int MAX_PROGRAM_NUMBER = XpanderConstants.SINGLE_TONES_MAX_COUNT - 1;
        private const int DEFAULT_EDITING_NUMBER = MAX_PROGRAM_NUMBER;
        private int _currentProgramNumber = DEFAULT_EDITING_NUMBER;
        private int _editingProgramNumber = DEFAULT_EDITING_NUMBER;

        /// <summary>
        /// default editing program number
        /// </summary>
        public int EditingProgramNumber
        {
            get { return _editingProgramNumber; }
            set { _editingProgramNumber = value; }
        }

        /// <summary>
        /// current program number. Progam number logic is handled (99+1 -> 00; 00-1 -> 99)
        /// </summary>
        public int CurrentProgramNumber
        {
            get { return _currentProgramNumber; }
            set
            {
                if (value < MIN_PROGRAM_NUMBER)
                {
                    _currentProgramNumber = MAX_PROGRAM_NUMBER;
                }
                else if (value > MAX_PROGRAM_NUMBER)
                {
                    _currentProgramNumber = MIN_PROGRAM_NUMBER; // reloop
                }
                else
                {
                    _currentProgramNumber = value;
                }
            }
        }
    }
}