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
using System.Drawing;
using System.Windows.Forms;

namespace Xplorer.View
{
    /// <summary>
    /// Generic progression form
    /// </summary>
    internal sealed partial class ProgressForm : Form
    {
        /// <summary>
        /// Gets or sets the label.
        /// </summary>
        /// <value>
        /// The label.
        /// </value>
        public string Label
        {
            get { return this._label.Text; }
            set { _label.Text = value; }
        }

        /// <summary>
        /// Gets or sets the min value.
        /// </summary>
        /// <value>
        /// The min value.
        /// </value>
        public int MinValue
        {
            get { return _progressBar.Minimum; }
            set { _progressBar.Minimum = value; }
        }

        /// <summary>
        /// Gets or sets the max value.
        /// </summary>
        /// <value>
        /// The max value.
        /// </value>
        public int MaxValue
        {
            get { return _progressBar.Maximum; }
            set { _progressBar.Maximum = value; }
        }

        /// <summary>
        /// Gets or sets the value.
        /// </summary>
        /// <value>
        /// The value.
        /// </value>
        public int Value
        {
            get { return _progressBar.Value; }
            set { _progressBar.Value = value; }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="RenamePatchForm"/> class.
        /// </summary>
        public ProgressForm()
        {
            InitializeComponent();
            this._progressBar.ForeColor = Color.FromArgb(Xplorer.Controller.Service.Settings.AllUsersSettingsService.AllUsersSettings.UiConfig.KnobLedBorderColor);
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

        #region Factory

        private static ProgressForm _instance;

        /// <summary>
        /// returns / create the progressform instance
        /// </summary>
        /// <param name="parentForm">the parent form</param>
        /// <returns>the instance</returns>
        public static ProgressForm CreateInstance(Form parentForm)
        {
            if (_instance == null)
            {
                _instance = new ProgressForm();
                _instance.Owner = parentForm;
                _instance.StartPosition = FormStartPosition.Manual;
                _instance.TopMost = true;
                _instance.Location =
                    new System.Drawing.Point(
                    _instance.Owner.Location.X + (parentForm.Width - _instance.Width) / 2,
                    _instance.Owner.Location.Y + (parentForm.Height - _instance.Height) / 2);
            }
            return _instance;
        }

        /// <summary>
        /// Destroys the instance.
        /// </summary>
        public static void DestroyInstance()
        {
            if (_instance != null)
            {
                _instance.Hide();
                _instance.Close();
                _instance.Dispose();
                _instance = null;
            }
        }

        public static ProgressForm Instance
        {
            get
            {
                return _instance;
            }
        }

        #endregion Factory
    }
}