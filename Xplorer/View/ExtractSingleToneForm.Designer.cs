namespace Xplorer.View
{
    partial class ExtractSingleToneForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
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
            this.components = new System.ComponentModel.Container();
            this.btCancel = new System.Windows.Forms.Button();
            this.btOK = new System.Windows.Forms.Button();
            this._txtBankFilename = new System.Windows.Forms.TextBox();
            this._errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
            this._ChooseBankButton = new System.Windows.Forms.Button();
            this._txtDestinationFolder = new System.Windows.Forms.TextBox();
            this._ChooseDestinationFolder = new System.Windows.Forms.Button();
            this._lbBankFilename = new System.Windows.Forms.Label();
            this._lbDestinationFolder = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this._errorProvider)).BeginInit();
            this.SuspendLayout();
            // 
            // btCancel
            // 
            this.btCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btCancel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this.btCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btCancel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btCancel.ForeColor = System.Drawing.SystemColors.ControlText;
            this.btCancel.Location = new System.Drawing.Point(301, 110);
            this.btCancel.Name = "btCancel";
            this.btCancel.Size = new System.Drawing.Size(93, 23);
            this.btCancel.TabIndex = 2;
            this.btCancel.Text = "Cancel";
            this.btCancel.UseVisualStyleBackColor = false;
            this.btCancel.Click += new System.EventHandler(this.btCancel_Click);
            // 
            // btOK
            // 
            this.btOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btOK.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this.btOK.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btOK.ForeColor = System.Drawing.SystemColors.ControlText;
            this.btOK.Location = new System.Drawing.Point(202, 110);
            this.btOK.Name = "btOK";
            this.btOK.Size = new System.Drawing.Size(93, 23);
            this.btOK.TabIndex = 1;
            this.btOK.Text = "OK";
            this.btOK.UseVisualStyleBackColor = false;
            this.btOK.Click += new System.EventHandler(this.btOK_Click);
            // 
            // _txtBankFilename
            // 
            this._txtBankFilename.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this._txtBankFilename.Location = new System.Drawing.Point(107, 19);
            this._txtBankFilename.MaxLength = 8;
            this._txtBankFilename.Name = "_txtBankFilename";
            this._txtBankFilename.ReadOnly = true;
            this._txtBankFilename.Size = new System.Drawing.Size(228, 20);
            this._txtBankFilename.TabIndex = 0;
            // 
            // _errorProvider
            // 
            this._errorProvider.ContainerControl = this;
            // 
            // _ChooseBankButton
            // 
            this._ChooseBankButton.AccessibleDescription = "Select bank file name";
            this._ChooseBankButton.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this._ChooseBankButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._ChooseBankButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._ChooseBankButton.ForeColor = System.Drawing.SystemColors.ControlText;
            this._ChooseBankButton.Location = new System.Drawing.Point(341, 19);
            this._ChooseBankButton.Name = "_ChooseBankButton";
            this._ChooseBankButton.Size = new System.Drawing.Size(27, 23);
            this._ChooseBankButton.TabIndex = 3;
            this._ChooseBankButton.Text = "...";
            this._ChooseBankButton.UseVisualStyleBackColor = false;
            this._ChooseBankButton.Click += new System.EventHandler(this._ChooseBankButton_Click);
            // 
            // _txtDestinationFolder
            // 
            this._txtDestinationFolder.AccessibleDescription = "Destination folder";
            this._txtDestinationFolder.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this._txtDestinationFolder.Location = new System.Drawing.Point(107, 51);
            this._txtDestinationFolder.MaxLength = 8;
            this._txtDestinationFolder.Name = "_txtDestinationFolder";
            this._txtDestinationFolder.ReadOnly = true;
            this._txtDestinationFolder.Size = new System.Drawing.Size(228, 20);
            this._txtDestinationFolder.TabIndex = 4;
            // 
            // _ChooseDestinationFolder
            // 
            this._ChooseDestinationFolder.AccessibleDescription = "Select destination folder";
            this._ChooseDestinationFolder.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this._ChooseDestinationFolder.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._ChooseDestinationFolder.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._ChooseDestinationFolder.ForeColor = System.Drawing.SystemColors.ControlText;
            this._ChooseDestinationFolder.Location = new System.Drawing.Point(341, 48);
            this._ChooseDestinationFolder.Name = "_ChooseDestinationFolder";
            this._ChooseDestinationFolder.Size = new System.Drawing.Size(27, 23);
            this._ChooseDestinationFolder.TabIndex = 5;
            this._ChooseDestinationFolder.Text = "...";
            this._ChooseDestinationFolder.UseVisualStyleBackColor = false;
            this._ChooseDestinationFolder.Click += new System.EventHandler(this._ChooseDestinationFolder_Click);
            // 
            // _lbBankFilename
            // 
            this._lbBankFilename.AutoSize = true;
            this._lbBankFilename.Location = new System.Drawing.Point(12, 24);
            this._lbBankFilename.Name = "_lbBankFilename";
            this._lbBankFilename.Size = new System.Drawing.Size(77, 13);
            this._lbBankFilename.TabIndex = 6;
            this._lbBankFilename.Text = "Bank file name";
            // 
            // _lbDestinationFolder
            // 
            this._lbDestinationFolder.AutoSize = true;
            this._lbDestinationFolder.Location = new System.Drawing.Point(12, 53);
            this._lbDestinationFolder.Name = "_lbDestinationFolder";
            this._lbDestinationFolder.Size = new System.Drawing.Size(89, 13);
            this._lbDestinationFolder.TabIndex = 7;
            this._lbDestinationFolder.Text = "Destination folder";
            // 
            // ExtractSingleToneForm
            // 
            this.AcceptButton = this.btOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.CancelButton = this.btCancel;
            this.ClientSize = new System.Drawing.Size(406, 145);
            this.Controls.Add(this._lbDestinationFolder);
            this.Controls.Add(this._lbBankFilename);
            this.Controls.Add(this._ChooseDestinationFolder);
            this.Controls.Add(this._txtDestinationFolder);
            this.Controls.Add(this._ChooseBankButton);
            this.Controls.Add(this._txtBankFilename);
            this.Controls.Add(this.btOK);
            this.Controls.Add(this.btCancel);
            this.DoubleBuffered = true;
            this.ForeColor = System.Drawing.Color.White;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ExtractSingleToneForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Extract single patches from patch bank";
            this.Load += new System.EventHandler(this.ExtractSingleToneForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this._errorProvider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btCancel;
        private System.Windows.Forms.Button btOK;
        private System.Windows.Forms.TextBox _txtBankFilename;
        private System.Windows.Forms.ErrorProvider _errorProvider;
        private System.Windows.Forms.Button _ChooseBankButton;
        private System.Windows.Forms.Button _ChooseDestinationFolder;
        private System.Windows.Forms.TextBox _txtDestinationFolder;
        private System.Windows.Forms.Label _lbDestinationFolder;
        private System.Windows.Forms.Label _lbBankFilename;
    }
}
