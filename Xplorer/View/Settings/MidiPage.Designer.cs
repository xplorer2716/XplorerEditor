namespace Xplorer.View.Settings
{
    partial class MidiPage
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
            this.rdMatrix12 = new System.Windows.Forms.RadioButton();
            this.rdXpander = new System.Windows.Forms.RadioButton();
            this.lbSynthType = new System.Windows.Forms.Label();
            this.nmEditPatchNumber = new System.Windows.Forms.NumericUpDown();
            this.lbEditingProgram = new System.Windows.Forms.Label();
            this.lbMIDIChannel = new System.Windows.Forms.Label();
            this.LvAutomation = new MidiApp.UIControls.ListViewEx();
            this.Parameter = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.ControlChange = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.contextMenuStripAutomation = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.exportAsTextFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.nmMIDIChannel = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this._tooltip = new System.Windows.Forms.ToolTip(this.components);
            this.nUpDownDelay = new System.Windows.Forms.NumericUpDown();
            this._synthInputCombobox = new System.Windows.Forms.ComboBox();
            this._automationInputCombobox = new System.Windows.Forms.ComboBox();
            this._synthOutputComboBox = new System.Windows.Forms.ComboBox();
            this._cbSmartAllNotesOff = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.comboControlChange = new System.Windows.Forms.ComboBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this._colorDialog = new System.Windows.Forms.ColorDialog();
            ((System.ComponentModel.ISupportInitialize)(this.nmEditPatchNumber)).BeginInit();
            this.contextMenuStripAutomation.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nmMIDIChannel)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nUpDownDelay)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // rdMatrix12
            // 
            this.rdMatrix12.AutoSize = true;
            this.rdMatrix12.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.rdMatrix12.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.rdMatrix12.Location = new System.Drawing.Point(206, 111);
            this.rdMatrix12.Name = "rdMatrix12";
            this.rdMatrix12.Size = new System.Drawing.Size(68, 17);
            this.rdMatrix12.TabIndex = 8;
            this.rdMatrix12.TabStop = true;
            this.rdMatrix12.Text = "Matrix-12";
            this.rdMatrix12.UseVisualStyleBackColor = true;
            this.rdMatrix12.CheckedChanged += new System.EventHandler(this.rdMatrix12_CheckedChanged);
            // 
            // rdXpander
            // 
            this.rdXpander.AutoSize = true;
            this.rdXpander.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.rdXpander.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.rdXpander.Location = new System.Drawing.Point(128, 111);
            this.rdXpander.Name = "rdXpander";
            this.rdXpander.Size = new System.Drawing.Size(65, 17);
            this.rdXpander.TabIndex = 7;
            this.rdXpander.TabStop = true;
            this.rdXpander.Text = "Xpander";
            this.rdXpander.UseVisualStyleBackColor = true;
            this.rdXpander.Click += new System.EventHandler(this.rdXpander_Click);
            // 
            // lbSynthType
            // 
            this.lbSynthType.AutoSize = true;
            this.lbSynthType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.lbSynthType.ForeColor = System.Drawing.Color.White;
            this.lbSynthType.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.lbSynthType.Location = new System.Drawing.Point(15, 113);
            this.lbSynthType.Name = "lbSynthType";
            this.lbSynthType.Size = new System.Drawing.Size(84, 13);
            this.lbSynthType.TabIndex = 6;
            this.lbSynthType.Text = "Synthesizer type";
            // 
            // nmEditPatchNumber
            // 
            this.nmEditPatchNumber.Location = new System.Drawing.Point(142, 82);
            this.nmEditPatchNumber.Maximum = new decimal(new int[] {
            99,
            0,
            0,
            0});
            this.nmEditPatchNumber.Name = "nmEditPatchNumber";
            this.nmEditPatchNumber.Size = new System.Drawing.Size(51, 20);
            this.nmEditPatchNumber.TabIndex = 3;
            this._tooltip.SetToolTip(this.nmEditPatchNumber, "Patch number used as working buffer");
            this.nmEditPatchNumber.Value = new decimal(new int[] {
            99,
            0,
            0,
            0});
            // 
            // lbEditingProgram
            // 
            this.lbEditingProgram.AutoSize = true;
            this.lbEditingProgram.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.lbEditingProgram.ForeColor = System.Drawing.Color.White;
            this.lbEditingProgram.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.lbEditingProgram.Location = new System.Drawing.Point(15, 86);
            this.lbEditingProgram.Name = "lbEditingProgram";
            this.lbEditingProgram.Size = new System.Drawing.Size(109, 13);
            this.lbEditingProgram.TabIndex = 2;
            this.lbEditingProgram.Text = "Default patch number";
            // 
            // lbMIDIChannel
            // 
            this.lbMIDIChannel.AutoSize = true;
            this.lbMIDIChannel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.lbMIDIChannel.ForeColor = System.Drawing.Color.White;
            this.lbMIDIChannel.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.lbMIDIChannel.Location = new System.Drawing.Point(15, 28);
            this.lbMIDIChannel.Name = "lbMIDIChannel";
            this.lbMIDIChannel.Size = new System.Drawing.Size(106, 13);
            this.lbMIDIChannel.TabIndex = 0;
            this.lbMIDIChannel.Text = "Output MIDI channel";
            // 
            // LvAutomation
            // 
            this.LvAutomation.AllowColumnReorder = true;
            this.LvAutomation.AutoArrange = false;
            this.LvAutomation.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Parameter,
            this.ControlChange});
            this.LvAutomation.ContextMenuStrip = this.contextMenuStripAutomation;
            this.LvAutomation.DoubleClickActivation = false;
            this.LvAutomation.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.LvAutomation.FullRowSelect = true;
            this.LvAutomation.GridLines = true;
            this.LvAutomation.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.LvAutomation.LabelWrap = false;
            this.LvAutomation.Location = new System.Drawing.Point(9, 19);
            this.LvAutomation.MultiSelect = false;
            this.LvAutomation.Name = "LvAutomation";
            this.LvAutomation.ShowGroups = false;
            this.LvAutomation.Size = new System.Drawing.Size(724, 165);
            this.LvAutomation.TabIndex = 0;
            this.LvAutomation.UseCompatibleStateImageBehavior = false;
            this.LvAutomation.View = System.Windows.Forms.View.Details;
            this.LvAutomation.SubItemClicked += new MidiApp.UIControls.SubItemEventHandler(this.LvAutomation_SubItemClicked);
            // 
            // Parameter
            // 
            this.Parameter.Text = "Parameter";
            this.Parameter.Width = 217;
            // 
            // ControlChange
            // 
            this.ControlChange.Text = "Control change number";
            this.ControlChange.Width = 505;
            // 
            // contextMenuStripAutomation
            // 
            this.contextMenuStripAutomation.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exportAsTextFileToolStripMenuItem});
            this.contextMenuStripAutomation.Name = "contextMenuStripAutomation";
            this.contextMenuStripAutomation.Size = new System.Drawing.Size(186, 26);
            // 
            // exportAsTextFileToolStripMenuItem
            // 
            this.exportAsTextFileToolStripMenuItem.Name = "exportAsTextFileToolStripMenuItem";
            this.exportAsTextFileToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.exportAsTextFileToolStripMenuItem.Text = "Export as HTML file...";
            this.exportAsTextFileToolStripMenuItem.Click += new System.EventHandler(this.exportAsTextFileToolStripMenuItem_Click);
            // 
            // nmMIDIChannel
            // 
            this.nmMIDIChannel.Location = new System.Drawing.Point(142, 24);
            this.nmMIDIChannel.Maximum = new decimal(new int[] {
            16,
            0,
            0,
            0});
            this.nmMIDIChannel.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nmMIDIChannel.Name = "nmMIDIChannel";
            this.nmMIDIChannel.Size = new System.Drawing.Size(51, 20);
            this.nmMIDIChannel.TabIndex = 1;
            this._tooltip.SetToolTip(this.nmMIDIChannel, "Synthesizer MIDI Channel (MASTER PAGE/MIDI/Channel)");
            this.nmMIDIChannel.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.label2.Location = new System.Drawing.Point(15, 57);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(86, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "SysEx delay (ms)";
            // 
            // _tooltip
            // 
            this._tooltip.AutoPopDelay = 10000;
            this._tooltip.InitialDelay = 500;
            this._tooltip.ReshowDelay = 100;
            // 
            // nUpDownDelay
            // 
            this.nUpDownDelay.Location = new System.Drawing.Point(142, 53);
            this.nUpDownDelay.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.nUpDownDelay.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nUpDownDelay.Name = "nUpDownDelay";
            this.nUpDownDelay.Size = new System.Drawing.Size(51, 20);
            this.nUpDownDelay.TabIndex = 5;
            this._tooltip.SetToolTip(this.nUpDownDelay, "Delay between each SysEx messages (Try to increase the value if communication wit" +
        "h the synth misbehave. Value lower than 10 ms may hang up the synth).");
            this.nUpDownDelay.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // _synthInputCombobox
            // 
            this._synthInputCombobox.AccessibleDescription = "Synthesizer MIDI IN";
            this._synthInputCombobox.AccessibleName = "Synthesizer MIDI IN";
            this._synthInputCombobox.AccessibleRole = System.Windows.Forms.AccessibleRole.DropList;
            this._synthInputCombobox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._synthInputCombobox.FormattingEnabled = true;
            this._synthInputCombobox.Location = new System.Drawing.Point(163, 82);
            this._synthInputCombobox.Name = "_synthInputCombobox";
            this._synthInputCombobox.Size = new System.Drawing.Size(204, 21);
            this._synthInputCombobox.TabIndex = 5;
            this._tooltip.SetToolTip(this._synthInputCombobox, "Input port from synthesizer (connected to the MIDI OUT port of the synth)");
            // 
            // _automationInputCombobox
            // 
            this._automationInputCombobox.AccessibleDescription = "Automation MIDI IN";
            this._automationInputCombobox.AccessibleName = "Automation MIDI IN";
            this._automationInputCombobox.AccessibleRole = System.Windows.Forms.AccessibleRole.DropList;
            this._automationInputCombobox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._automationInputCombobox.FormattingEnabled = true;
            this._automationInputCombobox.Location = new System.Drawing.Point(163, 24);
            this._automationInputCombobox.Name = "_automationInputCombobox";
            this._automationInputCombobox.Size = new System.Drawing.Size(204, 21);
            this._automationInputCombobox.TabIndex = 1;
            this._tooltip.SetToolTip(this._automationInputCombobox, "(optional) External MIDI Controller Input port or DAW Virtual MIDI Input port");
            // 
            // _synthOutputComboBox
            // 
            this._synthOutputComboBox.AccessibleDescription = "Synthesizer MIDI OUT";
            this._synthOutputComboBox.AccessibleName = "MIDI Device Output";
            this._synthOutputComboBox.AccessibleRole = System.Windows.Forms.AccessibleRole.DropList;
            this._synthOutputComboBox.BackColor = System.Drawing.SystemColors.Window;
            this._synthOutputComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._synthOutputComboBox.FormattingEnabled = true;
            this._synthOutputComboBox.Location = new System.Drawing.Point(163, 54);
            this._synthOutputComboBox.Name = "_synthOutputComboBox";
            this._synthOutputComboBox.Size = new System.Drawing.Size(204, 21);
            this._synthOutputComboBox.TabIndex = 3;
            this._tooltip.SetToolTip(this._synthOutputComboBox, "Output port to synthesizer (connected to the MIDI IN port of the synth)");
            // 
            // _cbSmartAllNotesOff
            // 
            this._cbSmartAllNotesOff.AutoSize = true;
            this._cbSmartAllNotesOff.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this._cbSmartAllNotesOff.Location = new System.Drawing.Point(211, 26);
            this._cbSmartAllNotesOff.Name = "_cbSmartAllNotesOff";
            this._cbSmartAllNotesOff.Size = new System.Drawing.Size(120, 17);
            this._cbSmartAllNotesOff.TabIndex = 9;
            this._cbSmartAllNotesOff.Text = "Smart \"all notes off\"";
            this._tooltip.SetToolTip(this._cbSmartAllNotesOff, "Sends an \"All notes off\" message before single patch change to avoid stuck notes");
            this._cbSmartAllNotesOff.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this._cbSmartAllNotesOff);
            this.groupBox2.Controls.Add(this.rdMatrix12);
            this.groupBox2.Controls.Add(this.rdXpander);
            this.groupBox2.Controls.Add(this.lbSynthType);
            this.groupBox2.Controls.Add(this.nmEditPatchNumber);
            this.groupBox2.Controls.Add(this.lbEditingProgram);
            this.groupBox2.Controls.Add(this.lbMIDIChannel);
            this.groupBox2.Controls.Add(this.nmMIDIChannel);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.nUpDownDelay);
            this.groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            this.groupBox2.ForeColor = System.Drawing.Color.White;
            this.groupBox2.Location = new System.Drawing.Point(393, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(348, 140);
            this.groupBox2.TabIndex = 7;
            this.groupBox2.TabStop = false;
            this.groupBox2.Tag = "";
            this.groupBox2.Text = "MIDI communication";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label6.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.label6.Location = new System.Drawing.Point(11, 28);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(129, 13);
            this.label6.TabIndex = 0;
            this.label6.Text = "Controller/DAW input port";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label4.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.label4.Location = new System.Drawing.Point(11, 58);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(127, 13);
            this.label4.TabIndex = 2;
            this.label4.Text = "Output port to synthesizer";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.label5.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.label5.Location = new System.Drawing.Point(11, 86);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(130, 13);
            this.label5.TabIndex = 4;
            this.label5.Text = "Input port from synthesizer";
            // 
            // comboControlChange
            // 
            this.comboControlChange.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboControlChange.Location = new System.Drawing.Point(502, 6);
            this.comboControlChange.Name = "comboControlChange";
            this.comboControlChange.Size = new System.Drawing.Size(121, 21);
            this.comboControlChange.TabIndex = 1;
            this.comboControlChange.Visible = false;
            // 
            // groupBox3
            // 
            this.groupBox3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this._synthInputCombobox);
            this.groupBox3.Controls.Add(this._automationInputCombobox);
            this.groupBox3.Controls.Add(this._synthOutputComboBox);
            this.groupBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            this.groupBox3.ForeColor = System.Drawing.Color.White;
            this.groupBox3.Location = new System.Drawing.Point(0, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(387, 140);
            this.groupBox3.TabIndex = 6;
            this.groupBox3.TabStop = false;
            this.groupBox3.Tag = "";
            this.groupBox3.Text = "MIDI ports";
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.groupBox1.Controls.Add(this.LvAutomation);
            this.groupBox1.Controls.Add(this.comboControlChange);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            this.groupBox1.ForeColor = System.Drawing.Color.White;
            this.groupBox1.Location = new System.Drawing.Point(0, 149);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(741, 190);
            this.groupBox1.TabIndex = 8;
            this.groupBox1.TabStop = false;
            this.groupBox1.Tag = "";
            this.groupBox1.Text = "Automation";
            // 
            // _colorDialog
            // 
            this._colorDialog.AnyColor = true;
            this._colorDialog.FullOpen = true;
            this._colorDialog.SolidColorOnly = true;
            // 
            // MidiPage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.MaximumSize = new System.Drawing.Size(750, 350);
            this.MinimumSize = new System.Drawing.Size(750, 350);
            this.Name = "MidiPage";
            this.Size = new System.Drawing.Size(750, 350);
            ((System.ComponentModel.ISupportInitialize)(this.nmEditPatchNumber)).EndInit();
            this.contextMenuStripAutomation.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nmMIDIChannel)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nUpDownDelay)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RadioButton rdMatrix12;
        private System.Windows.Forms.RadioButton rdXpander;
        private System.Windows.Forms.Label lbSynthType;
        private System.Windows.Forms.NumericUpDown nmEditPatchNumber;
        private System.Windows.Forms.ToolTip _tooltip;
        private System.Windows.Forms.Label lbEditingProgram;
        private System.Windows.Forms.Label lbMIDIChannel;
        private MidiApp.UIControls.ListViewEx LvAutomation;
        private System.Windows.Forms.ColumnHeader Parameter;
        private System.Windows.Forms.ColumnHeader ControlChange;
        private System.Windows.Forms.ContextMenuStrip contextMenuStripAutomation;
        private System.Windows.Forms.ToolStripMenuItem exportAsTextFileToolStripMenuItem;
        private System.Windows.Forms.NumericUpDown nmMIDIChannel;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.NumericUpDown nUpDownDelay;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox comboControlChange;
        private System.Windows.Forms.ComboBox _synthInputCombobox;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ComboBox _automationInputCombobox;
        private System.Windows.Forms.ComboBox _synthOutputComboBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ColorDialog _colorDialog;
        private System.Windows.Forms.CheckBox _cbSmartAllNotesOff;
    }
}
