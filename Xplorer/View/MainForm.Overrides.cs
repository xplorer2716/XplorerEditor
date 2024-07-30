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
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Resources;
using System.Windows.Forms;
using Xplorer.Common;
using Xplorer.Controller;
using Xplorer.Controller.Events;

// for constants and mod entry only
using Xplorer.Model;
using Xplorer.Properties;

namespace Xplorer.View
{
    internal partial class MainForm : AbstractControllerMainForm
    {
        // See CheckScreenSize
        private Size _minimalSize = new Size(1266, 838);

        // map for the radio buttons. this will enable the 'X' select
        private Dictionary<string, RadioButton> _pagesRadioButtonsMap = new Dictionary<string, RadioButton>();

        // map for knob controls
        private Dictionary<string, KnobControl> _knobControlsMap = new Dictionary<string, KnobControl>();

        // MIDI Led initial remaining time in ms
        private int INITIAL_MIDI_LED_REMAINING_TIME = 100;

        // current remaining time for MIDI Port LED status
        private int[] _midiStatusLedRemainingTime = new int[3];

        /// <summary>
        /// Controller
        /// </summary>
        private XpanderController _controller = null;

        protected override AbstractController Controller
        {
            get
            {
                if (_controller == null)
                {
                    _controller = new XpanderController();
                }
                return _controller;
            }
        }

        /// <summary>
        /// Applies the action on form's controls.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <param name="parameter">The parameter.</param>
        private void ApplyActionOnControls(Action<Control, object> action, object parameter)
        {
            SuspendLayout();
            foreach (Control control in this.Controls)
            {
                action(control, parameter);
            }
            ResumeLayout();
        }

        /// <summary>
        /// Recursively register some often used controls
        /// </summary>
        /// <param name="control"></param>
        private void RecursivelyRegisterControls()
        {
            _pagesRadioButtonsMap.Clear();
            _knobControlsMap.Clear();
            ApplyActionOnControls(RegisterControls, null);
        }

        /// <summary>
        /// Registers the control and subcontrols as radio button if relevant
        /// </summary>
        /// <param name="control">The control.</param>
        /// <param name="parameter">The parameter.</param>
        private void RegisterControls(Control control, object parameter)
        {
            RadioButton radio = control as RadioButton;
            KnobControl knob = control as KnobControl;

            // page radio buttons
            if (radio != null && radio.Appearance == Appearance.Button)
            {
                _pagesRadioButtonsMap.Add(radio.Name, radio);
            }
            else if (knob != null)
            {
                _knobControlsMap.Add(knob.Name, knob);
            }
            else
            {
                foreach (Control subControl in control.Controls)
                {
                    RegisterControls(subControl, null);
                }
            }
        }

        /// <summary>
        /// Apply the user interface settings
        /// </summary>
        /// <param name="ledBorderColor">Color of the led border.</param>
        /// <param name="isMovementLinear">if set to <c>true</c> [is movement linear].</param>
        /// <param name="isKnobStyleStandard">if set to <c>true</c> [is knob style standard].</param>
        private void ApplyUserInterfaceSettingsOnControls(Color ledBorderColor, bool isMovementLinear, bool isKnobStyleStandard)
        {
            _pagesRadioButtonsMap.Values.ToList().ForEach(
                button =>
                {
                    button.ForeColor = DefaultUiColors.ButtonForegroundColorFromLedColor(ledBorderColor);
                    button.FlatAppearance.BorderColor = button.ForeColor;
                });

            _knobControlsMap.Values.ToList().ForEach(
                knob =>
                {
                    knob.LedBorderColor = ledBorderColor;
                    knob.RotatingMode = isMovementLinear ? EnumRotatingMode.Linear : EnumRotatingMode.Rotating;
                    knob.KnobStyle = isKnobStyleStandard ? EnumKnobStyle.Standard : EnumKnobStyle.FlatStyle;
                });

            if (_pianoControlForm != null)
            {
                _pianoControlForm.NoteOnColor = ledBorderColor;
            }
        }

