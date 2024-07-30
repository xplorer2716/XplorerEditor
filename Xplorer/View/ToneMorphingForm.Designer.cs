using System.IO;
namespace Xplorer.View
{
    partial class ToneMorphingForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        private bool _disposed=false;
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <remarks> see "http://reedcopsey.com/2009/03/30/idisposable-part-2-subclass-from-an-idisposable-class"</remarks> 
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();

                if (!this._disposed)
                {
                    _disposed = true;
                    if (File.Exists(_toneATemporaryFile))
                    {
                        try { File.Delete(_toneATemporaryFile); }
                        catch { }
                    }
                    if (File.Exists(_toneBTemporaryFile))
                    {
                        try { File.Delete(_toneBTemporaryFile); }
                        catch { }
                    }
                }
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this._groupBoxPatchA = new System.Windows.Forms.GroupBox();
            this._buttonAChooseFile = new System.Windows.Forms.Button();
            this._textBoxAFilename = new System.Windows.Forms.TextBox();
            this._comboAPatchNumber = new System.Windows.Forms.ComboBox();
            this._radioAFromFile = new System.Windows.Forms.RadioButton();
            this._radioAFromSynth = new System.Windows.Forms.RadioButton();
            this._buttonOK = new System.Windows.Forms.Button();
            this._buttonCancel = new System.Windows.Forms.Button();
            this._groupBoxPatchB = new System.Windows.Forms.GroupBox();
            this._buttonBChooseFile = new System.Windows.Forms.Button();
            this._textBoxBFilename = new System.Windows.Forms.TextBox();
            this._comboBPatchNumber = new System.Windows.Forms.ComboBox();
            this._radioBFromFile = new System.Windows.Forms.RadioButton();
            this._radioBFromSynth = new System.Windows.Forms.RadioButton();
            this._buttonAPreview = new System.Windows.Forms.Button();
            this._buttonBPreview = new System.Windows.Forms.Button();
            this._labelA = new System.Windows.Forms.Label();
            this._labelB = new System.Windows.Forms.Label();
            this._buttonMorphPreview = new System.Windows.Forms.Button();
            this._slider = new System.Windows.Forms.TrackBar();
            this._groupBoxPatchA.SuspendLayout();
            this._groupBoxPatchB.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this._slider)).BeginInit();
            this.SuspendLayout();
            // 
            // _groupBoxPatchA
            // 
            this._groupBoxPatchA.Controls.Add(this._buttonAChooseFile);
            this._groupBoxPatchA.Controls.Add(this._textBoxAFilename);
            this._groupBoxPatchA.Controls.Add(this._comboAPatchNumber);
            this._groupBoxPatchA.Controls.Add(this._radioAFromFile);
            this._groupBoxPatchA.Controls.Add(this._radioAFromSynth);
            this._groupBoxPatchA.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._groupBoxPatchA.ForeColor = System.Drawing.Color.White;
            this._groupBoxPatchA.Location = new System.Drawing.Point(12, 12);
            this._groupBoxPatchA.Name = "_groupBoxPatchA";
            this._groupBoxPatchA.Size = new System.Drawing.Size(310, 83);
            this._groupBoxPatchA.TabIndex = 0;
            this._groupBoxPatchA.TabStop = false;
            this._groupBoxPatchA.Text = "Patch A:";
            // 
            // _buttonAChooseFile
            // 
            this._buttonAChooseFile.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._buttonAChooseFile.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._buttonAChooseFile.ForeColor = System.Drawing.SystemColors.ControlText;
            this._buttonAChooseFile.Location = new System.Drawing.Point(268, 44);
            this._buttonAChooseFile.Name = "_buttonAChooseFile";
            this._buttonAChooseFile.Size = new System.Drawing.Size(36, 23);
            this._buttonAChooseFile.TabIndex = 4;
            this._buttonAChooseFile.Text = "...";
            this._buttonAChooseFile.UseVisualStyleBackColor = false;
            this._buttonAChooseFile.Click += new System.EventHandler(this._buttonAChooseFile_Click);
            // 
            // _textBoxAFilename
            // 
            this._textBoxAFilename.Enabled = false;
            this._textBoxAFilename.Location = new System.Drawing.Point(118, 46);
            this._textBoxAFilename.Name = "_textBoxAFilename";
            this._textBoxAFilename.Size = new System.Drawing.Size(147, 20);
            this._textBoxAFilename.TabIndex = 3;
            // 
            // _comboAPatchNumber
            // 
            this._comboAPatchNumber.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._comboAPatchNumber.FormattingEnabled = true;
            this._comboAPatchNumber.Location = new System.Drawing.Point(118, 19);
            this._comboAPatchNumber.Name = "_comboAPatchNumber";
            this._comboAPatchNumber.Size = new System.Drawing.Size(147, 21);
            this._comboAPatchNumber.TabIndex = 2;
            this._comboAPatchNumber.SelectedIndexChanged += new System.EventHandler(this._comboAPatchNumber_SelectedIndexChanged);
            // 
            // _radioAFromFile
            // 
            this._radioAFromFile.AutoSize = true;
            this._radioAFromFile.ForeColor = System.Drawing.Color.White;
            this._radioAFromFile.Location = new System.Drawing.Point(27, 46);
            this._radioAFromFile.Name = "_radioAFromFile";
            this._radioAFromFile.Size = new System.Drawing.Size(67, 17);
            this._radioAFromFile.TabIndex = 1;
            this._radioAFromFile.TabStop = true;
            this._radioAFromFile.Text = "From file:";
            this._radioAFromFile.UseVisualStyleBackColor = true;
            this._radioAFromFile.CheckedChanged += new System.EventHandler(this._radioAFromFile_CheckedChanged);
            // 
            // _radioAFromSynth
            // 
            this._radioAFromSynth.AutoSize = true;
            this._radioAFromSynth.ForeColor = System.Drawing.Color.White;
            this._radioAFromSynth.Location = new System.Drawing.Point(27, 20);
            this._radioAFromSynth.Name = "_radioAFromSynth";
            this._radioAFromSynth.Size = new System.Drawing.Size(79, 17);
            this._radioAFromSynth.TabIndex = 0;
            this._radioAFromSynth.TabStop = true;
            this._radioAFromSynth.Text = "From synth:";
            this._radioAFromSynth.UseVisualStyleBackColor = true;
            this._radioAFromSynth.CheckedChanged += new System.EventHandler(this._radioAFromSynth_CheckedChanged);
            // 
            // _buttonOK
            // 
            this._buttonOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this._buttonOK.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this._buttonOK.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._buttonOK.Location = new System.Drawing.Point(482, 279);
            this._buttonOK.Name = "_buttonOK";
            this._buttonOK.Size = new System.Drawing.Size(75, 23);
            this._buttonOK.TabIndex = 1;
            this._buttonOK.Text = "OK";
            this._buttonOK.UseVisualStyleBackColor = false;
            this._buttonOK.Click += new System.EventHandler(this._buttonOK_Click);
            // 
            // _buttonCancel
            // 
            this._buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this._buttonCancel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this._buttonCancel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._buttonCancel.Location = new System.Drawing.Point(563, 279);
            this._buttonCancel.Name = "_buttonCancel";
            this._buttonCancel.Size = new System.Drawing.Size(75, 23);
            this._buttonCancel.TabIndex = 2;
            this._buttonCancel.Text = "Cancel";
            this._buttonCancel.UseVisualStyleBackColor = false;
            this._buttonCancel.Click += new System.EventHandler(this._buttonCancel_Click);
            // 
            // _groupBoxPatchB
            // 
            this._groupBoxPatchB.Controls.Add(this._buttonBChooseFile);
            this._groupBoxPatchB.Controls.Add(this._textBoxBFilename);
            this._groupBoxPatchB.Controls.Add(this._comboBPatchNumber);
            this._groupBoxPatchB.Controls.Add(this._radioBFromFile);
            this._groupBoxPatchB.Controls.Add(this._radioBFromSynth);
            this._groupBoxPatchB.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._groupBoxPatchB.ForeColor = System.Drawing.Color.White;
            this._groupBoxPatchB.Location = new System.Drawing.Point(328, 12);
            this._groupBoxPatchB.Name = "_groupBoxPatchB";
            this._groupBoxPatchB.Size = new System.Drawing.Size(310, 83);
            this._groupBoxPatchB.TabIndex = 3;
            this._groupBoxPatchB.TabStop = false;
            this._groupBoxPatchB.Text = "Patch B:";
            // 
            // _buttonBChooseFile
            // 
            this._buttonBChooseFile.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._buttonBChooseFile.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._buttonBChooseFile.ForeColor = System.Drawing.SystemColors.ControlText;
            this._buttonBChooseFile.Location = new System.Drawing.Point(268, 44);
            this._buttonBChooseFile.Name = "_buttonBChooseFile";
            this._buttonBChooseFile.Size = new System.Drawing.Size(36, 23);
            this._buttonBChooseFile.TabIndex = 4;
            this._buttonBChooseFile.Text = "...";
            this._buttonBChooseFile.UseVisualStyleBackColor = false;
            this._buttonBChooseFile.Click += new System.EventHandler(this._buttonBChooseFile_Click);
            // 
            // _textBoxBFilename
            // 
            this._textBoxBFilename.Enabled = false;
            this._textBoxBFilename.Location = new System.Drawing.Point(118, 46);
            this._textBoxBFilename.Name = "_textBoxBFilename";
            this._textBoxBFilename.Size = new System.Drawing.Size(147, 20);
            this._textBoxBFilename.TabIndex = 3;
            // 
            // _comboBPatchNumber
            // 
            this._comboBPatchNumber.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._comboBPatchNumber.FormattingEnabled = true;
            this._comboBPatchNumber.Location = new System.Drawing.Point(118, 19);
            this._comboBPatchNumber.Name = "_comboBPatchNumber";
            this._comboBPatchNumber.Size = new System.Drawing.Size(147, 21);
            this._comboBPatchNumber.TabIndex = 2;
            this._comboBPatchNumber.SelectedIndexChanged += new System.EventHandler(this._comboBPatchNumber_SelectedIndexChanged);
            // 
            // _radioBFromFile
            // 
            this._radioBFromFile.AutoSize = true;
            this._radioBFromFile.ForeColor = System.Drawing.Color.White;
            this._radioBFromFile.Location = new System.Drawing.Point(27, 46);
            this._radioBFromFile.Name = "_radioBFromFile";
            this._radioBFromFile.Size = new System.Drawing.Size(67, 17);
            this._radioBFromFile.TabIndex = 1;
            this._radioBFromFile.TabStop = true;
            this._radioBFromFile.Text = "From file:";
            this._radioBFromFile.UseVisualStyleBackColor = true;
            this._radioBFromFile.CheckedChanged += new System.EventHandler(this._radioBFromFile_CheckedChanged);
            // 
            // _radioBFromSynth
            // 
            this._radioBFromSynth.AutoSize = true;
            this._radioBFromSynth.ForeColor = System.Drawing.Color.White;
            this._radioBFromSynth.Location = new System.Drawing.Point(27, 20);
            this._radioBFromSynth.Name = "_radioBFromSynth";
            this._radioBFromSynth.Size = new System.Drawing.Size(79, 17);
            this._radioBFromSynth.TabIndex = 0;
            this._radioBFromSynth.TabStop = true;
            this._radioBFromSynth.Text = "From synth:";
            this._radioBFromSynth.UseVisualStyleBackColor = true;
            this._radioBFromSynth.CheckedChanged += new System.EventHandler(this._radioBFromSynth_CheckedChanged);
            // 
            // _buttonAPreview
            // 
            this._buttonAPreview.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._buttonAPreview.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._buttonAPreview.Location = new System.Drawing.Point(12, 101);
            this._buttonAPreview.Name = "_buttonAPreview";
            this._buttonAPreview.Size = new System.Drawing.Size(310, 23);
            this._buttonAPreview.TabIndex = 4;
            this._buttonAPreview.Text = "Preview patch A";
            this._buttonAPreview.UseVisualStyleBackColor = false;
            this._buttonAPreview.Click += new System.EventHandler(this._buttonAPreview_Click);
            // 
            // _buttonBPreview
            // 
            this._buttonBPreview.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._buttonBPreview.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._buttonBPreview.Location = new System.Drawing.Point(328, 101);
            this._buttonBPreview.Name = "_buttonBPreview";
            this._buttonBPreview.Size = new System.Drawing.Size(310, 23);
            this._buttonBPreview.TabIndex = 5;
            this._buttonBPreview.Text = "Preview patch B";
            this._buttonBPreview.UseVisualStyleBackColor = false;
            this._buttonBPreview.Click += new System.EventHandler(this._buttonBPreview_Click);
            // 
            // _labelA
            // 
            this._labelA.AutoSize = true;
            this._labelA.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._labelA.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._labelA.ForeColor = System.Drawing.Color.White;
            this._labelA.Location = new System.Drawing.Point(7, 144);
            this._labelA.Name = "_labelA";
            this._labelA.Size = new System.Drawing.Size(29, 29);
            this._labelA.TabIndex = 7;
            this._labelA.Text = "A";
            // 
            // _labelB
            // 
            this._labelB.AutoSize = true;
            this._labelB.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._labelB.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._labelB.ForeColor = System.Drawing.Color.White;
            this._labelB.Location = new System.Drawing.Point(608, 144);
            this._labelB.Name = "_labelB";
            this._labelB.Size = new System.Drawing.Size(30, 29);
            this._labelB.TabIndex = 8;
            this._labelB.Text = "B";
            // 
            // _buttonMorphPreview
            // 
            this._buttonMorphPreview.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this._buttonMorphPreview.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._buttonMorphPreview.DialogResult = System.Windows.Forms.DialogResult.OK;
            this._buttonMorphPreview.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._buttonMorphPreview.Location = new System.Drawing.Point(482, 189);
            this._buttonMorphPreview.Name = "_buttonMorphPreview";
            this._buttonMorphPreview.Size = new System.Drawing.Size(157, 23);
            this._buttonMorphPreview.TabIndex = 9;
            this._buttonMorphPreview.Text = "Preview";
            this._buttonMorphPreview.UseVisualStyleBackColor = false;
            this._buttonMorphPreview.Click += new System.EventHandler(this._buttonMorphPreview_Click);
            // 
            // _slider
            // 
            this._slider.Location = new System.Drawing.Point(37, 144);
            this._slider.Maximum = 100;
            this._slider.Name = "_slider";
            this._slider.Size = new System.Drawing.Size(565, 45);
            this._slider.TabIndex = 12;
            this._slider.ValueChanged += new System.EventHandler(this._slider_ValueChanged);
            // 
            // ToneMorphingForm
            // 
            this.AcceptButton = this._buttonOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ClientSize = new System.Drawing.Size(648, 313);
            this.Controls.Add(this._slider);
            this.Controls.Add(this._buttonMorphPreview);
            this.Controls.Add(this._labelB);
            this.Controls.Add(this._labelA);
            this.Controls.Add(this._buttonBPreview);
            this.Controls.Add(this._buttonAPreview);
            this.Controls.Add(this._groupBoxPatchB);
            this.Controls.Add(this._buttonCancel);
            this.Controls.Add(this._buttonOK);
            this.Controls.Add(this._groupBoxPatchA);
            this.DoubleBuffered = true;
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(664, 352);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(664, 352);
            this.Name = "ToneMorphingForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Tone morphing";
            this.TopMost = true;
            this._groupBoxPatchA.ResumeLayout(false);
            this._groupBoxPatchA.PerformLayout();
            this._groupBoxPatchB.ResumeLayout(false);
            this._groupBoxPatchB.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this._slider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox _groupBoxPatchA;
        private System.Windows.Forms.Button _buttonAChooseFile;
        private System.Windows.Forms.TextBox _textBoxAFilename;
        private System.Windows.Forms.ComboBox _comboAPatchNumber;
        private System.Windows.Forms.RadioButton _radioAFromFile;
        private System.Windows.Forms.RadioButton _radioAFromSynth;
        private System.Windows.Forms.Button _buttonOK;
        private System.Windows.Forms.Button _buttonCancel;
        private System.Windows.Forms.GroupBox _groupBoxPatchB;
        private System.Windows.Forms.Button _buttonBChooseFile;
        private System.Windows.Forms.TextBox _textBoxBFilename;
        private System.Windows.Forms.ComboBox _comboBPatchNumber;
        private System.Windows.Forms.RadioButton _radioBFromFile;
        private System.Windows.Forms.RadioButton _radioBFromSynth;
        private System.Windows.Forms.Button _buttonAPreview;
        private System.Windows.Forms.Button _buttonBPreview;
        private System.Windows.Forms.Label _labelA;
        private System.Windows.Forms.Label _labelB;
        private System.Windows.Forms.Button _buttonMorphPreview;
        private System.Windows.Forms.TrackBar _slider;
    }
}