namespace Xplorer.View
{
    partial class RenamePatchForm
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
            this.tbxPatchName = new System.Windows.Forms.TextBox();
            this.tbxPatchNameErrorProvider = new System.Windows.Forms.ErrorProvider(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.tbxPatchNameErrorProvider)).BeginInit();
            this.SuspendLayout();
            // 
            // btCancel
            // 
            this.btCancel.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.btCancel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this.btCancel.CausesValidation = false;
            this.btCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btCancel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btCancel.ForeColor = System.Drawing.SystemColors.ControlText;
            this.btCancel.Location = new System.Drawing.Point(177, 51);
            this.btCancel.Name = "btCancel";
            this.btCancel.Size = new System.Drawing.Size(75, 23);
            this.btCancel.TabIndex = 2;
            this.btCancel.Text = "Cancel";
            this.btCancel.UseVisualStyleBackColor = false;
            this.btCancel.Click += new System.EventHandler(this.btCancel_Click);
            // 
            // btOK
            // 
            this.btOK.Anchor = System.Windows.Forms.AnchorStyles.Right;
            this.btOK.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this.btOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btOK.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btOK.ForeColor = System.Drawing.SystemColors.ControlText;
            this.btOK.Location = new System.Drawing.Point(96, 51);
            this.btOK.Name = "btOK";
            this.btOK.Size = new System.Drawing.Size(75, 23);
            this.btOK.TabIndex = 1;
            this.btOK.Text = "OK";
            this.btOK.UseVisualStyleBackColor = false;
            this.btOK.Click += new System.EventHandler(this.btOK_Click);
            // 
            // tbxPatchName
            // 
            this.tbxPatchName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbxPatchName.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.tbxPatchName.Location = new System.Drawing.Point(12, 12);
            this.tbxPatchName.MaxLength = 8;
            this.tbxPatchName.Name = "tbxPatchName";
            this.tbxPatchName.Size = new System.Drawing.Size(228, 20);
            this.tbxPatchName.TabIndex = 0;
            this.tbxPatchName.Validating += new System.ComponentModel.CancelEventHandler(this.tbxPatchName_Validating);
            // 
            // tbxPatchNameErrorProvider
            // 
            this.tbxPatchNameErrorProvider.ContainerControl = this;
            // 
            // RenamePatchForm
            // 
            this.AcceptButton = this.btOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.CancelButton = this.btCancel;
            this.ClientSize = new System.Drawing.Size(264, 86);
            this.Controls.Add(this.tbxPatchName);
            this.Controls.Add(this.btOK);
            this.Controls.Add(this.btCancel);
            this.ForeColor = System.Drawing.Color.White;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "RenamePatchForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Patch name";
            this.Load += new System.EventHandler(this.RenamePatchFor_Load);
            ((System.ComponentModel.ISupportInitialize)(this.tbxPatchNameErrorProvider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btCancel;
        private System.Windows.Forms.Button btOK;
        private System.Windows.Forms.TextBox tbxPatchName;
        private System.Windows.Forms.ErrorProvider tbxPatchNameErrorProvider;
    }
}
