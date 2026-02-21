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
using Sanford.Multimedia.Midi;
using Sanford.Multimedia.Midi.Core.Sanford.Multimedia.Midi.Messages;
using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Resources;
using System.Text;
using System.Windows.Forms;
using Xplorer.Common;
using Xplorer.Properties;

namespace Xplorer.View.Settings
{
    /// <summary>
    /// Midi settings page impl
    /// </summary>
    public partial class MidiPage : UserControl, ISettingsPage
    {
        // automation devices are optional. use this name to indicate it is not used
        private const string DEVICE_NONE = "None";

        private string _synthOutputDeviceName;

        /// <summary>
        /// Gets or sets the name of the synth output device.
        /// </summary>
        /// <value>
        /// The name of the synth output device.
        /// </value>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public string SynthOutputDeviceName
        {
            get { return _synthOutputDeviceName; }
            set { _synthOutputDeviceName = value; }
        }

        private string _automationInputDeviceName;

        /// <summary>
        /// Gets or sets the name of the automation input device.
        /// </summary>
        /// <value>
        /// The name of the automation input device.
        /// </value>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public string AutomationInputDeviceName
        {
            get { return _automationInputDeviceName; }
            set { _automationInputDeviceName = value; }
        }

        private string _synthInputDeviceName;

        /// <summary>
        /// Gets or sets the name of the synth input device.
        /// </summary>
        /// <value>
        /// The name of the synth input device.
        /// </value>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public string SynthInputDeviceName
        {
            get { return _synthInputDeviceName; }
            set { _synthInputDeviceName = value; }
        }

        /// <summary>
        /// Smart "All Notes Off" management
        /// </summary>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public bool SmartAllNotesOff { get; set; }

        /// <summary>
        /// default transmit delay (Xpander)
        /// </summary>
        private const int SYSEX_TRANSMIT_DELAY_XPANDER_DEFAULT = 30; // 30 ms

        /// <summary>
        /// default transmit delay (M12)
        /// </summary>
        private const int SYSEX_TRANSMIT_DELAY_MATRIX12_DEFAULT = 100; // 100 ms

        private int _sysExTransmitDelay = SYSEX_TRANSMIT_DELAY_XPANDER_DEFAULT;

        /// <summary>
        /// Gets or sets the sys ex transmit delay.
        /// </summary>
        /// <value>
        /// The sys ex transmit delay.
        /// </value>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public int SysExTransmitDelay
        {
            get { return _sysExTransmitDelay; }
            set { _sysExTransmitDelay = value; }
        }

        private int _midiChannel = 1;

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public int MIDIChannel
        {
            get { return _midiChannel; }
            set { _midiChannel = value; }
        }

        private int _editingProgramNumber = XpanderConstants.SINGLE_TONES_MAX_COUNT - 1;

        /// <summary>
        /// Gets or sets the editing program number.
        /// </summary>
        /// <value>
        /// The editing program number.
        /// </value>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public int EditingProgramNumber
        {
            get { return _editingProgramNumber; }
            set { _editingProgramNumber = value; }
        }

        private bool _SynthTypeIsMatrix12 = false;

        /// <summary>
        /// Gets or sets a value indicating whether [synth type is matrix12 or Xpander].
        /// </summary>
        /// <value>
        /// 	<c>true</c> if [synth type is matrix12]; otherwise, <c>false</c>.
        /// </value>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public bool SynthTypeIsMatrix12
        {
            get { return _SynthTypeIsMatrix12; }
            set { _SynthTypeIsMatrix12 = value; }
        }

        /// <summary>
        /// Gets or sets the colorString of the knob led border.
        /// </summary>
        /// <value>
        /// The colorString of the knob led border.
        /// </value>
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public Color KnobLedBorderColor { get; set; }

        private readonly OrderedDictionary _automationTable = new OrderedDictionary();

        /// <summary>
        /// Gets the automation table.
        /// </summary>
        public OrderedDictionary AutomationTable
        {
            get
            {
                return _automationTable;
            }
        }

