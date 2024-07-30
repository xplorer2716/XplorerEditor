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
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Xplorer.Common;
using Xplorer.Controller;

namespace Xplorer.View
{
    /// <summary>
    /// Tone moprhing user interface. Work in progress
    /// </summary>
    public sealed partial class ToneMorphingForm : Form
    {
        private XpanderController _controller = null;

        // preview note
        private const int PREVIEW_NOTE = 60; // C4

        // preview note length
        private const int PREVIEW_NOTE_LENGTH = 500; // in ms

        // temporary files for tone A/B, will avoid to retrieve tone
        // from sytnh each time we generate a tone morphing
        private string _toneATemporaryFile = null;

        private string _toneBTemporaryFile = null;

        //choose between red or blue pill...
        private Color _toneAColor = Color.Salmon;

        private Color _toneBColor = Color.SkyBlue;

        /// <summary>
        /// For designer mode only
        /// </summary>
        internal ToneMorphingForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ToneMorphingForm"/> class.
        /// </summary>
        /// <param name="controller">The controller.</param>
        internal ToneMorphingForm(XpanderController controller)
            : this()
        {
            Debug.Assert(controller != null);
            _controller = controller;
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
        /// OnLoad override
        /// </summary>
        /// <param name="e"><see cref="T:System.EventArgs" /> </param>
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            Initialize();
        }

        /// <summary>
        /// Initializes this instance.
        /// </summary>
        private void Initialize()
        {
#warning - we should memorize the current tone value, before showing morph interface so we can restore it on cancel

            _slider.Minimum = 0;
            _slider.Maximum = 100;
            _slider.Value = _slider.Minimum;

            _groupBoxPatchA.ForeColor = _toneAColor;
            _labelA.ForeColor = _toneAColor;

            _groupBoxPatchB.ForeColor = _toneBColor;
            _labelB.ForeColor = _toneBColor;

            _textBoxAFilename.Enabled = false;
            _textBoxBFilename.Enabled = false;
            _textBoxAFilename.Tag = null;
            _textBoxBFilename.Tag = null;

            // emulates radio selection
            _radioAFromSynth.Checked = true;
            _radioBFromSynth.Checked = true;

            // range for patch numbers
            object[] values = Enumerable.Range(1, XpanderConstants.SINGLE_TONES_MAX_COUNT - 1).Reverse().Cast<object>().ToArray();
            _comboAPatchNumber.Items.AddRange(values);
            _comboAPatchNumber.SelectedItem = values.First();
            _comboBPatchNumber.Items.AddRange(values);
            _comboBPatchNumber.SelectedItem = values.First();
        }

        /// <summary>
        /// Plays a note with the current controller tone
        /// </summary>
        private void Play()
        {
            //send note on in UI thread
            _controller.PlayNote(true, new Sanford.Multimedia.Midi.UI.PianoKeyEventArgs(PREVIEW_NOTE));

            // wait async and send note off back in ui thread
            Task task = Task.Delay(PREVIEW_NOTE_LENGTH);
            Task noteOff = task.ContinueWith((o) =>
            {
                _controller.PlayNote(false, new Sanford.Multimedia.Midi.UI.PianoKeyEventArgs(PREVIEW_NOTE));
            }, TaskScheduler.FromCurrentSynchronizationContext());
        }

        #region Tone A

        /// <summary>
        /// Handles the CheckedChanged event of the _radioAFromSynth control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _radioAFromSynth_CheckedChanged(object sender, EventArgs e)
        {
            _comboAPatchNumber.Enabled = _radioAFromSynth.Checked;
            _buttonAChooseFile.Enabled = !_radioAFromSynth.Checked;
            _textBoxAFilename.Tag = null;
            _textBoxAFilename.Text = null;
        }

        /// <summary>
        /// Handles the CheckedChanged event of the _radioAFromFile control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _radioAFromFile_CheckedChanged(object sender, EventArgs e)
        {
            _comboAPatchNumber.Enabled = !_radioAFromFile.Checked;
            _buttonAChooseFile.Enabled = _radioAFromFile.Checked;
        }

