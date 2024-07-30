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
using MidiApp.MidiController.Service;
using MidiApp.MidiController.View;
using MidiApp.UIControls;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;
using Xplorer.Common;
using Xplorer.Controller;
using Xplorer.Controller.Service.Settings;

namespace Xplorer.View
{
    /// <summary>
    /// Xplorer's main form
    /// </summary>
    internal sealed partial class MainForm : AbstractControllerMainForm
    {
        /// <summary>
        /// SplashScreen form
        /// </summary>
        public SplashScreenForm SplashScreen { get; private set; }

        /// <summary>
        /// Helper for display management
        /// </summary>
        private VfdDisplayHelper _vfdDisplayHelper;

        // virtual keyboard
        private PianoControlForm _pianoControlForm;

        /// <summary>
        /// Ctor (design mode only)
        /// </summary>
        public MainForm()
        {
            SuspendLayout();
            InitializeComponent();
            ResumeLayout();
        }

        /// <summary>
        /// Ctor
        /// </summary>
        /// <param name="splash"></param>
        public MainForm(SplashScreenForm splash)
        {
            SplashScreen = splash;
            SplashScreen.VersionInformation = ((XpanderController)Controller).GetProductNameAndVersionAsString();

            SuspendLayout();
            SplashScreen.NextStep("Initializing");
            InitializeComponent();
            ResumeLayout();
        }

        /// <summary>
        /// //intialize the controller with settings values
        /// </summary>
        /// <returns></returns>
        private void LoadSettings()
        {
            bool wasRunning = false;
            XpanderController xController = (XpanderController)Controller;

            try
            {
                if (xController.IsRunning)
                {
                    xController.Stop();
                    wasRunning = true;
                }

                bool deviceNameError = false;
                // if device names are not the same or undefined, this will throw an exception
                // only warn the user about it, but continue to use other settings values
                try
                {
                    xController.SetAutomationInputDevice(AllUsersSettingsService.AllUsersSettings.MidiConfig.AutomationInputDeviceName);
                }
                catch
                {
                    deviceNameError = true;
                }
                try
                {
                    xController.SetSynthInputDevice(AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthInputDeviceName);
                }
                catch
                {
                    deviceNameError = true;
                }
                try
                {
                    xController.SetSynthOutputDevice(AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthOutputDeviceName);
                }
                catch
                {
                    deviceNameError = true;
                }
                xController.ParameterTransmitDelay = AllUsersSettingsService.AllUsersSettings.MidiConfig.SysexTransmitDelay;
                xController.SetMIDIChannel(AllUsersSettingsService.AllUsersSettings.MidiConfig.MidiChannel - 1); //0 based
                xController.SetEditingProgramNumber(AllUsersSettingsService.AllUsersSettings.MidiConfig.EditingProgramNumber);

                xController.ControlChangeAutomationTable.Clear();

                foreach (String entry in AllUsersSettingsService.AllUsersSettings.MidiConfig.AutomationTable)
                {
                    // entries are semicolon separated
                    int delimiter = entry.LastIndexOf(';');
                    string parameterName = entry.Substring(0, delimiter);
                    string controlChangeNumber = entry.Substring(delimiter + 1, entry.Length - delimiter - 1);
                    int intControlChangeNumber;
                    if (int.TryParse(controlChangeNumber, out intControlChangeNumber))
                    {
                        if (intControlChangeNumber < 1) intControlChangeNumber = 1;
                        if (intControlChangeNumber > 128) intControlChangeNumber = 128;
                        xController.ControlChangeAutomationTable.Add(new StringIntDualEntry(parameterName, intControlChangeNumber));
                    }
                }

                if (deviceNameError)
                {
                    Logger.WriteLine(this, TraceLevel.Warning, "LoadSettings() device name error");
                }

                // apply user interface settings
                ApplyUserInterfaceSettingsOnControls(
                    System.Drawing.Color.FromArgb(AllUsersSettingsService.AllUsersSettings.UiConfig.KnobLedBorderColor),
                    AllUsersSettingsService.AllUsersSettings.UiConfig.KnobMovementIsLinear,
                    AllUsersSettingsService.AllUsersSettings.UiConfig.KnobStyleIsStandard);
            }
            catch (Exception ex)
            {
                // whatever the exception is, consider it as non fatal
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
            }
            finally
            {
                if (wasRunning)
                {
                    xController.Start();
                }
            }
        }

        /// <summary>
        /// events when LAG time radio are checked
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void RadioExpoLinear_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton radio = sender as RadioButton;
            if (radio != null)
            {
                if (radio == this.rdLinear)
                {
                    this.LAG_LINEAR_EQUAL_TIME.Enabled = true;
                }
                else
                {
                    this.LAG_LINEAR_EQUAL_TIME.Enabled = false;
                }
                this.LAG_TIMING_LINEAR_EXPO.UpdateValueFromRadioButton(radio);
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the RadioFMDest control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void RadioFMDest_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton radio = sender as RadioButton;
            if (radio != null)
            {
                this.FM_DESTINATION.UpdateValueFromRadioButton(radio);
            }
        }