        /// <summary>
        /// return the parameter name for the given knob control tag and current active radio button
        /// </summary>
        /// <param name="tag"></param>
        /// <returns></returns>
        private string GetParameterNameForValuedControlTag(string valuedControlTag)
        {
            string tag = valuedControlTag;

            if (tag.StartsWith("ENV_X") || tag.StartsWith("LFO_X"))
            {
                for (int iX = 1; iX < XpanderConstants.LFO_COUNT + 1; iX++) // LFO and ENV have same count
                {
                    string sRadioButtonName = tag.Substring(0, 5).Replace('X', iX.ToString()[0]);
                    // search for a checked radio button
                    RadioButton rdButton = _pagesRadioButtonsMap[sRadioButtonName];
                    if (rdButton.Checked)
                    {
                        tag = tag.Replace("_X_", String.Format("_{0}_", iX.ToString()[0]));
                        break;
                    }
                }
            }
            else if (tag.StartsWith("TRACK_X"))
            {
                for (int iX = 1; iX < XpanderConstants.TRACK_COUNT + 1; iX++)
                {
                    string sRadioButtonName = tag.Substring(0, 7).Replace('X', iX.ToString()[0]);
                    // search for a checked radio button
                    RadioButton rdButton = _pagesRadioButtonsMap[sRadioButtonName];
                    if (rdButton.Checked)
                    {
                        tag = tag.Replace("_X_", String.Format("_{0}_", iX.ToString()[0]));
                        break;
                    }
                }
            }
            else if (tag.StartsWith("RAMP_X"))
            {
                for (int iX = 1; iX < XpanderConstants.RAMP_COUNT + 1; iX++)
                {
                    string sRadioButtonName = tag.Substring(0, 6).Replace('X', iX.ToString()[0]);
                    // search for a checked radio button
                    RadioButton rdButton = _pagesRadioButtonsMap[sRadioButtonName];
                    if (rdButton.Checked)
                    {
                        tag = tag.Replace("_X_", String.Format("_{0}_", iX.ToString()[0]));
                        break;
                    }
                }
            }

            return tag;
        }

        /// <summary>
        /// return the knob control tag and page button tag given the parameter name
        /// </summary>
        /// <param name="parameterName"></param>
        /// <param name="knobControlTag"></param>
        /// <param name="radioButtonTag"></param>
        private bool GetValuedControlAndButtonNameForParameterName(string parameterName, out string valuedControlTag, out string radioButtonTag)
        {
#warning TODO too much litterals, we should unify it
            // voir ce qui a été fait avec IsPageEnvLfoRampTrack, mais ici separer en fonction du nombre de char; idem code plus haut à revoir
            string tag = parameterName;

            if (tag.StartsWith("ENV_") || tag.StartsWith("LFO_"))
            {
                string sNumber = tag.Substring(4, 1);
                valuedControlTag = tag.Replace(sNumber, "X");
                radioButtonTag = tag.Substring(0, 5);
                return true;
            }
            else if (tag.StartsWith("TRACK_"))
            {
                const int numberIndex = 6;
                // TRACK_N_POINT_M => TRACK_X_POINT_M
                valuedControlTag = tag.Substring(0, numberIndex) + "X" + tag.Substring(numberIndex + 1, tag.Length - (numberIndex + 1));
                radioButtonTag = tag.Substring(0, numberIndex + 1);
                return true;
            }
            else if (tag.StartsWith("RAMP_"))
            {
                string sNumber = tag.Substring(5, 1);
                valuedControlTag = tag.Replace(sNumber, "X");
                radioButtonTag = tag.Substring(0, 6);
                return true;
            }

            // not a multipage control
            else
            {
                valuedControlTag = null;
                radioButtonTag = null;
            }

            return false;
        }