        /// <summary>
        /// ctor
        /// </summary>
        public MidiPage()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Handles the SelectedValueChanged event of the comboControlChange control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void comboControlChange_SelectedValueChanged(object sender, System.EventArgs e)
        {
            if (LvAutomation.SelectedItems.Count != 0)
            {
                ListViewItem item = LvAutomation.SelectedItems[0];
                // use index as CC number
                item.Tag = comboControlChange.SelectedIndex;
            }
            LvAutomation.EndEditing(true);
        }

        /// <summary>
        /// Helper for showing Control change names
        /// </summary>
        /// <param name="number"></param>
        /// <returns></returns>
        private static string GetControlChangeNameForNumber(int number)
        {
            string name = null;
            if (number < 128)
            {
                name = String.Format("{0} : {1}", number.ToString(), ControlChangesNames.Names[number]);
            }
            else
            {
                name = String.Format("{0}", ControlChangesNames.Names[number]);
            }
            return name;
        }

        /// <summary>
        /// Handles the Click event of the rdXpander control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void rdXpander_Click(object sender, EventArgs e)
        {
            this.nUpDownDelay.Value = SYSEX_TRANSMIT_DELAY_XPANDER_DEFAULT;
        }

        /// <summary>
        /// Handles the CheckedChanged event of the rdMatrix12 control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void rdMatrix12_CheckedChanged(object sender, EventArgs e)
        {
            this.nUpDownDelay.Value = SYSEX_TRANSMIT_DELAY_MATRIX12_DEFAULT;
        }

        /// <summary>
        /// Handles the SubItemClicked event of the LvAutomation control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="MidiApp.UIControls.SubItemEventArgs"/> instance containing the event data.</param>
        private void LvAutomation_SubItemClicked(object sender, MidiApp.UIControls.SubItemEventArgs e)
        {
            // avoid editing of paramter name
            if (e.SubItem == 0) return;

            if (LvAutomation.SelectedItems.Count != 0)
            {
                comboControlChange.SelectedIndex = (int)LvAutomation.SelectedItems[0].Tag;
            }
            else
            {
                comboControlChange.SelectedIndex = 1;
            }
            LvAutomation.StartEditing(comboControlChange, e.Item, e.SubItem);
        }

        #region ISettingsPage Membres

        #region dump utilities

        /// <summary>
        /// Dump input device infos to log file
        /// </summary>
        /// <param name="inCaps">The in caps.</param>
        /// <param name="deviceNumber">The device number.</param>
        private void DumpInputDevice(MidiInCaps inCaps, int deviceNumber)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("InputDevice # " + deviceNumber);
            ushort version = Convert.ToUInt16(inCaps.driverVersion);
            sb = sb.AppendLine("  name: " + inCaps.name.ToString());
            sb = sb.AppendFormat("  version: (v{0}.{1}) \r\n", (version & 0xFF00) >> 8, (version & 0x00FF));
            sb = sb.AppendLine("  mid: " + inCaps.mid.ToString());
            sb = sb.AppendLine("  pid: " + inCaps.pid.ToString());
            sb = sb.AppendLine("  support: " + inCaps.support.ToString());
            Logger.WriteLine(this, TraceLevel.Verbose, sb.ToString());
        }

