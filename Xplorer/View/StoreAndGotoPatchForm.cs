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
using MidiApp.UIControls;
using System;
using System.Windows.Forms;
using Xplorer.Model;

namespace Xplorer.View
{
    /// <summary>
    /// Form for "goto patch #" and "Store patch to #"
    /// </summary>
    internal sealed partial class StoreAndGotoPatchForm : Form
    {
        private int _programNumber;

        public int ProgramNumber
        {
            get { return _programNumber; }
            set { _programNumber = value; }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="StoreAndGotoPatchForm"/> class.
        /// </summary>
        public StoreAndGotoPatchForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// CreateParams override (double buffering)
        /// </summary>
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams createParams = base.CreateParams;
                if (!ExecutionModeService.IsDesignModeActive)
                {
                    const int WS_ES_COMPOSITED = 0x02000000;
                    // Activate double buffering at the form level.  All child controls will be double buffered as well.
                    createParams.ExStyle |= WS_ES_COMPOSITED;   // WS_EX_COMPOSITED
                }
                return createParams;
            }
        }

        /// <summary>
        /// Handles the Click event of the btOK control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void btOK_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.ProgramNumber = (int)_ComboProgramNumber.SelectedItem;
            Close();
        }

        /// <summary>
        /// Handles the Click event of the btCancel control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void btCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            Close();
        }

        /// <summary>
        /// Handles the Load event of the StoreAndGotoPatchFor control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void StoreAndGotoPatchFor_Load(object sender, EventArgs e)
        {
            // tag for the licence key validation code injection

            bool isSelected = false;
            for (int i = XpanderTone.MIN_PROGRAM_NUMBER + 1; i < XpanderTone.MAX_PROGRAM_NUMBER + 1; i++)
            {
                _ComboProgramNumber.Items.Add(i);
                if (i == ProgramNumber)
                {
                    this._ComboProgramNumber.SelectedItem = i;
                    isSelected = true;
                }
            }
            if (!isSelected)
            {
                this._ComboProgramNumber.SelectedIndex = 0;
            }
        }
    }
}