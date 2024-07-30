namespace Xplorer.View.Settings
{
    partial class UserInterfacePage
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
            this.components = new System.ComponentModel.Container();
            this._colorDialog = new System.Windows.Forms.ColorDialog();
            this._knobGroupBox = new System.Windows.Forms.GroupBox();
            this._panelMovement = new System.Windows.Forms.Panel();
            this._radioCircular = new System.Windows.Forms.RadioButton();
            this._radioLinear = new System.Windows.Forms.RadioButton();
            this._labelMovement = new System.Windows.Forms.Label();
            this._panelStyle = new System.Windows.Forms.Panel();
            this._radioFlat = new System.Windows.Forms.RadioButton();
            this._radioStandard = new System.Windows.Forms.RadioButton();
            this._labelStyle = new System.Windows.Forms.Label();
            this._colorButton = new System.Windows.Forms.Button();
            this._knob = new MidiApp.UIControls.KnobControl();
            this._tooltip = new System.Windows.Forms.ToolTip(this.components);
            this._knobGroupBox.SuspendLayout();
            this._panelMovement.SuspendLayout();
            this._panelStyle.SuspendLayout();
            this.SuspendLayout();
            // 
            // _knobGroupBox
            // 
            this._knobGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._knobGroupBox.Controls.Add(this._panelMovement);
            this._knobGroupBox.Controls.Add(this._labelMovement);
            this._knobGroupBox.Controls.Add(this._panelStyle);
            this._knobGroupBox.Controls.Add(this._labelStyle);
            this._knobGroupBox.Controls.Add(this._colorButton);
            this._knobGroupBox.Controls.Add(this._knob);
            this._knobGroupBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            this._knobGroupBox.ForeColor = System.Drawing.Color.White;
            this._knobGroupBox.Location = new System.Drawing.Point(3, 3);
            this._knobGroupBox.Name = "_knobGroupBox";
            this._knobGroupBox.Size = new System.Drawing.Size(744, 104);
            this._knobGroupBox.TabIndex = 7;
            this._knobGroupBox.TabStop = false;
            this._knobGroupBox.Tag = "";
            this._knobGroupBox.Text = "Knobs";
            // 
            // _panelMovement
            // 
            this._panelMovement.Controls.Add(this._radioCircular);
            this._panelMovement.Controls.Add(this._radioLinear);
            this._panelMovement.Location = new System.Drawing.Point(162, 38);
            this._panelMovement.Name = "_panelMovement";
            this._panelMovement.Size = new System.Drawing.Size(76, 60);
            this._panelMovement.TabIndex = 9;
            // 
            // _radioCircular
            // 
            this._radioCircular.AutoSize = true;
            this._radioCircular.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._radioCircular.Location = new System.Drawing.Point(3, 29);
            this._radioCircular.Name = "_radioCircular";
            this._radioCircular.Size = new System.Drawing.Size(59, 17);
            this._radioCircular.TabIndex = 5;
            this._radioCircular.TabStop = true;
            this._radioCircular.Text = "circular";
            this._tooltip.SetToolTip(this._radioCircular, "Circular movement (mimics real knobs)");
            this._radioCircular.UseVisualStyleBackColor = true;
            // 
            // _radioLinear
            // 
            this._radioLinear.AutoSize = true;
            this._radioLinear.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._radioLinear.Location = new System.Drawing.Point(3, 6);
            this._radioLinear.Name = "_radioLinear";
            this._radioLinear.Size = new System.Drawing.Size(50, 17);
            this._radioLinear.TabIndex = 4;
            this._radioLinear.TabStop = true;
            this._radioLinear.Text = "linear";
            this._tooltip.SetToolTip(this._radioLinear, "Linear movement");
            this._radioLinear.UseVisualStyleBackColor = true;
            this._radioLinear.CheckedChanged += new System.EventHandler(this._radioLinear_CheckedChanged);
            // 
            // _labelMovement
            // 
            this._labelMovement.AutoSize = true;
            this._labelMovement.Location = new System.Drawing.Point(159, 19);
            this._labelMovement.Name = "_labelMovement";
            this._labelMovement.Size = new System.Drawing.Size(65, 13);
            this._labelMovement.TabIndex = 11;
            this._labelMovement.Text = "Movement";
            // 
            // _panelStyle
            // 
            this._panelStyle.Controls.Add(this._radioFlat);
            this._panelStyle.Controls.Add(this._radioStandard);
            this._panelStyle.Location = new System.Drawing.Point(79, 38);
            this._panelStyle.Name = "_panelStyle";
            this._panelStyle.Size = new System.Drawing.Size(76, 60);
            this._panelStyle.TabIndex = 8;
            // 
            // _radioFlat
            // 
            this._radioFlat.AutoSize = true;
            this._radioFlat.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._radioFlat.Location = new System.Drawing.Point(3, 29);
            this._radioFlat.Name = "_radioFlat";
            this._radioFlat.Size = new System.Drawing.Size(39, 17);
            this._radioFlat.TabIndex = 3;
            this._radioFlat.TabStop = true;
            this._radioFlat.Text = "flat";
            this._tooltip.SetToolTip(this._radioFlat, "Flat style");
            this._radioFlat.UseVisualStyleBackColor = true;
            // 
            // _radioStandard
            // 
            this._radioStandard.AutoSize = true;
            this._radioStandard.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._radioStandard.Location = new System.Drawing.Point(3, 6);
            this._radioStandard.Name = "_radioStandard";
            this._radioStandard.Size = new System.Drawing.Size(66, 17);
            this._radioStandard.TabIndex = 2;
            this._radioStandard.TabStop = true;
            this._radioStandard.Text = "standard";
            this._tooltip.SetToolTip(this._radioStandard, "Standard style");
            this._radioStandard.UseVisualStyleBackColor = true;
            this._radioStandard.CheckedChanged += new System.EventHandler(this._radioStandard_CheckedChanged);
            // 
            // _labelStyle
            // 
            this._labelStyle.AutoSize = true;
            this._labelStyle.Location = new System.Drawing.Point(76, 19);
            this._labelStyle.Name = "_labelStyle";
            this._labelStyle.Size = new System.Drawing.Size(35, 13);
            this._labelStyle.TabIndex = 10;
            this._labelStyle.Text = "Style";
            // 
            // _colorButton
            // 
            this._colorButton.BackColor = System.Drawing.SystemColors.Control;
            this._colorButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._colorButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._colorButton.ForeColor = System.Drawing.Color.Black;
            this._colorButton.Location = new System.Drawing.Point(6, 65);
            this._colorButton.Name = "_colorButton";
            this._colorButton.Size = new System.Drawing.Size(40, 22);
            this._colorButton.TabIndex = 1;
            this._colorButton.Text = "...";
            this._tooltip.SetToolTip(this._colorButton, "Set up the knob color");
            this._colorButton.UseVisualStyleBackColor = false;
            this._colorButton.Click += new System.EventHandler(this._colorButton_Click);
            // 
            // _knob
            // 
            this._knob.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this._knob.Location = new System.Drawing.Point(6, 19);
            this._knob.Maximum = 255;
            this._knob.Minimum = 0;
            this._knob.Name = "_knob";
            this._knob.PredefinedValues = new int[] {
        0,
        29,
        57,
        85,
        114,
        142,
        170,
        199,
        227,
        255};
            this._knob.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this._knob.Size = new System.Drawing.Size(40, 40);
            this._knob.Step = 1;
            this._knob.TabIndex = 0;
            this._knob.Value = 127;
            // 
            // UserInterfacePage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.Controls.Add(this._knobGroupBox);
            this.MaximumSize = new System.Drawing.Size(750, 350);
            this.MinimumSize = new System.Drawing.Size(750, 350);
            this.Name = "UserInterfacePage";
            this.Size = new System.Drawing.Size(750, 350);
            this._knobGroupBox.ResumeLayout(false);
            this._knobGroupBox.PerformLayout();
            this._panelMovement.ResumeLayout(false);
            this._panelMovement.PerformLayout();
            this._panelStyle.ResumeLayout(false);
            this._panelStyle.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private MidiApp.UIControls.KnobControl _knob;
        private System.Windows.Forms.ColorDialog _colorDialog;
        private System.Windows.Forms.GroupBox _knobGroupBox;
        private System.Windows.Forms.Button _colorButton;
        private System.Windows.Forms.Label _labelMovement;
        private System.Windows.Forms.Label _labelStyle;
        private System.Windows.Forms.RadioButton _radioFlat;
        private System.Windows.Forms.RadioButton _radioStandard;
        private System.Windows.Forms.ToolTip _tooltip;
        private System.Windows.Forms.RadioButton _radioCircular;
        private System.Windows.Forms.RadioButton _radioLinear;
        private System.Windows.Forms.Panel _panelMovement;
        private System.Windows.Forms.Panel _panelStyle;
    }
}
