/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2026 Pascal Schmitt

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
using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using MidiApp.MidiController.Controller;
using Xplorer.Common;
using Xplorer.Controller;
using Xplorer.Controller.Service.Settings;

namespace Xplorer.View
{
    /// <summary>
    /// Manages settings load/save operations for the main form
    /// </summary>
    internal sealed class SettingsManager
    {
        private readonly MainForm _form;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="form">The main form</param>
        internal SettingsManager(MainForm form)
        {
            Debug.Assert(form != null);
            _form = form;
        }

        /// <summary>
        /// Initialize the controller with settings values
        /// </summary>
        internal void LoadSettings()
        {
            bool wasRunning = false;
            XpanderController xController = _form.XController;

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
                    Logger.WriteLine(_form, TraceLevel.Warning, "LoadSettings() device name error");
                }

                // apply user interface settings
                _form.ApplyUserInterfaceSettingsOnControls(
                    Color.FromArgb(AllUsersSettingsService.AllUsersSettings.UiConfig.KnobLedBorderColor),
                    AllUsersSettingsService.AllUsersSettings.UiConfig.KnobMovementIsLinear,
                    AllUsersSettingsService.AllUsersSettings.UiConfig.KnobStyleIsStandard);
            }
            catch (Exception ex)
            {
                // whatever the exception is, consider it as non fatal
                Logger.WriteLine(_form, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
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
        /// Shows the settings dialog and applies changes
        /// </summary>
        /// <returns>true if settings were modified</returns>
        internal bool ShowSettingsDialog()
        {
            bool settingsModified = false;
            _form.XController.Stop();

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

                foreach (StringIntDualEntry entry in _form.XController.ControlChangeAutomationTable)
                {
                    settingsDialog.MidiPage.AutomationTable.Add(entry.StringValue, entry.IntValue);
                }

                // User interface page
                settingsDialog.UserInterfacePage.KnobLedBorderColor = Color.FromArgb(AllUsersSettingsService.AllUsersSettings.UiConfig.KnobLedBorderColor);
                settingsDialog.UserInterfacePage.IsLinearMovement = AllUsersSettingsService.AllUsersSettings.UiConfig.KnobMovementIsLinear;
                settingsDialog.UserInterfacePage.IsKnobStandardStyle = AllUsersSettingsService.AllUsersSettings.UiConfig.KnobStyleIsStandard;

                // Randomizer page
                settingsDialog.RandomizerPage.Configuration = AllUsersSettingsService.AllUsersSettings.RandomizerConfig;

                if (settingsDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
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
                _form.XController.Start();
            }

            if (settingsModified)
            {
                // simulate a settings reload
                LoadSettings();
            }

            return settingsModified;
        }
    }
}