        /// <summary>
        /// Event handler for controller internal parameter changes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="eventArg"></param>
        protected override void OnAutomationParameterChange(object sender, AbstractController.ParameterChangeEventArgs eventArg)
        {
            // retrieve the registered control from parameter name
            string knobControlTag, radioButtonTag;

            // check if it is on a specific page
            if (GetValuedControlAndButtonNameForParameterName(eventArg.ParameterName, out knobControlTag, out radioButtonTag))
            {
                // only update the control if the page radio button is checked
                RadioButton radio;
                _pagesRadioButtonsMap.TryGetValue(radioButtonTag, out radio);
                if (radio != null && radio.Checked)
                {
                    Control control = null;
                    RegisteredControlsMap.TryGetValue(knobControlTag, out control);
                    OnAutomationParameterChangeUpdateControl(eventArg, control);
                }
            }
            else
            {
                Control control = null;
                if (RegisteredControlsMap.TryGetValue(eventArg.ParameterName, out control))
                {
                    OnAutomationParameterChangeUpdateControl(eventArg, control);
                }
                else
                {
                    Logger.WriteLine(this, TraceLevel.Warning, "OnAutomationParameterChange] No control found for arg.ParameterName: " + eventArg.ParameterName);
                }
            }
        }

        /// <summary>
        /// Helper method for OnAutomationParameterChange
        /// </summary>
        /// <param name="eventArg"></param>
        /// <param name="control"></param>
        private void OnAutomationParameterChangeUpdateControl(AbstractController.ParameterChangeEventArgs eventArg, Control control)
        {
            IValuedControl current = control as IValuedControl;
            if (current != null)
            {
                current.Value = eventArg.Value;
                //on set value, knob & combos don't fire an event. so the display will not be updated. do it manually
                if ((current as KnobControl) != null || current as ComboBoxValuedControl != null)
                {
                    this._vfdDisplayHelper.UpdateState(current);
                }
                Logger.WriteLine(this, TraceLevel.Verbose,
                                    String.Format("OnAutomationParameterChangeUpdateControl:  parameter:{0}, value:{1}", eventArg.ParameterName, eventArg.Value));
            }
            else
            {
                Logger.WriteLine(this, TraceLevel.Warning,
                    String.Format("OnAutomationParameterChangeUpdateControl: could not find control for parameter {0}", eventArg.ParameterName));
            }
        }

        /// <summary>
        /// returns Main form default title
        /// </summary>
        /// <returns></returns>
        private string DefaultTitle()
        {
            string defaultTitle;
            // show debug mode if defined
            if ((int)MidiApp.MidiController.Service.Logger.Switch.Level > (int)TraceLevel.Error)
            {
                defaultTitle = string.Format("{0} [Debug mode: {1}]", AssemblyService.GetProductName(), MidiApp.MidiController.Service.Logger.Switch.Level);
            }
            else
            {
                defaultTitle = AssemblyService.GetProductName();
            }
            return defaultTitle;
        }

        /// <summary>
        /// OnLoad override.
        /// </summary>
        /// <param name="e"><see cref="T:System.EventArgs"/> args</param>
        protected override void OnLoad(EventArgs e)
        {
            if (ExecutionModeService.IsDesignModeActive) return;

            SplashScreen.NextStep("Loading settings");

            // controls registration before loading the settings
            RecursivelyRegisterControls();
            LoadSettings();

            // instanciate display helper
            _vfdDisplayHelper = new VfdDisplayHelper(_vfdDisplay, Controller, GetParameterNameForValuedControlTag);

            //associate enums to combos
            RegisterComboBoxValuedControls();

            SplashScreen.NextStep("Starting MIDI engine");

            RegisterForControllerEvents();

            // set predefined values for knob controls
            SetKnobControlsPredefinedValues();

            this.Text = DefaultTitle();

            // set MIDI led colors, this can't be done in Designer.cs (readonly)
            // other stuff already done in Designer.cs
            // automation in
            this._ledPanelControl.LedColors[0] = Color.FromArgb(144, 255, 144);
            // synth in and out
            this._ledPanelControl.LedColors[1] = Color.FromArgb(92, 171, 255);
            this._ledPanelControl.LedColors[2] = Color.FromArgb(255, 64, 32);

            if (SplashScreen != null)
            {
                SplashScreen.Terminate();
                SplashScreen = null;
            }

            //UI refresh
            Application.DoEvents();

            // check if screen resolution is big enough
            CheckScreenSize();

            // dump Midi device info into log file
            DumpMidiInfoToLogFile();

            // enable UI timer
            _timer.Start();

            // starts controller
            Controller.Start();

            this.Activate();
        }

