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
using System.Diagnostics;
using Xplorer.Model;

namespace Xplorer.Controller.Events
{
    /// <summary>
    /// indicate which parameter in the ModulationEntry has changed
    /// </summary>
    internal enum EnumModulationParameter
    {
        MODULATIONSOURCE,
        MODULATIONDESTINATION,
        MODULATIONAMOUNT,
        MODULATIONQUANTIZE,
        ALL,
        NONE,
    }

    /// <summary>
    /// event for a modulation entry change
    /// </summary>
    internal class ModulationEntryChangeEventArgs : EventArgs
    {
        // the updated modulation entry
        private readonly ModulationMatrixEntry _entry = null;

        // modulation entry number
        private readonly int _entryNumber;

        private readonly EnumModulationParameter _parameter;

        /// <summary>
        /// ctor
        /// </summary>
        /// <param name="entry">the entry</param>
        /// <param name="entryNumber">entry number</param>
        /// <param name="parameter">parameter change of the entry</param>
        internal ModulationEntryChangeEventArgs(ModulationMatrixEntry entry, int entryNumber, EnumModulationParameter parameter)
        {
            Debug.Assert(parameter != EnumModulationParameter.NONE);
            _entryNumber = entryNumber;
            _entry = entry;
            _parameter = parameter;
        }

        /// <summary>
        /// Prevents a default instance of the <see cref="ModulationEntryChangeEventArgs"/> class from being created.
        /// </summary>
        private ModulationEntryChangeEventArgs()
        {
            Debug.Fail("Do not use this ctor");
        }

        public EnumModulationParameter ModulationParameter
        {
            get { return _parameter; }
        }

        /// <summary>
        /// the updated modulation entry
        /// </summary>
        internal ModulationMatrixEntry Entry
        {
            get
            {
                return this._entry;
            }
        }

        /// <summary>
        /// The modulation entry number in the modulation matrix
        /// </summary>
        internal int EntryNumber
        {
            get { return _entryNumber; }
        }

        /// <summary>
        /// The updated parameter of the entry
        /// </summary>
        internal EnumModulationParameter Parameter
        {
            get
            {
                return _parameter;
            }
        }
    }
}