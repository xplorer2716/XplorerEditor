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

namespace Xplorer.Controller.Events
{
    /// <summary>
    /// Args for AllDataDumpRequestProgressionEvent occurs
    /// </summary>
    internal class AllDataDumpRequestProgressionEventArgs : EventArgs
    {
        /// <summary>
        /// All data dump request state
        /// </summary>
        private readonly AllDataDumpRequestState _allDataDumpRequestState = null;

        /// <summary>
        /// Initializes a new instance of the <see cref="AllDataDumpRequestProgressionEventArgs"/> class.
        /// </summary>
        /// <param name="allDataDumpRequestState">State of all data dump request.</param>
        internal AllDataDumpRequestProgressionEventArgs(AllDataDumpRequestState allDataDumpRequestState)
        {
            _allDataDumpRequestState = allDataDumpRequestState;
        }

        /// <summary>
        /// Prevents a default instance of the <see cref="FullToneChangeEventArgs"/> class from being created.
        /// </summary>
        private AllDataDumpRequestProgressionEventArgs()
        {
            Debug.Assert(false, "Do not use this ctor");
        }

        /// <summary>
        /// Gets the state of all data dump request.
        /// </summary>
        /// <value>
        /// The state of all data dump request.
        /// </value>
        public AllDataDumpRequestState AllDataDumpRequestState
        {
            get { return _allDataDumpRequestState; }
        }
    }
}