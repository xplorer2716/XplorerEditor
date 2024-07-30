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
using System.ComponentModel;
using System.Windows.Forms;

namespace Xplorer.View
{
    /// <summary>
    /// Rename patch form
    /// </summary>
    internal sealed partial class RenamePatchForm : Form
    {

        /// <summary>
        /// Gets or sets the name of the patch.
        /// </summary>
        /// <value>
        /// The name of the patch.
        /// </value>
        public string PatchName { get; set; }
       
        /// <summary>
        /// Initializes a new instance of the <see cref="RenamePatchForm"/> class.
        /// </summary>
        public RenamePatchForm()
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
            // validation performed in tbxPatchName_Validating
            PatchName = tbxPatchName.Text;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        /// <summary>
        /// Handles the Click event of the btCancel control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void btCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /// <summary>
        /// Handles the Load event of the RenamePatchFor control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void RenamePatchFor_Load(object sender, EventArgs e)
        {
            this.tbxPatchName.Text = PatchName;

            // tag for the licence key validation code injection
        }

        /// <summary>
        /// Determines whether [is patch name valid] [the specified patch name].
        /// </summary>
        /// <param name="patchName">Name of the patch.</param>
        /// <returns>
        ///   <c>true</c> if [is patch name valid] [the specified patch name]; otherwise, <c>false</c>.
        /// </returns>
        private static bool IsPatchNameValid(string patchName)
        {
            const string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789</>+-*$ ";          
            for (int i = 0; i < patchName.Length; i++)
            {
                if (validChars.LastIndexOf(patchName[i]) == -1)
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// Handles the Validating event of the tbxPatchName control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.ComponentModel.CancelEventArgs"/> instance containing the event data.</param>
        private void tbxPatchName_Validating(object sender, CancelEventArgs e)
        {
            if (!IsPatchNameValid(tbxPatchName.Text))
            {
                tbxPatchNameErrorProvider.SetError(tbxPatchName, "Invalid patch name !");
                e.Cancel = true;
            }
        }
    }
}