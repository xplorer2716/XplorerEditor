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
using MidiApp.MidiController.Service;
using MidiApp.UIControls;
using System;
using System.Windows.Forms;

namespace Xplorer.View
{
    /// <summary>
    /// Rename patch form
    /// </summary>
    internal sealed partial class ExtractSingleToneForm : Form
    {
        private string _bankFinename;

        /// <summary>
        /// Gets or sets the bank finename.
        /// </summary>
        /// <value>
        /// The bank finename.
        /// </value>
        public string BankFilename
        {
            get { return _bankFinename; }
            private set { _bankFinename = value; }
        }

        private string _destinationFolder;

        /// <summary>
        /// Gets or sets the destination folder.
        /// </summary>
        /// <value>
        /// The destination folder.
        /// </value>
        public string DestinationFolder
        {
            get { return _destinationFolder; }
            private set { _destinationFolder = value; }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ExtractSingleToneForm"/> class.
        /// </summary>
        public ExtractSingleToneForm()
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
            if (string.IsNullOrEmpty(_txtBankFilename.Text))
            {
                this._errorProvider.SetError(this._ChooseBankButton, "Bank file name must be defined!");
                this.DialogResult = System.Windows.Forms.DialogResult.None;
                return;
            }

            if (string.IsNullOrEmpty(this._txtDestinationFolder.Text))
            {
                this._errorProvider.SetError(this._ChooseDestinationFolder, "Destination folder must be defined!");
                this.DialogResult = System.Windows.Forms.DialogResult.None;
                return;
            }

            // validation performed in tbxPatchName_Validating
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
        private void ExtractSingleToneForm_Load(object sender, EventArgs e)
        {
            // tag for the licence key validation code injection
        }

        private void _ChooseBankButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = FileUtils.SYSEX_FILE_FILTER;
            ofd.RestoreDirectory = true;

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                _txtBankFilename.Text = ofd.FileName;
                BankFilename = ofd.FileName;
            }
            ofd.Dispose();
        }

        /// <summary>
        /// Handles the Click event of the _ChooseDestinationFolder control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _ChooseDestinationFolder_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            fbd.ShowNewFolderButton = true;
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                _txtDestinationFolder.Text = fbd.SelectedPath;
                DestinationFolder = fbd.SelectedPath;
            }
            fbd.Dispose();
        }
    }
}