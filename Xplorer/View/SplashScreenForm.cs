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
using System.Diagnostics;
using System.Windows.Forms;

namespace Xplorer.View
{
    /// <summary>
    /// a basic Splashcreen
    /// </summary>
    internal sealed partial class SplashScreenForm : Form
    {
        private object _lockObject = new object();
        private int _currentStepIndex = 0;
        private int _stepCount = 0;

        /// <summary>
        /// Ctor
        /// </summary>
        public SplashScreenForm()
        {
            InitializeComponent();
            this._labelLoading.Text = string.Empty;
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
        /// Gets or sets the version information.
        /// </summary>
        /// <value>
        /// The version information.
        /// </value>
        public string VersionInformation
        {
            get
            {
                return _lbVersion.Text;
            }
            set
            {
                if (InvokeRequired)
                {
                    Invoke((Action)(() => { _lbVersion.Text = value; }));
                }
                else { _lbVersion.Text = value; }
            }
        }

        /// <summary>
        /// number of steps
        /// </summary>
        /// <value>
        /// The step count.
        /// </value>
        public int StepCount
        {
            get
            {
                return _stepCount;
            }
            set
            {
                lock (_lockObject)
                {
                    Debug.Assert(value > 0);
                    _stepCount = value;
                }
            }
        }

        /// <summary>
        /// Define the next step
        /// </summary>
        /// <param name="message">The message.</param>
        private void NextStepImplementation(string message)
        {
            lock (_lockObject)
            {
                Debug.Assert(_currentStepIndex < StepCount);
                _currentStepIndex++;
                this._labelLoading.Text = string.Format("(Step {0}/{1}): {2}", _currentStepIndex, StepCount, message);
                if (!_timer.Enabled)
                {
                    _timer.Start();
                }
            }
            Application.DoEvents();
        }

        /// <summary>
        /// Define the next step
        /// </summary>
        /// <param name="message">The message.</param>
        public void NextStep(string message)
        {
            if (InvokeRequired)
            {
                Invoke((Action)(() => NextStepImplementation(message)));
            }
            else NextStepImplementation(message);
        }

        /// <summary>
        /// Close the form
        /// </summary>
        public void Terminate()
        {
            if (InvokeRequired)
            {
                Invoke((Action)(() => Close()));
            }
            else this.Close();
        }

        /// <summary>
        /// Timer tick
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _timer_Tick(object sender, EventArgs e)
        {
            lock (_lockObject)
            {
                Action action = () =>
                {
                    if (this.IsHandleCreated)
                    {
                        this._labelLoading.Text += ".";
                    }
                    Application.DoEvents();
                };
                if (InvokeRequired)
                {
                    Invoke(action);
                }
                else action();
            }
        }
    }
}