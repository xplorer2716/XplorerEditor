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
using System.Resources;
using System.Windows.Forms;
using Xplorer.Common;
using Xplorer.Controller.Service.Settings;
using static Xplorer.Controller.Service.Settings.AllUsersSettings;

namespace Xplorer.View.Settings
{
    /// <summary>
    /// Randomizer settings page impl
    /// </summary>
    public partial class RandomizerPage : UserControl, ISettingsPage
    {
        private bool _isInitialized = false;

        /// <summary>
        /// Gets or sets randomizer configuration.
        /// </summary>
        /// <value>
        /// The configuration.
        /// </value>
        public AllUsersSettings.RandomizerConfiguration Configuration { get; set; }

        /// <summary>
        /// ctor
        /// </summary>
        public RandomizerPage()
        {
            InitializeComponent();
        }

        #region ISettingsPage Membres

        /// <summary>
        /// Initialize the page
        /// </summary>
        public void Initialize()
        {
            Debug.Assert(this.IsHandleCreated);
            SetControlFromValues(Configuration);
        }

        /// <summary>
        /// set controls from values
        /// </summary>
        private void SetControlFromValues(RandomizerConfiguration configuration)
        {
            _cbVco2FM.Checked = configuration.VCO2FmNoiseSync.HasFlag(EnumRandomVCO2.EnableFM);
            _cbVco2Noise.Checked = configuration.VCO2FmNoiseSync.HasFlag(EnumRandomVCO2.EnableNoise);
            _cbVco2Sync.Checked = configuration.VCO2FmNoiseSync.HasFlag(EnumRandomVCO2.EnableSync);

            _cbModQuantize.Checked = configuration.ModulationMatrix.HasFlag(EnumRandomModMatrix.EnableQuantize);
            _cbModAmount.Checked = configuration.ModulationMatrix.HasFlag(EnumRandomModMatrix.EnableAmount);
            _cbModSourceAndDestination.Checked = configuration.ModulationMatrix.HasFlag(EnumRandomModMatrix.EnableSourcesAndDestinations);

            ResourceManager resourceManager = Properties.Resources.ResourceManager;
            _comboVca2EnvelopePreset.SetEnumType(typeof(EnumRandomVCAEnv), resourceManager);
            _comboVca2EnvelopePreset.Value = (int)configuration.VCA2Env;

            _comboVcoDetune.SetEnumType(typeof(EnumRandomVCODetune), resourceManager);
            _comboVcoDetune.Value = (int)configuration.VCODetune;

            _comboVCoFreq.SetEnumType(typeof(EnumRandomVCOFreq), resourceManager);
            _comboVCoFreq.Value = (int)configuration.VCOFreq;
        }

        #endregion ISettingsPage Membres

        /// <summary>
        /// True if Initialize was already called
        /// </summary>
        public bool IsInitialized
        {
            get { return _isInitialized; }
            private set { _isInitialized = value; }
        }

        /// <summary>
        /// set up internal state when OnOK is fired on the page host
        /// </summary>
        public void ProcessOnOK()
        {
            SetControlsToValues();
        }

        private void SetControlsToValues()
        {
            //VCO/VCO2
            Configuration.VCOFreq = (EnumRandomVCOFreq)_comboVCoFreq.Value;
            Configuration.VCODetune = (EnumRandomVCODetune)_comboVcoDetune.Value;
            Configuration.VCO2FmNoiseSync = 0;
            if (_cbVco2FM.Checked) Configuration.VCO2FmNoiseSync |= EnumRandomVCO2.EnableFM;
            if (_cbVco2Noise.Checked) Configuration.VCO2FmNoiseSync |= EnumRandomVCO2.EnableNoise;
            if (_cbVco2Sync.Checked) Configuration.VCO2FmNoiseSync |= EnumRandomVCO2.EnableSync;
            // VCA2 envelope
            Configuration.VCA2Env = (EnumRandomVCAEnv)_comboVca2EnvelopePreset.Value;

            // Mod matrix
            Configuration.ModulationMatrix = 0;
            if (_cbModAmount.Checked) Configuration.ModulationMatrix |= EnumRandomModMatrix.EnableAmount;
            if (_cbModQuantize.Checked) Configuration.ModulationMatrix |= EnumRandomModMatrix.EnableQuantize;
            if (_cbModSourceAndDestination.Checked) Configuration.ModulationMatrix |= EnumRandomModMatrix.EnableSourcesAndDestinations;
        }

        private void _btRandomizeAll_Click(object sender, EventArgs e)
        {
            var randomAll = AllUsersSettingsService.GetRandomizerConfigurationDefault();
            SetControlFromValues(randomAll);
        }
    }
}