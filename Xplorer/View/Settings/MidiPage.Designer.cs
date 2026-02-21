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
            components = new System.ComponentModel.Container();
            rdMatrix12 = new System.Windows.Forms.RadioButton();
            rdXpander = new System.Windows.Forms.RadioButton();
            lbSynthType = new System.Windows.Forms.Label();
            nmEditPatchNumber = new System.Windows.Forms.NumericUpDown();
            lbEditingProgram = new System.Windows.Forms.Label();
            lbMIDIChannel = new System.Windows.Forms.Label();
            LvAutomation = new MidiApp.UIControls.ListViewEx();
            Parameter = new System.Windows.Forms.ColumnHeader();
            ControlChange = new System.Windows.Forms.ColumnHeader();
            contextMenuStripAutomation = new System.Windows.Forms.ContextMenuStrip(components);
            exportAsTextFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            nmMIDIChannel = new System.Windows.Forms.NumericUpDown();
            label2 = new System.Windows.Forms.Label();
            _tooltip = new System.Windows.Forms.ToolTip(components);
            nUpDownDelay = new System.Windows.Forms.NumericUpDown();
            _synthInputCombobox = new System.Windows.Forms.ComboBox();
            _automationInputCombobox = new System.Windows.Forms.ComboBox();
            _synthOutputComboBox = new System.Windows.Forms.ComboBox();
            _cbSmartAllNotesOff = new System.Windows.Forms.CheckBox();
            groupBox2 = new System.Windows.Forms.GroupBox();
            label6 = new System.Windows.Forms.Label();
            label4 = new System.Windows.Forms.Label();
            label5 = new System.Windows.Forms.Label();
            comboControlChange = new System.Windows.Forms.ComboBox();
            groupBox3 = new System.Windows.Forms.GroupBox();
            groupBox1 = new System.Windows.Forms.GroupBox();
            _colorDialog = new System.Windows.Forms.ColorDialog();
            toolStripMenuItemResetControlChanges = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)nmEditPatchNumber).BeginInit();
            contextMenuStripAutomation.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)nmMIDIChannel).BeginInit();
            ((System.ComponentModel.ISupportInitialize)nUpDownDelay).BeginInit();
            groupBox2.SuspendLayout();
            groupBox3.SuspendLayout();
            groupBox1.SuspendLayout();
            SuspendLayout();
            // 
            // rdMatrix12
            // 
            rdMatrix12.AutoSize = true;
            rdMatrix12.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            rdMatrix12.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            rdMatrix12.Location = new System.Drawing.Point(240, 128);
            rdMatrix12.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            rdMatrix12.Name = "rdMatrix12";
            rdMatrix12.Size = new System.Drawing.Size(68, 17);
            rdMatrix12.TabIndex = 8;
            rdMatrix12.TabStop = true;
            rdMatrix12.Text = "Matrix-12";
            rdMatrix12.UseVisualStyleBackColor = true;
            rdMatrix12.CheckedChanged += rdMatrix12_CheckedChanged;
            // 
            // rdXpander
            // 
            rdXpander.AutoSize = true;
            rdXpander.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            rdXpander.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            rdXpander.Location = new System.Drawing.Point(149, 128);
            rdXpander.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            rdXpander.Name = "rdXpander";
            rdXpander.Size = new System.Drawing.Size(65, 17);
            rdXpander.TabIndex = 7;
            rdXpander.TabStop = true;
            rdXpander.Text = "Xpander";
            rdXpander.UseVisualStyleBackColor = true;
            rdXpander.Click += rdXpander_Click;
            // 
            // lbSynthType
            // 
            lbSynthType.AutoSize = true;
            lbSynthType.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            lbSynthType.ForeColor = System.Drawing.Color.White;
            lbSynthType.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            lbSynthType.Location = new System.Drawing.Point(18, 130);
            lbSynthType.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            lbSynthType.Name = "lbSynthType";
            lbSynthType.Size = new System.Drawing.Size(84, 13);
            lbSynthType.TabIndex = 6;
            lbSynthType.Text = "Synthesizer type";
            // 
            // nmEditPatchNumber
            // 
            nmEditPatchNumber.Location = new System.Drawing.Point(166, 95);
            nmEditPatchNumber.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            nmEditPatchNumber.Maximum = new decimal(new int[] { 99, 0, 0, 0 });
            nmEditPatchNumber.Name = "nmEditPatchNumber";
            nmEditPatchNumber.Size = new System.Drawing.Size(59, 20);
            nmEditPatchNumber.TabIndex = 3;
            _tooltip.SetToolTip(nmEditPatchNumber, "Patch number used as working buffer");
            nmEditPatchNumber.Value = new decimal(new int[] { 99, 0, 0, 0 });
            // 
            // lbEditingProgram
            // 
            lbEditingProgram.AutoSize = true;
            lbEditingProgram.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            lbEditingProgram.ForeColor = System.Drawing.Color.White;
            lbEditingProgram.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            lbEditingProgram.Location = new System.Drawing.Point(18, 99);
            lbEditingProgram.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            lbEditingProgram.Name = "lbEditingProgram";
            lbEditingProgram.Size = new System.Drawing.Size(109, 13);
            lbEditingProgram.TabIndex = 2;
            lbEditingProgram.Text = "Default patch number";
            // 
            // lbMIDIChannel
            // 
            lbMIDIChannel.AutoSize = true;
            lbMIDIChannel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            lbMIDIChannel.ForeColor = System.Drawing.Color.White;
            lbMIDIChannel.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            lbMIDIChannel.Location = new System.Drawing.Point(18, 32);
            lbMIDIChannel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            lbMIDIChannel.Name = "lbMIDIChannel";
            lbMIDIChannel.Size = new System.Drawing.Size(106, 13);
            lbMIDIChannel.TabIndex = 0;
            lbMIDIChannel.Text = "Output MIDI channel";
            // 
            // LvAutomation
            // 
            LvAutomation.AllowColumnReorder = true;
            LvAutomation.AutoArrange = false;
            LvAutomation.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] { Parameter, ControlChange });
            LvAutomation.ContextMenuStrip = contextMenuStripAutomation;
            LvAutomation.DoubleClickActivation = false;
            LvAutomation.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            LvAutomation.FullRowSelect = true;
            LvAutomation.GridLines = true;
            LvAutomation.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            LvAutomation.LabelWrap = false;
            LvAutomation.Location = new System.Drawing.Point(10, 22);
            LvAutomation.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            LvAutomation.MultiSelect = false;
            LvAutomation.Name = "LvAutomation";
            LvAutomation.ShowGroups = false;
            LvAutomation.Size = new System.Drawing.Size(844, 190);
            LvAutomation.TabIndex = 0;
            LvAutomation.UseCompatibleStateImageBehavior = false;
            LvAutomation.View = System.Windows.Forms.View.Details;
            LvAutomation.SubItemClicked += LvAutomation_SubItemClicked;
            // 
            // Parameter
            // 
            Parameter.Text = "Parameter";
            Parameter.Width = 217;
            // 
            // ControlChange
            // 
            ControlChange.Text = "Control change number";
            ControlChange.Width = 505;
            // 
            // contextMenuStripAutomation
            // 
            contextMenuStripAutomation.Items.AddRange(new System.Windows.Forms.ToolStripItem[] { exportAsTextFileToolStripMenuItem, toolStripMenuItemResetControlChanges });
            contextMenuStripAutomation.Name = "contextMenuStripAutomation";
            contextMenuStripAutomation.Size = new System.Drawing.Size(197, 70);
            // 
            // exportAsTextFileToolStripMenuItem
            // 
            exportAsTextFileToolStripMenuItem.Name = "exportAsTextFileToolStripMenuItem";
            exportAsTextFileToolStripMenuItem.Size = new System.Drawing.Size(196, 22);
            exportAsTextFileToolStripMenuItem.Text = "Export as HTML file...";
            exportAsTextFileToolStripMenuItem.Click += exportAsTextFileToolStripMenuItem_Click;
            // 
            // nmMIDIChannel
            // 
            nmMIDIChannel.Location = new System.Drawing.Point(166, 28);
            nmMIDIChannel.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            nmMIDIChannel.Maximum = new decimal(new int[] { 16, 0, 0, 0 });
            nmMIDIChannel.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
            nmMIDIChannel.Name = "nmMIDIChannel";
            nmMIDIChannel.Size = new System.Drawing.Size(59, 20);
            nmMIDIChannel.TabIndex = 1;
            _tooltip.SetToolTip(nmMIDIChannel, "Synthesizer MIDI Channel (MASTER PAGE/MIDI/Channel)");
            nmMIDIChannel.Value = new decimal(new int[] { 1, 0, 0, 0 });
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            label2.ForeColor = System.Drawing.Color.White;
            label2.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            label2.Location = new System.Drawing.Point(18, 66);
            label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(86, 13);
            label2.TabIndex = 4;
            label2.Text = "SysEx delay (ms)";
            // 
            // _tooltip
            // 
            _tooltip.AutoPopDelay = 10000;
            _tooltip.InitialDelay = 500;
            _tooltip.ReshowDelay = 100;
            // 
            // nUpDownDelay
            // 
            nUpDownDelay.Location = new System.Drawing.Point(166, 61);
            nUpDownDelay.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            nUpDownDelay.Maximum = new decimal(new int[] { 1000, 0, 0, 0 });
            nUpDownDelay.Minimum = new decimal(new int[] { 5, 0, 0, 0 });
            nUpDownDelay.Name = "nUpDownDelay";
            nUpDownDelay.Size = new System.Drawing.Size(59, 20);
            nUpDownDelay.TabIndex = 5;
            _tooltip.SetToolTip(nUpDownDelay, "Delay between each SysEx messages (Try to increase the value if communication with the synth misbehave. Value lower than 10 ms may hang up the synth).");
            nUpDownDelay.Value = new decimal(new int[] { 30, 0, 0, 0 });
            // 
            // _synthInputCombobox
            // 
            _synthInputCombobox.AccessibleDescription = "Synthesizer MIDI IN";
            _synthInputCombobox.AccessibleName = "Synthesizer MIDI IN";
            _synthInputCombobox.AccessibleRole = System.Windows.Forms.AccessibleRole.DropList;
            _synthInputCombobox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            _synthInputCombobox.FormattingEnabled = true;
            _synthInputCombobox.Location = new System.Drawing.Point(190, 95);
            _synthInputCombobox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            _synthInputCombobox.Name = "_synthInputCombobox";
            _synthInputCombobox.Size = new System.Drawing.Size(237, 21);
            _synthInputCombobox.TabIndex = 5;
            _tooltip.SetToolTip(_synthInputCombobox, "Input port from synthesizer (connected to the MIDI OUT port of the synth)");
            // 
            // _automationInputCombobox
            // 
            _automationInputCombobox.AccessibleDescription = "Automation MIDI IN";
            _automationInputCombobox.AccessibleName = "Automation MIDI IN";
            _automationInputCombobox.AccessibleRole = System.Windows.Forms.AccessibleRole.DropList;
            _automationInputCombobox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            _automationInputCombobox.FormattingEnabled = true;
            _automationInputCombobox.Location = new System.Drawing.Point(190, 28);
            _automationInputCombobox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            _automationInputCombobox.Name = "_automationInputCombobox";
            _automationInputCombobox.Size = new System.Drawing.Size(237, 21);
            _automationInputCombobox.TabIndex = 1;
            _tooltip.SetToolTip(_automationInputCombobox, "(optional) External MIDI Controller Input port or DAW Virtual MIDI Input port");
            // 
            // _synthOutputComboBox
            // 
            _synthOutputComboBox.AccessibleDescription = "Synthesizer MIDI OUT";
            _synthOutputComboBox.AccessibleName = "MIDI Device Output";
            _synthOutputComboBox.AccessibleRole = System.Windows.Forms.AccessibleRole.DropList;
            _synthOutputComboBox.BackColor = System.Drawing.SystemColors.Window;
            _synthOutputComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            _synthOutputComboBox.FormattingEnabled = true;
            _synthOutputComboBox.Location = new System.Drawing.Point(190, 62);
            _synthOutputComboBox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            _synthOutputComboBox.Name = "_synthOutputComboBox";
            _synthOutputComboBox.Size = new System.Drawing.Size(237, 21);
            _synthOutputComboBox.TabIndex = 3;
            _tooltip.SetToolTip(_synthOutputComboBox, "Output port to synthesizer (connected to the MIDI IN port of the synth)");
            // 
            // _cbSmartAllNotesOff
            // 
            _cbSmartAllNotesOff.AutoSize = true;
            _cbSmartAllNotesOff.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            _cbSmartAllNotesOff.Location = new System.Drawing.Point(246, 30);
            _cbSmartAllNotesOff.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            _cbSmartAllNotesOff.Name = "_cbSmartAllNotesOff";
            _cbSmartAllNotesOff.Size = new System.Drawing.Size(120, 17);
            _cbSmartAllNotesOff.TabIndex = 9;
            _cbSmartAllNotesOff.Text = "Smart \"all notes off\"";
            _tooltip.SetToolTip(_cbSmartAllNotesOff, "Sends an \"All notes off\" message before single patch change to avoid stuck notes");
            _cbSmartAllNotesOff.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            groupBox2.Controls.Add(_cbSmartAllNotesOff);
            groupBox2.Controls.Add(rdMatrix12);
            groupBox2.Controls.Add(rdXpander);
            groupBox2.Controls.Add(lbSynthType);
            groupBox2.Controls.Add(nmEditPatchNumber);
            groupBox2.Controls.Add(lbEditingProgram);
            groupBox2.Controls.Add(lbMIDIChannel);
            groupBox2.Controls.Add(nmMIDIChannel);
            groupBox2.Controls.Add(label2);
            groupBox2.Controls.Add(nUpDownDelay);
            groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            groupBox2.ForeColor = System.Drawing.Color.White;
            groupBox2.Location = new System.Drawing.Point(458, 3);
            groupBox2.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            groupBox2.Name = "groupBox2";
            groupBox2.Padding = new System.Windows.Forms.Padding(4, 3, 4, 3);
            groupBox2.Size = new System.Drawing.Size(406, 162);
            groupBox2.TabIndex = 7;
            groupBox2.TabStop = false;
            groupBox2.Tag = "";
            groupBox2.Text = "MIDI communication";
            // 
            // label6
            // 
            label6.AutoSize = true;
            label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            label6.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            label6.Location = new System.Drawing.Point(13, 32);
            label6.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            label6.Name = "label6";
            label6.Size = new System.Drawing.Size(129, 13);
            label6.TabIndex = 0;
            label6.Text = "Controller/DAW input port";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            label4.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            label4.Location = new System.Drawing.Point(13, 67);
            label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(127, 13);
            label4.TabIndex = 2;
            label4.Text = "Output port to synthesizer";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            label5.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            label5.Location = new System.Drawing.Point(13, 99);
            label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            label5.Name = "label5";
            label5.Size = new System.Drawing.Size(130, 13);
            label5.TabIndex = 4;
            label5.Text = "Input port from synthesizer";
            // 
            // comboControlChange
            // 
            comboControlChange.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            comboControlChange.Location = new System.Drawing.Point(586, 7);
            comboControlChange.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            comboControlChange.Name = "comboControlChange";
            comboControlChange.Size = new System.Drawing.Size(140, 21);
            comboControlChange.TabIndex = 1;
            comboControlChange.Visible = false;
            // 
            // groupBox3
            // 
            groupBox3.BackColor = System.Drawing.Color.FromArgb(54, 54, 62);
            groupBox3.Controls.Add(label6);
            groupBox3.Controls.Add(label5);
            groupBox3.Controls.Add(label4);
            groupBox3.Controls.Add(_synthInputCombobox);
            groupBox3.Controls.Add(_automationInputCombobox);
            groupBox3.Controls.Add(_synthOutputComboBox);
            groupBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            groupBox3.ForeColor = System.Drawing.Color.White;
            groupBox3.Location = new System.Drawing.Point(0, 3);
            groupBox3.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            groupBox3.Name = "groupBox3";
            groupBox3.Padding = new System.Windows.Forms.Padding(4, 3, 4, 3);
            groupBox3.Size = new System.Drawing.Size(451, 162);
            groupBox3.TabIndex = 6;
            groupBox3.TabStop = false;
            groupBox3.Tag = "";
            groupBox3.Text = "MIDI ports";
            // 
            // groupBox1
            // 
            groupBox1.BackColor = System.Drawing.Color.FromArgb(54, 54, 62);
            groupBox1.Controls.Add(LvAutomation);
            groupBox1.Controls.Add(comboControlChange);
            groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold);
            groupBox1.ForeColor = System.Drawing.Color.White;
            groupBox1.Location = new System.Drawing.Point(0, 172);
            groupBox1.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            groupBox1.Name = "groupBox1";
            groupBox1.Padding = new System.Windows.Forms.Padding(4, 3, 4, 3);
            groupBox1.Size = new System.Drawing.Size(864, 219);
            groupBox1.TabIndex = 8;
            groupBox1.TabStop = false;
            groupBox1.Tag = "";
            groupBox1.Text = "Automation";
            // 
            // _colorDialog
            // 
            _colorDialog.AnyColor = true;
            _colorDialog.FullOpen = true;
            _colorDialog.SolidColorOnly = true;
            // 
            // toolStripMenuItemResetControlChanges
            // 
            toolStripMenuItemResetControlChanges.Name = "toolStripMenuItemResetControlChanges";
            toolStripMenuItemResetControlChanges.Size = new System.Drawing.Size(196, 22);
            toolStripMenuItemResetControlChanges.Text = "Reset all MIDI mapping";
            toolStripMenuItemResetControlChanges.Click += toolStripMenuItemResetControlChanges_Click;
            // 
            // MidiPage
            // 
            AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            BackColor = System.Drawing.Color.FromArgb(54, 54, 62);
            Controls.Add(groupBox3);
            Controls.Add(groupBox2);
            Controls.Add(groupBox1);
            Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            MaximumSize = new System.Drawing.Size(875, 404);
            MinimumSize = new System.Drawing.Size(875, 404);
            Name = "MidiPage";
            Size = new System.Drawing.Size(875, 404);
            ((System.ComponentModel.ISupportInitialize)nmEditPatchNumber).EndInit();
            contextMenuStripAutomation.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)nmMIDIChannel).EndInit();
            ((System.ComponentModel.ISupportInitialize)nUpDownDelay).EndInit();
            groupBox2.ResumeLayout(false);
            groupBox2.PerformLayout();
            groupBox3.ResumeLayout(false);
            groupBox3.PerformLayout();
            groupBox1.ResumeLayout(false);
            ResumeLayout(false);

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
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItemResetControlChanges;
    }
}