        /// <summary>
        /// Handles the SelectedIndexChanged event of the _comboAPatchNumber control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _comboAPatchNumber_SelectedIndexChanged(object sender, EventArgs e)
        {
            // compute morphing on ok and/ or preview only
        }

        /// <summary>
        /// Handles the Click event of the _buttonAChooseFile control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs" /> instance containing the event data.</param>
        private void _buttonAChooseFile_Click(object sender, EventArgs e)
        {
            //use tag for full path
            _textBoxAFilename.Tag = ChooseSysexFile();
            _textBoxAFilename.Text = Path.GetFileName((string)_textBoxAFilename.Tag);
        }

        /// <summary>
        /// Chooses the sysex file.
        /// </summary>
        /// <returns></returns>
        private string ChooseSysexFile()
        {
            string value = string.Empty;
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.Filter = FileUtils.SYSEX_FILE_FILTER;
                ofd.RestoreDirectory = true;

                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    value = ofd.FileName;
                }
            }

            return value;
        }

        /// <summary>
        /// Handles the Click event of the _buttonAPreview control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs" /> instance containing the event data.</param>
        private void _buttonAPreview_Click(object sender, EventArgs e)
        {
            TonePreview(_radioAFromFile, _textBoxAFilename, _comboAPatchNumber);
        }

        /// <summary>
        /// Get a temporary file name to save the current tone to it
        /// </summary>
        /// <param name="suffix">The suffix (used as discriminant).</param>
        /// <returns></returns>
        private string GetTempToneFilename(string suffix)
        {
            return Path.Combine(Path.GetTempPath(), _controller.ToneName + "." + suffix);
        }

        /// <summary>
        /// Handle the Tone A/B  preview.
        /// </summary>
        private void TonePreview(RadioButton radioFromFile, TextBox _textboxFilename, ComboBox comboPatchNumber)
        {
            bool isToneA = radioFromFile.Equals(_radioAFromFile);

            if (radioFromFile.Checked)
            {
                string filename = (string)_textboxFilename.Tag;
                if (File.Exists(filename))
                {
                    _controller.LoadTone(filename);

                    // check if we are previewing A or B
                    if (isToneA)
                    {
                        _toneATemporaryFile = filename;
                    }
                    else
                    {
                        _toneBTemporaryFile = filename;
                    }
                }
            }
            else
            {
                // change current program number
                int? patchNumber = (int?)comboPatchNumber.SelectedItem;
                if (patchNumber.HasValue)
                {
                    _controller.SendProgramChangeAndGetSinglePatchFromSynth(patchNumber.Value);

                    // check if we are previewing A or B
                    if (isToneA)
                    {
                        _toneATemporaryFile = GetTempToneFilename("a");
                        _controller.SaveTone(_toneATemporaryFile);
                    }
                    else
                    {
                        _toneBTemporaryFile = GetTempToneFilename("b");
                        _controller.SaveTone(_toneBTemporaryFile);
                    }
                }
            }

            Play();
        }

        #endregion Tone A

        #region Tone B

        /// <summary>
        /// Handles the CheckedChanged event of the _radioBFromSynth control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _radioBFromSynth_CheckedChanged(object sender, EventArgs e)
        {
            _comboBPatchNumber.Enabled = _radioBFromSynth.Checked;
            _buttonBChooseFile.Enabled = !_radioBFromSynth.Checked;

            _textBoxBFilename.Tag = null;
            _textBoxBFilename.Text = null;
        }

        /// <summary>
        /// Handles the CheckedChanged event of the _radioBFromFile control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _radioBFromFile_CheckedChanged(object sender, EventArgs e)
        {
            _comboBPatchNumber.Enabled = !_radioBFromFile.Checked;
            _textBoxBFilename.Enabled = _radioBFromFile.Checked;
            _buttonBChooseFile.Enabled = _radioBFromFile.Checked;
        }

        /// <summary>
        /// Handles the SelectedIndexChanged event of the _comboBPatchNumber control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _comboBPatchNumber_SelectedIndexChanged(object sender, EventArgs e)
        {
            // compute morphing on ok and/ or preview only
        }

        /// <summary>
        /// Handles the Click event of the _buttonBChooseFile control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _buttonBChooseFile_Click(object sender, EventArgs e)
        {
            _textBoxBFilename.Tag = ChooseSysexFile();
            _textBoxBFilename.Text = Path.GetFileName((string)_textBoxBFilename.Tag);
        }

        /// <summary>
        /// Handles the Click event of the _buttonBPreview control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _buttonBPreview_Click(object sender, EventArgs e)
        {
            TonePreview(_radioBFromFile, _textBoxBFilename, _comboBPatchNumber);
        }

        #endregion Tone B

        /// <summary>
        /// Handles the Click event of the _buttonMorphPreview control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _buttonMorphPreview_Click(object sender, EventArgs e)
        {
            // morphing tone is already in synth's memory so only play a note here
            Play();
        }

        /// <summary>
        /// Handles the ValueChanged event of the _slider control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void _slider_ValueChanged(object sender, EventArgs e)
        {
#warning issue on value, we get 98 max instead of 100 ?
            Debug.WriteLine("_slider_ValueChanged:" + _slider.Value);

            DoMorphing();
        }

        /// <summary>
        /// Handles the Click event of the _buttonOK control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _buttonOK_Click(object sender, EventArgs e)
        {
            //TODO
        }

        /// <summary>
        /// Handles the Click event of the _buttonCancel control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void _buttonCancel_Click(object sender, EventArgs e)
        {
            //TODO on cancel we should reapply the current tone (outside this UI ?)
        }

        /// <summary>
        /// Compute and update the morphing tone
        /// </summary>
        private void DoMorphing()
        {
            Tuple<string, string> filenames = GetSourceToneFilenames();

            bool isValid = filenames.Item1 != null && filenames.Item2 != null;

            if (isValid)
            {
                _controller.MorphTones(filenames.Item1, filenames.Item2, (float)_slider.Value / (float)_slider.Maximum);
            }
            else
            {
                Debug.Fail("No valid tones defined");
            }
        }

        /// <summary>
        /// Get two tone filenames as input parameters for tone morphing, depending on
        /// the last selection and preview use cases
        /// </summary>
        /// <returns></returns>
        private Tuple<string, string> GetSourceToneFilenames()
        {
            string toneA = null;
            string toneB = null;

            // if "from synth", try to get the temporary file name, which is defined if user did a preview
            // else we need to get the tone from synth.
            // if "from file", file should be already defined. if not, it's an error
            if (_radioAFromSynth.Checked)
            {
                // no preview was made
                if (!File.Exists(_toneATemporaryFile))
                {
                    _controller.SendProgramChangeAndGetSinglePatchFromSynth(((int?)_comboAPatchNumber.SelectedItem).Value);
                    _toneATemporaryFile = GetTempToneFilename("a");
                    _controller.SaveTone(_toneATemporaryFile);
                }
                toneA = _toneATemporaryFile;
            }
            else
            {
                if (!File.Exists(_toneATemporaryFile))
                {
                    // file was never choose
                }
            }
            // same for ToneB
            if (_radioBFromSynth.Checked)
            {
                // no preview was made
                if (!File.Exists(_toneBTemporaryFile))
                {
                    _controller.SendProgramChangeAndGetSinglePatchFromSynth(((int?)_comboBPatchNumber.SelectedItem).Value);
                    _toneBTemporaryFile = GetTempToneFilename("b");
                    _controller.SaveTone(_toneBTemporaryFile);
                }
                toneB = _toneBTemporaryFile;
            }
            else
            {
                if (!File.Exists(_toneBTemporaryFile))
                {
                    // file was never choose
                }
            }

            return new Tuple<string, string>(toneA, toneB);
        }
    }
}