using MidiApp.MidiController.Service;
using MidiApp.UIControls;
using System;
using System.Diagnostics;
using System.Windows.Forms;
using Xplorer.Controller;

namespace Xplorer.View
{
    /// <summary>
    /// The infamous "About" form
    /// </summary>
    internal sealed partial class AboutForm : Form
    {
        private XpanderController _controller = null;

        /// <summary>
        /// Initializes a new instance of the <see cref="AboutForm"/> class (Design Mode only)
        /// </summary>
        public AboutForm()
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
        /// Initializes a new instance of the <see cref="AboutForm"/> class.
        /// </summary>
        /// <param name="controller">The controller.</param>
        internal AboutForm(XpanderController controller)
            : this()
        {
            _controller = controller;
            this._lbVersion.Text = controller.GetProductNameAndVersionAsString();
        }

        /// <summary>
        /// Handles the Click event of the AboutForm control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void AboutForm_Click(object sender, EventArgs e)
        {
            _controller = null;
            Close();
        }

        /// <summary>
        /// Handles the LinkClicked event of the _lbLink control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.Windows.Forms.LinkLabelLinkClickedEventArgs"/> instance containing the event data.</param>
        private void _lbLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            try
            {
                // go to website
                Process.Start(_lbLink.Text);
            }
            catch (Exception ex)
            {
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
            }
        }

        /// <summary>
        /// OnLoad<see cref="E:System.Windows.Forms.Form.Load"/>.
        /// </summary>
        /// <param name="e"><see cref="T:System.EventArgs"/> args</param>
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            _controller.SendGreetingsToSynth("* DO SOME NOIZE *");
        }
    }
}