        /// <summary>
        /// Sets the knob controls predefined values.
        /// </summary>
        private void SetKnobControlsPredefinedValues()
        {
            //  VCO values (harmonic values in semi tones) 3,5,12,...
            int[] vcoValues = new int[] { 0, 4, 7, 12, 16, 19, 24, 28, 31, 36 };
            VCO1_FREQ.PredefinedValues = vcoValues;
            VCO2_FREQ.PredefinedValues = vcoValues;
        }

        /// <summary>
        /// Checks the size of the screen.
        /// In 2 cases, the Autoscale mode will not perform as needed:
        /// - if the screen resolution is too small, the Minimum size of the main form will not be displayed entirely
        /// - if the font dpi settings is set in manner than the main form will not be displayed entirely
        /// </summary>
        private void CheckScreenSize()
        {
            Logger.WriteLine(this,
                string.Format(CultureInfo.InvariantCulture, "MainForm size: {0},  scale factor: {1}",
                this.Size, this._currentFormScaleFactor));
            Logger.WriteLine(this, TraceLevel.Info, BugReportFactory.CreateScreensInfo(this));

            // if screen size is lower than minimal form size, form size AND Minimum size can be resized lower than its predefined minimum value in the ressources
            // even if this value is already defined (thanks M$...)
            // because background image should be aligned at least with the minimum size, it will result in a wrongly displayed UI. We can't do nothing against that.
            // simply warn the user about that.
            bool sizeLowerThanMinimumSize = this.MinimumSize.Width < _minimalSize.Width || this.MinimumSize.Height < _minimalSize.Height;

            // check screen size
            Screen mainformScreen = Screen.FromControl(this);
            Rectangle workingArea = mainformScreen.Bounds;
            bool workingAreaTooSmall = (workingArea.Width < this.Width || workingArea.Height < this.Height);

            if (sizeLowerThanMinimumSize || workingAreaTooSmall)
            {
                string message = string.Format(CultureInfo.InvariantCulture,
                    "Screen working area size is lower than the minimal required size ({0}). User interface may behave incorreclty.",
                    this._minimalSize);

                Logger.WriteLine(this, TraceLevel.Error, message);

                MessageBox.Show(
                    message,
                    "Warning",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Warning);
            }
        }

        /// <summary>
        /// Dump midi devices informations to log file
        /// </summary>
        private void DumpMidiInfoToLogFile()
        {
            string midiInfo = BugReportFactory.CreateMidiDevicesInfo();
            Logger.WriteLine(this, TraceLevel.Off, Environment.NewLine + midiInfo);
        }

        /// <summary>
        /// OnActivated override; update the display (handle deactivate/re-activated for Vfd display)
        /// </summary>
        /// <param name="e">arg</param>
        protected override void OnActivated(EventArgs e)
        {
            base.OnActivated(e);
            _vfdDisplayHelper?.ForceUpdate();
        }

        private SizeF _currentFormScaleFactor = new SizeF(1F, 1F);

        /// <summary>
        /// Override to know which scale factor is applied to the main form.
        /// The only way to get REALLY the applied scale factor, thanks M$ !%!?!{*!
        /// </summary>
        /// <param name="factor"></param>
        /// <param name="specified"></param>
        protected override void ScaleControl(SizeF factor, BoundsSpecified specified)
        {
            base.ScaleControl(factor, specified);
            _currentFormScaleFactor = new SizeF(_currentFormScaleFactor.Width * factor.Width, _currentFormScaleFactor.Height * factor.Width);
        }

