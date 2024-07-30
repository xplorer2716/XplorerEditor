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
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using Xplorer.Model;

namespace Xplorer.Controller.Events
{
    /// <summary>
    /// Event fired when a full tone change occurs
    /// </summary>
    internal class FullToneChangeEventArgs : EventArgs
    {
        private readonly ReadOnlyCollection<ModulationMatrixEntry> _modulationMatrix;
        private readonly Dictionary<string, int> _parameterMap;

        internal FullToneChangeEventArgs(
                    Dictionary<string, int> ParameterMap,
                    ReadOnlyCollection<ModulationMatrixEntry> ModulationMatrix)
        {
            _parameterMap = ParameterMap;
            _modulationMatrix = ModulationMatrix;
        }

        /// <summary>
        /// Prevents a default instance of the <see cref="FullToneChangeEventArgs"/> class from being created.
        /// </summary>
        private FullToneChangeEventArgs()
        {
            Debug.Fail("Do not use this ctor");
        }

        /// <summary>
        /// Gets the parameter map.
        /// </summary>
        public Dictionary<string, int> ParameterMap
        {
            get { return _parameterMap; }
        }

        /// <summary>
        /// Gets the modulation matrix.
        /// </summary>
        internal ReadOnlyCollection<ModulationMatrixEntry> ModulationMatrix
        {
            get
            {
                return _modulationMatrix;
            }
        }
    }
}