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
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Xplorer.Common;

namespace Xplorer.View
{
    /// <summary>
    /// Manages file operations (load, save, extract, backup, restore) for the main form
    /// </summary>
    internal sealed class FileOperationsManager
    {
        private readonly MainForm _form;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="form">The main form</param>
        internal FileOperationsManager(MainForm form)
        {
            Debug.Assert(form != null);
            _form = form;
        }

        /// <summary>
        /// Loads a single tone sysex file into the editor and sends it to the synth.
        /// </summary>
        /// <param name="fileName">Path to the sysex file containing a single tone.</param>
        internal void LoadToneFromFile(string fileName)
        {
            try
            {
                _form.XController.LoadTone(fileName);
                _form.SetToneFilename(fileName);
            }
            catch (Exception ex)
            {
                Logger.WriteLine(_form, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                MessageBox.Show("Unable to load patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        /// <summary>
        /// Restores an all data dump (bank) sysex file to the synth with progress indication.
        /// </summary>
        /// <param name="fileName">Path to the sysex file containing an all data dump.</param>
        internal void RestoreAllDataDumpFromFile(string fileName)
        {
            ProgressForm form = null;
            try
            {
#warning we shoud avoid user interaction while running dump
                form = ProgressForm.CreateInstance(_form);
                form.Show(_form);
                form.Text = "All data dump restore";
                Action<int, int> progressAction = (current, max) =>
                {
                    form.MinValue = 0;
                    form.MaxValue = max;
                    form.Value = current;
                    form.Label = string.Format(CultureInfo.InvariantCulture, "Sending data [{0}/{1}]", current.ToString("00", CultureInfo.InvariantCulture), max);
                    Application.DoEvents();
                };

                _form.XController.RestoreAllDataDumpToSynth(fileName, progressAction);
                ProgressForm.DestroyInstance();
            }
            catch (NonFatalException nfe)
            {
                if (form is not null)
                {
                    ProgressForm.DestroyInstance();
                }
                MessageBox.Show(nfe.Message, "All data dump restore", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Loads a sysex file by detecting its type (single tone or all data dump) and routing
        /// to the appropriate handler.
        /// </summary>
        /// <param name="fileName">Path to the sysex file.</param>
        internal void LoadSysexFileByType(string fileName)
        {
            SysexFileType fileType = _form.XController.DetermineSysexFileType(fileName);

            switch (fileType)
            {
                case SysexFileType.SingleTone:
                    LoadToneFromFile(fileName);
                    break;

                case SysexFileType.AllDataDump:
                    // always ask confirmation before sending an all data dump to the synth as it will overwrite all patches
                    const string AllDataDumpWarningMessage = "The selected file is a bank file that may overwrite ALL patches in the synth's memory. Proceed?";

                    if (MessageBox.Show(AllDataDumpWarningMessage,
                        "Confirm All Data Dump Restore", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.Yes)
                    {
                        RestoreAllDataDumpFromFile(fileName);
                    }
                    break;

                default:
                    MessageBox.Show("Unable to determine sysex file type.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
            }
        }

        /// <summary>
        /// Creates a new patch by loading the default sysex file.
        /// </summary>
        internal void NewPatch()
        {
            try
            {
                string filename = MainForm.DefaultToneFilename;

                if (!string.IsNullOrEmpty(filename) && File.Exists(filename))
                {
                    // read the tone sysex and fill the map with the parameters values
                    _form.XController.LoadTone(filename);
                    _form.SetToneFilename(filename);
                }
            }
            catch (Exception ex)
            {
                // whatever the exception is, consider it as non fatal
                Logger.WriteLine(_form, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                MessageBox.Show("Unable to create a new patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        /// <summary>
        /// Saves the current tone to the specified file, or triggers Save As if no valid filename exists.
        /// </summary>
        /// <param name="currentToneFileName">The current tone filename.</param>
        internal void SaveTone(string currentToneFileName)
        {
            char[] InvalidFileNameChars = Path.GetInvalidFileNameChars();
            string sFileName = currentToneFileName;
            if ((sFileName != null) &&
                (sFileName.Length > 0) &&
                (Path.GetFileName(sFileName).IndexOfAny(InvalidFileNameChars) == -1) &&
                (sFileName != MainForm.DefaultToneFilename))
            {
                try
                {
                    _form.XController.SaveTone(sFileName);
                }
                catch (Exception ex)
                {
                    // whatever the exception is, consider it as non fatal
                    Logger.WriteLine(_form, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                    MessageBox.Show("Unable to save patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            else
            {
                // save as
                SaveToneAs();
            }
        }

        /// <summary>
        /// Shows a Save As dialog and saves the tone.
        /// </summary>
        internal void SaveToneAs()
        {
            using (SaveFileDialog saveSysexFileDialog = new SaveFileDialog())
            {
                saveSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
                saveSysexFileDialog.RestoreDirectory = true;

                if (saveSysexFileDialog.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        _form.XController.SaveTone(saveSysexFileDialog.FileName);
                        _form.SetToneFilename(saveSysexFileDialog.FileName);
                    }
                    catch (Exception ex)
                    {
                        //whatever the exception is, consider it as non fatal
                        Logger.WriteLine(_form, TraceLevel.Warning, BugReportFactory.CreateDetailsFromException(ex));
                        MessageBox.Show("Unable to save patch: " + ex.Message, "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                }
            }
        }

        /// <summary>
        /// Shows the extract single patches dialog and extracts patches from a bank file.
        /// </summary>
        internal void ExtractPatches()
        {
            using (ExtractSingleToneForm form = new ExtractSingleToneForm())
            {
                if (form.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        IEnumerable<object> tones = _form.XController.ExtractSinglePatchesFromAllDataDumpFileToDirectory(form.BankFilename, form.DestinationFolder);
                        int count = tones.Count();
                        if (count == 0)
                        {
                            MessageBox.Show("Unable to extrat single patches from file !", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                        else
                        {
                            MessageBox.Show(
                            _form,
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
        /// Shows a save dialog and backs up all data dump to file.
        /// </summary>
        internal void BackupAllData()
        {
            using (SaveFileDialog saveSysexFileDialog = new SaveFileDialog())
            {
                saveSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
                saveSysexFileDialog.RestoreDirectory = true;

                if (saveSysexFileDialog.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        _form.XController.BackupAllDataDumpToFile(saveSysexFileDialog.FileName);
                    }
                    catch (NonFatalException nfe)
                    {
                        MessageBox.Show(nfe.Message, "All data dump backup", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /// <summary>
        /// Shows an open dialog and restores all data dump from file.
        /// </summary>
        internal void RestoreAllData()
        {
            using (OpenFileDialog openSysexFileDialog = new OpenFileDialog())
            {
                openSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
                openSysexFileDialog.RestoreDirectory = true;

                if (openSysexFileDialog.ShowDialog() == DialogResult.OK)
                {
                    RestoreAllDataDumpFromFile(openSysexFileDialog.FileName);
                }
            }
        }

        /// <summary>
        /// Shows a folder dialog and gets single patches from the synth.
        /// </summary>
        internal void GetSinglePatchesFromSynth()
        {
            using (FolderBrowserDialog fbd = new FolderBrowserDialog())
            {
                fbd.ShowNewFolderButton = true;
                fbd.Description = "Select a destination folder for single patch sysex files";
                if (fbd.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        _form.XController.GetSingleTonesFromSynth(fbd.SelectedPath);
                    }
                    catch (NonFatalException nfe)
                    {
                        MessageBox.Show(_form, nfe.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /// <summary>
        /// Shows an open file dialog and loads the selected sysex file.
        /// </summary>
        internal void OpenSysexFile()
        {
            using (OpenFileDialog openSysexFileDialog = new OpenFileDialog())
            {
                openSysexFileDialog.Filter = FileUtils.SYSEX_FILE_FILTER;
                openSysexFileDialog.RestoreDirectory = true;

                if (openSysexFileDialog.ShowDialog() == DialogResult.OK)
                {
                    LoadSysexFileByType(openSysexFileDialog.FileName);
                }
            }
        }
    }
}
