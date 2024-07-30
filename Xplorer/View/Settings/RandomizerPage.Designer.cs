namespace Xplorer.View.Settings
{
    partial class RandomizerPage
    {
        /// <summary> 
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur de composants

        /// <summary> 
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas 
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this._groupBoxMatric = new System.Windows.Forms.GroupBox();
            this._cbModQuantize = new System.Windows.Forms.CheckBox();
            this._cbModSourceAndDestination = new System.Windows.Forms.CheckBox();
            this._cbModAmount = new System.Windows.Forms.CheckBox();
            this._labelDetune = new System.Windows.Forms.Label();
            this._groupBoxVcaEnv = new System.Windows.Forms.GroupBox();
            this._comboVca2EnvelopePreset = new MidiApp.UIControls.ComboBoxValuedControl();
            this._lbEnveloppeType = new System.Windows.Forms.Label();
            this._groupBoxVCO = new System.Windows.Forms.GroupBox();
            this._comboVcoDetune = new MidiApp.UIControls.ComboBoxValuedControl();
            this._comboVCoFreq = new MidiApp.UIControls.ComboBoxValuedControl();
            this._lbFrequencies = new System.Windows.Forms.Label();
            this._cbVco2FM = new System.Windows.Forms.CheckBox();
            this._cbVco2Sync = new System.Windows.Forms.CheckBox();
            this._cbVco2Noise = new System.Windows.Forms.CheckBox();
            this._btRandomizeAll = new System.Windows.Forms.Button();
            this._groupBoxMatric.SuspendLayout();
            this._groupBoxVcaEnv.SuspendLayout();
            this._groupBoxVCO.SuspendLayout();
            this.SuspendLayout();
            // 
            // _groupBoxMatric
            // 
            this._groupBoxMatric.AccessibleDescription = "Modulation matrix";
            this._groupBoxMatric.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this._groupBoxMatric.Controls.Add(this._cbModQuantize);
            this._groupBoxMatric.Controls.Add(this._cbModSourceAndDestination);
            this._groupBoxMatric.Controls.Add(this._cbModAmount);
            this._groupBoxMatric.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            this._groupBoxMatric.ForeColor = System.Drawing.Color.White;
            this._groupBoxMatric.Location = new System.Drawing.Point(3, 101);
            this._groupBoxMatric.Name = "_groupBoxMatric";
            this._groupBoxMatric.Size = new System.Drawing.Size(744, 63);
            this._groupBoxMatric.TabIndex = 7;
            this._groupBoxMatric.TabStop = false;
            this._groupBoxMatric.Tag = "";
            this._groupBoxMatric.Text = "Modulation matrix";
            // 
            // _cbModQuantize
            // 
            this._cbModQuantize.AccessibleDescription = "Quantinze";
            this._cbModQuantize.AutoSize = true;
            this._cbModQuantize.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._cbModQuantize.Location = new System.Drawing.Point(277, 29);
            this._cbModQuantize.Name = "_cbModQuantize";
            this._cbModQuantize.Size = new System.Drawing.Size(68, 17);
            this._cbModQuantize.TabIndex = 12;
            this._cbModQuantize.Text = "Quantize";
            this._cbModQuantize.UseVisualStyleBackColor = true;
            // 
            // _cbModSourceAndDestination
            // 
            this._cbModSourceAndDestination.AccessibleDescription = "Modulation source and destination";
            this._cbModSourceAndDestination.AutoSize = true;
            this._cbModSourceAndDestination.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._cbModSourceAndDestination.Location = new System.Drawing.Point(74, 29);
            this._cbModSourceAndDestination.Name = "_cbModSourceAndDestination";
            this._cbModSourceAndDestination.Size = new System.Drawing.Size(198, 17);
            this._cbModSourceAndDestination.TabIndex = 11;
            this._cbModSourceAndDestination.Text = "Modulation sources and destinations";
            this._cbModSourceAndDestination.UseVisualStyleBackColor = true;
            // 
            // _cbModAmount
            // 
            this._cbModAmount.AccessibleName = "Amount";
            this._cbModAmount.AutoSize = true;
            this._cbModAmount.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._cbModAmount.Location = new System.Drawing.Point(9, 29);
            this._cbModAmount.Name = "_cbModAmount";
            this._cbModAmount.Size = new System.Drawing.Size(62, 17);
            this._cbModAmount.TabIndex = 10;
            this._cbModAmount.Text = "Amount";
            this._cbModAmount.UseVisualStyleBackColor = true;
            // 
            // _labelDetune
            // 
            this._labelDetune.AutoSize = true;
            this._labelDetune.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._labelDetune.ForeColor = System.Drawing.Color.White;
            this._labelDetune.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this._labelDetune.Location = new System.Drawing.Point(255, 22);
            this._labelDetune.Name = "_labelDetune";
            this._labelDetune.Size = new System.Drawing.Size(42, 13);
            this._labelDetune.TabIndex = 0;
            this._labelDetune.Text = "Detune";
            // 
            // _groupBoxVcaEnv
            // 
            this._groupBoxVcaEnv.Controls.Add(this._comboVca2EnvelopePreset);
            this._groupBoxVcaEnv.Controls.Add(this._lbEnveloppeType);
            this._groupBoxVcaEnv.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._groupBoxVcaEnv.ForeColor = System.Drawing.Color.White;
            this._groupBoxVcaEnv.Location = new System.Drawing.Point(482, 3);
            this._groupBoxVcaEnv.Name = "_groupBoxVcaEnv";
            this._groupBoxVcaEnv.Size = new System.Drawing.Size(265, 92);
            this._groupBoxVcaEnv.TabIndex = 8;
            this._groupBoxVcaEnv.TabStop = false;
            this._groupBoxVcaEnv.Text = "VCA2 Envelope";
            // 
            // _comboVca2EnvelopePreset
            // 
            this._comboVca2EnvelopePreset.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._comboVca2EnvelopePreset.FormattingEnabled = true;
            this._comboVca2EnvelopePreset.Location = new System.Drawing.Point(43, 19);
            this._comboVca2EnvelopePreset.Maximum = 1;
            this._comboVca2EnvelopePreset.Minimum = 0;
            this._comboVca2EnvelopePreset.Name = "_comboVca2EnvelopePreset";
            this._comboVca2EnvelopePreset.Size = new System.Drawing.Size(216, 21);
            this._comboVca2EnvelopePreset.Step = 1;
            this._comboVca2EnvelopePreset.TabIndex = 14;
            this._comboVca2EnvelopePreset.Value = -2147483648;
            // 
            // _lbEnveloppeType
            // 
            this._lbEnveloppeType.AutoSize = true;
            this._lbEnveloppeType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._lbEnveloppeType.ForeColor = System.Drawing.Color.White;
            this._lbEnveloppeType.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this._lbEnveloppeType.Location = new System.Drawing.Point(6, 22);
            this._lbEnveloppeType.Name = "_lbEnveloppeType";
            this._lbEnveloppeType.Size = new System.Drawing.Size(31, 13);
            this._lbEnveloppeType.TabIndex = 10;
            this._lbEnveloppeType.Text = "Type";
            // 
            // _groupBoxVCO
            // 
            this._groupBoxVCO.Controls.Add(this._comboVcoDetune);
            this._groupBoxVCO.Controls.Add(this._comboVCoFreq);
            this._groupBoxVCO.Controls.Add(this._lbFrequencies);
            this._groupBoxVCO.Controls.Add(this._cbVco2FM);
            this._groupBoxVCO.Controls.Add(this._cbVco2Sync);
            this._groupBoxVCO.Controls.Add(this._cbVco2Noise);
            this._groupBoxVCO.Controls.Add(this._labelDetune);
            this._groupBoxVCO.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._groupBoxVCO.ForeColor = System.Drawing.Color.White;
            this._groupBoxVCO.Location = new System.Drawing.Point(3, 3);
            this._groupBoxVCO.Name = "_groupBoxVCO";
            this._groupBoxVCO.Size = new System.Drawing.Size(473, 92);
            this._groupBoxVCO.TabIndex = 9;
            this._groupBoxVCO.TabStop = false;
            this._groupBoxVCO.Text = "VCO";
            // 
            // _comboVcoDetune
            // 
            this._comboVcoDetune.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._comboVcoDetune.FormattingEnabled = true;
            this._comboVcoDetune.Location = new System.Drawing.Point(303, 19);
            this._comboVcoDetune.Maximum = 1;
            this._comboVcoDetune.Minimum = 0;
            this._comboVcoDetune.Name = "_comboVcoDetune";
            this._comboVcoDetune.Size = new System.Drawing.Size(164, 21);
            this._comboVcoDetune.Step = 1;
            this._comboVcoDetune.TabIndex = 14;
            this._comboVcoDetune.Value = -2147483648;
            // 
            // _comboVCoFreq
            // 
            this._comboVCoFreq.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._comboVCoFreq.FormattingEnabled = true;
            this._comboVCoFreq.Location = new System.Drawing.Point(69, 19);
            this._comboVCoFreq.Maximum = 1;
            this._comboVCoFreq.Minimum = 0;
            this._comboVCoFreq.Name = "_comboVCoFreq";
            this._comboVCoFreq.Size = new System.Drawing.Size(180, 21);
            this._comboVCoFreq.Step = 1;
            this._comboVCoFreq.TabIndex = 14;
            this._comboVCoFreq.Value = -2147483648;
            // 
            // _lbFrequencies
            // 
            this._lbFrequencies.AutoSize = true;
            this._lbFrequencies.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._lbFrequencies.ForeColor = System.Drawing.Color.White;
            this._lbFrequencies.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this._lbFrequencies.Location = new System.Drawing.Point(6, 22);
            this._lbFrequencies.Name = "_lbFrequencies";
            this._lbFrequencies.Size = new System.Drawing.Size(57, 13);
            this._lbFrequencies.TabIndex = 10;
            this._lbFrequencies.Text = "Frequency";
            // 
            // _cbVco2FM
            // 
            this._cbVco2FM.AccessibleDescription = "FM";
            this._cbVco2FM.AutoSize = true;
            this._cbVco2FM.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._cbVco2FM.Location = new System.Drawing.Point(70, 51);
            this._cbVco2FM.Name = "_cbVco2FM";
            this._cbVco2FM.Size = new System.Drawing.Size(41, 17);
            this._cbVco2FM.TabIndex = 15;
            this._cbVco2FM.Text = "FM";
            this._cbVco2FM.UseVisualStyleBackColor = true;
            // 
            // _cbVco2Sync
            // 
            this._cbVco2Sync.AccessibleDescription = "Sync";
            this._cbVco2Sync.AutoSize = true;
            this._cbVco2Sync.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._cbVco2Sync.Location = new System.Drawing.Point(117, 51);
            this._cbVco2Sync.Name = "_cbVco2Sync";
            this._cbVco2Sync.Size = new System.Drawing.Size(50, 17);
            this._cbVco2Sync.TabIndex = 14;
            this._cbVco2Sync.Text = "Sync";
            this._cbVco2Sync.UseVisualStyleBackColor = true;
            // 
            // _cbVco2Noise
            // 
            this._cbVco2Noise.AccessibleName = "Noise";
            this._cbVco2Noise.AutoSize = true;
            this._cbVco2Noise.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._cbVco2Noise.Location = new System.Drawing.Point(10, 51);
            this._cbVco2Noise.Name = "_cbVco2Noise";
            this._cbVco2Noise.Size = new System.Drawing.Size(53, 17);
            this._cbVco2Noise.TabIndex = 13;
            this._cbVco2Noise.Text = "Noise";
            this._cbVco2Noise.UseVisualStyleBackColor = true;
            // 
            // _btRandomizeAll
            // 
            this._btRandomizeAll.Location = new System.Drawing.Point(9, 181);
            this._btRandomizeAll.Name = "_btRandomizeAll";
            this._btRandomizeAll.Size = new System.Drawing.Size(75, 23);
            this._btRandomizeAll.TabIndex = 14;
            this._btRandomizeAll.Text = "reset";
            this._btRandomizeAll.UseVisualStyleBackColor = true;
            this._btRandomizeAll.Click += new System.EventHandler(this._btRandomizeAll_Click);
            // 
            // RandomizerPage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.Controls.Add(this._btRandomizeAll);
            this.Controls.Add(this._groupBoxVCO);
            this.Controls.Add(this._groupBoxVcaEnv);
            this.Controls.Add(this._groupBoxMatric);
            this.MaximumSize = new System.Drawing.Size(750, 350);
            this.MinimumSize = new System.Drawing.Size(750, 350);
            this.Name = "RandomizerPage";
            this.Size = new System.Drawing.Size(750, 350);
            this._groupBoxMatric.ResumeLayout(false);
            this._groupBoxMatric.PerformLayout();
            this._groupBoxVcaEnv.ResumeLayout(false);
            this._groupBoxVcaEnv.PerformLayout();
            this._groupBoxVCO.ResumeLayout(false);
            this._groupBoxVCO.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox _groupBoxMatric;
        private System.Windows.Forms.Label _labelDetune;
        private System.Windows.Forms.CheckBox _cbModQuantize;
        private System.Windows.Forms.CheckBox _cbModSourceAndDestination;
        private System.Windows.Forms.CheckBox _cbModAmount;
        private System.Windows.Forms.GroupBox _groupBoxVcaEnv;
        private System.Windows.Forms.GroupBox _groupBoxVCO;
        private System.Windows.Forms.CheckBox _cbVco2FM;
        private System.Windows.Forms.CheckBox _cbVco2Sync;
        private System.Windows.Forms.CheckBox _cbVco2Noise;
        private System.Windows.Forms.Label _lbFrequencies;
        private System.Windows.Forms.Label _lbEnveloppeType;
        private MidiApp.UIControls.ComboBoxValuedControl _comboVca2EnvelopePreset;
        private MidiApp.UIControls.ComboBoxValuedControl _comboVcoDetune;
        private MidiApp.UIControls.ComboBoxValuedControl _comboVCoFreq;
        private System.Windows.Forms.Button _btRandomizeAll;
    }
}
