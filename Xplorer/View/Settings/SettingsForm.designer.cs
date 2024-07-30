namespace Xplorer.View.Settings
{
    partial class SettingsForm
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
            if(disposing && (components != null))
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SettingsForm));
            this._okButton = new System.Windows.Forms.Button();
            this._cancelButton = new System.Windows.Forms.Button();
            this.contextMenuStripAutomation = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.exportAsTextFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this._tooltip = new System.Windows.Forms.ToolTip(this.components);
            this._tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this._midiPageButton = new System.Windows.Forms.RadioButton();
            this._UIPageButton = new System.Windows.Forms.RadioButton();
            this._randomizerPageButton = new System.Windows.Forms.RadioButton();
            this.contextMenuStripAutomation.SuspendLayout();
            this.SuspendLayout();
            // 
            // _okButton
            // 
            resources.ApplyResources(this._okButton, "_okButton");
            this._okButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this._okButton.Name = "_okButton";
            this._okButton.UseVisualStyleBackColor = false;
            this._okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // _cancelButton
            // 
            resources.ApplyResources(this._cancelButton, "_cancelButton");
            this._cancelButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(203)))), ((int)(((byte)(203)))), ((int)(((byte)(203)))));
            this._cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this._cancelButton.Name = "_cancelButton";
            this._cancelButton.UseVisualStyleBackColor = false;
            this._cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // contextMenuStripAutomation
            // 
            this.contextMenuStripAutomation.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exportAsTextFileToolStripMenuItem});
            this.contextMenuStripAutomation.Name = "contextMenuStripAutomation";
            resources.ApplyResources(this.contextMenuStripAutomation, "contextMenuStripAutomation");
            // 
            // exportAsTextFileToolStripMenuItem
            // 
            this.exportAsTextFileToolStripMenuItem.Name = "exportAsTextFileToolStripMenuItem";
            resources.ApplyResources(this.exportAsTextFileToolStripMenuItem, "exportAsTextFileToolStripMenuItem");
            // 
            // _tooltip
            // 
            this._tooltip.AutoPopDelay = 10000;
            this._tooltip.InitialDelay = 500;
            this._tooltip.ReshowDelay = 100;
            // 
            // _tableLayoutPanel
            // 
            resources.ApplyResources(this._tableLayoutPanel, "_tableLayoutPanel");
            this._tableLayoutPanel.Name = "_tableLayoutPanel";
            // 
            // _midiPageButton
            // 
            resources.ApplyResources(this._midiPageButton, "_midiPageButton");
            this._midiPageButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this._midiPageButton.Checked = true;
            this._midiPageButton.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this._midiPageButton.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._midiPageButton.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._midiPageButton.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._midiPageButton.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this._midiPageButton.Name = "_midiPageButton";
            this._midiPageButton.TabStop = true;
            this._midiPageButton.Tag = "";
            this._midiPageButton.UseVisualStyleBackColor = false;
            this._midiPageButton.CheckedChanged += new System.EventHandler(this._MidiPageButton_CheckedChanged);
            // 
            // _UIPageButton
            // 
            resources.ApplyResources(this._UIPageButton, "_UIPageButton");
            this._UIPageButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this._UIPageButton.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this._UIPageButton.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._UIPageButton.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._UIPageButton.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._UIPageButton.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this._UIPageButton.Name = "_UIPageButton";
            this._UIPageButton.Tag = "";
            this._UIPageButton.UseVisualStyleBackColor = false;
            this._UIPageButton.CheckedChanged += new System.EventHandler(this._UIPageButton_CheckedChanged);
            // 
            // _randomizerPageButton
            // 
            resources.ApplyResources(this._randomizerPageButton, "_randomizerPageButton");
            this._randomizerPageButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this._randomizerPageButton.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this._randomizerPageButton.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._randomizerPageButton.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._randomizerPageButton.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this._randomizerPageButton.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this._randomizerPageButton.Name = "_randomizerPageButton";
            this._randomizerPageButton.Tag = "";
            this._randomizerPageButton.UseVisualStyleBackColor = false;
            this._randomizerPageButton.CheckedChanged += new System.EventHandler(this._randomizerPageButton_CheckedChanged);
            // 
            // SettingsForm
            // 
            this.AcceptButton = this._okButton;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.CancelButton = this._cancelButton;
            this.Controls.Add(this._randomizerPageButton);
            this.Controls.Add(this._cancelButton);
            this.Controls.Add(this._UIPageButton);
            this.Controls.Add(this._midiPageButton);
            this.Controls.Add(this._tableLayoutPanel);
            this.Controls.Add(this._okButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SettingsForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.contextMenuStripAutomation.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button _okButton;
        private System.Windows.Forms.Button _cancelButton;
        private System.Windows.Forms.ContextMenuStrip contextMenuStripAutomation;
        private System.Windows.Forms.ToolStripMenuItem exportAsTextFileToolStripMenuItem;
        private System.Windows.Forms.ToolTip _tooltip;
        private System.Windows.Forms.TableLayoutPanel _tableLayoutPanel;
        private System.Windows.Forms.RadioButton _midiPageButton;
        private System.Windows.Forms.RadioButton _UIPageButton;
        private System.Windows.Forms.RadioButton _randomizerPageButton;
    }
}
