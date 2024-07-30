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
using MidiApp.MidiController.Service;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using Xplorer.Common;

namespace Xplorer.Model
{
    /// <summary>
    /// XpanderTone - Modulation Matrix logic
    /// </summary>
    internal partial class XpanderTone : AbstractTone
    {
        internal delegate void UpdateModulationParameterDelegate(AbstractParameter param);

        // map containing the source # for each destination _value of each entry (-> 20*47 entries)
        private const int UNDEFINED_MODULATION_SOURCE_NUMBER = -1;

        public static readonly int NO_AVAILABLE_MOD_ENTRY = -1;

        private Collection<ModulationMatrixEntry> _modulationMatrix = new Collection<ModulationMatrixEntry>();

        /// <summary>
        /// return a read-only view of the internal modulation matrix
        /// </summary>
        internal ReadOnlyCollection<ModulationMatrixEntry> ModulationMatrix
        {
            get
            {
                return new ReadOnlyCollection<ModulationMatrixEntry>(_modulationMatrix);
            }
        }

        /// <summary>
        /// Initialize the modulation matrix with default values (no modulations at all)
        /// </summary>
        private void ClearModulationMatrix()
        {
            _modulationMatrix.Clear();
            for (int entry = 0; entry < XpanderConstants.MODENTRIES_COUNT; entry++)
            {
                _modulationMatrix.Add(new ModulationMatrixEntry()
                {
                    Destination = XpanderConstants.EnumModulationDestinations.VCO1_FRQ,
                    Source = XpanderConstants.EnumModulationSourcesModMatrix.NONE,
                    IdSource = UNDEFINED_MODULATION_SOURCE_NUMBER,
                    Amount = 0,
                    Quantize = 0
                });
            }
        }

