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
using MidiApp.MidiController.Controller;
using System;
using System.Collections.Generic;
using System.Linq;
using Xplorer.Common;
using Xplorer.Model;

namespace Xplorer.Controller
{
    /// <summary>
    /// Controller mod matrix handling
    /// </summary>
    internal partial class XpanderController : AbstractController
    {
        /// <summary>
        /// Changes the modulation destination.
        /// </summary>
        /// <param name="modulationSource">The modulation source.</param>
        /// <param name="modulationSourceAmount">The modulation source amount.</param>
        /// <param name="modulationQuantize">The modulation quantize.</param>
        /// <param name="oldModulationDestination">The old modulation destination.</param>
        /// <param name="newModulationDestination">The new modulation destination.</param>
        /// <param name="entryNumber">The entry number.</param>
        internal void ChangeModulationDestination(int modulationSource, int modulationSourceAmount, int modulationQuantize,
            int oldModulationDestination, int newModulationDestination, int entryNumber)
        {
            if (EnableSetParameter)
            {
                ((XpanderTone)Tone).ChangeModulationDestination(modulationSource, modulationSourceAmount, modulationQuantize, oldModulationDestination, newModulationDestination, entryNumber, this.EnQueueParameter);
            }
        }

        /// <summary>
        /// Changes the modulation source.
        /// </summary>
        /// <param name="newModulationSource">The new modulation source.</param>
        /// <param name="modulationSourceAmount">The modulation source amount.</param>
        /// <param name="modulationQuantize">The modulation quantize.</param>
        /// <param name="modulationDestination">The modulation destination.</param>
        /// <param name="entryNumber">The entry number.</param>
        internal void ChangeModulationSource(int newModulationSource, int modulationSourceAmount, int modulationQuantize,
            int modulationDestination, int entryNumber)
        {
            if (EnableSetParameter)
            {
                ((XpanderTone)Tone).ChangeModulationSource(newModulationSource, modulationSourceAmount, modulationQuantize, modulationDestination, entryNumber, this.EnQueueParameter);
            }
        }

        /// <summary>
        /// Changes the modulation source amount.
        /// </summary>
        /// <param name="modulationSource">The modulation source.</param>
        /// <param name="modulationSourceAmount">The modulation source amount.</param>
        /// <param name="modulationDestination">The modulation destination.</param>
        /// <param name="entryNumber">The entry number.</param>
        internal void ChangeModulationSourceAmount(int modulationSource, int modulationSourceAmount, int modulationDestination, int entryNumber)
        {
            if (EnableSetParameter)
            {
                ((XpanderTone)Tone).ChangeModulationSourceAmount(modulationSource, modulationSourceAmount, modulationDestination, entryNumber, this.EnQueueParameter);
            }
        }

        /// <summary>
        /// Changes the modulation source quantize.
        /// </summary>
        /// <param name="modulationSource">The modulation source.</param>
        /// <param name="modulationDestination">The modulation destination.</param>
        /// <param name="mModulationQuantize">The m modulation quantize.</param>
        /// <param name="entryNumber">The entry number.</param>
        internal void ChangeModulationSourceQuantize(int modulationSource, int modulationDestination, int mModulationQuantize, int entryNumber)
        {
            if (EnableSetParameter)
            {
                ((XpanderTone)Tone).ChangeModulationSourceQuantize(modulationSource, modulationDestination, mModulationQuantize, entryNumber, this.EnQueueParameter);
            }
        }

        /// <summary>
        /// helper for mod matrix UI update - if max source is reached, user can't add more source, UI is disabled
        /// </summary>
        /// <param name="modulationDestination">The modulation destination.</param>
        /// <returns>
        ///   <c>true</c> if [is max source count for destination reached] [the specified modulation destination]; otherwise, <c>false</c>.
        /// </returns>
        internal bool IsMaxSourceCountForDestinationReached(XpanderConstants.EnumModulationDestinations modulationDestination)
        {
            return ((XpanderTone)Tone).IsMaxSourceCountForDestinationReached(modulationDestination);
        }

        /// <summary>
        /// Returns true if a source can be choosen for the given entry, depending on the modulation destination of the entry
        /// </summary>
        /// <param name="entryNumber">The entry number.</param>
        /// <returns></returns>
        public bool SourceAvailabilityForEntry(int entryNumber)
        {
            ModulationMatrixEntry entry = GetModulationEntryByNumber(entryNumber);
            bool isMaxSourceCountForDestinationReached = IsMaxSourceCountForDestinationReached(entry.Destination);

            bool availability =
                (!isMaxSourceCountForDestinationReached) ||
                (isMaxSourceCountForDestinationReached && entry.Source != XpanderConstants.EnumModulationSourcesModMatrix.NONE);

            return availability;
        }

        /// <summary>
        /// Get all the available destination for the entry
        /// </summary>
        /// <param name="entryNumber"></param>
        /// <returns></returns>
        internal System.Collections.Generic.IEnumerable<XpanderConstants.EnumModulationDestinations> GetAvailableModulationDestinationsForEntry(int entryNumber)
        {
            List<XpanderConstants.EnumModulationDestinations> destinations = new List<XpanderConstants.EnumModulationDestinations>();
            ModulationMatrixEntry entry = GetModulationEntryByNumber(entryNumber);

            // only add available destinations, and the current destination of the entry; place the enrty destination at the same index
            foreach (XpanderConstants.EnumModulationDestinations destination in Enum.GetValues(XpanderConstants.ModulationDestinationType))
            {
                if (!IsMaxSourceCountForDestinationReached(destination) || (IsMaxSourceCountForDestinationReached(destination) && destination == entry.Destination))
                {
                    destinations.Add(destination);
                }
            }

            return destinations.AsEnumerable();
        }
    }
}