        /// <summary>
        /// Dumps the output device to log file
        /// </summary>
        /// <param name="outCaps">The out caps.</param>
        /// <param name="deviceNumber">The device number.</param>
        /// Dump input device infos to log file
        private void DumpOutputDevice(MidiOutCaps outCaps, int deviceNumber)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("OutputDevice # " + deviceNumber);
            ushort version = Convert.ToUInt16(outCaps.driverVersion);
            sb = sb.AppendLine("  name: " + outCaps.name.ToString());
            sb = sb.AppendFormat("  version: (v{0}.{1}) \r\n", (version & 0xFF00) >> 8, (version & 0x00FF));
            sb = sb.AppendLine("  mid: " + outCaps.mid.ToString());
            sb = sb.AppendLine("  pid: " + outCaps.pid.ToString());
            sb = sb.AppendLine("  support: " + outCaps.support.ToString());
            Logger.WriteLine(this, TraceLevel.Verbose, sb.ToString());
        }

        #endregion dump utilities

        /// <summary>
        /// Initialize the page
        /// </summary>
        public void Initialize()
        {
            Debug.Assert(this.IsHandleCreated);

            int synthOutputDeviceID = -1;
            int synthInputDeviceID = -1;
            int automationInputDeviceID = -1;

            //Input Device
            int inputDeviceCount = InputDevice.DeviceCount;

            Logger.WriteLine(this, TraceLevel.Verbose, "InputDevice.DeviceCount returned: " + inputDeviceCount.ToString());
            _automationInputCombobox.Items.Add(DEVICE_NONE);
            _synthInputCombobox.Items.Add(DEVICE_NONE);

            for (int i = 0; i < inputDeviceCount; i++)
            {
                MidiInCaps inCaps = InputDevice.GetDeviceCapabilities(i);
                DumpInputDevice(inCaps, i);

                string name = inCaps.name;
                _automationInputCombobox.Items.Add(name);
                _synthInputCombobox.Items.Add(name);
                if (name.CompareTo(_automationInputDeviceName) == 0)
                {
                    automationInputDeviceID = i;
                }
                if (name.CompareTo(_synthInputDeviceName) == 0)
                {
                    synthInputDeviceID = i;
                }
            }
            // +1 because we added "NONE" previously
            _automationInputCombobox.SelectedIndex = automationInputDeviceID + 1;
            _synthInputCombobox.SelectedIndex = synthInputDeviceID + 1;

            // Output device

            int outputDeviceCount = OutputDevice.DeviceCount;
            Logger.WriteLine(this, TraceLevel.Verbose, "OutputDevice.DeviceCount returned: " + inputDeviceCount.ToString());
            // synth output must  be choosed by user
            for (int i = 0; i < outputDeviceCount; i++)
            {
                MidiOutCaps outCaps = OutputDevice.GetDeviceCapabilities(i);
                DumpOutputDevice(outCaps, i);
                string name = outCaps.name;
                _synthOutputComboBox.Items.Add(name);

                if (name.CompareTo(_synthOutputDeviceName) == 0)
                {
                    synthOutputDeviceID = i;
                }
            }
            _synthOutputComboBox.SelectedIndex = synthOutputDeviceID;

            // Initialize the Automation Table
            ResourceManager resourceManager = Resources.ResourceManager;
            foreach (DictionaryEntry entry in AutomationTable)
            {
                string text = resourceManager.GetString((string)entry.Key);
                if (string.IsNullOrEmpty(text))
                {
                    Debug.Fail("Settings: missing resource or bad parameter name: " + (string)entry.Key);
                }
                ListViewItem item = LvAutomation.Items.Add(text);
                item.Name = (string)entry.Key;
                item.Tag = (entry.Value); // tag is control change number
                item.SubItems.Add(GetControlChangeNameForNumber((int)entry.Value));
            }

            // initialize the CC combo content
            for (int iControlChangeNumber = 0; iControlChangeNumber < ControlChangesNames.Names.Length; iControlChangeNumber++)
            {
                comboControlChange.Items.Add(GetControlChangeNameForNumber(iControlChangeNumber));
            }
            comboControlChange.SelectedIndexChanged += new EventHandler(comboControlChange_SelectedValueChanged);

            //delay,midi channel,editing program
            nUpDownDelay.Value = SysExTransmitDelay;
            nmEditPatchNumber.Value = EditingProgramNumber;
            nmMIDIChannel.Value = MIDIChannel;

            if (SynthTypeIsMatrix12)
            {
                rdMatrix12.Checked = true;
                rdXpander.Checked = false;
            }
            else
            {
                rdMatrix12.Checked = false;
                rdXpander.Checked = true;
            }

            // column size adjustment
            LvAutomation.Columns[1].Width = LvAutomation.Width - LvAutomation.Columns[0].Width - System.Windows.Forms.SystemInformation.VerticalScrollBarWidth;

            // all notes off
            _cbSmartAllNotesOff.Checked = SmartAllNotesOff;

            IsInitialized = true;
        }

        /// <summary>
        /// set up internal state when OnOK is fired on the page host
        /// </summary>
        public void ProcessOnOK()
        {
            // midi ports update
            if (InputDevice.DeviceCount > 0)
            {
                AutomationInputDeviceName = _automationInputCombobox.Text;
                SynthInputDeviceName = _synthInputCombobox.Text;
            }

            if (OutputDevice.DeviceCount > 0)
            {
                SynthOutputDeviceName = _synthOutputComboBox.Text;
            }

            // automation table
            foreach (ListViewItem item in LvAutomation.Items)
            {
                //update the control change number
                AutomationTable[item.Name] = (int)item.Tag;
            }

            //SysExTransmitDelay
            SysExTransmitDelay = (int)nUpDownDelay.Value;
            MIDIChannel = (int)nmMIDIChannel.Value;
            EditingProgramNumber = (int)nmEditPatchNumber.Value;
            SynthTypeIsMatrix12 = rdMatrix12.Checked;
            SmartAllNotesOff = _cbSmartAllNotesOff.Checked;
        }

        private bool _isInitialized = false;

        /// <summary>
        /// True if Initialize was already called
        /// </summary>
        public bool IsInitialized
        {
            get { return _isInitialized; }
            private set { _isInitialized = value; }
        }

        #endregion ISettingsPage Membres

        #region HTML

        /// <summary>
        /// Modern HTML5 prologue with print-ready styling
        /// </summary>
        private static readonly string _htmlPrologue =
