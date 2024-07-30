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
using System.Drawing;
using System.Windows.Forms;

namespace Xplorer.View.Settings
{
    /// <summary>
    ///  User interface page impl
    /// </summary>
    public partial class UserInterfacePage : UserControl, ISettingsPage
    {
        /// <summary>
        /// ctor
        /// </summary>
        public UserInterfacePage()
        {
            InitializeComponent();
        }

        #region ISettingsPage Membres

        /// <summary>
        /// Initialize the page
        /// </summary>
        public void Initialize()
        {
            this._knob.LedBorderColor = KnobLedBorderColor;
            _radioLinear.Checked = IsLinearMovement;
            _radioCircular.Checked = !IsLinearMovement;

            _radioFlat.Checked = !IsKnobStandardStyle;
            _radioStandard.Checked = IsKnobStandardStyle;

            _knob.KnobStyle = IsKnobStandardStyle ? MidiApp.UIControls.EnumKnobStyle.Standard : MidiApp.UIControls.EnumKnobStyle.FlatStyle;
            _knob.RotatingMode = IsLinearMovement ? MidiApp.UIControls.EnumRotatingMode.Linear : MidiApp.UIControls.EnumRotatingMode.Rotating;

            _isInitialized = true;
        }

        /// <summary>
        /// set up internal state when OnOK is fired on the page host
        /// </summary>
        public void ProcessOnOK()
        {
            KnobLedBorderColor = _knob.LedBorderColor;
            IsLinearMovement = _knob.RotatingMode == MidiApp.UIControls.EnumRotatingMode.Linear;
            IsKnobStandardStyle = _knob.KnobStyle == MidiApp.UIControls.EnumKnobStyle.Standard;
        }

        private bool _isInitialized = false;

        /// <summary>
        /// True if Initialize was already called
        /// </summary>
        public bool IsInitialized
        {
            get { return _isInitialized; }
            private set { _isInitialized = value; }
        }

        #endregion ISettingsPage Membres

        /// <summary>
        /// Gets or sets the color of the knob border.
        /// </summary>
        /// <value>
        /// The color of the knob led border.
        /// </value>
        public Color KnobLedBorderColor { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether the movement is linear.
        /// </summary>
        /// <value>
        ///   <c>true</c> if movement is linear; otherwise, <c>false</c>.
        /// </value>
        public bool IsLinearMovement { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether the style is standard or flat
        /// </summary>
        /// <value>
        ///   <c>true</c> if style is standard; flat otherwise
        /// </value>
        public bool IsKnobStandardStyle { get; set; }

        /// <summary>
        /// Handles the Click event of the _colorButton control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _colorButton_Click(object sender, EventArgs e)
        {
            _colorDialog.AllowFullOpen = true;
            _colorDialog.AnyColor = true;
            _colorDialog.FullOpen = true;
            _colorDialog.Color = this._knob.LedBorderColor;
            if (_colorDialog.ShowDialog() == DialogResult.OK)
            {
                _knob.LedBorderColor = _colorDialog.Color;
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the _radioLinear control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _radioLinear_CheckedChanged(object sender, EventArgs e)
        {
            if (this._radioLinear.Checked)
            {
                _knob.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            }
            else
            {
                _knob.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Rotating;
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the _radioStandard control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _radioStandard_CheckedChanged(object sender, EventArgs e)
        {
            if (this._radioStandard.Checked)
            {
                _knob.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            }
            else
            {
                _knob.KnobStyle = MidiApp.UIControls.EnumKnobStyle.FlatStyle;
            }
        }
    }
}