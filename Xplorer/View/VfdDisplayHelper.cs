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
using MidiApp.UIControls;
using System;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Resources;
using System.Windows.Forms;
using Xplorer.Common;

using Xplorer.Controller;
using Xplorer.Model;
using Xplorer.Properties;

namespace Xplorer.View
{
    /// <summary>
    /// A class to handle VFD display update logic
    /// </summary>
    internal sealed class VfdDisplayHelper
    {
        //controller
        private readonly AbstractController _controller;

        //display
        private readonly VacuumFluoDisplayControl _display;

        // delegate to get parameter name from tag value
        private readonly Func<string, string> _parameterNameForTagDelegate;

        // flag indicating display need to be updated
        private bool _isVfdDisplayUpdateNeeded = false;

        // falg indicating that whole vfd update in needed
        private bool _isVfdDisplayWholeUpdateNeeded = false;

        private IValuedControl _lastUpdatedControl = null;
        private ModulationMatrixEntry _lastUpdatedModEntry = null;
        private bool _maxSourceReached = false;

        /// <summary>
        /// Do not use this one
        /// </summary>
        private VfdDisplayHelper()
        {
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="display">the display</param>
        /// <param name="controler">the controler</param>
        /// <param name="parameterNameForTagDelegate">elegate to get parameter name from tag value</param>
        internal VfdDisplayHelper(VacuumFluoDisplayControl display, AbstractController controller, Func<string, string> parameterNameForTagDelegate)
        {
            Debug.Assert(display != null && display.IsHandleCreated && controller != null && parameterNameForTagDelegate != null);

            _display = display;
            _controller = controller;
            _parameterNameForTagDelegate = parameterNameForTagDelegate;
        }

        /// <summary>
        /// Updates display
        /// </summary>
        internal void UpdateState()
        {
            UpdateState(null, null, false);
        }

        /// <summary>
        /// Updates display
        /// </summary>
        internal void UpdateState(IValuedControl lastUpdateControl)
        {
            UpdateState(lastUpdateControl, null, false);
        }

        /// <summary>
        /// Updates display
        /// </summary>
        internal void UpdateState(ModulationMatrixEntry lastUpdatedModEntry, bool maxSourceReached)
        {
            UpdateState(null, lastUpdatedModEntry, maxSourceReached);
        }

        /// <summary>
        /// Update helper's state
        /// </summary>
        /// <param name="lastUpdateControl">The last update control.</param>
        /// <param name="lastUpdatedModEntry">The last updated mod entry.</param>
        private void UpdateState(IValuedControl lastUpdateControl, ModulationMatrixEntry lastUpdatedModEntry, bool maxSourceReached)
        {
            _lastUpdatedControl = lastUpdateControl;
            _lastUpdatedModEntry = lastUpdatedModEntry;
            _maxSourceReached = maxSourceReached;
            _isVfdDisplayUpdateNeeded = true;
        }

        /// <summary>
        /// Force whole VFD update
        /// </summary>
        internal void ForceUpdate()
        {
            _isVfdDisplayUpdateNeeded = true;
            _isVfdDisplayWholeUpdateNeeded = true;
        }

        /// <summary>
        /// Update the display
        /// </summary>
        internal void UpdateDisplay()
        {
            if (!_isVfdDisplayUpdateNeeded) return;

            if (_isVfdDisplayWholeUpdateNeeded)
            {
                //force whole update
                _display.SetText(string.Empty);
                _isVfdDisplayWholeUpdateNeeded = false;
            }

            ResourceManager resourceManager = Resources.ResourceManager;
            XpanderController xController = (XpanderController)_controller;

            string displayLine1 = string.Format("* S{0} {1} *", xController.CurrentProgramNumber.ToString("00", CultureInfo.InvariantCulture), xController.ToneName);
            string displayLine2 = string.Empty;
            string displayLine3 = string.Empty;

            // a control was tweaked
            if (_lastUpdatedControl != null)
            {
                // value depends on control's type
                string sValue = null;
                // checkbox -> Y/N
                if (_lastUpdatedControl as CheckBoxValuedControl != null)
                {
                    sValue = _lastUpdatedControl.Value == 1 ? "Y" : "N";
                }
                // combobox, show resource
                else if (_lastUpdatedControl as ComboBoxValuedControl != null)
                {
                    ComboBoxValuedControlItem item = (ComboBoxValuedControlItem)((ComboBoxValuedControl)_lastUpdatedControl).SelectedItem;
                    sValue = item.ToString();
                }
                // radiobutton, use control's text
                else if ((_lastUpdatedControl as RadioButtonPanel != null))
                {
                    RadioButtonPanel panel = (RadioButtonPanel)_lastUpdatedControl;
                    RadioButton button = panel.Controls.OfType<RadioButton>().FirstOrDefault(rd => rd.Checked);
                    if (button != null) sValue = button.Text;
                }
                // knobcontrol, value of the control
                else
                {
                    sValue = _lastUpdatedControl.Value.ToString("00", CultureInfo.InvariantCulture);
                }
                // get parameter name from resources
                string parameterName = _parameterNameForTagDelegate((string)((Control)_lastUpdatedControl).Tag);
                string endUserParameterName = resourceManager.GetString(parameterName);

                // depending on the length of the text, make it a single line or two.
                displayLine2 = string.Format(CultureInfo.InvariantCulture, "{0}:{1}", string.IsNullOrEmpty(endUserParameterName) ? parameterName : endUserParameterName, sValue);
                if (displayLine2.Length > _display.MaxCharsPerLine)
                {
                    displayLine2 = string.Format(CultureInfo.InvariantCulture, "{0}:", string.IsNullOrEmpty(endUserParameterName) ? parameterName : endUserParameterName);
                    displayLine3 = sValue;
                }
                _lastUpdatedControl = null;
            }

            // a modulation change happened
            else if (_lastUpdatedModEntry != null)
            {
                if (Enum.GetName(_lastUpdatedModEntry.Source.GetType(), _lastUpdatedModEntry.Source) !=
                    Enum.GetName(typeof(XpanderConstants.EnumModulationSourcesModMatrix), XpanderConstants.EnumModulationSourcesModMatrix.NONE))
                {
                    // something changed in the modulation matrix
                    displayLine2 = string.Format(CultureInfo.InvariantCulture, "{0:G} TO {1:G}:",
                        UIService.GetStringForEnumValue(typeof(XpanderConstants.EnumModulationSourcesModMatrix), (int)_lastUpdatedModEntry.Source, resourceManager),
                        UIService.GetStringForEnumValue(typeof(XpanderConstants.EnumModulationDestinations), (int)_lastUpdatedModEntry.Destination, resourceManager));
                    displayLine3 = string.Format(CultureInfo.InvariantCulture, "AMNT:{0} QTZ:{1}", _lastUpdatedModEntry.Amount.ToString("00"), _lastUpdatedModEntry.Quantize == 1 ? "Y" : "N");
                }
                else
                {
                    if (_maxSourceReached)
                    {
                        displayLine2 = "MAX SRC COUNT REACHED FOR";
                        displayLine3 = string.Format(CultureInfo.InvariantCulture, "{0:G}",
                            UIService.GetStringForEnumValue(typeof(XpanderConstants.EnumModulationDestinations), (int)_lastUpdatedModEntry.Destination, resourceManager));
                        _maxSourceReached = false;
                    }
                }
                _lastUpdatedModEntry = null;
            }

            _display.SetText(string.Format(CultureInfo.InvariantCulture, "{0}{1}{2}",
                    displayLine1.PadRight(_display.MaxCharsPerLine),
                    displayLine2.PadRight(_display.MaxCharsPerLine),
                    displayLine3.PadRight(_display.MaxCharsPerLine)));

            _isVfdDisplayUpdateNeeded = false;
        }
    }
}