        /// <summary>
        /// Initialize combo box controls showing enums
        /// </summary>
        private void RegisterComboBoxValuedControls()
        {
            ResourceManager resourceManager = Resources.ResourceManager;

            this.TRACK_X_IN.SetEnumType(XpanderConstants.ModulationSourcesType, resourceManager);
            this.LAG_IN.SetEnumType(XpanderConstants.ModulationSourcesType, resourceManager);
            this.ENV_X_TRIG_LFO_SOURCE.SetEnumType(XpanderConstants.LFOTriggerSourceType, resourceManager);
            this.RAMP_X_TRIG_LFO_SOURCE.SetEnumType(XpanderConstants.LFOTriggerSourceType, resourceManager);
            this.LFO_X_RETRIG_MODE.SetEnumType(XpanderConstants.LFORetrigModesType, resourceManager);
            this.VCF_MODE.SetEnumType(XpanderConstants.VCFFilterModesType, resourceManager);
            this.LFO_X_WAVESHAPE.SetEnumType(XpanderConstants.LFOWaveShapesType, resourceManager);
            this.LFO_X_SAMPLE_INPUT.SetEnumType(XpanderConstants.ModulationSourcesType, resourceManager);

            this.MOD_SRC_1.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_2.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_3.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_4.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_5.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_6.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_7.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_8.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_9.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_10.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_11.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_12.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_13.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_14.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_15.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_16.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_17.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_18.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_19.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
            this.MOD_SRC_20.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);

            this.MOD_DEST_1.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_2.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_3.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_4.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_5.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_6.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_7.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_8.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_9.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_10.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_11.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_12.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_13.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_14.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_15.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_16.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_17.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_18.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_19.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
            this.MOD_DEST_20.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
        }

        /// <summary>
        /// This method is called whenever the form is closing or when a fatal error occurs
        /// </summary>
        protected override void DoCleanupBeforeClosing()
        {
            _timer.Enabled = false;
            _timer.Stop();
            base.DoCleanupBeforeClosing();
        }

        /// <summary>
        /// register for controller events (internal parameter changes)
        /// </summary>
        protected override void RegisterForControllerEvents()
        {
            base.RegisterForControllerEvents();
            //register for global modulation matrix change
            ((XpanderController)Controller).FullToneChangeEvent += OnFullToneGlobalChange;
            ((XpanderController)Controller).ModulationEntryChangeEvent += OnModulationEntryChange;
            ((XpanderController)Controller).PageChangeEvent += OnPageChange;

            ((XpanderController)Controller).AllDataDumpRequestProgressionEvent += OnAllDataDumpRequestProgression;

            ((XpanderController)Controller).MIDIDataSendReceiveEvent += OnMidiDataSendReceive;
        }

        /// <summary>
        /// Unregister for controller events
        /// </summary>
        protected override void UnRegisterForControllerEvents()
        {
            base.UnRegisterForControllerEvents();
            ((XpanderController)Controller).FullToneChangeEvent -= OnFullToneGlobalChange;
            ((XpanderController)Controller).ModulationEntryChangeEvent -= OnModulationEntryChange;
            ((XpanderController)Controller).PageChangeEvent -= OnPageChange;
            ((XpanderController)Controller).AllDataDumpRequestProgressionEvent -= OnAllDataDumpRequestProgression;
            ((XpanderController)Controller).MIDIDataSendReceiveEvent -= OnMidiDataSendReceive;
        }