        /// <summary>
        /// Handles the SelectionChangeCommitted event of the ComboBoxValuedControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void ComboBoxValuedControl_SelectionChangeCommitted(object sender, EventArgs e)
        {
            ComboBox combo = sender as ComboBox;
            if (combo == null) return;

            AnyValuedControl_ValueChanged(sender);

            // special handling for the LFO WAVESHAPE : enable modulation source select
            // for sample wave shape, disable for other values

            if (combo == this.LFO_X_WAVESHAPE)
            {
                ComboBoxValuedControlItem item = (ComboBoxValuedControlItem)combo.SelectedItem;
                XpanderConstants.EnumLFOWaveShapes WaveShape = (XpanderConstants.EnumLFOWaveShapes)item.Item;
                if (WaveShape == XpanderConstants.EnumLFOWaveShapes.SAMPLE)
                {
                    this.LFO_X_SAMPLE_INPUT.Enabled = true;
                    this.lbSampleInput.Enabled = true;
                }
                else
                {
                    this.LFO_X_SAMPLE_INPUT.Enabled = false;
                    this.lbSampleInput.Enabled = false;
                }
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the CheckBoxValuedControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void CheckBoxValuedControl_CheckedChanged(object sender, EventArgs e)
        {
            AnyValuedControl_ValueChanged(sender);
        }

        /// <summary>
        /// Handles the CheckedChanged event of the ENV_X_TRIG_EXTRIG control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void ENV_X_TRIG_EXTRIG_CheckedChanged(object sender, EventArgs e)
        {
            if (ENV_X_TRIG_EXTRIG.Checked)
            {
                this.ENV_X_TRIG_LFOTRIG.Checked = false;
                this.ENV_X_TRIG_LFO_SOURCE.Enabled = false;
                this.ENV_X_TRIG_GATED.Enabled = true;
            }
            else
            {
                this.ENV_X_TRIG_LFO_SOURCE.Enabled = false;
                if (!this.ENV_X_TRIG_LFOTRIG.Checked)
                {
                    this.ENV_X_TRIG_GATED.Enabled = false;
                }
            }
            CheckBoxValuedControl_CheckedChanged(sender, e);
        }

        /// <summary>
        /// Handles the CheckedChanged event of the ENV_X_TRIG_LFOTRIG control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void ENV_X_TRIG_LFOTRIG_CheckedChanged(object sender, EventArgs e)
        {
            if (ENV_X_TRIG_LFOTRIG.Checked)
            {
                this.ENV_X_TRIG_EXTRIG.Checked = false;
                this.ENV_X_TRIG_LFO_SOURCE.Enabled = true;
                this.ENV_X_TRIG_GATED.Enabled = true;
            }
            else
            {
                this.ENV_X_TRIG_LFO_SOURCE.Enabled = false;
                if (this.ENV_X_TRIG_EXTRIG.Checked)
                {
                    this.ENV_X_TRIG_GATED.Enabled = false;
                }
            }
            CheckBoxValuedControl_CheckedChanged(sender, e);
        }

        /// <summary>
        /// Handles the CheckedChanged event of the RadioEnvXSINGLE_MULTI control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void RadioEnvXSINGLE_MULTI_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton radio = sender as RadioButton;
            if (radio != null)
            {
                this.ENV_X_TRIG_SINGLE_MULTI.UpdateValueFromRadioButton(radio);
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the RadioRampXSINGLE_MULTI control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void RadioRampXSINGLE_MULTI_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton radio = sender as RadioButton;
            if (radio != null)
            {
                this.RAMP_X_TRIG_SINGLE_MULTI.UpdateValueFromRadioButton(radio);
            }
        }

        /// <summary>
        /// Handles the CheckedChanged event of the RAMP_X_TRIG_EXTRIG control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void RAMP_X_TRIG_EXTRIG_CheckedChanged(object sender, EventArgs e)
        {
            if (RAMP_X_TRIG_EXTRIG.Checked)
            {
                this.RAMP_X_TRIG_LFOTRIG.Checked = false;
                this.RAMP_X_TRIG_LFO_SOURCE.Enabled = false;
                this.RAMP_X_TRIG_GATED.Enabled = true;
            }
            else
            {
                this.RAMP_X_TRIG_LFO_SOURCE.Enabled = false;
                if (!this.RAMP_X_TRIG_LFOTRIG.Checked)
                {
                    this.RAMP_X_TRIG_GATED.Enabled = false;
                }
            }
            CheckBoxValuedControl_CheckedChanged(sender, e);
        }

        /// <summary>
        /// Handles the CheckedChanged event of the RAMP_X_TRIG_LFOTRIG control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void RAMP_X_TRIG_LFOTRIG_CheckedChanged(object sender, EventArgs e)
        {
            if (RAMP_X_TRIG_LFOTRIG.Checked)
            {
                this.RAMP_X_TRIG_EXTRIG.Checked = false;
                this.RAMP_X_TRIG_LFO_SOURCE.Enabled = true;
                this.RAMP_X_TRIG_GATED.Enabled = true;
            }
            else
            {
                this.RAMP_X_TRIG_LFO_SOURCE.Enabled = false;
                if (!this.RAMP_X_TRIG_EXTRIG.Checked)
                {
                    this.RAMP_X_TRIG_GATED.Enabled = false;
                }
            }
            CheckBoxValuedControl_CheckedChanged(sender, e);
        }

        /// <summary>
        /// Anies the valued control_ value changed.
        /// </summary>
        /// <param name="sender">The sender.</param>
        private void AnyValuedControl_ValueChanged(object sender)
        {
            // assuming here we have IValuedControl
            IValuedControl valuedControl = sender as IValuedControl;
            Control control = sender as Control;
            if (control != null && valuedControl != null)
            {
                base.HandleControlValueChanged(GetParameterNameForValuedControlTag((string)control.Tag), valuedControl.Value);
                this._vfdDisplayHelper.UpdateState(valuedControl);
            }
        }

        /// <summary>
        /// Handles the MouseDown event of the AnyKnobControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.Windows.Forms.MouseEventArgs"/> instance containing the event data.</param>
        private void AnyKnobControl_MouseDown(object sender, MouseEventArgs e)
        {
            base.HandleControlMouseDown(sender, e);
        }

        /// <summary>
        /// Handles the MouseUp event of the AnyKnobControl control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.Windows.Forms.MouseEventArgs"/> instance containing the event data.</param>
        private void AnyKnobControl_MouseUp(object sender, MouseEventArgs e)
        {
            base.HandleControlMouseUp(sender, e);
        }

        /// <summary>
        /// fired when radio "ENV_1" to "ENV_5" are clicked
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Radio_ENV_X_Click(object sender, EventArgs e)
        {
            RadioButton button = sender as RadioButton;
            if (button == null) return;

            if (e != null)
            {
                // this happens when the user really click on the button and not when the controller updates the view
                // button's tag is same as page name
                ((XpanderController)Controller).SendPageUpdate((string)button.Tag);
            }

            // update all the controls of the selected "page", hardcoding which control is faster than enumerating all to find which
            // (order of intellisense here)
            string sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_ATTACK.Tag);
            this.ENV_X_ATTACK.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_DECAY.Tag);
            this.ENV_X_DECAY.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_DELAY.Tag);
            this.ENV_X_DELAY.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_MODE_DADR.Tag);
            this.ENV_X_MODE_DADR.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_MODE_FREERUN.Tag);
            this.ENV_X_MODE_FREERUN.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_MODE_RESET.Tag);
            this.ENV_X_MODE_RESET.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_RELEASE.Tag);
            this.ENV_X_RELEASE.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_SUSTAIN.Tag);
            this.ENV_X_SUSTAIN.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_TRIG_EXTRIG.Tag);
            this.ENV_X_TRIG_EXTRIG.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_TRIG_GATED.Tag);
            this.ENV_X_TRIG_GATED.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_TRIG_LFOTRIG.Tag);
            this.ENV_X_TRIG_LFOTRIG.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_TRIG_LFO_SOURCE.Tag);
            this.ENV_X_TRIG_LFO_SOURCE.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_TRIG_SINGLE_MULTI.Tag);
            this.ENV_X_TRIG_SINGLE_MULTI.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_VOLUME.Tag);
            this.ENV_X_VOLUME.Value = Controller.GetParameter(sParameterName).Value;

            this._vfdDisplayHelper.UpdateState(this.ENV_X_VOLUME);
        }

        /// <summary>
        /// Handles the Click event of the Radio_LFO_X control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void Radio_LFO_X_Click(object sender, EventArgs e)
        {
            RadioButton button = sender as RadioButton;
            if (button == null) return;
            if (e != null)
            {
                // this happens when the user really click on the button and not when the controller updates the view
                // button's tag is same as page name
                ((XpanderController)Controller).SendPageUpdate((string)button.Tag);
            }
            // update all the controls of the selected "page", hardcoding which control is faster than enumerating all to find which
            // (order of intellisense here)
            string sParameterName = GetParameterNameForValuedControlTag((string)this.LFO_X_AMP.Tag);
            this.LFO_X_AMP.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.LFO_X_LAG.Tag);
            this.LFO_X_LAG.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.LFO_X_RETRIG.Tag);
            this.LFO_X_RETRIG.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.LFO_X_RETRIG_MODE.Tag);
            this.LFO_X_RETRIG_MODE.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.LFO_X_SAMPLE_INPUT.Tag);
            this.LFO_X_SAMPLE_INPUT.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.LFO_X_SPEED.Tag);
            this.LFO_X_SPEED.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.LFO_X_WAVESHAPE.Tag);
            this.LFO_X_WAVESHAPE.Value = Controller.GetParameter(sParameterName).Value;

            this._vfdDisplayHelper.UpdateState(this.LFO_X_AMP);
        }

        /// <summary>
        /// Handles the Click event of the Radio_RAMP_X control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void Radio_RAMP_X_Click(object sender, EventArgs e)
        {
            RadioButton button = sender as RadioButton;
            if (button == null) return;
            if (e != null)
            {
                // this happens when the user really click on the button and not when the controller updates the view
                // button's tag is same as page name
                ((XpanderController)Controller).SendPageUpdate((string)button.Tag);
            }
            // update all the controls of the selected "page", hardcoding which control is faster than enumerating all to find which
            // (order of intellisense here)
            string sParameterName = GetParameterNameForValuedControlTag((string)this.RAMP_X_RATE.Tag);
            this.RAMP_X_RATE.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.RAMP_X_TRIG_EXTRIG.Tag);
            this.RAMP_X_TRIG_EXTRIG.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.RAMP_X_TRIG_GATED.Tag);
            this.RAMP_X_TRIG_GATED.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.RAMP_X_TRIG_LFO_SOURCE.Tag);
            this.RAMP_X_TRIG_LFO_SOURCE.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.RAMP_X_TRIG_LFOTRIG.Tag);
            this.RAMP_X_TRIG_LFOTRIG.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.RAMP_X_TRIG_LFO_SOURCE.Tag);
            this.RAMP_X_TRIG_LFO_SOURCE.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.RAMP_X_TRIG_SINGLE_MULTI.Tag);
            this.RAMP_X_TRIG_SINGLE_MULTI.Value = Controller.GetParameter(sParameterName).Value;

            this._vfdDisplayHelper.UpdateState(this.RAMP_X_RATE);
        }

        /// <summary>
        /// Handles the Click event of the Radio_TRACK_X control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void Radio_TRACK_X_Click(object sender, EventArgs e)
        {
            RadioButton button = sender as RadioButton;
            if (button == null) return;
            if (e != null)
            {
                // this happens when the user really click on the button and not when the controller updates the view
                // button's tag is same as page name
                ((XpanderController)Controller).SendPageUpdate((string)button.Tag);
            }
            string sParameterName = GetParameterNameForValuedControlTag((string)this.TRACK_X_IN.Tag);
            this.TRACK_X_IN.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.TRACK_X_PT1.Tag);
            this.TRACK_X_PT1.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.TRACK_X_PT2.Tag);
            this.TRACK_X_PT2.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.TRACK_X_PT3.Tag);
            this.TRACK_X_PT3.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.TRACK_X_PT4.Tag);
            this.TRACK_X_PT4.Value = Controller.GetParameter(sParameterName).Value;
            sParameterName = GetParameterNameForValuedControlTag((string)this.TRACK_X_PT5.Tag);
            this.TRACK_X_PT5.Value = Controller.GetParameter(sParameterName).Value;

            this._vfdDisplayHelper.UpdateState(this.TRACK_X_IN);
        }

        /// <summary>
        /// A modulation destination combo box value changed (this is a bit different from the standard CheckBoxValuedControl_CheckedChanged)
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ComboBoxValuedModDest_SelectionChangeCommitted(object sender, EventArgs e)
        {
            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)this.Controls[String.Format("MOD_SRC_{0}", entryNumber)];
                KnobControl knobAmount = (KnobControl)this.Controls[String.Format("MOD_AMNT_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)sender;
                CheckBoxValuedControl checkboxQuantize = (CheckBoxValuedControl)this.Controls[String.Format("MOD_QUANTIZE_{0}", entryNumber)];

                // if previous value is same as current don't do anything
                int oldDestinationValueMember = comboDest.Value;
                int.TryParse(comboDest.ValueMember, out oldDestinationValueMember);
                if (oldDestinationValueMember != comboDest.Value)
                {
                    XpanderController controller = (XpanderController)Controller;
                    controller.ChangeModulationDestination(comboSource.Value, knobAmount.Value, checkboxQuantize.Value, oldDestinationValueMember, comboDest.Value, entryNumber);
                    this._vfdDisplayHelper.UpdateState(((XpanderController)Controller).GetModulationEntryByNumber(entryNumber), false);
                }
            }
        }

        /// <summary>
        /// Gets the mod entry number by control tag.
        /// </summary>
        /// <param name="controlTag">The control tag.</param>
        /// <returns></returns>
        private int GetModEntryNumberByControlTag(string controlTag)
        {
            int separator = controlTag.LastIndexOf("_");
            string numberString = controlTag.Substring(separator + 1, controlTag.Length - (separator + 1));
            int number = 0;
            int.TryParse(numberString, out number);
            return number;
        }

        /// <summary>
        /// event fired when a mod matrix source combo is changed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ComboBoxValuedModSource_SelectionChangeCommitted(object sender, EventArgs e)
        {
            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)sender;
                KnobControl knobAmount = (KnobControl)this.Controls[String.Format("MOD_AMNT_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)this.Controls[String.Format("MOD_DEST_{0}", entryNumber)];
                CheckBoxValuedControl checkboxQuantize = (CheckBoxValuedControl)this.Controls[String.Format("MOD_QUANTIZE_{0}", entryNumber)];
                XpanderController controller = (XpanderController)Controller;
                controller.ChangeModulationSource(comboSource.Value, knobAmount.Value, checkboxQuantize.Value, comboDest.Value, entryNumber);
                this._vfdDisplayHelper.UpdateState(((XpanderController)Controller).GetModulationEntryByNumber(entryNumber), false);
            }
        }

        /// <summary>
        /// event fired when a mod source amount knob is changed
        /// </summary>
        /// <param name="sender"></param>
        private void KnobModSourceAmount_ValueChanged(object sender)
        {
            KnobControl knob = sender as KnobControl;
            if (knob == null) return;

            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)this.Controls[String.Format("MOD_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)this.Controls[String.Format("MOD_DEST_{0}", entryNumber)];
                XpanderController controller = (XpanderController)Controller;
                controller.ChangeModulationSourceAmount(comboSource.Value, knob.Value, comboDest.Value, entryNumber);
                this._vfdDisplayHelper.UpdateState(((XpanderController)Controller).GetModulationEntryByNumber(entryNumber), false);
            }
        }

        /// <summary>
        /// event fired when a mod source quantize is changed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CheckboxModSourceQuantize_CheckedChanged(object sender, EventArgs e)
        {
            CheckBoxValuedControl check = sender as CheckBoxValuedControl;
            if (check == null) return;

            //MOD_SRC_X, MOD_AMNT_SRC_X, MOD_DEST_X, MOD_QUANTIZE_X
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);
            if (entryNumber != 0)
            {
                ComboBoxValuedControl comboSource = (ComboBoxValuedControl)this.Controls[String.Format("MOD_SRC_{0}", entryNumber)];
                ComboBoxValuedControl comboDest = (ComboBoxValuedControl)this.Controls[String.Format("MOD_DEST_{0}", entryNumber)];
                XpanderController controller = (XpanderController)Controller;
                controller.ChangeModulationSourceQuantize(comboSource.Value, comboDest.Value, check.Value, entryNumber);
                this._vfdDisplayHelper.UpdateState(((XpanderController)Controller).GetModulationEntryByNumber(entryNumber), false);
            }
        }

        /// <summary>
        /// event fired when a modulation Destination combo box begins its drop down.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ComboBoxValuedModDest_DropDown(object sender, EventArgs e)
        {
            ComboBoxValuedControl comboDest = (ComboBoxValuedControl)sender;
            // memorize the actual value, use ValueMember as _buffer
            comboDest.ValueMember = comboDest.Value.ToString();

            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);

            // give destination choices depending on matrix usage
            XpanderController controller = (XpanderController)Controller;

            IEnumerable<XpanderConstants.EnumModulationDestinations> destinations = controller.GetAvailableModulationDestinationsForEntry(entryNumber);

            comboDest.BeginUpdate();
            object oldSelectedItem = comboDest.SelectedItem;
            comboDest.Items.Clear();
            Type type = XpanderConstants.ModulationDestinationType;
            foreach (XpanderConstants.EnumModulationDestinations destination in destinations)
            {
                string description = UIService.GetStringForEnumValue(type, (int)destination, Properties.Resources.ResourceManager);
                ComboBoxValuedControlItem item = new ComboBoxValuedControlItem(destination, description == null ? Enum.GetName(type, destination) : description);
                comboDest.Items.Add(item);
            }
            // reselect if possible
            if (oldSelectedItem != null && comboDest.Items.Contains(oldSelectedItem))
            {
                comboDest.SelectedItem = oldSelectedItem;
            }
            else
            {
                comboDest.SelectedIndex = 0;
            }

            comboDest.EndUpdate();
        }

        /// <summary>
        /// Event fired when a modulation Source combo box begins its drop down.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ComboBoxValuedModSource_DropDown(object sender, EventArgs e)
        {
            ComboBoxValuedControl comboSource = (ComboBoxValuedControl)sender;
            int entryNumber = GetModEntryNumberByControlTag((string)((Control)sender).Tag);

            // depending of the number of sources used for the entry, allow another source choice or none.
            XpanderController controller = (XpanderController)Controller;
            bool availability = controller.SourceAvailabilityForEntry(entryNumber);

            comboSource.BeginUpdate();

            if (availability)
            {
                object oldSelectedItem = comboSource.SelectedItem;
                comboSource.SetEnumType(comboSource.GetEnumType(), Properties.Resources.ResourceManager);
                // reselect if possible
                if (oldSelectedItem != null && comboSource.Items.Contains(oldSelectedItem))
                {
                    comboSource.SelectedItem = oldSelectedItem;
                }
                else
                {
                    comboSource.SelectedIndex = 0;
                }
            }
            else
            {
                // only "NONE" available
                comboSource.Items.Clear();
                Type type = XpanderConstants.ModulationSourcesModMatrixType;
                int value = (int)XpanderConstants.EnumModulationSourcesModMatrix.NONE;

                string description = UIService.GetStringForEnumValue(type, value, Properties.Resources.ResourceManager);
                ComboBoxValuedControlItem item = new ComboBoxValuedControlItem(value, description == null ? Enum.GetName(type, value) : description);

                comboSource.Items.Add(item);
                comboSource.SelectedIndex = 0;
                this._vfdDisplayHelper.UpdateState(controller.GetModulationEntryByNumber(entryNumber), true);
            }

            comboSource.EndUpdate();
        }

        /// <summary>
        /// timer event for UI - only for VFD display and LEDs
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UITimer_Tick(object sender, EventArgs e)
        {
            //stop timer while updating
            this._timer.Stop();

            // update VFD
            _vfdDisplayHelper.UpdateDisplay();

            // update MIDI Led panel
            for (int i = 0; i < _ledPanelControl.LedCount; i++)
            {
                if (_midiStatusLedRemainingTime[i] > 0)
                {
                    _ledPanelControl.LedStates[i] = true;
                    // _midiStatusLedRemainingTime can be set from another thread (see controller's event)
                    // but there is not major flaw here
                    _midiStatusLedRemainingTime[i] -= _timer.Interval;
                }
                else
                {
                    _ledPanelControl.LedStates[i] = false;
                }
            }
            _ledPanelControl.Invalidate();

            this._timer.Start();
        }

        /// <summary>
        /// Increase patch number button event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btPatchPlus_Click(object sender, EventArgs e)
        {
            XpanderController xController = (XpanderController)Controller;
            xController.IncreaseCurrentProgramNumber();
        }

        /// <summary>
        /// decrease patch number button event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btPatchMinus_Click(object sender, EventArgs e)
        {
            XpanderController xController = (XpanderController)Controller;
            xController.DecreaseCurrentProgramNumber();
        }

        /// <summary>
        /// goto patch button event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btPatchGoto_Click(object sender, EventArgs e)
        {
            XpanderController controller = (XpanderController)Controller;
            using (StoreAndGotoPatchForm form = new StoreAndGotoPatchForm())
            {
                form.Text = "Go to patch";
                form.ProgramNumber = controller.CurrentProgramNumber;
                if (form.ShowDialog() == DialogResult.OK)
                {
                    controller.SendProgramChangeAndGetSinglePatchFromSynth(form.ProgramNumber);
                }
            }
        }

        /// <summary>
        /// Randomize button event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btPatchRandom_Click(object sender, EventArgs e)
        {
            // controller will define itself the randomization depending on the configuration
            ((XpanderController)Controller)
                .RandomizeTone(new MidiApp.MidiController.Controller.Arguments.RandomizeToneArgument(null, null));
        }

        /// <summary>
        /// load patch button event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btPatchLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog openSysexFileDialog = new OpenFileDialog();

            openSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
            openSysexFileDialog.RestoreDirectory = true;

            if (openSysexFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    // read the tone sysex and fill the map with the parameters values
                    ((XpanderController)Controller).LoadTone(openSysexFileDialog.FileName);
                    // store the file name for save function
                    SetToneFilename(openSysexFileDialog.FileName);
                }
                catch (Exception ex)
                {
                    // whatever the exception is, consider it as non fatal
                    Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                    MessageBox.Show("Unable to load patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        /// <summary>
        /// Sets the tone filename and synchronize form's title
        /// </summary>
        private void SetToneFilename(string toneFilename)
        {
            this.ToneFileName = toneFilename;
            this.Text = DefaultTitle() + " - " + this.ToneFileName;
        }

        /// <summary>
        /// save patch button event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btPatchSave_Click(object sender, EventArgs e)
        {
            char[] InvalidFileNameChars = System.IO.Path.GetInvalidFileNameChars();
            string sFileName = this.ToneFileName;
            if ((sFileName != null) &&
                (sFileName.Length > 0) &&
                (Path.GetFileName(sFileName).IndexOfAny(InvalidFileNameChars) == -1) &&
                (sFileName != DefaultToneFilename))
            {
                try
                {
                    ((XpanderController)Controller).SaveTone(sFileName);
                }
                catch (Exception ex)
                {
                    // whatever the exception is, consider it as non fatal
                    Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                    MessageBox.Show("Unable to save patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            else
            {
                // save as
                this.saveAsToolStripMenuItem_Click(sender, e);
            }
        }

        /// <summary>
        /// Store patch button event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btPatchStore_Click(object sender, EventArgs e)
        {
            XpanderController controller = (XpanderController)Controller;
            using (StoreAndGotoPatchForm form = new StoreAndGotoPatchForm())
            {
                form.Text = "Store";
                form.ProgramNumber = controller.CurrentProgramNumber;
                if (form.ShowDialog() == DialogResult.OK)
                {
                    controller.StoreSinglePatchToSynth(form.ProgramNumber);
                }
            }
        }

        /// <summary>
        /// Settings button event
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void btSettings_Click(object sender, EventArgs e)
        {
            bool settingsModified = false;
            XpanderController controller = (XpanderController)Controller;
            controller.Stop();

            try
            {
                Settings.SettingsForm settingsDialog = new Settings.SettingsForm();

                // Midi page
                settingsDialog.MidiPage.SynthOutputDeviceName = AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthOutputDeviceName;
                settingsDialog.MidiPage.AutomationInputDeviceName = AllUsersSettingsService.AllUsersSettings.MidiConfig.AutomationInputDeviceName;
                settingsDialog.MidiPage.SynthInputDeviceName = AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthInputDeviceName;
                settingsDialog.MidiPage.SysExTransmitDelay = AllUsersSettingsService.AllUsersSettings.MidiConfig.SysexTransmitDelay;
                settingsDialog.MidiPage.MIDIChannel = AllUsersSettingsService.AllUsersSettings.MidiConfig.MidiChannel;
                settingsDialog.MidiPage.EditingProgramNumber = AllUsersSettingsService.AllUsersSettings.MidiConfig.EditingProgramNumber;
                settingsDialog.MidiPage.SynthTypeIsMatrix12 = AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthTypeIsMatrix12;
                settingsDialog.MidiPage.SmartAllNotesOff = AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff;

                foreach (StringIntDualEntry entry in Controller.ControlChangeAutomationTable)
                {
                    settingsDialog.MidiPage.AutomationTable.Add(entry.StringValue, entry.IntValue);
                }

                // User interface page
                settingsDialog.UserInterfacePage.KnobLedBorderColor = System.Drawing.Color.FromArgb(AllUsersSettingsService.AllUsersSettings.UiConfig.KnobLedBorderColor);
                settingsDialog.UserInterfacePage.IsLinearMovement = AllUsersSettingsService.AllUsersSettings.UiConfig.KnobMovementIsLinear;
                settingsDialog.UserInterfacePage.IsKnobStandardStyle = AllUsersSettingsService.AllUsersSettings.UiConfig.KnobStyleIsStandard;

                // Randomizer page
                settingsDialog.RandomizerPage.Configuration = AllUsersSettingsService.AllUsersSettings.RandomizerConfig;

                if (settingsDialog.ShowDialog() == DialogResult.OK)
                {
                    // update Midi properties
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthOutputDeviceName = settingsDialog.MidiPage.SynthOutputDeviceName;
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.AutomationInputDeviceName = settingsDialog.MidiPage.AutomationInputDeviceName;
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthInputDeviceName = settingsDialog.MidiPage.SynthInputDeviceName;
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.SysexTransmitDelay = settingsDialog.MidiPage.SysExTransmitDelay;
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.MidiChannel = settingsDialog.MidiPage.MIDIChannel;
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.EditingProgramNumber = settingsDialog.MidiPage.EditingProgramNumber;
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.SynthTypeIsMatrix12 = settingsDialog.MidiPage.SynthTypeIsMatrix12;
                    AllUsersSettingsService.AllUsersSettings.MidiConfig.SmartAllNotesOff = settingsDialog.MidiPage.SmartAllNotesOff;

                    AllUsersSettingsService.AllUsersSettings.MidiConfig.AutomationTable.Clear();
                    foreach (DictionaryEntry entry in settingsDialog.MidiPage.AutomationTable)
                    {
                        // controller's control change table will be refilled thru LoadSettings()
                        AllUsersSettingsService.AllUsersSettings.MidiConfig.AutomationTable.Add(String.Format("{0};{1}", (string)entry.Key, (int)entry.Value));
                    }

                    // update user interface properties
                    AllUsersSettingsService.AllUsersSettings.UiConfig.KnobLedBorderColor = settingsDialog.UserInterfacePage.KnobLedBorderColor.ToArgb();
                    AllUsersSettingsService.AllUsersSettings.UiConfig.KnobMovementIsLinear = settingsDialog.UserInterfacePage.IsLinearMovement;
                    AllUsersSettingsService.AllUsersSettings.UiConfig.KnobStyleIsStandard = settingsDialog.UserInterfacePage.IsKnobStandardStyle;

                    // Randomizer configuration

                    AllUsersSettingsService.AllUsersSettings.RandomizerConfig = settingsDialog.RandomizerPage.Configuration;

                    AllUsersSettingsService.SaveSettings(AllUsersSettingsService.AllUsersSettings);
                    settingsModified = true;
                }
            }
            finally
            {
                controller.Start();
            }

            if (settingsModified)
            {
                // simulate a settings reload
                LoadSettings();
            }
        }

        #region FILES

        /// <summary>
        /// Gets the default tone filename.
        /// </summary>
        private static string DefaultToneFilename
        {
            get
            {
                const string DEFAULT_SYSEX_FILE = "oberheim.syx";
                return Path.Combine(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), DEFAULT_SYSEX_FILE);
            }
        }

        /// <summary>
        /// Gets the help filename.
        /// </summary>
        private static string UserManualFilename
        {
            get
            {
                const string USER_MANUAL_FILE = "XplorerUserManual.pdf";
                return Path.Combine(System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), USER_MANUAL_FILE);
            }
        }

        #endregion FILES

        #region Menu Events

        /// <summary>
        /// Handles the Click event of the newToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // load the default sysex file
            try
            {
                string filename = DefaultToneFilename;

                if (!string.IsNullOrEmpty(filename) && File.Exists(filename))
                {
                    // read the tone sysex and fill the map with the parameters values
                    ((XpanderController)Controller).LoadTone(filename);
                    SetToneFilename(filename);
                }
            }
            catch (Exception ex)
            {
                // whatever the exception is, consider it as non fatal
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                MessageBox.Show("Unable to create a new patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        /// <summary>
        /// Handles the Click event of the openToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btPatchLoad_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the saveToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btPatchSave_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the saveAsToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveSysexFileDialog = new SaveFileDialog();
            saveSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
            saveSysexFileDialog.RestoreDirectory = true;

            if (saveSysexFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    ((XpanderController)Controller).SaveTone(saveSysexFileDialog.FileName);
                    SetToneFilename(saveSysexFileDialog.FileName);
                }
                catch (Exception ex)
                {
                    //whatever the exception is, consider it as non fatal
                    Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                    MessageBox.Show("Unable to save patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the exitToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        /// <summary>
        /// Handles the Click event of the nextPatchStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void nextPatchStripMenuItem_Click(object sender, EventArgs e)
        {
            btPatchPlus_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the previousPatchToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void previousPatchToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btPatchMinus_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the gotoPatchToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void gotoPatchToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btPatchGoto_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the randomizePatchToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void randomizePatchToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btPatchRandom_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the settingsToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btSettings_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the helpXplorerToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void helpXplorerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string filename = UserManualFilename;
                if (!string.IsNullOrEmpty(filename) && File.Exists(filename))
                {
                    Process.Start(filename);
                }
            }
            catch (Exception ex)
            {
                //whatever the exception is, consider it as non fatal
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                MessageBox.Show("Unable to launch help: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        /// <summary>
        /// Handles the Click event of the aboutdeToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void aboutdeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (AboutForm form = new AboutForm((XpanderController)Controller))
            {
                form.ShowDialog();
            }

            // needed to resynch synth's display (argh...)
            XpanderController controller = (XpanderController)Controller;
            controller.ForceSendPageSubPage();
        }

        /// <summary>
        /// Handles the Click event of the renameToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void renameToolStripMenuItem_Click(object sender, EventArgs e)
        {
            XpanderController controller = (XpanderController)Controller;
            using (RenamePatchForm form = new RenamePatchForm())
            {
                form.PatchName = controller.ToneName;
                if (form.ShowDialog() == DialogResult.OK)
                {
                    controller.ToneName = form.PatchName;
                    this._vfdDisplayHelper.UpdateState();
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the storeToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void storeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btPatchStore_Click(sender, e);
        }

        /// <summary>
        /// Handles the Click event of the toneMorphingToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void toneMorphingToolStripMenuItem_Click(object sender, EventArgs e)
        {
#warning TODO implement as singletion instance, see ProgressForm
            ToneMorphingForm form = new ToneMorphingForm((XpanderController)Controller);

            form.StartPosition = FormStartPosition.Manual;
            form.Location = new Point(this.Location.X + (this.Width - form.Width) / 2, this.Location.Y + (this.Height - form.Height) / 2);
            form.Show();
        }

        /// <summary>
        /// Handles the Click event of the synchronizeToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void synchronizeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // re-gets the current patch from X
            XpanderController controller = (XpanderController)Controller;
            controller.SendProgramChangeAndGetSinglePatchFromSynth(controller.CurrentProgramNumber);
        }

        /// <summary>
        /// Handles the Click event of the tuneRequestToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void tuneRequestToolStripMenuItem_Click(object sender, EventArgs e)
        {
            XpanderController controller = (XpanderController)Controller;
            controller.SendTuneRequestToSynth();

            // we have no way to check when the request is finished.
            // so warn the user about it.
            int programNumber = controller.CurrentProgramNumber;
            MessageBox.Show("Please click OK when the tune procedure is finished", "Tune request");
            controller.SendProgramChangeAndGetSinglePatchFromSynth(programNumber);
        }

        #region page context menu

        /// <summary>
        /// Handles the Opening event of the PageContextMenuStrip control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.ComponentModel.CancelEventArgs"/> instance containing the event data.</param>
        private void PageContextMenuStrip_Opening(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Control SourceControl = PageContextMenuStrip.SourceControl;
            if (SourceControl != null)
            {
                string sDestinationTag = SourceControl.Tag as string;
                if (((XpanderController)Controller).CanClipboardPasteTo(sDestinationTag))
                {
                    this.toolStripPageMenuItemPaste.Enabled = true;
                }
                else
                {
                    this.toolStripPageMenuItemPaste.Enabled = false;
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the toolStripPageMenuItemCopy control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void toolStripPageMenuItemCopy_Click(object sender, EventArgs e)
        {
            // happens when copy comes from menu
            Control SourceControl = PageContextMenuStrip.SourceControl;

            // may happen when copy comes from shortcut (Ctrl-C)
            if (SourceControl == null)
            {
                SourceControl = this._pagesRadioButtonsMap.Values.FirstOrDefault(control => control.Focused);
            }

            if (SourceControl != null)
            {
                ((XpanderController)Controller).ClipboardSource = (SourceControl.Tag as string);
            }
        }

        /// <summary>
        /// Paste page from context menu
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripPageMenuItemPaste_Click(object sender, EventArgs e)
        {
            // happens when copy comes from menu
            Control SourceControl = PageContextMenuStrip.SourceControl;

            // may happen when copy comes from shortcut (Ctrl-C)
            if (SourceControl == null)
            {
                SourceControl = this._pagesRadioButtonsMap.Values.FirstOrDefault(control => control.Focused);
            }

            if (SourceControl != null)
            {
                XpanderController xControl = ((XpanderController)Controller);
                xControl.PasteClipboardTo(SourceControl.Tag as string);
                xControl.ClearClipboard();
            }
        }

        #endregion page context menu

        /// <summary>
        /// Handles the Click event of the extractSingleTonesFromBankToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void extractSinglePatchesFromBankToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (ExtractSingleToneForm form = new ExtractSingleToneForm())
            {
                if (form.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    XpanderController controller = (XpanderController)Controller;
                    try
                    {
                        IEnumerable<object> tones = controller.ExtractSinglePatchesFromAllDataDumpFileToDirectory(form.BankFilename, form.DestinationFolder);
                        int count = tones.Count();
                        if (count == 0)
                        {
                            MessageBox.Show("Unable to extrat single patches from file !", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                        else
                        {
                            MessageBox.Show(
                            this,
                            string.Format(CultureInfo.InvariantCulture, "{0} files extracted successfully to folder {1}",
                            count,
                            form.DestinationFolder));
                        }
                    }
                    catch (NonFatalException nfe)
                    {
                        MessageBox.Show(nfe.Message, "Single patches extraction", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the AllDataDumpBackupToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void AllDataDumpBackupToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog saveSysexFileDialog = new SaveFileDialog())
            {
                saveSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
                saveSysexFileDialog.RestoreDirectory = true;

                if (saveSysexFileDialog.ShowDialog() == DialogResult.OK)
                {
                    XpanderController controller = (XpanderController)Controller;
                    try
                    {
                        controller.BackupAllDataDumpToFile(saveSysexFileDialog.FileName);
                    }
                    catch (NonFatalException nfe)
                    {
                        MessageBox.Show(nfe.Message, "All data dump backup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the AllDataDumpRestoreToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void AllDataDumpRestoreToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openSysexFileDialog = new OpenFileDialog())
            {
                openSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
                openSysexFileDialog.RestoreDirectory = true;

                if (openSysexFileDialog.ShowDialog() == DialogResult.OK)
                {
                    // allows UI to show off since we will use the UI thread for a long time
                    ProgressForm form = null;
                    try
                    {
#warning we shoud avoid user interaction while running dump
                        form = ProgressForm.CreateInstance(this);
                        form.Show(this);
                        form.Text = "All data dump restore";
                        Action<int, int> progressAction = (current, max) =>
                        {
                            form.MinValue = 0;
                            form.MaxValue = max;
                            form.Value = current;
                            form.Label = string.Format(CultureInfo.InvariantCulture, "Sending data [{0}/{1}]", current.ToString("00", CultureInfo.InvariantCulture), max);
                            Application.DoEvents();
                        };

                        XpanderController controller = (XpanderController)Controller;
                        controller.RestoreAllDataDumpToSynth(openSysexFileDialog.FileName, progressAction);
                        ProgressForm.DestroyInstance();
                    }
                    catch (NonFatalException nfe)
                    {
                        if (form != null)
                        {
                            // close progression before showing error message
                            ProgressForm.DestroyInstance();
                        }
                        MessageBox.Show(nfe.Message, "All data dump restore", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the getSingleTonesFromSynthToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void getSinglePatchesFromSynthToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (FolderBrowserDialog fbd = new FolderBrowserDialog())
            {
                fbd.ShowNewFolderButton = true;
                fbd.Description = "Select a destination folder for single patch sysex files";
                if (fbd.ShowDialog() == DialogResult.OK)
                {
                    XpanderController controller = (XpanderController)Controller;

                    try
                    {
                        controller.GetSingleTonesFromSynth(fbd.SelectedPath);
                    }
                    catch (NonFatalException nfe)
                    {
                        MessageBox.Show(this, nfe.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /// <summary>
        /// Handles the Click event of the checkForNewReleaseToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void checkForNewReleaseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            const string releaseUrl = "http://xplorer.programmer.free.fr/bb/viewforum.php?id=5";
            try
            {
                Process.Start(releaseUrl);
            }
            catch (Exception ex)
            {
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
            }
        }

        /// <summary>
        /// Handles the Click event of the goToWebsiteToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void goToWebsiteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            const string websiteUrl = "https://github.com/xplorer2716/XplorerEditor";
            try
            {
                Process.Start(websiteUrl);
            }
            catch (Exception ex)
            {
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
            }
        }

        /// <summary>
        /// Handles the Click event of the goToForumToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void goToForumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            const string forumUrl = "http://xplorer.programmer.free.fr/bb/";
            try
            {
                Process.Start(forumUrl);
            }
            catch (Exception ex)
            {
                Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
            }
        }

        #endregion Menu Events
    }
}