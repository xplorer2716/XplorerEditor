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
using System.Diagnostics;
using System.Linq;
using System.Windows.Forms;

namespace Xplorer.View.Settings
{
    /// <summary>
    /// Settings form
    /// </summary>
    internal partial class SettingsForm : Form
    {
        #region pages

        // I could use TabControl but:
        // - .NET provides no ways to customize background color for such controls
        // - custom implementations available on the net do make a heavy use of WIN32 API (we want to be platform independent as possible)
        // - so use the KISS principle
        private MidiPage _midiPage = null;

        /// <summary>
        /// Midi page
        /// </summary>
        public MidiPage MidiPage
        {
            get
            {
                if (_midiPage == null)
                {
                    _midiPage = new MidiPage();
                }
                return _midiPage;
            }
            private set
            {
                _midiPage = value;
            }
        }

        private UserInterfacePage _userInterfacePage = null;

        /// <summary>
        /// User inteface page
        /// </summary>
        public UserInterfacePage UserInterfacePage
        {
            get
            {
                if (_userInterfacePage == null)
                {
                    _userInterfacePage = new UserInterfacePage();
                }
                return _userInterfacePage;
            }
            private set
            {
                _userInterfacePage = value;
            }
        }

        private RandomizerPage _randomizerPage = null;

        /// <summary>
        /// Randomizer page
        /// </summary>
        public RandomizerPage RandomizerPage
        {
            get
            {
                if (_randomizerPage == null)
                {
                    _randomizerPage = new RandomizerPage();
                }
                return _randomizerPage;
            }
            private set
            {
                _randomizerPage = value;
            }
        }

        #endregion pages

        /// <summary>
        /// Initializes a new instance of the <see cref="SettingsForm"/> class.
        /// </summary>
        public SettingsForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Handles the Click event of the okButton control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void okButton_Click(object sender, EventArgs e)
        {
            _tableLayoutPanel
                .Controls.Cast<ISettingsPage>().ToList()
                .ForEach(page => page.ProcessOnOK());
        }

        /// <summary>
        /// Handles the Click event of the cancelButton control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void cancelButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        /// <summary>
        /// Handles the Load event of the SettingsDialog control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        protected override void OnLoad(EventArgs e)
        {
            _midiPageButton.ForeColor = _userInterfacePage.KnobLedBorderColor;
            _midiPageButton.FlatAppearance.BorderColor = _userInterfacePage.KnobLedBorderColor;

            _UIPageButton.ForeColor = _userInterfacePage.KnobLedBorderColor;
            _UIPageButton.FlatAppearance.BorderColor = _userInterfacePage.KnobLedBorderColor;

            _randomizerPageButton.ForeColor = _userInterfacePage.KnobLedBorderColor;
            _randomizerPageButton.FlatAppearance.BorderColor = _userInterfacePage.KnobLedBorderColor;

            base.OnLoad(e);
            ShowPage(MidiPage);
        }

        /// <summary>
        /// Handles the CheckedChanged event of the _MidiPageButton control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _MidiPageButton_CheckedChanged(object sender, EventArgs e)
        {
            if (_midiPageButton.Checked)
            {
                ShowPage(MidiPage);
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the _UIPageButton control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _UIPageButton_CheckedChanged(object sender, EventArgs e)
        {
            if (_UIPageButton.Checked)
            {
                ShowPage(UserInterfacePage);
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the _randomizerPageButton control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _randomizerPageButton_CheckedChanged(object sender, EventArgs e)
        {
            if (_randomizerPageButton.Checked)
            {
                ShowPage(RandomizerPage);
            }
        }

        /// <summary>
        /// Show the given page
        /// </summary>
        /// <param name="page">The page.</param>
        private void ShowPage(UserControl page)
        {
            Debug.Assert(page != null);

            SuspendLayout();
            page.SuspendLayout();

            _tableLayoutPanel.Controls.Cast<Control>().ToList().ForEach(c => c.Visible = false);
            page.Parent = _tableLayoutPanel;
            page.Dock = DockStyle.Fill;

            // initialize the page if it isn't done
            ISettingsPage settingsPage = page as ISettingsPage;
            Debug.Assert(settingsPage != null);
            if (!settingsPage.IsInitialized)
            {
                settingsPage.Initialize();
            }
            page.Visible = true;

            page.ResumeLayout();
            ResumeLayout();
        }
    }
}