$@"<!DOCTYPE html>
<html lang=""en"">
<head>
    <meta charset=""UTF-8"">
    <meta name=""viewport"" content=""width=device-width, initial-scale=1.0"">
    <title>Xplorer MIDI Automation Table</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}

        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: #f5f5f5;
            color: #333;
            padding: 2rem;
            line-height: 1.6;
        }}

        .container {{
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            padding: 2rem;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            border-radius: 8px;
        }}

        h1 {{
            color: #2c3e50;
            font-size: 2rem;
            margin-bottom: 0.5rem;
            border-bottom: 3px solid #3498db;
            padding-bottom: 0.5rem;
        }}

        .subtitle {{
            color: #7f8c8d;
            font-size: 0.9rem;
            margin-bottom: 2rem;
        }}

        .subtitle a {{
            color: #3498db;
            text-decoration: none;
            transition: color 0.3s;
        }}

        .subtitle a:hover {{
            color: #2980b9;
            text-decoration: underline;
        }}

        table {{
            width: 100%;
            border-collapse: collapse;
            margin-top: 1.5rem;
            box-shadow: 0 1px 3px rgba(0,0,0,0.1);
        }}

        thead {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }}

        th {{
            padding: 1rem;
            text-align: left;
            font-weight: 600;
            text-transform: uppercase;
            font-size: 0.875rem;
            letter-spacing: 0.5px;
        }}

        td {{
            padding: 0.875rem 1rem;
            border-bottom: 1px solid #ecf0f1;
        }}

        tbody tr {{
            transition: background-color 0.2s;
        }}

        tbody tr:nth-child(even) {{
            background-color: #f8f9fa;
        }}

        tbody tr:hover {{
            background-color: #e3f2fd;
        }}

        .generated-info {{
            margin-top: 2rem;
            padding: 1rem;
            background: #ecf0f1;
            border-left: 4px solid #3498db;
            font-size: 0.875rem;
            color: #7f8c8d;
        }}

        /* Print styles */
        @media print {{
            body {{
                background: white;
                padding: 0;
            }}

            .container {{
                box-shadow: none;
                padding: 0;
                max-width: 100%;
            }}

            h1 {{
                color: black;
                border-bottom: 2px solid black;
            }}

            thead {{
                background: #333 !important;
                color: white !important;
                -webkit-print-color-adjust: exact;
                print-color-adjust: exact;
            }}

            tbody tr:nth-child(even) {{
                background-color: #f0f0f0 !important;
                -webkit-print-color-adjust: exact;
                print-color-adjust: exact;
            }}

            tbody tr:hover {{
                background-color: transparent !important;
            }}

            .subtitle a {{
                color: black;
                text-decoration: none;
            }}

            table {{
                box-shadow: none;
            }}

            /* Ensure table doesn't break across pages */
            table {{
                page-break-inside: auto;
            }}

            tr {{
                page-break-inside: avoid;
                page-break-after: auto;
            }}

            thead {{
                display: table-header-group;
            }}
        }}
    </style>