        /// <summary>
        /// Handler for full tone change event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="arg"></param>
        private void OnFullToneGlobalChange(object sender, FullToneChangeEventArgs arg)
        {
            // update all tone paramters as if it was an automation update
            //
            AbstractController.ParameterChangeEventArgs parameterChangeArg = null;
            foreach (KeyValuePair<string, int> kvp in arg.ParameterMap)
            {
                if (parameterChangeArg == null)
                {
                    parameterChangeArg = new AbstractController.ParameterChangeEventArgs(kvp.Key, kvp.Value);
                }
                else
                {
                    parameterChangeArg.ParameterName = kvp.Key;
                    parameterChangeArg.Value = kvp.Value;
                }
                OnAutomationParameterChange(this, parameterChangeArg);
            }

            //TODO to encapsulate with OnModulationEntryChange
            // update modmatrix
            int i = 0;
            ResourceManager resourceManager = Resources.ResourceManager;

            List<ComboBoxValuedControl> comboboxes = new List<ComboBoxValuedControl>();
            foreach (ModulationMatrixEntry entry in arg.ModulationMatrix)
            {
                // TODO too much litterals, we should unify it
                string modulationSourceComboboxName = "MOD_SRC_" + (i + 1).ToString();
                string modulationDestComboboxName = "MOD_DEST_" + (i + 1).ToString();
                Control comboSourceControl = this.Controls[modulationSourceComboboxName];

                // reset source enable state before calling UpdateModulationSourceComboStateForDestination
                comboSourceControl.Enabled = true;

                // since modulation matrix combo-boxes can have in their content only a subset of the available values
                // re-set the enum type in order to handle any value accordingly
                ComboBoxValuedControl comboSourceIValuedControl = (ComboBoxValuedControl)comboSourceControl;
                ComboBoxValuedControl comboDestIValuedControl = (ComboBoxValuedControl)this.Controls[modulationDestComboboxName];

                comboSourceIValuedControl.BeginUpdate();
                comboSourceIValuedControl.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
                comboSourceIValuedControl.Value = (int)entry.Source;
                comboboxes.Add(comboSourceIValuedControl);

                comboDestIValuedControl.BeginUpdate();
                comboDestIValuedControl.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);
                comboDestIValuedControl.Value = (int)entry.Destination;
                comboboxes.Add(comboDestIValuedControl);

                i++;
            }
            comboboxes.ForEach(c => c.EndUpdate());
            this._vfdDisplayHelper.UpdateState();
        }

        /// <summary>
        /// Handler for modulation entry change event from synth
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="arg"></param>
        private void OnModulationEntryChange(object sender, ModulationEntryChangeEventArgs arg)
        {
            // TODO too much litterals, we should unify it
            string modulationAmountControlName = "MOD_AMNT_SRC_" + arg.EntryNumber.ToString();
            string modulationQuantizeControlName = "MOD_QUANTIZE_" + arg.EntryNumber.ToString();
            string modulationSourceComboboxName = "MOD_SRC_" + arg.EntryNumber.ToString();
            string modulationDestComboboxName = "MOD_DEST_" + arg.EntryNumber.ToString();
            AbstractController.ParameterChangeEventArgs paramArg = null;

            switch (arg.ModulationParameter)
            {
                // for amount or quantize, simulates an AutomationParameterChange
                case EnumModulationParameter.MODULATIONAMOUNT:
                    {
                        paramArg = new AbstractController.ParameterChangeEventArgs(modulationAmountControlName, arg.Entry.Amount);
                        OnAutomationParameterChange(this, paramArg);
                    }
                    break;

                case EnumModulationParameter.MODULATIONQUANTIZE:
                    {
                        paramArg = new AbstractController.ParameterChangeEventArgs(modulationQuantizeControlName, arg.Entry.Quantize);
                        OnAutomationParameterChange(this, paramArg);
                    }
                    break;

                // for modulation source, destionation and all, update all the controls of the modulation entry
                case EnumModulationParameter.MODULATIONSOURCE:
                case EnumModulationParameter.MODULATIONDESTINATION:
                case EnumModulationParameter.ALL:

                    // source and destination are not parameters, update manually

                    ComboBoxValuedControl comboSource = (ComboBoxValuedControl)this.Controls[modulationSourceComboboxName];
                    ComboBoxValuedControl comboDest = (ComboBoxValuedControl)this.Controls[modulationDestComboboxName];

                    // since modulation matrix combo-boxes can have in their content only a subset of the available values
                    // re-set the enum type in order to handle any value accordingly
                    ResourceManager resourceManager = Resources.ResourceManager;
                    comboSource.SetEnumType(XpanderConstants.ModulationSourcesModMatrixType, resourceManager);
                    comboDest.SetEnumType(XpanderConstants.ModulationDestinationType, resourceManager);

                    ((IValuedControl)comboSource).Value = (int)arg.Entry.Source;
                    ((IValuedControl)comboDest).Value = (int)arg.Entry.Destination;

                    // amount
                    paramArg = new AbstractController.ParameterChangeEventArgs(modulationAmountControlName, arg.Entry.Amount);
                    OnAutomationParameterChange(this, paramArg);
                    // quantize
                    paramArg = new AbstractController.ParameterChangeEventArgs(modulationQuantizeControlName, arg.Entry.Quantize);
                    OnAutomationParameterChange(this, paramArg);

                    break;
            }

            // update the display
            this._vfdDisplayHelper.UpdateState(arg.Entry, false);
        }

        /// <summary>
        /// Handler for page change event
        /// </summary>
        /// <param name="PageName"></param>
        private void OnPageChange(object sender, PageChangeEventArgs arg)
        {
            string sPageName = arg.PageName;
            RadioButton radio = null;
            if (_pagesRadioButtonsMap.TryGetValue(sPageName, out radio))
            {
                radio.Checked = true;
                if (sPageName.StartsWith("ENV"))
                {
                    this.Radio_ENV_X_Click(radio, null);
                }
                else if (sPageName.StartsWith("LFO"))
                {
                    Radio_LFO_X_Click(radio, null);
                }
                else if (sPageName.StartsWith("RAMP"))
                {
                    Radio_RAMP_X_Click(radio, null);
                }
                else if (sPageName.StartsWith("TRACK"))
                {
                    Radio_TRACK_X_Click(radio, null);
                }
            }
        }

        /// <summary>
        /// Called when [all data dump request progression] event is received
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="arg">The arg.</param>
        private void OnAllDataDumpRequestProgression(object sender, AllDataDumpRequestProgressionEventArgs arg)
        {
            // instanciate the progression form in not already done
            if (ProgressForm.Instance == null)
            {
                ProgressForm.CreateInstance(this);
                ProgressForm.Instance.MinValue = 1;

                if (arg.AllDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.SinglePatch)
                {
                    ProgressForm.Instance.MaxValue = XpanderConstants.SINGLE_TONES_MAX_COUNT;
                    ProgressForm.Instance.Text = "Receiving single patches...";
                }
                else if (arg.AllDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.All)
                {
                    ProgressForm.Instance.MaxValue = XpanderConstants.SINGLE_TONES_MAX_COUNT + XpanderConstants.MULTI_PATCHES_MAX_COUNT;
                    ProgressForm.Instance.Text = "All data dump request in progress...";
                }

                ProgressForm.Instance.Show(this);
            }

            // if we don't still have multi patches, we are receiving single patches
            int singlePatchesCount = arg.AllDataDumpRequestState.SinglePatches.Count();
            int multiPatchesCount = arg.AllDataDumpRequestState.MultiPatches.Count();

            // end of reception
            if (arg.AllDataDumpRequestState.IsWaitingForAllDataDumpRequest == false || ProgressForm.Instance.Value >= ProgressForm.Instance.MaxValue)
            {
                ProgressForm.DestroyInstance();

                // message depends on state's reception mode
                if (arg.AllDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.SinglePatch)
                {
                    string title = "Single patches";
                    MessageBox.Show(
                        this,
                        string.Format(CultureInfo.InvariantCulture, "{0} files extracted successfully to folder {1}",
                            singlePatchesCount,
                            arg.AllDataDumpRequestState.Destination),
                           title);
                }
                else if (arg.AllDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.All)
                {
                    string title = "All data dump";
                    MessageBox.Show(
                        this,
                        string.Format(CultureInfo.InvariantCulture, "All data dump saved to file {0}",
                            arg.AllDataDumpRequestState.Destination),
                            title);
                }

                arg.AllDataDumpRequestState.Clear();
            }

            // progression
            else
            {
                if (arg.AllDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.SinglePatch)
                {
                    ProgressForm.Instance.Value = singlePatchesCount;
                }
                else if (arg.AllDataDumpRequestState.ReceptionMode == AllDataDumpRequestState.EnumAllDataDumpRequestMode.All)
                {
                    ProgressForm.Instance.Value = singlePatchesCount + multiPatchesCount;
                }

                if (multiPatchesCount == 0)
                {
                    ProgressForm.Instance.Label = string.Format(CultureInfo.InvariantCulture, "Receiving single patch [{0}/{1}] - {2}",
                            singlePatchesCount.ToString("00", CultureInfo.InvariantCulture),
                           XpanderConstants.SINGLE_TONES_MAX_COUNT,
                            arg.AllDataDumpRequestState.SinglePatches[singlePatchesCount - 1].Item1.PadLeft(8, ' '));
                }
                else
                {
                    // this will only happen in EnumAllDataDumpRequestMode.All
                    ProgressForm.Instance.Label = string.Format(CultureInfo.InvariantCulture, "Receiving multi patch [{0}/{1}]",
                            multiPatchesCount.ToString("00", CultureInfo.InvariantCulture),
                           XpanderConstants.MULTI_PATCHES_MAX_COUNT);
                }
            }
        }

        /// <summary>
        /// Called when [MIDI data send receive] event is fired.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="arg">The arg.</param>
        private void OnMidiDataSendReceive(object sender, MidiDataSendReceivedEventArgs arg)
        {
            if (arg.Device != MidiDataSendReceivedEventArgs.EnumMIDIDevice.None)
            {
                switch (arg.Device)
                {
                    case MidiDataSendReceivedEventArgs.EnumMIDIDevice.AutomationInputDevice:
                        _midiStatusLedRemainingTime[0] = INITIAL_MIDI_LED_REMAINING_TIME;
                        break;

                    case MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthInputDevice:
                        _midiStatusLedRemainingTime[1] = INITIAL_MIDI_LED_REMAINING_TIME;
                        break;

                    case MidiDataSendReceivedEventArgs.EnumMIDIDevice.SynthOutputDevice:
                        _midiStatusLedRemainingTime[2] = INITIAL_MIDI_LED_REMAINING_TIME;
                        break;
                }
                // UI Update will be done when UI timer elapse
            }
        }

        #region drag and drop

        /// <summary>
        /// Déclenche l'événement <see cref="E:System.Windows.Forms.Control.DragEnter"/>.
        /// </summary>
        /// <param name="drgevent"><see cref="T:System.Windows.Forms.DragEventArgs"/> qui contient les données d'événement.</param>
        protected override void OnDragEnter(DragEventArgs drgevent)
        {
            if (drgevent.Data.GetDataPresent(DataFormats.FileDrop)) drgevent.Effect = DragDropEffects.Link;
        }

        /// <summary>
        /// Déclenche l'événement <see cref="E:System.Windows.Forms.Control.DragDrop"/>.
        /// </summary>
        /// <param name="drgevent"><see cref="T:System.Windows.Forms.DragEventArgs"/> qui contient les données de l'événement.</param>
        protected override void OnDragDrop(DragEventArgs drgevent)
        {
            string[] files = (string[])drgevent.Data.GetData(DataFormats.FileDrop);

            if (files.Any() && (File.Exists(files[0]) && string.Compare(Path.GetExtension(files[0]), FileUtils.SYSEX_FILE_EXTENSION_WITH_DOT, true) == 0))
            {
                // try to open this one as single tone
                try
                {
                    // read the tone sysex and fill the map with the parameters values
                    ((XpanderController)Controller).LoadTone(files[0]);
                    // store the file name for save function
                    SetToneFilename(files[0]);
                }
                catch (Exception ex)
                {
                    // whatever the exception is, consider it as non fatal
                    Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                    MessageBox.Show("Unable to load patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        #endregion drag and drop
    }
}