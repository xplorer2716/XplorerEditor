namespace Xplorer.View
{
    partial class SplashScreenForm
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
			this._lbVersion = new System.Windows.Forms.Label();
			this._lbMainTitle = new System.Windows.Forms.Label();
			this._pictureBoxSplash = new System.Windows.Forms.PictureBox();
			this._labelLoading = new System.Windows.Forms.Label();
			this._timer = new System.Windows.Forms.Timer(this.components);
			((System.ComponentModel.ISupportInitialize)(this._pictureBoxSplash)).BeginInit();
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
			// _pictureBoxSplash
			// 
			this._pictureBoxSplash.Dock = System.Windows.Forms.DockStyle.Left;
			this._pictureBoxSplash.Image = global::Xplorer.Properties.Resources.About;
			this._pictureBoxSplash.Location = new System.Drawing.Point(0, 0);
			this._pictureBoxSplash.Name = "_pictureBoxSplash";
			this._pictureBoxSplash.Size = new System.Drawing.Size(140, 275);
			this._pictureBoxSplash.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this._pictureBoxSplash.TabIndex = 2;
			this._pictureBoxSplash.TabStop = false;
			// 
			// _labelLoading
			// 
			this._labelLoading.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this._labelLoading.Location = new System.Drawing.Point(149, 133);
			this._labelLoading.Name = "_labelLoading";
			this._labelLoading.Size = new System.Drawing.Size(329, 25);
			this._labelLoading.TabIndex = 4;
			this._labelLoading.Text = "Loading...";
			this._labelLoading.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// _timer
			// 
			this._timer.Interval = 500;
			this._timer.Tick += new System.EventHandler(this._timer_Tick);
			// 
			// SplashScreenForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.White;
			this.ClientSize = new System.Drawing.Size(490, 275);
			this.Controls.Add(this._labelLoading);
			this.Controls.Add(this._pictureBoxSplash);
			this.Controls.Add(this._lbMainTitle);
			this.Controls.Add(this._lbVersion);
			this.DoubleBuffered = true;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(490, 300);
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(490, 275);
            // avoid deobfuscators (see reflexil)
            this.Name = "Class104";
			this.ShowIcon = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;			
			((System.ComponentModel.ISupportInitialize)(this._pictureBoxSplash)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label _lbVersion;
        private System.Windows.Forms.Label _lbMainTitle;
        private System.Windows.Forms.PictureBox _pictureBoxSplash;
        private System.Windows.Forms.Label _labelLoading;
        private System.Windows.Forms.Timer _timer;
    }
}