</head>
<body>
    <div class=""container"">
        <h1>MIDI Automation Table</h1>
        <div class=""subtitle"">
            Xplorer - Oberheim Xpander/Matrix-12 Real-Time Editor<br>
            <a href=""{XplorerConstants.WEBSITE_URL}"" target=""_blank"">{XplorerConstants.WEBSITE_URL}</a>
        </div>

        <table>
            <thead>
                <tr>
                    <th>Parameter</th>
                    <th>Control Change</th>
                </tr>
            </thead>
            <tbody>
";

        private static readonly string _htmlEpilogue =
$@"            </tbody>
        </table>

        <div class=""generated-info"">
            Generated on {DateTime.Now:yyyy-MM-dd HH:mm:ss}
        </div>
    </div>
</body>
</html>";

        /// <summary>
        /// Handles the Click event of the exportAsTextFileToolStripMenuItem control.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        private void exportAsTextFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            const string HTML_FILE_FILTER = "HTML files (*.htm)|*.htm";
            SaveFileDialog htmlExportDialog = new SaveFileDialog();

            htmlExportDialog.Filter = HTML_FILE_FILTER;
            htmlExportDialog.RestoreDirectory = true;

            if (htmlExportDialog.ShowDialog() == DialogResult.OK)
            {
                StreamWriter sw = null;
                try
                {
                    using (sw = new StreamWriter(htmlExportDialog.FileName, false))
                    {
                        sw.WriteLine(_htmlPrologue);
                        foreach (DictionaryEntry entry in AutomationTable)
                        {
                            string parameterString = Properties.Resources.ResourceManager.GetString((string)entry.Key);
                            if (string.IsNullOrEmpty(parameterString))
                            {
                                parameterString = (string)entry.Key;
                            }

                            sw.WriteLine($@"                <tr>
                    <td>{parameterString}</td>
                    <td>{GetControlChangeNameForNumber((int)entry.Value)}</td>
                </tr>");
                        }
                        sw.WriteLine(_htmlEpilogue);
                        try
                        {
                            var psi = new ProcessStartInfo
                            {
                                FileName = htmlExportDialog.FileName,
                                UseShellExecute = true
                            };
                            Process.Start(psi);
                        }
                        catch (Exception ex)
                        {
                            // do nothing more
                            Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                        }
                    }
                }
                catch (Exception ex)
                {
                    //whatever the exception is, consider it as non fatal
                    Logger.WriteLine(this, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                    MessageBox.Show("Unable to export automation table: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }//if
        }

        #endregion HTML

        private void toolStripMenuItemResetControlChanges_Click(object sender, EventArgs e)
        {

            foreach (ListViewItem item in LvAutomation.Items)
            {
                item.SubItems[1].Text = GetControlChangeNameForNumber(ControlChangesNames.Names.Length - 1);
                item.Tag = ControlChangesNames.Names.Length - 1;
            }

        }
    }
}