        // change the destination source for the specified mod entry
        internal void ChangeModulationSource(int newModulationSource, int modulationSourceAmount, int modulationQuantize,
            int modulationDestination, int entryNumber, UpdateModulationParameterDelegate updateModulationParameterDelegate)
        {
            ModulationMatrixEntry entry = _modulationMatrix[entryNumber - 1];
            if (entry.IdSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
            {
                // if new source is none, delete it, else change it
                if (newModulationSource == (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE)
                {
                    DeleteIDSource((XpanderConstants.EnumModulationDestinations)modulationDestination, entryNumber, updateModulationParameterDelegate);
                }
                else
                {
                    // change the current source
                    //get the delete source parameter for this entry
                    XpanderModMatrixParameter ChangeSourceParameter = (XpanderModMatrixParameter)XpanderTone._CHANGESourceParameter.Clone();
                    PageSubPage pages = XpanderConstants.PageSubPageForModulationDestination[modulationDestination];
                    ChangeSourceParameter.Page = (int)pages.Page;
                    ChangeSourceParameter.SubPage = pages.SubPage;
                    ChangeSourceParameter.IDSource = entry.IdSource;
                    ChangeSourceParameter.Value = newModulationSource; // _value is new source
                    //enqueue it directly instead of letting the worker thread do it for us. to force send before any other command
                    ChangeSourceParameter.Changed = false;
                    // keep the IdSource in the matrix as is, but update the modulation source
                    _modulationMatrix[entryNumber - 1].Source = (XpanderConstants.EnumModulationSourcesModMatrix)newModulationSource;

                    if (updateModulationParameterDelegate != null)
                    {
                        updateModulationParameterDelegate((AbstractParameter)ChangeSourceParameter);
                    }
                }
            }
            else if (newModulationSource != (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE)
            {
                // add a new modulation source
                AddModulationSource(newModulationSource, modulationSourceAmount, modulationQuantize, modulationDestination, entryNumber, updateModulationParameterDelegate);
            }
        }

        /// <summary>
        /// Change  amount of source modulation if defined
        /// </summary>
        /// <param name="modulationSource"></param>
        /// <param name="modulationSourceAmount"></param>
        /// <param name="?"></param>
        /// <param name="entryNumber"></param>

        internal void ChangeModulationSourceAmount(int modulationSource, int modulationSourceAmount, int modulationDestination, int entryNumber,
            UpdateModulationParameterDelegate updateModulationParameterDelegate)
        {
            // change the amount in the source if not set to NONE
            if (modulationSource != (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE)
            {
                // IDSource should be defined already
                ModulationMatrixEntry entry = _modulationMatrix[entryNumber - 1];

                if (entry.IdSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
                {
                    // set amount for this mod source
                    XpanderTone tone = this;
                    XpanderModMatrixParameter SetAmountSourceParameter = (XpanderModMatrixParameter)tone.ParameterMap[tone.GetAmountSourceParameterNameForEntry(entryNumber)];
                    PageSubPage pages = XpanderConstants.PageSubPageForModulationDestination[modulationDestination];

                    // do not resend the same value
                    if (
                        (SetAmountSourceParameter.Page == (int)pages.Page) &&
                        (SetAmountSourceParameter.SubPage == pages.SubPage) &&
                        (SetAmountSourceParameter.IDSource == entry.IdSource) &&
                        (SetAmountSourceParameter.Value == modulationSourceAmount))
                    {
                        return;
                    }
                    else
                    {
                        SetAmountSourceParameter.Page = (int)pages.Page;
                        SetAmountSourceParameter.SubPage = pages.SubPage;
                        SetAmountSourceParameter.IDSource = entry.IdSource;
                        SetAmountSourceParameter.Value = modulationSourceAmount;
                        SetAmountSourceParameter.Changed = false;
                        XpanderModMatrixParameter clone = (XpanderModMatrixParameter)SetAmountSourceParameter.Clone();
                        _modulationMatrix[entryNumber - 1].Amount = modulationSourceAmount;
                        //no need to clone the toogle bit message; original parameter keeps its signed value to permit resend test
                        if (updateModulationParameterDelegate != null)
                        {
                            updateModulationParameterDelegate((AbstractParameter)tone.AmountSetSignMessageForSetSourceAmountParameter(ref clone));
                            updateModulationParameterDelegate(clone);
                        }
                    }
                }
            }
        }

        /// <summary>
        ///  change the quantize of the specified modulationn source
        /// </summary>
        /// <param name="modulationSource"></param>
        /// <param name="modulationDestination"></param>
        /// <param name="modulationQuantize"></param>
        /// <param name="entryNumber"></param>
        internal void ChangeModulationSourceQuantize(int modulationSource, int modulationDestination, int modulationQuantize,
            int entryNumber, UpdateModulationParameterDelegate updateModulationParameterDelegate)
        {
            // change the quantize in the source is not set to NONE
            if (modulationSource != (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE)
            {
                // IDSource should be defined already
                ModulationMatrixEntry entry = _modulationMatrix[entryNumber - 1];

                if (entry.IdSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
                {
                    // set quantize for this mod source
                    XpanderTone tone = this;
                    XpanderModMatrixParameter SetQuantizeSourceParameter = (XpanderModMatrixParameter)tone.ParameterMap[tone.GetQuantizeSourceParameterNameForEntry(entryNumber)];
                    PageSubPage pages = XpanderConstants.PageSubPageForModulationDestination[modulationDestination];
                    SetQuantizeSourceParameter.Page = (int)pages.Page;
                    SetQuantizeSourceParameter.SubPage = pages.SubPage;
                    SetQuantizeSourceParameter.IDSource = entry.IdSource;
                    SetQuantizeSourceParameter.Value = modulationQuantize;
                    SetQuantizeSourceParameter.Changed = false;
                    _modulationMatrix[entryNumber - 1].Quantize = modulationQuantize;
                    if (updateModulationParameterDelegate != null)
                    {
                        updateModulationParameterDelegate((AbstractParameter)SetQuantizeSourceParameter.Clone());
                    }
                }
            }
        }

        // change the destination of the specified mod entry
        // changing the dest in the same as reprogram a new one an delete the old
        internal void ChangeModulationDestination(int modulationSource, int modulationSourceAmount, int modulationQuantize,
            int oldModulationDestination, int newModulationDestination, int entryNumber, UpdateModulationParameterDelegate updateModulationParameterDelegate)
        {
            // delete the old destination if defined
            ModulationMatrixEntry entry = _modulationMatrix[entryNumber - 1];

            if (entry.IdSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
            {
                DeleteIDSource((XpanderConstants.EnumModulationDestinations)oldModulationDestination, entryNumber, updateModulationParameterDelegate);
            }

            // add the source if it's other than NONE
            if (((XpanderConstants.EnumModulationSourcesModMatrix)modulationSource) != XpanderConstants.EnumModulationSourcesModMatrix.NONE)
            {
                AddModulationSource(modulationSource, modulationSourceAmount, modulationQuantize, newModulationDestination, entryNumber, updateModulationParameterDelegate);
            }
            else
            {
                // allow to change the destination in the matrix (for UI purpose). This will not change anything into the synth but allow
                // the user to change the destination even if the source is set to none.
                _modulationMatrix[entryNumber - 1].Destination = (XpanderConstants.EnumModulationDestinations)newModulationDestination;
            }
        }

        /// <summary>
        /// return true is the max of source (6) is reached for the given destination
        /// </summary>
        /// <param name="?"></param>
        /// <returns></returns>
        internal bool IsMaxSourceCountForDestinationReached(XpanderConstants.EnumModulationDestinations modulationDestination)
        {
            int SourceCount = 0;
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                ModulationMatrixEntry entry = _modulationMatrix[i];
                if ((entry.Destination == modulationDestination) && (entry.IdSource != UNDEFINED_MODULATION_SOURCE_NUMBER))
                {
                    SourceCount++;
                }
            }
            bool isMaxSourceCountForDestinationReached = SourceCount >= XpanderConstants.MAX_MODULATION_SOURCE;
            return isMaxSourceCountForDestinationReached;
        }

        /// <summary>
        /// Adds the modulation source.
        /// </summary>
        /// <param name="modulationSource">The modulation source.</param>
        /// <param name="modulationSourceAmount">The modulation source amount.</param>
        /// <param name="modulationQuantize">The modulation quantize.</param>
        /// <param name="newModulationDestination">The new modulation destination.</param>
        /// <param name="entryNumber">The entry number.</param>
        /// <param name="updateModulationParameterDelegate">The update modulation parameter delegate.</param>
        /// <returns></returns>
        internal bool AddModulationSource(int modulationSource, int modulationSourceAmount, int modulationQuantize,
            int newModulationDestination, int entryNumber, UpdateModulationParameterDelegate updateModulationParameterDelegate)
        {
            // get the next available source ID for the new dest
            int ModIDSource = GetNextAvailableModIDSourceForDest((XpanderConstants.EnumModulationDestinations)newModulationDestination);
            if (ModIDSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
            {
                // change the current source
                XpanderTone tone = this;

                // add the new source
                XpanderModMatrixParameter AddSourceParameter = (XpanderModMatrixParameter)XpanderTone._ADDSourceParameter.Clone();
                PageSubPage pages = XpanderConstants.PageSubPageForModulationDestination[newModulationDestination];
                AddSourceParameter.Page = (int)pages.Page;
                AddSourceParameter.SubPage = pages.SubPage;
                AddSourceParameter.IDSource = 0x00;
                AddSourceParameter.Value = modulationSource;
                AddSourceParameter.Changed = false;
                if (updateModulationParameterDelegate != null)
                {
                    updateModulationParameterDelegate((AbstractParameter)AddSourceParameter);
                }

                // set amount for this mod source
                XpanderModMatrixParameter SetAmountSourceParameter = (XpanderModMatrixParameter)tone.ParameterMap[tone.GetAmountSourceParameterNameForEntry(entryNumber)];
                pages = XpanderConstants.PageSubPageForModulationDestination[newModulationDestination];
                SetAmountSourceParameter.Page = (int)pages.Page;
                SetAmountSourceParameter.SubPage = pages.SubPage;
                SetAmountSourceParameter.IDSource = ModIDSource;
                SetAmountSourceParameter.Value = modulationSourceAmount;
                SetAmountSourceParameter.Changed = false;

                //no need to clone the toogle bit message
                if (updateModulationParameterDelegate != null)
                {
                    XpanderModMatrixParameter clone = (XpanderModMatrixParameter)SetAmountSourceParameter.Clone();
                    updateModulationParameterDelegate((AbstractParameter)tone.AmountSetSignMessageForSetSourceAmountParameter(ref clone));
                    updateModulationParameterDelegate((AbstractParameter)clone);
                }

                //set quantize
                XpanderModMatrixParameter SetQuantizeSourceParameter = (XpanderModMatrixParameter)tone.ParameterMap[tone.GetQuantizeSourceParameterNameForEntry(entryNumber)];
                pages = XpanderConstants.PageSubPageForModulationDestination[newModulationDestination];
                SetQuantizeSourceParameter.Page = (int)pages.Page;
                SetQuantizeSourceParameter.SubPage = pages.SubPage;
                SetQuantizeSourceParameter.IDSource = ModIDSource;
                SetQuantizeSourceParameter.Value = modulationQuantize;
                SetQuantizeSourceParameter.Changed = false;
                if (updateModulationParameterDelegate != null)
                {
                    updateModulationParameterDelegate((AbstractParameter)SetQuantizeSourceParameter.Clone());
                }
            }

            // update the matrix
            _modulationMatrix[entryNumber - 1].Destination = (XpanderConstants.EnumModulationDestinations)newModulationDestination;
            _modulationMatrix[entryNumber - 1].Source = (XpanderConstants.EnumModulationSourcesModMatrix)modulationSource;
            _modulationMatrix[entryNumber - 1].IdSource = ModIDSource;
            _modulationMatrix[entryNumber - 1].Amount = modulationSourceAmount;
            _modulationMatrix[entryNumber - 1].Quantize = modulationQuantize;

            return true;
        }

        /// <summary>
        /// delete the source into the Xpander, and reset the local entry to set it available for the next time
        /// </summary>
        /// <param name="oldDestination">The old destination.</param>
        /// <param name="entryNumber">The entry number.</param>
        /// <param name="updateModulationParameterDelegate">The update modulation parameter delegate.</param>
        private void DeleteIDSource(XpanderConstants.EnumModulationDestinations oldDestination, int entryNumber,
            UpdateModulationParameterDelegate updateModulationParameterDelegate)
        {
            //get the delete source parameter for this entry
            XpanderModMatrixParameter DeleteSourceParameter = (XpanderModMatrixParameter)XpanderTone._DELETESourceParameter.Clone();

            // set page and subpage according to destination
            PageSubPage pages = XpanderConstants.PageSubPageForModulationDestination[(int)oldDestination];
            DeleteSourceParameter.Page = (int)pages.Page;
            DeleteSourceParameter.SubPage = pages.SubPage;

            // get the source ID for the destination of this entry
            ModulationMatrixEntry entry = _modulationMatrix[entryNumber - 1];
            if (entry.IdSource != UNDEFINED_MODULATION_SOURCE_NUMBER)
            {
                // the source ID is defined, delete the source; set source ID to DeleteSource parameter
                DeleteSourceParameter.IDSource = entry.IdSource;
                //enqueue it directly instead of letting the worker thread do it for us. to force send before any other command
                DeleteSourceParameter.Changed = false;
                if (updateModulationParameterDelegate != null)
                {
                    updateModulationParameterDelegate((AbstractParameter)DeleteSourceParameter);
                }
            }

            // update the other ID Sources for the same destination
            UpdateIDSourceAfterDelete(entry);

            // reset the current entry
            _modulationMatrix[entryNumber - 1].Reset();
        }

        /// <summary>
        /// Update the other ID Sources of a mod destination where a source was deleted
        /// </summary>
        /// <remarks> when a source number N is deleted, Xpander set source id=N+1 to N,  source id=N+2 to N+1,etc...
        /// we must do the same in our matrix, since we'll get out of sync and edit a bad id source next time
        /// </remarks>
        /// <param name="destination">mod destination where a source was deleted</param>
        private void UpdateIDSourceAfterDelete(ModulationMatrixEntry entry)
        {
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                ModulationMatrixEntry currentEntry = _modulationMatrix[i];
                if ((currentEntry.Destination == entry.Destination) && (currentEntry.IdSource > entry.IdSource))
                {
                    currentEntry.IdSource -= 1;
                }
            }
        }

        // returns the next available source number for the specified destination
        private int GetNextAvailableModIDSourceForDest(XpanderConstants.EnumModulationDestinations modDestination)
        {
            bool[] UsedSources = new bool[XpanderConstants.MAX_MODULATION_SOURCE] { false, false, false, false, false, false };

            // find the available source numbers
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                ModulationMatrixEntry entry = _modulationMatrix[i];
                if ((entry.Destination == modDestination) && (entry.IdSource != UNDEFINED_MODULATION_SOURCE_NUMBER))
                {
                    UsedSources[entry.IdSource] = true;
                }
            }

            // take the first source id available
            for (int i = 0; i < XpanderConstants.MAX_MODULATION_SOURCE; i++)
            {
                if (UsedSources[i] == false)
                {
                    return i;
                }
            }

            // none available
            Logger.WriteLine(this, TraceLevel.Info,
                "XpanderTone.GetNextAvailableModIDSourceForDest: No IdSource available for " +
                Enum.GetName(XpanderConstants.ModulationDestinationType, modDestination));
            return UNDEFINED_MODULATION_SOURCE_NUMBER;
        }

        /// <summary>
        /// Returns the next avaible entry in the matrix (first entry where source is set to NONE / idsource is set to UNDEFINED_MODULATION_SOURCE_NUMBER)
        /// or NO_AVAILABLE_MOD_ENTRY if none
        /// </summary>
        /// <returns></returns>
        internal int GetNextAvailableModEntry()
        {
            // find the available source numbers
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                ModulationMatrixEntry entry = _modulationMatrix[i];
                if (entry.IdSource == UNDEFINED_MODULATION_SOURCE_NUMBER)
                {
                    return i;
                }
            }
            Logger.WriteLine(this, TraceLevel.Info,
                "XpanderTone.GetNextAvailableModEntry: No modulation entry available");
            return NO_AVAILABLE_MOD_ENTRY;
        }

        /// <summary>
        /// Randomize the modulation matrix
        /// </summary>
        /// <remarks>if humanizeRatio is defined, only  amount are modified if enableAmount is set</remarks>
        /// <param name="enableAmount">if set to <c>true</c> [enable amount randomize].</param>
        /// <param name="enableQuantize">if set to <c>true</c> [enable quantize randomize].</param>
        /// <param name="enableSourceAndDest">if set to <c>true</c> [enable source and dest randomize].</param>
        /// <param name="humanizeRatio">The humanize ratio (only for amount)</param>
        internal void RandomizeModulationMatrix(bool enableAmount, bool enableQuantize, bool enableSourceAndDest, float? humanizeRatio)
        {
            ClearModulationMatrix();
            Random randomizer = new Random(unchecked((int)DateTime.Now.Ticks));

            for (int entry = 0; entry < XpanderConstants.MODENTRIES_COUNT; entry++)
            {
                // source and destination
                int NextAvailableModIDSource = UNDEFINED_MODULATION_SOURCE_NUMBER;
                XpanderConstants.EnumModulationDestinations dest = XpanderConstants.EnumModulationDestinations.VCO1_FRQ;

                if (enableSourceAndDest && !humanizeRatio.HasValue)
                {
                    // we must ensure that the randomize destination can accept a modulationsource
                    while (NextAvailableModIDSource == UNDEFINED_MODULATION_SOURCE_NUMBER)
                    {
                        dest = (XpanderConstants.EnumModulationDestinations)
                            randomizer.Next((int)XpanderConstants.EnumModulationDestinations.VCO1_FRQ,
                            (int)XpanderConstants.EnumModulationDestinations.LAG_RATE + 1);

                        NextAvailableModIDSource = GetNextAvailableModIDSourceForDest(dest);
                    }

                    _modulationMatrix[entry].Destination = dest;
                    _modulationMatrix[entry].Source = (XpanderConstants.EnumModulationSourcesModMatrix)
                        randomizer.Next((int)XpanderConstants.EnumModulationSourcesModMatrix.KBD,
                        (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE + 1);
                }

                if (enableAmount)
                {
                    _modulationMatrix[entry].Amount = GetNextRandomValueForModulationAmount(randomizer, _modulationMatrix[entry].Amount, humanizeRatio);
                }

                if (enableQuantize && !humanizeRatio.HasValue)
                {
                    // special handling for "boolean" parameter else we'll rarely get 1
                    if (randomizer.NextDouble() > 0.5F)
                    {
                        _modulationMatrix[entry].Quantize = ModulationMatrixEntry.MAX_QUANTIZE;
                    }
                    else { _modulationMatrix[entry].Quantize = ModulationMatrixEntry.MIN_QUANTIZE; }
                }

                // synchronize amount and quantize parameters of the parameter map
                PageSubPage pages = XpanderConstants.PageSubPageForModulationDestination[(int)dest];
                ((XpanderModMatrixParameter)_parameterMap[GetAmountSourceParameterNameForEntry(entry + 1)]).Page = (int)pages.Page;
                ((XpanderModMatrixParameter)_parameterMap[GetAmountSourceParameterNameForEntry(entry + 1)]).SubPage = pages.SubPage;
                ((XpanderModMatrixParameter)_parameterMap[GetAmountSourceParameterNameForEntry(entry + 1)]).Value = _modulationMatrix[entry].Amount;
                ((XpanderModMatrixParameter)_parameterMap[GetQuantizeSourceParameterNameForEntry(entry + 1)]).Value = _modulationMatrix[entry].Quantize;

                _modulationMatrix[entry].IdSource = NextAvailableModIDSource;
            }
        }

        /// <summary>
        /// Gets the next random value for modulation amount.
        /// </summary>
        /// <param name="randomizer">The randomizer.</param>
        /// <param name="parameter">The parameter.</param>
        /// <param name="humanizeRatio">The humanize ratio (not used)</param>
        /// <returns></returns>
        private int GetNextRandomValueForModulationAmount(Random randomizer, int amount, float? humanizeRatio)
        {
            int value = 0;
            if (!humanizeRatio.HasValue)
            {
                value = randomizer.Next(ModulationMatrixEntry.MIN_AMOUNT, ModulationMatrixEntry.MAX_AMOUNT);
            }
            else
            {
                // determine the operation of the humanize ratio (+/-)
                bool addValue = (randomizer.Next(0, 1) == 1);

                if (addValue)
                {
                    value = (value > ModulationMatrixEntry.MAX_AMOUNT) ? ModulationMatrixEntry.MAX_AMOUNT : value;
                }
                else
                {
                    value = (value < ModulationMatrixEntry.MIN_AMOUNT) ? ModulationMatrixEntry.MIN_AMOUNT : value;
                }
            } // humanizeRatio.HasValue

            return value;
        }

        /// <summary>
        /// for debug purpose: dump all the sources of the destination
        /// </summary>
        /// <param name="modDestination"></param>
        internal void DumpModulationMatrixDestination(XpanderConstants.EnumModulationDestinations modDestination)
        {
            Logger.WriteLine(this, TraceLevel.Verbose, string.Format(" -- Mod Sources for modDestination {0}: --", modDestination));
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                ModulationMatrixEntry entry = _modulationMatrix[i];
                if ((entry.Destination == modDestination))
                {
                    Logger.WriteLine(this, TraceLevel.Verbose, string.Format($"index:{i.ToString("00")}, EntryNumber:{(i + 1).ToString("00")}: {entry.ToString()}"));
                }
            }
        }

        /// <summary>
        /// for debug purpose. dump all the mod matrix
        /// </summary>
        internal void DumpModulationMatrix()
        {
            Logger.WriteLine(this, TraceLevel.Verbose, " -- Modulation Matrix: --");
            for (int i = 0; i < XpanderConstants.MODENTRIES_COUNT; i++)
            {
                ModulationMatrixEntry entry = _modulationMatrix[i];
                Logger.WriteLine(this, TraceLevel.Verbose, string.Format($"index:{i.ToString("00")}, EntryNumber:{(i + 1).ToString("00")}: {entry.ToString()}"));
            }
        }
    }
}