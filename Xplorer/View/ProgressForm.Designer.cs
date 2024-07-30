namespace Xplorer.View
{
    partial class ProgressForm
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
            this._progressBar = new System.Windows.Forms.ProgressBar();
            this._label = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // _progressBar
            // 
            this._progressBar.Location = new System.Drawing.Point(15, 68);
            this._progressBar.Name = "_progressBar";
            this._progressBar.Size = new System.Drawing.Size(263, 23);
            this._progressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this._progressBar.TabIndex = 0;
            this._progressBar.Value = 50;
            // 
            // _label
            // 
            this._label.AutoEllipsis = true;
            this._label.Location = new System.Drawing.Point(12, 22);
            this._label.MaximumSize = new System.Drawing.Size(268, 43);
            this._label.MinimumSize = new System.Drawing.Size(268, 43);
            this._label.Name = "_label";
            this._label.Size = new System.Drawing.Size(268, 43);
            this._label.TabIndex = 1;
            this._label.Text = "label here";
            this._label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ProgressForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ClientSize = new System.Drawing.Size(290, 119);
            this.ControlBox = false;
            this.Controls.Add(this._label);
            this.Controls.Add(this._progressBar);
            this.DoubleBuffered = true;
            this.ForeColor = System.Drawing.Color.White;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(296, 144);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(296, 144);
            this.Name = "ProgressForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Title here";
            this.TopMost = true;
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label _label;
        private System.Windows.Forms.ProgressBar _progressBar;
    }
}
