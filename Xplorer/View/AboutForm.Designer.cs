namespace Xplorer.View
{
    partial class AboutForm
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
            this._lbVersion = new System.Windows.Forms.Label();
            this._lbMainTitle = new System.Windows.Forms.Label();
            this._pictureBoxAbout = new System.Windows.Forms.PictureBox();
            this._lbCopyright = new System.Windows.Forms.Label();
            this._lbLink = new System.Windows.Forms.LinkLabel();
            this._groupBox = new System.Windows.Forms.GroupBox();
            this._lbNotice = new System.Windows.Forms.Label();
            this._linklicense = new System.Windows.Forms.LinkLabel();
            ((System.ComponentModel.ISupportInitialize)(this._pictureBoxAbout)).BeginInit();
            this.SuspendLayout();
            // 
            // _lbVersion
            // 
            this._lbVersion.AutoSize = true;
            this._lbVersion.Location = new System.Drawing.Point(146, 34);
            this._lbVersion.Name = "_lbVersion";
            this._lbVersion.Size = new System.Drawing.Size(66, 13);
            this._lbVersion.TabIndex = 0;
            this._lbVersion.Text = "Version here";
            // 
            // _lbMainTitle
            // 
            this._lbMainTitle.AutoSize = true;
            this._lbMainTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._lbMainTitle.Location = new System.Drawing.Point(146, 9);
            this._lbMainTitle.Name = "_lbMainTitle";
            this._lbMainTitle.Size = new System.Drawing.Size(47, 13);
            this._lbMainTitle.TabIndex = 1;
            this._lbMainTitle.Text = "Xplorer";
            // 
            // _pictureBoxAbout
            // 
            this._pictureBoxAbout.Dock = System.Windows.Forms.DockStyle.Left;
            this._pictureBoxAbout.Image = global::Xplorer.Properties.Resources.About;
            this._pictureBoxAbout.Location = new System.Drawing.Point(0, 0);
            this._pictureBoxAbout.Name = "_pictureBoxAbout";
            this._pictureBoxAbout.Size = new System.Drawing.Size(140, 261);
            this._pictureBoxAbout.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this._pictureBoxAbout.TabIndex = 2;
            this._pictureBoxAbout.TabStop = false;
            // 
            // _lbCopyright
            // 
            this._lbCopyright.AutoSize = true;
            this._lbCopyright.Location = new System.Drawing.Point(146, 59);
            this._lbCopyright.Name = "_lbCopyright";
            this._lbCopyright.Size = new System.Drawing.Size(207, 13);
            this._lbCopyright.TabIndex = 3;
            this._lbCopyright.Text = "Copyright (c) 2009-2024 by Pascal Schmitt";
            // 
            // _lbLink
            // 
            this._lbLink.AutoSize = true;
            this._lbLink.ForeColor = System.Drawing.Color.Blue;
            this._lbLink.Location = new System.Drawing.Point(146, 79);
            this._lbLink.Name = "_lbLink";
            this._lbLink.Size = new System.Drawing.Size(220, 13);
            this._lbLink.TabIndex = 4;
            this._lbLink.TabStop = true;
            this._lbLink.Text = "https://github.com/xplorer2716/XplorerEditor";
            this._lbLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this._lbLink_LinkClicked);
            // 
            // _groupBox
            // 
            this._groupBox.Location = new System.Drawing.Point(149, 204);
            this._groupBox.Name = "_groupBox";
            this._groupBox.Size = new System.Drawing.Size(316, 5);
            this._groupBox.TabIndex = 5;
            this._groupBox.TabStop = false;
            // 
            // _lbNotice
            // 
            this._lbNotice.ForeColor = System.Drawing.Color.Gray;
            this._lbNotice.Location = new System.Drawing.Point(146, 212);
            this._lbNotice.Name = "_lbNotice";
            this._lbNotice.Size = new System.Drawing.Size(319, 24);
            this._lbNotice.TabIndex = 6;
            this._lbNotice.Text = "This software is released under GNU General Public License v3.0";
            // 
            // _linklicense
            // 
            this._linklicense.AutoSize = true;
            this._linklicense.ForeColor = System.Drawing.Color.Blue;
            this._linklicense.Location = new System.Drawing.Point(149, 231);
            this._linklicense.Name = "_linklicense";
            this._linklicense.Size = new System.Drawing.Size(208, 13);
            this._linklicense.TabIndex = 7;
            this._linklicense.TabStop = true;
            this._linklicense.Text = "https://www.gnu.org/licenses/gpl-3.0.html";
            // 
            // AboutForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(474, 261);
            this.Controls.Add(this._linklicense);
            this.Controls.Add(this._lbNotice);
            this.Controls.Add(this._groupBox);
            this.Controls.Add(this._lbLink);
            this.Controls.Add(this._lbCopyright);
            this.Controls.Add(this._pictureBoxAbout);
            this.Controls.Add(this._lbMainTitle);
            this.Controls.Add(this._lbVersion);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(490, 300);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(490, 300);
            this.Name = "AboutForm";
            this.ShowIcon = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "About";
            this.Click += new System.EventHandler(this.AboutForm_Click);
            ((System.ComponentModel.ISupportInitialize)(this._pictureBoxAbout)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label _lbVersion;
        private System.Windows.Forms.Label _lbMainTitle;
        private System.Windows.Forms.PictureBox _pictureBoxAbout;
        private System.Windows.Forms.Label _lbCopyright;
        private System.Windows.Forms.LinkLabel _lbLink;
        private System.Windows.Forms.GroupBox _groupBox;
        private System.Windows.Forms.Label _lbNotice;
        private System.Windows.Forms.LinkLabel _linklicense;
    }
}
