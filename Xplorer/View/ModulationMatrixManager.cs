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
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

using MidiApp.MidiController.Service;
using MidiApp.UIControls;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Forms;
using Xplorer.Common;

namespace Xplorer.View
{
    /// <summary>
    /// Manages modulation matrix UI interaction logic (source/destination combos, amount knobs, quantize checkboxes)
    /// </summary>
    internal sealed class ModulationMatrixManager
    {
        private readonly MainForm _form;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="form">The main form</param>
        internal ModulationMatrixManager(MainForm form)
        {
            Debug.Assert(form != null);
            _form = form;
        }

        /// <summary>
        /// Gets the mod entry number by control tag.
        /// </summary>
        /// <param name="controlTag">The control tag.</param>
        /// <returns>The entry number, or 0 if not found.</returns>
        private int GetModEntryNumberByControlTag(string controlTag)
        {
            int separator = controlTag.LastIndexOf("_");
            string numberString = controlTag.Substring(separator + 1, controlTag.Length - (separator + 1));
            int number = 0;
            int.TryParse(numberString, out number);
            return number;
        }

        /// <summary>
        /// A modulation destination combo box value changed
        /// </summary>
        internal void OnModDestChanged(object sender, EventArgs e)
        {
            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)_form.Controls[String.Format("MOD_SRC_{0}", entryNumber)];
                KnobControl knobAmount = (KnobControl)_form.Controls[String.Format("MOD_AMNT_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)sender;
                CheckBoxValuedControl checkboxQuantize = (CheckBoxValuedControl)_form.Controls[String.Format("MOD_QUANTIZE_{0}", entryNumber)];

                // if previous value is same as current don't do anything
                int oldDestinationValueMember = comboDest.Value;
                int.TryParse(comboDest.ValueMember, out oldDestinationValueMember);
                if (oldDestinationValueMember != comboDest.Value)
                {
                    _form.XController.ChangeModulationDestination(comboSource.Value, knobAmount.Value, checkboxQuantize.Value, oldDestinationValueMember, comboDest.Value, entryNumber);
                    _form.VfdDisplayHelper.UpdateState(_form.XController.GetModulationEntryByNumber(entryNumber), false);
                }
            }
        }

        /// <summary>
        /// Event fired when a mod matrix source combo is changed
        /// </summary>
        internal void OnModSourceChanged(object sender, EventArgs e)
        {
            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)sender;
                KnobControl knobAmount = (KnobControl)_form.Controls[String.Format("MOD_AMNT_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)_form.Controls[String.Format("MOD_DEST_{0}", entryNumber)];
                CheckBoxValuedControl checkboxQuantize = (CheckBoxValuedControl)_form.Controls[String.Format("MOD_QUANTIZE_{0}", entryNumber)];
                _form.XController.ChangeModulationSource(comboSource.Value, knobAmount.Value, checkboxQuantize.Value, comboDest.Value, entryNumber);
                _form.VfdDisplayHelper.UpdateState(_form.XController.GetModulationEntryByNumber(entryNumber), false);
            }
        }

        /// <summary>
        /// Event fired when a mod source amount knob is changed
        /// </summary>
        internal void OnModAmountChanged(object sender)
        {
            KnobControl knob = sender as KnobControl;
            if (knob == null) return;

            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)_form.Controls[String.Format("MOD_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)_form.Controls[String.Format("MOD_DEST_{0}", entryNumber)];
                _form.XController.ChangeModulationSourceAmount(comboSource.Value, knob.Value, comboDest.Value, entryNumber);
                _form.VfdDisplayHelper.UpdateState(_form.XController.GetModulationEntryByNumber(entryNumber), false);
            }
        }

        /// <summary>
        /// Event fired when a mod source quantize is changed
        /// </summary>
        internal void OnModQuantizeChanged(object sender, EventArgs e)
        {
            CheckBoxValuedControl check = sender as CheckBoxValuedControl;
            if (check == null) return;

            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)_form.Controls[String.Format("MOD_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)_form.Controls[String.Format("MOD_DEST_{0}", entryNumber)];
                _form.XController.ChangeModulationSourceQuantize(comboSource.Value, comboDest.Value, check.Value, entryNumber);
                _form.VfdDisplayHelper.UpdateState(_form.XController.GetModulationEntryByNumber(entryNumber), false);
            }
        }

        /// <summary>
        /// Event fired when a modulation Destination combo box begins its drop down.
        /// </summary>
        internal void OnModDestDropDown(object sender, EventArgs e)
        {
            ComboBoxValuedControl comboDest = (ComboBoxValuedControl)sender;
            // memorize the actual value, use ValueMember as _buffer
            comboDest.ValueMember = comboDest.Value.ToString();

            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);

            // give destination choices depending on matrix usage
            IEnumerable<XpanderConstants.EnumModulationDestinations> destinations = _form.XController.GetAvailableModulationDestinationsForEntry(entryNumber);

            comboDest.BeginUpdate();
            object oldSelectedItem = comboDest.SelectedItem;
            comboDest.Items.Clear();
            Type type = XpanderConstants.ModulationDestinationType;
            foreach (XpanderConstants.EnumModulationDestinations destination in destinations)
            {
                string description = UIService.GetStringForEnumValue(type, (int)destination, Properties.Resources.ResourceManager);
                ComboBoxValuedControlItem item = new ComboBoxValuedControlItem(destination, description == null ? Enum.GetName(type, destination) : description);
                comboDest.Items.Add(item);
            }
            // reselect if possible
            if (oldSelectedItem != null && comboDest.Items.Contains(oldSelectedItem))
            {
                comboDest.SelectedItem = oldSelectedItem;
            }
            else
            {
                comboDest.SelectedIndex = 0;
            }

            comboDest.EndUpdate();
        }

        /// <summary>
        /// Event fired when a modulation Source combo box begins its drop down.
        /// </summary>
        internal void OnModSourceDropDown(object sender, EventArgs e)
        {
            ComboBoxValuedControl comboSource = (ComboBoxValuedControl)sender;
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);

            // depending of the number of sources used for the entry, allow another source choice or none.
            bool availability = _form.XController.SourceAvailabilityForEntry(entryNumber);

            comboSource.BeginUpdate();

            if (availability)
            {
                object oldSelectedItem = comboSource.SelectedItem;
                comboSource.SetEnumType(comboSource.GetEnumType(), Properties.Resources.ResourceManager);
                // reselect if possible
                if (oldSelectedItem != null && comboSource.Items.Contains(oldSelectedItem))
                {
                    comboSource.SelectedItem = oldSelectedItem;
                }
                else
                {
                    comboSource.SelectedIndex = 0;
                }
            }
            else
            {
                // only "NONE" available
                comboSource.Items.Clear();
                Type type = XpanderConstants.ModulationSourcesModMatrixType;
                int value = (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE;

                string description = UIService.GetStringForEnumValue(type, value, Properties.Resources.ResourceManager);
                ComboBoxValuedControlItem item = new ComboBoxValuedControlItem(value, description == null ? Enum.GetName(type, value) : description);

                comboSource.Items.Add(item);
                comboSource.SelectedIndex = 0;
                _form.VfdDisplayHelper.UpdateState(_form.XController.GetModulationEntryByNumber(entryNumber), true);
            }

            comboSource.EndUpdate();
        }
    }
}
