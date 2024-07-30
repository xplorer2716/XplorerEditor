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
using System.Diagnostics;
using Xplorer.Common;

namespace Xplorer.Controller
{
    /// <summary>
    /// internal class for AllDataDump Request
    /// </summary>
    internal sealed class AllDataDumpRequestState
    {
        // state
        private bool _isWaitingForAllDataDumpRequest;

        // lock
        private readonly object _lockObject = new object();

        // reception mode
        private EnumAllDataDumpRequestMode _mode;

        // list of multipatches
        private IList<byte[]> _multipatches;

        // list of single patches
        private IList<Tuple<string, byte[]>> _singlePatches;

        /// <summary>
        /// defines AllDataDumpRequestState reception mode
        /// </summary>
        public enum EnumAllDataDumpRequestMode
        {
            /// <summary>
            /// Undefined
            /// </summary>
            None,

            /// <summary>
            /// Only single patches
            /// </summary>
            SinglePatch,

            /// <summary>
            /// All data dump (single + multi)
            /// </summary>
            All
        }

        /// <summary>
        /// Destination folder (ReceptionMode == Single)  of filename (ReceptionMode == All)
        /// </summary>
        public string Destination { get; private set; }

        /// <summary>
        /// true if still waiting for patches
        /// </summary>
        public bool IsWaitingForAllDataDumpRequest
        {
            get
            {
                return _isWaitingForAllDataDumpRequest;
            }
            set
            {
                lock (_lockObject)
                {
                    _isWaitingForAllDataDumpRequest = value;
                }
            }
        }

        /// <summary>
        /// Received multi patches
        /// </summary>
        public IList<byte[]> MultiPatches
        {
            get
            {
                if (_multipatches == null)
                {
                    _multipatches = new List<byte[]>(XpanderConstants.MULTI_PATCHES_MAX_COUNT);
                }
                return _multipatches;
            }
        }

        /// <summary>
        /// Reception mode
        /// </summary>
        public EnumAllDataDumpRequestMode ReceptionMode
        {
            get { return _mode; }
            private set { _mode = value; }
        }

        /// <summary>
        /// Received single patches
        /// </summary>
        public IList<Tuple<string, byte[]>> SinglePatches
        {
            get
            {
                if (_singlePatches == null)
                {
                    _singlePatches = new List<Tuple<string, byte[]>>(XpanderConstants.SINGLE_TONES_MAX_COUNT);
                }
                return _singlePatches;
            }
        }

        /// <summary>
        /// Clear received  single and muti patches
        /// </summary>
        public void Clear()
        {
            SinglePatches?.Clear();
            MultiPatches?.Clear();
        }

        /// <summary>
        /// Initializes the state
        /// </summary>
        /// <param name="destination">The destination: if receptionMode is All, the All data dump file name. If Single, the destination folder</param>
        /// <param name="receptionMode">The reception mode.</param>
        public void Initialize(string destination, EnumAllDataDumpRequestMode receptionMode)
        {
            Debug.Assert(!string.IsNullOrEmpty(destination));
            Debug.Assert(receptionMode != EnumAllDataDumpRequestMode.None);

            Clear();

            IsWaitingForAllDataDumpRequest = true;
            Destination = destination;
            ReceptionMode = receptionMode;
        }
    }
}