using Xplorer.Model;
using MidiApp.UIControls;

namespace Xplorer.View
{
    partial class MainForm
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

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.PageContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.toolStripPageMenuItemCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripPageMenuItemPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.labelRetrigMode = new System.Windows.Forms.Label();
            this.lbSampleInput = new System.Windows.Forms.Label();
            this._vfdDisplay = new MidiApp.UIControls.VacuumFluoDisplayControl();
            this._timer = new System.Windows.Forms.Timer(this.components);
            this.lbSOURCE = new System.Windows.Forms.Label();
            this.lbDESTINATION = new System.Windows.Forms.Label();
            this.lbAMOUNT = new System.Windows.Forms.Label();
            this.lbModQuantize = new System.Windows.Forms.Label();
            this.mainFormMenstrip = new System.Windows.Forms.MenuStrip();
            this.FileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.patchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.previousPatchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.nextPatchStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.gotoPatchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.randomizePatchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.renameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.storeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.synchronizeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tuneRequestToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.singlePatchesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.getSingleTonesFromSynthToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.extractSingleTonesFromBankToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.allDataDumpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpXplorerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.checkForNewReleaseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.goToWebsiteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutdeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.btPatchStore = new MidiApp.UIControls.BackgroundImageButton();
            this.btPatchPlus = new MidiApp.UIControls.BackgroundImageButton();
            this.MOD_QUANTIZE_20 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_19 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_18 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.btPatchSave = new MidiApp.UIControls.BackgroundImageButton();
            this.MOD_QUANTIZE_17 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.btPatchLoad = new MidiApp.UIControls.BackgroundImageButton();
            this.btPatchRandom = new MidiApp.UIControls.BackgroundImageButton();
            this.btPatchGoto = new MidiApp.UIControls.BackgroundImageButton();
            this.btPatchMinus = new MidiApp.UIControls.BackgroundImageButton();
            this.btSettings = new MidiApp.UIControls.BackgroundImageButton();
            this.LFO_X_WAVESHAPE = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_QUANTIZE_16 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.LFO_X_SAMPLE_INPUT = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_QUANTIZE_15 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.ENV_X_TRIG_SINGLE_MULTI = new MidiApp.UIControls.RadioButtonPanel();
            this.rdSINGLE = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.rdMULTI = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.MOD_QUANTIZE_14 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_13 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_12 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_11 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.LFO_X_RETRIG_MODE = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_QUANTIZE_10 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_9 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_8 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.LAG_IN = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_QUANTIZE_7 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.PANEL_RAMP = new MidiApp.UIControls.FakePanel();
            this.RAMP_X_TRIG_LFO_SOURCE = new MidiApp.UIControls.ComboBoxValuedControl();
            this.RAMP_X_TRIG_GATED = new MidiApp.UIControls.CheckBoxValuedControl();
            this.RAMP_X_TRIG_LFOTRIG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.RAMP_X_TRIG_EXTRIG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCF_MODE = new MidiApp.UIControls.ComboBoxValuedControl();
            this.LFO_X_LAG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_6 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_5 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.TRACK_X_IN = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_QUANTIZE_4 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_3 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_2 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.PANEL_ENV_X_MODE = new MidiApp.UIControls.FakePanel();
            this.label4 = new System.Windows.Forms.Label();
            this.ENV_X_MODE_DADR = new MidiApp.UIControls.CheckBoxValuedControl();
            this.ENV_X_MODE_FREERUN = new MidiApp.UIControls.CheckBoxValuedControl();
            this.ENV_X_MODE_RESET = new MidiApp.UIControls.CheckBoxValuedControl();
            this.RAMP_X_TRIG_SINGLE_MULTI = new MidiApp.UIControls.RadioButtonPanel();
            this.rdRampXSingle = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.rdRampXMulti = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.Panel_ENV_X_TRIGGER = new MidiApp.UIControls.FakePanel();
            this.ENV_X_TRIG_LFO_SOURCE = new MidiApp.UIControls.ComboBoxValuedControl();
            this.ENV_X_TRIG_GATED = new MidiApp.UIControls.CheckBoxValuedControl();
            this.ENV_X_TRIG_LFOTRIG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.ENV_X_TRIG_EXTRIG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_QUANTIZE_1 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.FM_DESTINATION = new MidiApp.UIControls.RadioButtonPanel();
            this.rdFMDestVCO1 = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.rdFMDestVCF = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.LAG_MODE_LEGATO = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_DEST_20 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_20 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCF_VCA_MOD = new MidiApp.UIControls.FakePanel();
            this.label3 = new System.Windows.Forms.Label();
            this.VCF_MOD_VIB = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCF_MOD_LEV1 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCF_MOD_LAG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCF_MOD_KEYB = new MidiApp.UIControls.CheckBoxValuedControl();
            this.LAG_TIMING_LINEAR_EXPO = new MidiApp.UIControls.RadioButtonPanel();
            this.rdExpo = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.rdLinear = new MidiApp.UIControls.BackgroundImageRadioButton();
            this.MOD_AMNT_SRC_20 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_19 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.LAG_LINEAR_EQUAL_TIME = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO2_MODS = new MidiApp.UIControls.FakePanel();
            this.VCO2_WAVE_SYNC = new MidiApp.UIControls.CheckBoxValuedControl();
            this.label2 = new System.Windows.Forms.Label();
            this.VCO2_MOD_VIB = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO2_MOD_LEV1 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO2_MOD_LAG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO2_MOD_KEYB = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_SRC_19 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_19 = new MidiApp.UIControls.KnobControl();
            this.VCO1_MODS = new MidiApp.UIControls.FakePanel();
            this.label1 = new System.Windows.Forms.Label();
            this.VCO1_MOD_VIB = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO1_MOD_LEV1 = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO1_MOD_LAG = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO1_MOD_KEYB = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO2_WAVESHAPE_PULSE = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_DEST_18 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_18 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_18 = new MidiApp.UIControls.KnobControl();
            this.PanelTRACK = new MidiApp.UIControls.FakePanel();
            this.TRACK_1 = new System.Windows.Forms.RadioButton();
            this.TRACK_2 = new System.Windows.Forms.RadioButton();
            this.TRACK_3 = new System.Windows.Forms.RadioButton();
            this.VCO2_WAVESHAPE_NOISE = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_DEST_17 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.PanelRAMP = new MidiApp.UIControls.FakePanel();
            this.RAMP_1 = new System.Windows.Forms.RadioButton();
            this.RAMP_2 = new System.Windows.Forms.RadioButton();
            this.RAMP_3 = new System.Windows.Forms.RadioButton();
            this.RAMP_4 = new System.Windows.Forms.RadioButton();
            this.MOD_SRC_17 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCO2_WAVESHAPE_SAW = new MidiApp.UIControls.CheckBoxValuedControl();
            this.VCO2_WAVESHAPE_TRI = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_AMNT_SRC_17 = new MidiApp.UIControls.KnobControl();
            this.VCO1_WAVESHAPE_PULSE = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_DEST_16 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_16 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.RAMP_X_RATE = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_15 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_16 = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_15 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.LFO_X_AMP = new MidiApp.UIControls.KnobControl();
            this.LFO_X_RETRIG = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_15 = new MidiApp.UIControls.KnobControl();
            this.LFO_X_SPEED = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_14 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCO1_WAVESHAPE_SAW = new MidiApp.UIControls.CheckBoxValuedControl();
            this.MOD_SRC_14 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_14 = new MidiApp.UIControls.KnobControl();
            this.VCO1_WAVESHAPE_TRI = new MidiApp.UIControls.CheckBoxValuedControl();
            this.FMLAG_RATE = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_13 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_13 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.FM_AMP = new MidiApp.UIControls.KnobControl();
            this.TRACK_X_PT5 = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_13 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_12 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_12 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.TRACK_X_PT4 = new MidiApp.UIControls.KnobControl();
            this.PanelLFO = new MidiApp.UIControls.FakePanel();
            this.LFO_5 = new System.Windows.Forms.RadioButton();
            this.LFO_1 = new System.Windows.Forms.RadioButton();
            this.LFO_4 = new System.Windows.Forms.RadioButton();
            this.LFO_2 = new System.Windows.Forms.RadioButton();
            this.LFO_3 = new System.Windows.Forms.RadioButton();
            this.TRACK_X_PT3 = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_12 = new MidiApp.UIControls.KnobControl();
            this.TRACK_X_PT2 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_11 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.TRACK_X_PT1 = new MidiApp.UIControls.KnobControl();
            this.VCO2_PW = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_11 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_11 = new MidiApp.UIControls.KnobControl();
            this.ENV_X_DELAY = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_10 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCO2_VOLUME = new MidiApp.UIControls.KnobControl();
            this.VCO2_DETUNE = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_10 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.ENV_X_VOLUME = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_10 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_9 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.ENV_X_RELEASE = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_9 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.ENV_X_SUSTAIN = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_9 = new MidiApp.UIControls.KnobControl();
            this.ENV_X_DECAY = new MidiApp.UIControls.KnobControl();
            this.ENV_X_ATTACK = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_8 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_8 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCO2_FREQ = new MidiApp.UIControls.KnobControl();
            this.VCF_VCA2_VOLUME = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_8 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_7 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_7 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCF_VCA1_VOLUME = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_7 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_6 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCF_RES = new MidiApp.UIControls.KnobControl();
            this.PanelENV = new MidiApp.UIControls.FakePanel();
            this.ENV_5 = new System.Windows.Forms.RadioButton();
            this.ENV_1 = new System.Windows.Forms.RadioButton();
            this.ENV_4 = new System.Windows.Forms.RadioButton();
            this.ENV_2 = new System.Windows.Forms.RadioButton();
            this.ENV_3 = new System.Windows.Forms.RadioButton();
            this.VCO1_PW = new MidiApp.UIControls.KnobControl();
            this.VCF_FREQ = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_6 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_6 = new MidiApp.UIControls.KnobControl();
            this.VCO1_FREQ = new MidiApp.UIControls.KnobControl();
            this.VCO1_VOLUME = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_5 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.VCO1_DETUNE = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_5 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_1 = new MidiApp.UIControls.KnobControl();
            this.MOD_AMNT_SRC_5 = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_1 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_DEST_4 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_DEST_1 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_4 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_4 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_3 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_2 = new MidiApp.UIControls.KnobControl();
            this.MOD_SRC_3 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_SRC_2 = new MidiApp.UIControls.ComboBoxValuedControl();
            this.MOD_AMNT_SRC_3 = new MidiApp.UIControls.KnobControl();
            this.MOD_DEST_2 = new MidiApp.UIControls.ComboBoxValuedControl();
            this._toolTip = new System.Windows.Forms.ToolTip(this.components);
            this._ledPanelControl = new MidiApp.UIControls.LedPanelControl();
            this.backupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.restoreToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.PageContextMenuStrip.SuspendLayout();
            this.mainFormMenstrip.SuspendLayout();
            this.ENV_X_TRIG_SINGLE_MULTI.SuspendLayout();
            this.PANEL_RAMP.SuspendLayout();
            this.PANEL_ENV_X_MODE.SuspendLayout();
            this.RAMP_X_TRIG_SINGLE_MULTI.SuspendLayout();
            this.Panel_ENV_X_TRIGGER.SuspendLayout();
            this.FM_DESTINATION.SuspendLayout();
            this.VCF_VCA_MOD.SuspendLayout();
            this.LAG_TIMING_LINEAR_EXPO.SuspendLayout();
            this.VCO2_MODS.SuspendLayout();
            this.VCO1_MODS.SuspendLayout();
            this.PanelTRACK.SuspendLayout();
            this.PanelRAMP.SuspendLayout();
            this.PanelLFO.SuspendLayout();
            this.PanelENV.SuspendLayout();
            this.SuspendLayout();
            // 
            // PageContextMenuStrip
            // 
            this.PageContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripPageMenuItemCopy,
            this.toolStripPageMenuItemPaste});
            this.PageContextMenuStrip.Name = "PageContextMenuStrip";
            resources.ApplyResources(this.PageContextMenuStrip, "PageContextMenuStrip");
            this.PageContextMenuStrip.Opening += new System.ComponentModel.CancelEventHandler(this.PageContextMenuStrip_Opening);
            // 
            // toolStripPageMenuItemCopy
            // 
            this.toolStripPageMenuItemCopy.Name = "toolStripPageMenuItemCopy";
            resources.ApplyResources(this.toolStripPageMenuItemCopy, "toolStripPageMenuItemCopy");
            this.toolStripPageMenuItemCopy.Click += new System.EventHandler(this.toolStripPageMenuItemCopy_Click);
            // 
            // toolStripPageMenuItemPaste
            // 
            this.toolStripPageMenuItemPaste.Name = "toolStripPageMenuItemPaste";
            resources.ApplyResources(this.toolStripPageMenuItemPaste, "toolStripPageMenuItemPaste");
            this.toolStripPageMenuItemPaste.Click += new System.EventHandler(this.toolStripPageMenuItemPaste_Click);
            // 
            // labelRetrigMode
            // 
            resources.ApplyResources(this.labelRetrigMode, "labelRetrigMode");
            this.labelRetrigMode.BackColor = System.Drawing.Color.Transparent;
            this.labelRetrigMode.ForeColor = System.Drawing.Color.White;
            this.labelRetrigMode.Name = "labelRetrigMode";
            // 
            // lbSampleInput
            // 
            resources.ApplyResources(this.lbSampleInput, "lbSampleInput");
            this.lbSampleInput.BackColor = System.Drawing.Color.Transparent;
            this.lbSampleInput.ForeColor = System.Drawing.Color.White;
            this.lbSampleInput.Name = "lbSampleInput";
            // 
            // _vfdDisplay
            // 
            resources.ApplyResources(this._vfdDisplay, "_vfdDisplay");
            this._vfdDisplay.BackColor = System.Drawing.Color.Black;
            this._vfdDisplay.Name = "_vfdDisplay";
            // 
            // _timer
            // 
            this._timer.Interval = 30;
            this._timer.Tick += new System.EventHandler(this.UITimer_Tick);
            // 
            // lbSOURCE
            // 
            resources.ApplyResources(this.lbSOURCE, "lbSOURCE");
            this.lbSOURCE.BackColor = System.Drawing.Color.Transparent;
            this.lbSOURCE.ForeColor = System.Drawing.Color.White;
            this.lbSOURCE.Name = "lbSOURCE";
            // 
            // lbDESTINATION
            // 
            resources.ApplyResources(this.lbDESTINATION, "lbDESTINATION");
            this.lbDESTINATION.BackColor = System.Drawing.Color.Transparent;
            this.lbDESTINATION.ForeColor = System.Drawing.Color.White;
            this.lbDESTINATION.Name = "lbDESTINATION";
            // 
            // lbAMOUNT
            // 
            resources.ApplyResources(this.lbAMOUNT, "lbAMOUNT");
            this.lbAMOUNT.BackColor = System.Drawing.Color.Transparent;
            this.lbAMOUNT.ForeColor = System.Drawing.Color.White;
            this.lbAMOUNT.Name = "lbAMOUNT";
            // 
            // lbModQuantize
            // 
            resources.ApplyResources(this.lbModQuantize, "lbModQuantize");
            this.lbModQuantize.BackColor = System.Drawing.Color.Transparent;
            this.lbModQuantize.ForeColor = System.Drawing.Color.White;
            this.lbModQuantize.Name = "lbModQuantize";
            // 
            // mainFormMenstrip
            // 
            this.mainFormMenstrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.FileToolStripMenuItem,
            this.patchToolStripMenuItem,
            this.toolsToolStripMenuItem,
            this.helpToolStripMenuItem});
            resources.ApplyResources(this.mainFormMenstrip, "mainFormMenstrip");
            this.mainFormMenstrip.Name = "mainFormMenstrip";
            // 
            // FileToolStripMenuItem
            // 
            this.FileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.toolStripSeparator,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripSeparator2,
            this.exitToolStripMenuItem});
            this.FileToolStripMenuItem.Name = "FileToolStripMenuItem";
            resources.ApplyResources(this.FileToolStripMenuItem, "FileToolStripMenuItem");
            // 
            // newToolStripMenuItem
            // 
            resources.ApplyResources(this.newToolStripMenuItem, "newToolStripMenuItem");
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            resources.ApplyResources(this.openToolStripMenuItem, "openToolStripMenuItem");
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // toolStripSeparator
            // 
            this.toolStripSeparator.Name = "toolStripSeparator";
            resources.ApplyResources(this.toolStripSeparator, "toolStripSeparator");
            // 
            // saveToolStripMenuItem
            // 
            resources.ApplyResources(this.saveToolStripMenuItem, "saveToolStripMenuItem");
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            resources.ApplyResources(this.saveAsToolStripMenuItem, "saveAsToolStripMenuItem");
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            resources.ApplyResources(this.toolStripSeparator2, "toolStripSeparator2");
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            resources.ApplyResources(this.exitToolStripMenuItem, "exitToolStripMenuItem");
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // patchToolStripMenuItem
            // 
            this.patchToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.previousPatchToolStripMenuItem,
            this.nextPatchStripMenuItem,
            this.gotoPatchToolStripMenuItem,
            this.toolStripSeparator4,
            this.randomizePatchToolStripMenuItem,
            this.renameToolStripMenuItem,
            this.storeToolStripMenuItem,
            this.synchronizeToolStripMenuItem});
            this.patchToolStripMenuItem.Name = "patchToolStripMenuItem";
            resources.ApplyResources(this.patchToolStripMenuItem, "patchToolStripMenuItem");
            // 
            // previousPatchToolStripMenuItem
            // 
            this.previousPatchToolStripMenuItem.Name = "previousPatchToolStripMenuItem";
            resources.ApplyResources(this.previousPatchToolStripMenuItem, "previousPatchToolStripMenuItem");
            this.previousPatchToolStripMenuItem.Click += new System.EventHandler(this.previousPatchToolStripMenuItem_Click);
            // 
            // nextPatchStripMenuItem
            // 
            this.nextPatchStripMenuItem.Name = "nextPatchStripMenuItem";
            resources.ApplyResources(this.nextPatchStripMenuItem, "nextPatchStripMenuItem");
            this.nextPatchStripMenuItem.Click += new System.EventHandler(this.nextPatchStripMenuItem_Click);
            // 
            // gotoPatchToolStripMenuItem
            // 
            this.gotoPatchToolStripMenuItem.Name = "gotoPatchToolStripMenuItem";
            resources.ApplyResources(this.gotoPatchToolStripMenuItem, "gotoPatchToolStripMenuItem");
            this.gotoPatchToolStripMenuItem.Click += new System.EventHandler(this.gotoPatchToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            resources.ApplyResources(this.toolStripSeparator4, "toolStripSeparator4");
            // 
            // randomizePatchToolStripMenuItem
            // 
            this.randomizePatchToolStripMenuItem.Name = "randomizePatchToolStripMenuItem";
            resources.ApplyResources(this.randomizePatchToolStripMenuItem, "randomizePatchToolStripMenuItem");
            this.randomizePatchToolStripMenuItem.Click += new System.EventHandler(this.randomizePatchToolStripMenuItem_Click);
            // 
            // renameToolStripMenuItem
            // 
            this.renameToolStripMenuItem.Name = "renameToolStripMenuItem";
            resources.ApplyResources(this.renameToolStripMenuItem, "renameToolStripMenuItem");
            this.renameToolStripMenuItem.Click += new System.EventHandler(this.renameToolStripMenuItem_Click);
            // 
            // storeToolStripMenuItem
            // 
            this.storeToolStripMenuItem.Name = "storeToolStripMenuItem";
            resources.ApplyResources(this.storeToolStripMenuItem, "storeToolStripMenuItem");
            this.storeToolStripMenuItem.Click += new System.EventHandler(this.storeToolStripMenuItem_Click);
            // 
            // synchronizeToolStripMenuItem
            // 
            this.synchronizeToolStripMenuItem.Name = "synchronizeToolStripMenuItem";
            resources.ApplyResources(this.synchronizeToolStripMenuItem, "synchronizeToolStripMenuItem");
            this.synchronizeToolStripMenuItem.Click += new System.EventHandler(this.synchronizeToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.settingsToolStripMenuItem,
            this.tuneRequestToolStripMenuItem,
            this.singlePatchesToolStripMenuItem,
            this.allDataDumpToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            resources.ApplyResources(this.toolsToolStripMenuItem, "toolsToolStripMenuItem");
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            resources.ApplyResources(this.settingsToolStripMenuItem, "settingsToolStripMenuItem");
            this.settingsToolStripMenuItem.Click += new System.EventHandler(this.settingsToolStripMenuItem_Click);
            // 
            // tuneRequestToolStripMenuItem
            // 
            this.tuneRequestToolStripMenuItem.Name = "tuneRequestToolStripMenuItem";
            resources.ApplyResources(this.tuneRequestToolStripMenuItem, "tuneRequestToolStripMenuItem");
            this.tuneRequestToolStripMenuItem.Click += new System.EventHandler(this.tuneRequestToolStripMenuItem_Click);
            // 
            // singlePatchesToolStripMenuItem
            // 
            this.singlePatchesToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.getSingleTonesFromSynthToolStripMenuItem,
            this.extractSingleTonesFromBankToolStripMenuItem});
            this.singlePatchesToolStripMenuItem.Name = "singlePatchesToolStripMenuItem";
            resources.ApplyResources(this.singlePatchesToolStripMenuItem, "singlePatchesToolStripMenuItem");
            // 
            // getSingleTonesFromSynthToolStripMenuItem
            // 
            this.getSingleTonesFromSynthToolStripMenuItem.Name = "getSingleTonesFromSynthToolStripMenuItem";
            resources.ApplyResources(this.getSingleTonesFromSynthToolStripMenuItem, "getSingleTonesFromSynthToolStripMenuItem");
            this.getSingleTonesFromSynthToolStripMenuItem.Click += new System.EventHandler(this.getSinglePatchesFromSynthToolStripMenuItem_Click);
            // 
            // extractSingleTonesFromBankToolStripMenuItem
            // 
            this.extractSingleTonesFromBankToolStripMenuItem.Name = "extractSingleTonesFromBankToolStripMenuItem";
            resources.ApplyResources(this.extractSingleTonesFromBankToolStripMenuItem, "extractSingleTonesFromBankToolStripMenuItem");
            this.extractSingleTonesFromBankToolStripMenuItem.Click += new System.EventHandler(this.extractSinglePatchesFromBankToolStripMenuItem_Click);
            // 
            // allDataDumpToolStripMenuItem
            // 
            this.allDataDumpToolStripMenuItem.Name = "allDataDumpToolStripMenuItem";
            resources.ApplyResources(this.allDataDumpToolStripMenuItem, "allDataDumpToolStripMenuItem");
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.helpXplorerToolStripMenuItem,
            this.toolStripSeparator5,
            this.checkForNewReleaseToolStripMenuItem,
            this.goToWebsiteToolStripMenuItem,
            this.aboutdeToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            resources.ApplyResources(this.helpToolStripMenuItem, "helpToolStripMenuItem");
            // 
            // helpXplorerToolStripMenuItem
            // 
            this.helpXplorerToolStripMenuItem.Name = "helpXplorerToolStripMenuItem";
            resources.ApplyResources(this.helpXplorerToolStripMenuItem, "helpXplorerToolStripMenuItem");
            this.helpXplorerToolStripMenuItem.Click += new System.EventHandler(this.helpXplorerToolStripMenuItem_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            resources.ApplyResources(this.toolStripSeparator5, "toolStripSeparator5");
            // 
            // checkForNewReleaseToolStripMenuItem
            // 
            this.checkForNewReleaseToolStripMenuItem.Name = "checkForNewReleaseToolStripMenuItem";
            resources.ApplyResources(this.checkForNewReleaseToolStripMenuItem, "checkForNewReleaseToolStripMenuItem");
            this.checkForNewReleaseToolStripMenuItem.Click += new System.EventHandler(this.checkForNewReleaseToolStripMenuItem_Click);
            // 
            // goToWebsiteToolStripMenuItem
            // 
            this.goToWebsiteToolStripMenuItem.Name = "goToWebsiteToolStripMenuItem";
            resources.ApplyResources(this.goToWebsiteToolStripMenuItem, "goToWebsiteToolStripMenuItem");
            this.goToWebsiteToolStripMenuItem.Click += new System.EventHandler(this.goToWebsiteToolStripMenuItem_Click);
            // 
            // aboutdeToolStripMenuItem
            // 
            this.aboutdeToolStripMenuItem.Name = "aboutdeToolStripMenuItem";
            resources.ApplyResources(this.aboutdeToolStripMenuItem, "aboutdeToolStripMenuItem");
            this.aboutdeToolStripMenuItem.Click += new System.EventHandler(this.aboutdeToolStripMenuItem_Click);
            // 
            // btPatchStore
            // 
            this.btPatchStore.BackColor = System.Drawing.Color.Transparent;
            this.btPatchStore.BackgroundImage = global::Xplorer.Properties.Resources.store;
            resources.ApplyResources(this.btPatchStore, "btPatchStore");
            this.btPatchStore.DownImage = global::Xplorer.Properties.Resources.storedown;
            this.btPatchStore.FlatAppearance.BorderSize = 0;
            this.btPatchStore.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btPatchStore.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btPatchStore.HoverImage = global::Xplorer.Properties.Resources.storehover;
            this.btPatchStore.Name = "btPatchStore";
            this._toolTip.SetToolTip(this.btPatchStore, resources.GetString("btPatchStore.ToolTip"));
            this.btPatchStore.UseVisualStyleBackColor = false;
            this.btPatchStore.Click += new System.EventHandler(this.btPatchStore_Click);
            // 
            // btPatchPlus
            // 
            this.btPatchPlus.BackColor = System.Drawing.Color.Transparent;
            this.btPatchPlus.BackgroundImage = global::Xplorer.Properties.Resources.plus;
            resources.ApplyResources(this.btPatchPlus, "btPatchPlus");
            this.btPatchPlus.DownImage = global::Xplorer.Properties.Resources.plusdown;
            this.btPatchPlus.FlatAppearance.BorderSize = 0;
            this.btPatchPlus.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btPatchPlus.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btPatchPlus.HoverImage = global::Xplorer.Properties.Resources.plushover;
            this.btPatchPlus.Name = "btPatchPlus";
            this._toolTip.SetToolTip(this.btPatchPlus, resources.GetString("btPatchPlus.ToolTip"));
            this.btPatchPlus.UseVisualStyleBackColor = false;
            this.btPatchPlus.Click += new System.EventHandler(this.btPatchPlus_Click);
            // 
            // MOD_QUANTIZE_20
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_20, "MOD_QUANTIZE_20");
            this.MOD_QUANTIZE_20.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_20.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_20.Maximum = 1;
            this.MOD_QUANTIZE_20.Minimum = 0;
            this.MOD_QUANTIZE_20.Name = "MOD_QUANTIZE_20";
            this.MOD_QUANTIZE_20.Step = 1;
            this.MOD_QUANTIZE_20.Tag = "MOD_QUANTIZE_20";
            this.MOD_QUANTIZE_20.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_20.Value = 0;
            this.MOD_QUANTIZE_20.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_19
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_19, "MOD_QUANTIZE_19");
            this.MOD_QUANTIZE_19.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_19.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_19.Maximum = 1;
            this.MOD_QUANTIZE_19.Minimum = 0;
            this.MOD_QUANTIZE_19.Name = "MOD_QUANTIZE_19";
            this.MOD_QUANTIZE_19.Step = 1;
            this.MOD_QUANTIZE_19.Tag = "MOD_QUANTIZE_19";
            this.MOD_QUANTIZE_19.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_19.Value = 0;
            this.MOD_QUANTIZE_19.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_18
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_18, "MOD_QUANTIZE_18");
            this.MOD_QUANTIZE_18.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_18.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_18.Maximum = 1;
            this.MOD_QUANTIZE_18.Minimum = 0;
            this.MOD_QUANTIZE_18.Name = "MOD_QUANTIZE_18";
            this.MOD_QUANTIZE_18.Step = 1;
            this.MOD_QUANTIZE_18.Tag = "MOD_QUANTIZE_18";
            this.MOD_QUANTIZE_18.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_18.Value = 0;
            this.MOD_QUANTIZE_18.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // btPatchSave
            // 
            this.btPatchSave.BackColor = System.Drawing.Color.Transparent;
            this.btPatchSave.BackgroundImage = global::Xplorer.Properties.Resources.save;
            resources.ApplyResources(this.btPatchSave, "btPatchSave");
            this.btPatchSave.DownImage = global::Xplorer.Properties.Resources.savedown;
            this.btPatchSave.FlatAppearance.BorderSize = 0;
            this.btPatchSave.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btPatchSave.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btPatchSave.HoverImage = global::Xplorer.Properties.Resources.savehover;
            this.btPatchSave.Name = "btPatchSave";
            this._toolTip.SetToolTip(this.btPatchSave, resources.GetString("btPatchSave.ToolTip"));
            this.btPatchSave.UseVisualStyleBackColor = false;
            this.btPatchSave.Click += new System.EventHandler(this.btPatchSave_Click);
            // 
            // MOD_QUANTIZE_17
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_17, "MOD_QUANTIZE_17");
            this.MOD_QUANTIZE_17.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_17.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_17.Maximum = 1;
            this.MOD_QUANTIZE_17.Minimum = 0;
            this.MOD_QUANTIZE_17.Name = "MOD_QUANTIZE_17";
            this.MOD_QUANTIZE_17.Step = 1;
            this.MOD_QUANTIZE_17.Tag = "MOD_QUANTIZE_17";
            this.MOD_QUANTIZE_17.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_17.Value = 0;
            this.MOD_QUANTIZE_17.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // btPatchLoad
            // 
            this.btPatchLoad.BackColor = System.Drawing.Color.Transparent;
            this.btPatchLoad.BackgroundImage = global::Xplorer.Properties.Resources.load;
            resources.ApplyResources(this.btPatchLoad, "btPatchLoad");
            this.btPatchLoad.DownImage = global::Xplorer.Properties.Resources.loaddown;
            this.btPatchLoad.FlatAppearance.BorderSize = 0;
            this.btPatchLoad.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btPatchLoad.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btPatchLoad.HoverImage = global::Xplorer.Properties.Resources.loadhover;
            this.btPatchLoad.Name = "btPatchLoad";
            this._toolTip.SetToolTip(this.btPatchLoad, resources.GetString("btPatchLoad.ToolTip"));
            this.btPatchLoad.UseVisualStyleBackColor = false;
            this.btPatchLoad.Click += new System.EventHandler(this.btPatchLoad_Click);
            // 
            // btPatchRandom
            // 
            this.btPatchRandom.BackColor = System.Drawing.Color.Transparent;
            this.btPatchRandom.BackgroundImage = global::Xplorer.Properties.Resources.random;
            resources.ApplyResources(this.btPatchRandom, "btPatchRandom");
            this.btPatchRandom.DownImage = global::Xplorer.Properties.Resources.randomdown;
            this.btPatchRandom.FlatAppearance.BorderSize = 0;
            this.btPatchRandom.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btPatchRandom.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btPatchRandom.HoverImage = global::Xplorer.Properties.Resources.randomhover;
            this.btPatchRandom.Name = "btPatchRandom";
            this._toolTip.SetToolTip(this.btPatchRandom, resources.GetString("btPatchRandom.ToolTip"));
            this.btPatchRandom.UseVisualStyleBackColor = false;
            this.btPatchRandom.Click += new System.EventHandler(this.btPatchRandom_Click);
            // 
            // btPatchGoto
            // 
            this.btPatchGoto.BackColor = System.Drawing.Color.Transparent;
            this.btPatchGoto.BackgroundImage = global::Xplorer.Properties.Resources.gotopatch;
            resources.ApplyResources(this.btPatchGoto, "btPatchGoto");
            this.btPatchGoto.DownImage = global::Xplorer.Properties.Resources.gotodown;
            this.btPatchGoto.FlatAppearance.BorderSize = 0;
            this.btPatchGoto.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btPatchGoto.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btPatchGoto.HoverImage = global::Xplorer.Properties.Resources.gotohover;
            this.btPatchGoto.Name = "btPatchGoto";
            this._toolTip.SetToolTip(this.btPatchGoto, resources.GetString("btPatchGoto.ToolTip"));
            this.btPatchGoto.UseVisualStyleBackColor = false;
            this.btPatchGoto.Click += new System.EventHandler(this.btPatchGoto_Click);
            // 
            // btPatchMinus
            // 
            this.btPatchMinus.BackColor = System.Drawing.Color.Transparent;
            this.btPatchMinus.BackgroundImage = global::Xplorer.Properties.Resources.minus;
            resources.ApplyResources(this.btPatchMinus, "btPatchMinus");
            this.btPatchMinus.DownImage = global::Xplorer.Properties.Resources.minusdown;
            this.btPatchMinus.FlatAppearance.BorderSize = 0;
            this.btPatchMinus.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btPatchMinus.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btPatchMinus.HoverImage = global::Xplorer.Properties.Resources.minushover;
            this.btPatchMinus.Name = "btPatchMinus";
            this._toolTip.SetToolTip(this.btPatchMinus, resources.GetString("btPatchMinus.ToolTip"));
            this.btPatchMinus.UseVisualStyleBackColor = false;
            this.btPatchMinus.Click += new System.EventHandler(this.btPatchMinus_Click);
            // 
            // btSettings
            // 
            this.btSettings.BackColor = System.Drawing.Color.Transparent;
            this.btSettings.BackgroundImage = global::Xplorer.Properties.Resources.settings;
            resources.ApplyResources(this.btSettings, "btSettings");
            this.btSettings.DownImage = global::Xplorer.Properties.Resources.settingsdown;
            this.btSettings.FlatAppearance.BorderSize = 0;
            this.btSettings.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            this.btSettings.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            this.btSettings.HoverImage = global::Xplorer.Properties.Resources.settingshover;
            this.btSettings.Name = "btSettings";
            this._toolTip.SetToolTip(this.btSettings, resources.GetString("btSettings.ToolTip"));
            this.btSettings.UseVisualStyleBackColor = false;
            this.btSettings.Click += new System.EventHandler(this.btSettings_Click);
            // 
            // LFO_X_WAVESHAPE
            // 
            this.LFO_X_WAVESHAPE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_X_WAVESHAPE.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.LFO_X_WAVESHAPE, "LFO_X_WAVESHAPE");
            this.LFO_X_WAVESHAPE.ForeColor = System.Drawing.Color.White;
            this.LFO_X_WAVESHAPE.FormattingEnabled = true;
            this.LFO_X_WAVESHAPE.Maximum = 1;
            this.LFO_X_WAVESHAPE.Minimum = 0;
            this.LFO_X_WAVESHAPE.Name = "LFO_X_WAVESHAPE";
            this.LFO_X_WAVESHAPE.Step = 1;
            this.LFO_X_WAVESHAPE.Tag = "LFO_X_WAVESHAPE";
            this.LFO_X_WAVESHAPE.Value = 0;
            this.LFO_X_WAVESHAPE.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // MOD_QUANTIZE_16
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_16, "MOD_QUANTIZE_16");
            this.MOD_QUANTIZE_16.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_16.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_16.Maximum = 1;
            this.MOD_QUANTIZE_16.Minimum = 0;
            this.MOD_QUANTIZE_16.Name = "MOD_QUANTIZE_16";
            this.MOD_QUANTIZE_16.Step = 1;
            this.MOD_QUANTIZE_16.Tag = "MOD_QUANTIZE_16";
            this.MOD_QUANTIZE_16.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_16.Value = 0;
            this.MOD_QUANTIZE_16.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // LFO_X_SAMPLE_INPUT
            // 
            this.LFO_X_SAMPLE_INPUT.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_X_SAMPLE_INPUT.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.LFO_X_SAMPLE_INPUT, "LFO_X_SAMPLE_INPUT");
            this.LFO_X_SAMPLE_INPUT.ForeColor = System.Drawing.Color.White;
            this.LFO_X_SAMPLE_INPUT.FormattingEnabled = true;
            this.LFO_X_SAMPLE_INPUT.Maximum = 1;
            this.LFO_X_SAMPLE_INPUT.Minimum = 0;
            this.LFO_X_SAMPLE_INPUT.Name = "LFO_X_SAMPLE_INPUT";
            this.LFO_X_SAMPLE_INPUT.Step = 1;
            this.LFO_X_SAMPLE_INPUT.Tag = "LFO_X_SAMPLE_INPUT";
            this.LFO_X_SAMPLE_INPUT.Value = 0;
            this.LFO_X_SAMPLE_INPUT.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // MOD_QUANTIZE_15
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_15, "MOD_QUANTIZE_15");
            this.MOD_QUANTIZE_15.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_15.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_15.Maximum = 1;
            this.MOD_QUANTIZE_15.Minimum = 0;
            this.MOD_QUANTIZE_15.Name = "MOD_QUANTIZE_15";
            this.MOD_QUANTIZE_15.Step = 1;
            this.MOD_QUANTIZE_15.Tag = "MOD_QUANTIZE_15";
            this.MOD_QUANTIZE_15.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_15.Value = 0;
            this.MOD_QUANTIZE_15.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // ENV_X_TRIG_SINGLE_MULTI
            // 
            this.ENV_X_TRIG_SINGLE_MULTI.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.ENV_X_TRIG_SINGLE_MULTI, "ENV_X_TRIG_SINGLE_MULTI");
            this.ENV_X_TRIG_SINGLE_MULTI.Controls.Add(this.rdSINGLE);
            this.ENV_X_TRIG_SINGLE_MULTI.Controls.Add(this.rdMULTI);
            this.ENV_X_TRIG_SINGLE_MULTI.Maximum = 1;
            this.ENV_X_TRIG_SINGLE_MULTI.Minimum = 0;
            this.ENV_X_TRIG_SINGLE_MULTI.Name = "ENV_X_TRIG_SINGLE_MULTI";
            this.ENV_X_TRIG_SINGLE_MULTI.Step = 1;
            this.ENV_X_TRIG_SINGLE_MULTI.Tag = "ENV_X_TRIG_SINGLE_MULTI";
            this.ENV_X_TRIG_SINGLE_MULTI.Value = 0;
            this.ENV_X_TRIG_SINGLE_MULTI.ValueChanged += new MidiApp.UIControls.RadioButtonPanel.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            // 
            // rdSINGLE
            // 
            resources.ApplyResources(this.rdSINGLE, "rdSINGLE");
            this.rdSINGLE.BackColor = System.Drawing.Color.Black;
            this.rdSINGLE.Checked = true;
            this.rdSINGLE.ForeColor = System.Drawing.Color.White;
            this.rdSINGLE.Name = "rdSINGLE";
            this.rdSINGLE.TabStop = true;
            this.rdSINGLE.Tag = "0";
            this.rdSINGLE.UseVisualStyleBackColor = false;
            this.rdSINGLE.CheckedChanged += new System.EventHandler(this.RadioEnvXSINGLE_MULTI_CheckedChanged);
            // 
            // rdMULTI
            // 
            resources.ApplyResources(this.rdMULTI, "rdMULTI");
            this.rdMULTI.BackColor = System.Drawing.Color.Black;
            this.rdMULTI.ForeColor = System.Drawing.Color.White;
            this.rdMULTI.Name = "rdMULTI";
            this.rdMULTI.Tag = "1";
            this.rdMULTI.UseVisualStyleBackColor = false;
            this.rdMULTI.CheckedChanged += new System.EventHandler(this.RadioEnvXSINGLE_MULTI_CheckedChanged);
            // 
            // MOD_QUANTIZE_14
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_14, "MOD_QUANTIZE_14");
            this.MOD_QUANTIZE_14.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_14.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_14.Maximum = 1;
            this.MOD_QUANTIZE_14.Minimum = 0;
            this.MOD_QUANTIZE_14.Name = "MOD_QUANTIZE_14";
            this.MOD_QUANTIZE_14.Step = 1;
            this.MOD_QUANTIZE_14.Tag = "MOD_QUANTIZE_14";
            this.MOD_QUANTIZE_14.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_14.Value = 0;
            this.MOD_QUANTIZE_14.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_13
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_13, "MOD_QUANTIZE_13");
            this.MOD_QUANTIZE_13.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_13.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_13.Maximum = 1;
            this.MOD_QUANTIZE_13.Minimum = 0;
            this.MOD_QUANTIZE_13.Name = "MOD_QUANTIZE_13";
            this.MOD_QUANTIZE_13.Step = 1;
            this.MOD_QUANTIZE_13.Tag = "MOD_QUANTIZE_13";
            this.MOD_QUANTIZE_13.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_13.Value = 0;
            this.MOD_QUANTIZE_13.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_12
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_12, "MOD_QUANTIZE_12");
            this.MOD_QUANTIZE_12.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_12.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_12.Maximum = 1;
            this.MOD_QUANTIZE_12.Minimum = 0;
            this.MOD_QUANTIZE_12.Name = "MOD_QUANTIZE_12";
            this.MOD_QUANTIZE_12.Step = 1;
            this.MOD_QUANTIZE_12.Tag = "MOD_QUANTIZE_12";
            this.MOD_QUANTIZE_12.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_12.Value = 0;
            this.MOD_QUANTIZE_12.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_11
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_11, "MOD_QUANTIZE_11");
            this.MOD_QUANTIZE_11.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_11.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_11.Maximum = 1;
            this.MOD_QUANTIZE_11.Minimum = 0;
            this.MOD_QUANTIZE_11.Name = "MOD_QUANTIZE_11";
            this.MOD_QUANTIZE_11.Step = 1;
            this.MOD_QUANTIZE_11.Tag = "MOD_QUANTIZE_11";
            this.MOD_QUANTIZE_11.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_11.Value = 0;
            this.MOD_QUANTIZE_11.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // LFO_X_RETRIG_MODE
            // 
            this.LFO_X_RETRIG_MODE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_X_RETRIG_MODE.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.LFO_X_RETRIG_MODE, "LFO_X_RETRIG_MODE");
            this.LFO_X_RETRIG_MODE.ForeColor = System.Drawing.Color.White;
            this.LFO_X_RETRIG_MODE.FormattingEnabled = true;
            this.LFO_X_RETRIG_MODE.Maximum = 1;
            this.LFO_X_RETRIG_MODE.Minimum = 0;
            this.LFO_X_RETRIG_MODE.Name = "LFO_X_RETRIG_MODE";
            this.LFO_X_RETRIG_MODE.Step = 1;
            this.LFO_X_RETRIG_MODE.Tag = "LFO_X_RETRIG_MODE";
            this.LFO_X_RETRIG_MODE.Value = 0;
            this.LFO_X_RETRIG_MODE.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // MOD_QUANTIZE_10
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_10, "MOD_QUANTIZE_10");
            this.MOD_QUANTIZE_10.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_10.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_10.Maximum = 1;
            this.MOD_QUANTIZE_10.Minimum = 0;
            this.MOD_QUANTIZE_10.Name = "MOD_QUANTIZE_10";
            this.MOD_QUANTIZE_10.Step = 1;
            this.MOD_QUANTIZE_10.Tag = "MOD_QUANTIZE_10";
            this.MOD_QUANTIZE_10.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_10.Value = 0;
            this.MOD_QUANTIZE_10.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_9
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_9, "MOD_QUANTIZE_9");
            this.MOD_QUANTIZE_9.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_9.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_9.Maximum = 1;
            this.MOD_QUANTIZE_9.Minimum = 0;
            this.MOD_QUANTIZE_9.Name = "MOD_QUANTIZE_9";
            this.MOD_QUANTIZE_9.Step = 1;
            this.MOD_QUANTIZE_9.Tag = "MOD_QUANTIZE_9";
            this.MOD_QUANTIZE_9.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_9.Value = 0;
            this.MOD_QUANTIZE_9.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_8
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_8, "MOD_QUANTIZE_8");
            this.MOD_QUANTIZE_8.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_8.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_8.Maximum = 1;
            this.MOD_QUANTIZE_8.Minimum = 0;
            this.MOD_QUANTIZE_8.Name = "MOD_QUANTIZE_8";
            this.MOD_QUANTIZE_8.Step = 1;
            this.MOD_QUANTIZE_8.Tag = "MOD_QUANTIZE_8";
            this.MOD_QUANTIZE_8.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_8.Value = 0;
            this.MOD_QUANTIZE_8.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // LAG_IN
            // 
            this.LAG_IN.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LAG_IN.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.LAG_IN, "LAG_IN");
            this.LAG_IN.ForeColor = System.Drawing.Color.White;
            this.LAG_IN.FormattingEnabled = true;
            this.LAG_IN.Maximum = 1;
            this.LAG_IN.Minimum = 0;
            this.LAG_IN.Name = "LAG_IN";
            this.LAG_IN.Step = 1;
            this.LAG_IN.Tag = "LAG_IN";
            this.LAG_IN.Value = 0;
            this.LAG_IN.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // MOD_QUANTIZE_7
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_7, "MOD_QUANTIZE_7");
            this.MOD_QUANTIZE_7.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_7.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_7.Maximum = 1;
            this.MOD_QUANTIZE_7.Minimum = 0;
            this.MOD_QUANTIZE_7.Name = "MOD_QUANTIZE_7";
            this.MOD_QUANTIZE_7.Step = 1;
            this.MOD_QUANTIZE_7.Tag = "MOD_QUANTIZE_7";
            this.MOD_QUANTIZE_7.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_7.Value = 0;
            this.MOD_QUANTIZE_7.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // PANEL_RAMP
            // 
            this.PANEL_RAMP.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.PANEL_RAMP, "PANEL_RAMP");
            this.PANEL_RAMP.Controls.Add(this.RAMP_X_TRIG_LFO_SOURCE);
            this.PANEL_RAMP.Controls.Add(this.RAMP_X_TRIG_GATED);
            this.PANEL_RAMP.Controls.Add(this.RAMP_X_TRIG_LFOTRIG);
            this.PANEL_RAMP.Controls.Add(this.RAMP_X_TRIG_EXTRIG);
            this.PANEL_RAMP.Name = "PANEL_RAMP";
            // 
            // RAMP_X_TRIG_LFO_SOURCE
            // 
            this.RAMP_X_TRIG_LFO_SOURCE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.RAMP_X_TRIG_LFO_SOURCE.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.RAMP_X_TRIG_LFO_SOURCE, "RAMP_X_TRIG_LFO_SOURCE");
            this.RAMP_X_TRIG_LFO_SOURCE.ForeColor = System.Drawing.Color.White;
            this.RAMP_X_TRIG_LFO_SOURCE.FormattingEnabled = true;
            this.RAMP_X_TRIG_LFO_SOURCE.Maximum = 1;
            this.RAMP_X_TRIG_LFO_SOURCE.Minimum = 0;
            this.RAMP_X_TRIG_LFO_SOURCE.Name = "RAMP_X_TRIG_LFO_SOURCE";
            this.RAMP_X_TRIG_LFO_SOURCE.Step = 1;
            this.RAMP_X_TRIG_LFO_SOURCE.Tag = "RAMP_X_TRIG_LFO_SOURCE";
            this.RAMP_X_TRIG_LFO_SOURCE.Value = 0;
            this.RAMP_X_TRIG_LFO_SOURCE.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // RAMP_X_TRIG_GATED
            // 
            resources.ApplyResources(this.RAMP_X_TRIG_GATED, "RAMP_X_TRIG_GATED");
            this.RAMP_X_TRIG_GATED.BackColor = System.Drawing.Color.Black;
            this.RAMP_X_TRIG_GATED.ForeColor = System.Drawing.Color.White;
            this.RAMP_X_TRIG_GATED.Maximum = 1;
            this.RAMP_X_TRIG_GATED.Minimum = 0;
            this.RAMP_X_TRIG_GATED.Name = "RAMP_X_TRIG_GATED";
            this.RAMP_X_TRIG_GATED.Step = 1;
            this.RAMP_X_TRIG_GATED.Tag = "RAMP_X_TRIG_GATED";
            this.RAMP_X_TRIG_GATED.UseVisualStyleBackColor = false;
            this.RAMP_X_TRIG_GATED.Value = 0;
            this.RAMP_X_TRIG_GATED.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // RAMP_X_TRIG_LFOTRIG
            // 
            resources.ApplyResources(this.RAMP_X_TRIG_LFOTRIG, "RAMP_X_TRIG_LFOTRIG");
            this.RAMP_X_TRIG_LFOTRIG.BackColor = System.Drawing.Color.Black;
            this.RAMP_X_TRIG_LFOTRIG.ForeColor = System.Drawing.Color.White;
            this.RAMP_X_TRIG_LFOTRIG.Maximum = 1;
            this.RAMP_X_TRIG_LFOTRIG.Minimum = 0;
            this.RAMP_X_TRIG_LFOTRIG.Name = "RAMP_X_TRIG_LFOTRIG";
            this.RAMP_X_TRIG_LFOTRIG.Step = 1;
            this.RAMP_X_TRIG_LFOTRIG.Tag = "RAMP_X_TRIG_LFOTRIG";
            this.RAMP_X_TRIG_LFOTRIG.UseVisualStyleBackColor = false;
            this.RAMP_X_TRIG_LFOTRIG.Value = 0;
            this.RAMP_X_TRIG_LFOTRIG.CheckedChanged += new System.EventHandler(this.RAMP_X_TRIG_LFOTRIG_CheckedChanged);
            // 
            // RAMP_X_TRIG_EXTRIG
            // 
            resources.ApplyResources(this.RAMP_X_TRIG_EXTRIG, "RAMP_X_TRIG_EXTRIG");
            this.RAMP_X_TRIG_EXTRIG.BackColor = System.Drawing.Color.Black;
            this.RAMP_X_TRIG_EXTRIG.ForeColor = System.Drawing.Color.White;
            this.RAMP_X_TRIG_EXTRIG.Maximum = 1;
            this.RAMP_X_TRIG_EXTRIG.Minimum = 0;
            this.RAMP_X_TRIG_EXTRIG.Name = "RAMP_X_TRIG_EXTRIG";
            this.RAMP_X_TRIG_EXTRIG.Step = 1;
            this.RAMP_X_TRIG_EXTRIG.Tag = "RAMP_X_TRIG_EXTRIG";
            this.RAMP_X_TRIG_EXTRIG.UseVisualStyleBackColor = false;
            this.RAMP_X_TRIG_EXTRIG.Value = 0;
            this.RAMP_X_TRIG_EXTRIG.CheckedChanged += new System.EventHandler(this.RAMP_X_TRIG_EXTRIG_CheckedChanged);
            // 
            // VCF_MODE
            // 
            this.VCF_MODE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.VCF_MODE.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.VCF_MODE, "VCF_MODE");
            this.VCF_MODE.ForeColor = System.Drawing.Color.White;
            this.VCF_MODE.FormattingEnabled = true;
            this.VCF_MODE.Maximum = 1;
            this.VCF_MODE.Minimum = 0;
            this.VCF_MODE.Name = "VCF_MODE";
            this.VCF_MODE.Step = 1;
            this.VCF_MODE.Tag = "VCF_MODE";
            this.VCF_MODE.Value = 0;
            this.VCF_MODE.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // LFO_X_LAG
            // 
            resources.ApplyResources(this.LFO_X_LAG, "LFO_X_LAG");
            this.LFO_X_LAG.BackColor = System.Drawing.Color.Black;
            this.LFO_X_LAG.ForeColor = System.Drawing.Color.White;
            this.LFO_X_LAG.Maximum = 1;
            this.LFO_X_LAG.Minimum = 0;
            this.LFO_X_LAG.Name = "LFO_X_LAG";
            this.LFO_X_LAG.Step = 1;
            this.LFO_X_LAG.Tag = "LFO_X_LAG";
            this.LFO_X_LAG.UseVisualStyleBackColor = false;
            this.LFO_X_LAG.Value = 0;
            this.LFO_X_LAG.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_QUANTIZE_6
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_6, "MOD_QUANTIZE_6");
            this.MOD_QUANTIZE_6.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_6.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_6.Maximum = 1;
            this.MOD_QUANTIZE_6.Minimum = 0;
            this.MOD_QUANTIZE_6.Name = "MOD_QUANTIZE_6";
            this.MOD_QUANTIZE_6.Step = 1;
            this.MOD_QUANTIZE_6.Tag = "MOD_QUANTIZE_6";
            this.MOD_QUANTIZE_6.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_6.Value = 0;
            this.MOD_QUANTIZE_6.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_5
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_5, "MOD_QUANTIZE_5");
            this.MOD_QUANTIZE_5.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_5.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_5.Maximum = 1;
            this.MOD_QUANTIZE_5.Minimum = 0;
            this.MOD_QUANTIZE_5.Name = "MOD_QUANTIZE_5";
            this.MOD_QUANTIZE_5.Step = 1;
            this.MOD_QUANTIZE_5.Tag = "MOD_QUANTIZE_5";
            this.MOD_QUANTIZE_5.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_5.Value = 0;
            this.MOD_QUANTIZE_5.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // TRACK_X_IN
            // 
            this.TRACK_X_IN.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.TRACK_X_IN.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.TRACK_X_IN, "TRACK_X_IN");
            this.TRACK_X_IN.ForeColor = System.Drawing.Color.White;
            this.TRACK_X_IN.FormattingEnabled = true;
            this.TRACK_X_IN.Maximum = 1;
            this.TRACK_X_IN.Minimum = 0;
            this.TRACK_X_IN.Name = "TRACK_X_IN";
            this.TRACK_X_IN.Step = 1;
            this.TRACK_X_IN.Tag = "TRACK_X_IN";
            this.TRACK_X_IN.Value = 0;
            this.TRACK_X_IN.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // MOD_QUANTIZE_4
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_4, "MOD_QUANTIZE_4");
            this.MOD_QUANTIZE_4.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_4.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_4.Maximum = 1;
            this.MOD_QUANTIZE_4.Minimum = 0;
            this.MOD_QUANTIZE_4.Name = "MOD_QUANTIZE_4";
            this.MOD_QUANTIZE_4.Step = 1;
            this.MOD_QUANTIZE_4.Tag = "MOD_QUANTIZE_4";
            this.MOD_QUANTIZE_4.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_4.Value = 0;
            this.MOD_QUANTIZE_4.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_3
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_3, "MOD_QUANTIZE_3");
            this.MOD_QUANTIZE_3.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_3.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_3.Maximum = 1;
            this.MOD_QUANTIZE_3.Minimum = 0;
            this.MOD_QUANTIZE_3.Name = "MOD_QUANTIZE_3";
            this.MOD_QUANTIZE_3.Step = 1;
            this.MOD_QUANTIZE_3.Tag = "MOD_QUANTIZE_3";
            this.MOD_QUANTIZE_3.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_3.Value = 0;
            this.MOD_QUANTIZE_3.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // MOD_QUANTIZE_2
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_2, "MOD_QUANTIZE_2");
            this.MOD_QUANTIZE_2.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_2.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_2.Maximum = 1;
            this.MOD_QUANTIZE_2.Minimum = 0;
            this.MOD_QUANTIZE_2.Name = "MOD_QUANTIZE_2";
            this.MOD_QUANTIZE_2.Step = 1;
            this.MOD_QUANTIZE_2.Tag = "MOD_QUANTIZE_2";
            this.MOD_QUANTIZE_2.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_2.Value = 0;
            this.MOD_QUANTIZE_2.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // PANEL_ENV_X_MODE
            // 
            this.PANEL_ENV_X_MODE.BackColor = System.Drawing.Color.Transparent;
            resources.ApplyResources(this.PANEL_ENV_X_MODE, "PANEL_ENV_X_MODE");
            this.PANEL_ENV_X_MODE.Controls.Add(this.label4);
            this.PANEL_ENV_X_MODE.Controls.Add(this.ENV_X_MODE_DADR);
            this.PANEL_ENV_X_MODE.Controls.Add(this.ENV_X_MODE_FREERUN);
            this.PANEL_ENV_X_MODE.Controls.Add(this.ENV_X_MODE_RESET);
            this.PANEL_ENV_X_MODE.Name = "PANEL_ENV_X_MODE";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.ForeColor = System.Drawing.Color.White;
            this.label4.Name = "label4";
            // 
            // ENV_X_MODE_DADR
            // 
            resources.ApplyResources(this.ENV_X_MODE_DADR, "ENV_X_MODE_DADR");
            this.ENV_X_MODE_DADR.BackColor = System.Drawing.Color.Black;
            this.ENV_X_MODE_DADR.ForeColor = System.Drawing.Color.White;
            this.ENV_X_MODE_DADR.Maximum = 1;
            this.ENV_X_MODE_DADR.Minimum = 0;
            this.ENV_X_MODE_DADR.Name = "ENV_X_MODE_DADR";
            this.ENV_X_MODE_DADR.Step = 1;
            this.ENV_X_MODE_DADR.Tag = "ENV_X_MODE_DADR";
            this.ENV_X_MODE_DADR.UseVisualStyleBackColor = false;
            this.ENV_X_MODE_DADR.Value = 0;
            this.ENV_X_MODE_DADR.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // ENV_X_MODE_FREERUN
            // 
            resources.ApplyResources(this.ENV_X_MODE_FREERUN, "ENV_X_MODE_FREERUN");
            this.ENV_X_MODE_FREERUN.BackColor = System.Drawing.Color.Black;
            this.ENV_X_MODE_FREERUN.ForeColor = System.Drawing.Color.White;
            this.ENV_X_MODE_FREERUN.Maximum = 1;
            this.ENV_X_MODE_FREERUN.Minimum = 0;
            this.ENV_X_MODE_FREERUN.Name = "ENV_X_MODE_FREERUN";
            this.ENV_X_MODE_FREERUN.Step = 1;
            this.ENV_X_MODE_FREERUN.Tag = "ENV_X_MODE_FREERUN";
            this.ENV_X_MODE_FREERUN.UseVisualStyleBackColor = false;
            this.ENV_X_MODE_FREERUN.Value = 0;
            this.ENV_X_MODE_FREERUN.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // ENV_X_MODE_RESET
            // 
            resources.ApplyResources(this.ENV_X_MODE_RESET, "ENV_X_MODE_RESET");
            this.ENV_X_MODE_RESET.BackColor = System.Drawing.Color.Black;
            this.ENV_X_MODE_RESET.ForeColor = System.Drawing.Color.White;
            this.ENV_X_MODE_RESET.Maximum = 1;
            this.ENV_X_MODE_RESET.Minimum = 0;
            this.ENV_X_MODE_RESET.Name = "ENV_X_MODE_RESET";
            this.ENV_X_MODE_RESET.Step = 1;
            this.ENV_X_MODE_RESET.Tag = "ENV_X_MODE_RESET";
            this.ENV_X_MODE_RESET.UseVisualStyleBackColor = false;
            this.ENV_X_MODE_RESET.Value = 0;
            this.ENV_X_MODE_RESET.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // RAMP_X_TRIG_SINGLE_MULTI
            // 
            this.RAMP_X_TRIG_SINGLE_MULTI.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.RAMP_X_TRIG_SINGLE_MULTI, "RAMP_X_TRIG_SINGLE_MULTI");
            this.RAMP_X_TRIG_SINGLE_MULTI.Controls.Add(this.rdRampXSingle);
            this.RAMP_X_TRIG_SINGLE_MULTI.Controls.Add(this.rdRampXMulti);
            this.RAMP_X_TRIG_SINGLE_MULTI.Maximum = 1;
            this.RAMP_X_TRIG_SINGLE_MULTI.Minimum = 0;
            this.RAMP_X_TRIG_SINGLE_MULTI.Name = "RAMP_X_TRIG_SINGLE_MULTI";
            this.RAMP_X_TRIG_SINGLE_MULTI.Step = 1;
            this.RAMP_X_TRIG_SINGLE_MULTI.Tag = "RAMP_X_TRIG_SINGLE_MULTI";
            this.RAMP_X_TRIG_SINGLE_MULTI.Value = 0;
            this.RAMP_X_TRIG_SINGLE_MULTI.ValueChanged += new MidiApp.UIControls.RadioButtonPanel.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            // 
            // rdRampXSingle
            // 
            resources.ApplyResources(this.rdRampXSingle, "rdRampXSingle");
            this.rdRampXSingle.BackColor = System.Drawing.Color.Black;
            this.rdRampXSingle.Checked = true;
            this.rdRampXSingle.ForeColor = System.Drawing.Color.White;
            this.rdRampXSingle.Name = "rdRampXSingle";
            this.rdRampXSingle.TabStop = true;
            this.rdRampXSingle.Tag = "0";
            this.rdRampXSingle.UseVisualStyleBackColor = false;
            this.rdRampXSingle.CheckedChanged += new System.EventHandler(this.RadioRampXSINGLE_MULTI_CheckedChanged);
            // 
            // rdRampXMulti
            // 
            resources.ApplyResources(this.rdRampXMulti, "rdRampXMulti");
            this.rdRampXMulti.BackColor = System.Drawing.Color.Black;
            this.rdRampXMulti.ForeColor = System.Drawing.Color.White;
            this.rdRampXMulti.Name = "rdRampXMulti";
            this.rdRampXMulti.Tag = "1";
            this.rdRampXMulti.UseVisualStyleBackColor = false;
            this.rdRampXMulti.CheckedChanged += new System.EventHandler(this.RadioRampXSINGLE_MULTI_CheckedChanged);
            // 
            // Panel_ENV_X_TRIGGER
            // 
            this.Panel_ENV_X_TRIGGER.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.Panel_ENV_X_TRIGGER, "Panel_ENV_X_TRIGGER");
            this.Panel_ENV_X_TRIGGER.Controls.Add(this.ENV_X_TRIG_LFO_SOURCE);
            this.Panel_ENV_X_TRIGGER.Controls.Add(this.ENV_X_TRIG_GATED);
            this.Panel_ENV_X_TRIGGER.Controls.Add(this.ENV_X_TRIG_LFOTRIG);
            this.Panel_ENV_X_TRIGGER.Controls.Add(this.ENV_X_TRIG_EXTRIG);
            this.Panel_ENV_X_TRIGGER.Name = "Panel_ENV_X_TRIGGER";
            // 
            // ENV_X_TRIG_LFO_SOURCE
            // 
            this.ENV_X_TRIG_LFO_SOURCE.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ENV_X_TRIG_LFO_SOURCE.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.ENV_X_TRIG_LFO_SOURCE, "ENV_X_TRIG_LFO_SOURCE");
            this.ENV_X_TRIG_LFO_SOURCE.ForeColor = System.Drawing.Color.White;
            this.ENV_X_TRIG_LFO_SOURCE.FormattingEnabled = true;
            this.ENV_X_TRIG_LFO_SOURCE.Maximum = 1;
            this.ENV_X_TRIG_LFO_SOURCE.Minimum = 0;
            this.ENV_X_TRIG_LFO_SOURCE.Name = "ENV_X_TRIG_LFO_SOURCE";
            this.ENV_X_TRIG_LFO_SOURCE.Step = 1;
            this.ENV_X_TRIG_LFO_SOURCE.Tag = "ENV_X_TRIG_LFO_SOURCE";
            this.ENV_X_TRIG_LFO_SOURCE.Value = 0;
            this.ENV_X_TRIG_LFO_SOURCE.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedControl_SelectionChangeCommitted);
            // 
            // ENV_X_TRIG_GATED
            // 
            resources.ApplyResources(this.ENV_X_TRIG_GATED, "ENV_X_TRIG_GATED");
            this.ENV_X_TRIG_GATED.BackColor = System.Drawing.Color.Black;
            this.ENV_X_TRIG_GATED.ForeColor = System.Drawing.Color.White;
            this.ENV_X_TRIG_GATED.Maximum = 1;
            this.ENV_X_TRIG_GATED.Minimum = 0;
            this.ENV_X_TRIG_GATED.Name = "ENV_X_TRIG_GATED";
            this.ENV_X_TRIG_GATED.Step = 1;
            this.ENV_X_TRIG_GATED.Tag = "ENV_X_TRIG_GATED";
            this.ENV_X_TRIG_GATED.UseVisualStyleBackColor = false;
            this.ENV_X_TRIG_GATED.Value = 0;
            this.ENV_X_TRIG_GATED.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // ENV_X_TRIG_LFOTRIG
            // 
            resources.ApplyResources(this.ENV_X_TRIG_LFOTRIG, "ENV_X_TRIG_LFOTRIG");
            this.ENV_X_TRIG_LFOTRIG.BackColor = System.Drawing.Color.Black;
            this.ENV_X_TRIG_LFOTRIG.ForeColor = System.Drawing.Color.White;
            this.ENV_X_TRIG_LFOTRIG.Maximum = 1;
            this.ENV_X_TRIG_LFOTRIG.Minimum = 0;
            this.ENV_X_TRIG_LFOTRIG.Name = "ENV_X_TRIG_LFOTRIG";
            this.ENV_X_TRIG_LFOTRIG.Step = 1;
            this.ENV_X_TRIG_LFOTRIG.Tag = "ENV_X_TRIG_LFOTRIG";
            this.ENV_X_TRIG_LFOTRIG.UseVisualStyleBackColor = false;
            this.ENV_X_TRIG_LFOTRIG.Value = 0;
            this.ENV_X_TRIG_LFOTRIG.CheckedChanged += new System.EventHandler(this.ENV_X_TRIG_LFOTRIG_CheckedChanged);
            // 
            // ENV_X_TRIG_EXTRIG
            // 
            resources.ApplyResources(this.ENV_X_TRIG_EXTRIG, "ENV_X_TRIG_EXTRIG");
            this.ENV_X_TRIG_EXTRIG.BackColor = System.Drawing.Color.Black;
            this.ENV_X_TRIG_EXTRIG.ForeColor = System.Drawing.Color.White;
            this.ENV_X_TRIG_EXTRIG.Maximum = 1;
            this.ENV_X_TRIG_EXTRIG.Minimum = 0;
            this.ENV_X_TRIG_EXTRIG.Name = "ENV_X_TRIG_EXTRIG";
            this.ENV_X_TRIG_EXTRIG.Step = 1;
            this.ENV_X_TRIG_EXTRIG.Tag = "ENV_X_TRIG_EXTRIG";
            this.ENV_X_TRIG_EXTRIG.UseVisualStyleBackColor = false;
            this.ENV_X_TRIG_EXTRIG.Value = 0;
            this.ENV_X_TRIG_EXTRIG.CheckedChanged += new System.EventHandler(this.ENV_X_TRIG_EXTRIG_CheckedChanged);
            // 
            // MOD_QUANTIZE_1
            // 
            resources.ApplyResources(this.MOD_QUANTIZE_1, "MOD_QUANTIZE_1");
            this.MOD_QUANTIZE_1.BackColor = System.Drawing.Color.Black;
            this.MOD_QUANTIZE_1.ForeColor = System.Drawing.Color.White;
            this.MOD_QUANTIZE_1.Maximum = 1;
            this.MOD_QUANTIZE_1.Minimum = 0;
            this.MOD_QUANTIZE_1.Name = "MOD_QUANTIZE_1";
            this.MOD_QUANTIZE_1.Step = 1;
            this.MOD_QUANTIZE_1.Tag = "MOD_QUANTIZE_1";
            this.MOD_QUANTIZE_1.UseVisualStyleBackColor = false;
            this.MOD_QUANTIZE_1.Value = 0;
            this.MOD_QUANTIZE_1.CheckedChanged += new System.EventHandler(this.CheckboxModSourceQuantize_CheckedChanged);
            // 
            // FM_DESTINATION
            // 
            this.FM_DESTINATION.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.FM_DESTINATION, "FM_DESTINATION");
            this.FM_DESTINATION.Controls.Add(this.rdFMDestVCO1);
            this.FM_DESTINATION.Controls.Add(this.rdFMDestVCF);
            this.FM_DESTINATION.Maximum = 1;
            this.FM_DESTINATION.Minimum = 0;
            this.FM_DESTINATION.Name = "FM_DESTINATION";
            this.FM_DESTINATION.Step = 1;
            this.FM_DESTINATION.Tag = "FM_DESTINATION";
            this.FM_DESTINATION.Value = 0;
            this.FM_DESTINATION.ValueChanged += new MidiApp.UIControls.RadioButtonPanel.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            // 
            // rdFMDestVCO1
            // 
            resources.ApplyResources(this.rdFMDestVCO1, "rdFMDestVCO1");
            this.rdFMDestVCO1.BackColor = System.Drawing.Color.Black;
            this.rdFMDestVCO1.Checked = true;
            this.rdFMDestVCO1.ForeColor = System.Drawing.Color.White;
            this.rdFMDestVCO1.Name = "rdFMDestVCO1";
            this.rdFMDestVCO1.TabStop = true;
            this.rdFMDestVCO1.Tag = "0";
            this.rdFMDestVCO1.UseVisualStyleBackColor = false;
            this.rdFMDestVCO1.CheckedChanged += new System.EventHandler(this.RadioFMDest_CheckedChanged);
            // 
            // rdFMDestVCF
            // 
            resources.ApplyResources(this.rdFMDestVCF, "rdFMDestVCF");
            this.rdFMDestVCF.BackColor = System.Drawing.Color.Black;
            this.rdFMDestVCF.ForeColor = System.Drawing.Color.White;
            this.rdFMDestVCF.Name = "rdFMDestVCF";
            this.rdFMDestVCF.Tag = "1";
            this.rdFMDestVCF.UseVisualStyleBackColor = false;
            this.rdFMDestVCF.CheckedChanged += new System.EventHandler(this.RadioFMDest_CheckedChanged);
            // 
            // LAG_MODE_LEGATO
            // 
            resources.ApplyResources(this.LAG_MODE_LEGATO, "LAG_MODE_LEGATO");
            this.LAG_MODE_LEGATO.BackColor = System.Drawing.Color.Black;
            this.LAG_MODE_LEGATO.ForeColor = System.Drawing.Color.White;
            this.LAG_MODE_LEGATO.Maximum = 1;
            this.LAG_MODE_LEGATO.Minimum = 0;
            this.LAG_MODE_LEGATO.Name = "LAG_MODE_LEGATO";
            this.LAG_MODE_LEGATO.Step = 1;
            this.LAG_MODE_LEGATO.Tag = "LAG_MODE_LEGATO";
            this.LAG_MODE_LEGATO.UseVisualStyleBackColor = false;
            this.LAG_MODE_LEGATO.Value = 0;
            this.LAG_MODE_LEGATO.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_DEST_20
            // 
            this.MOD_DEST_20.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_20.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_20, "MOD_DEST_20");
            this.MOD_DEST_20.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_20.FormattingEnabled = true;
            this.MOD_DEST_20.Maximum = 1;
            this.MOD_DEST_20.Minimum = 0;
            this.MOD_DEST_20.Name = "MOD_DEST_20";
            this.MOD_DEST_20.Step = 1;
            this.MOD_DEST_20.Tag = "MOD_DEST_20";
            this.MOD_DEST_20.Value = 0;
            this.MOD_DEST_20.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_20.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_20
            // 
            this.MOD_SRC_20.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_20.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_20, "MOD_SRC_20");
            this.MOD_SRC_20.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_20.FormattingEnabled = true;
            this.MOD_SRC_20.Maximum = 27;
            this.MOD_SRC_20.Minimum = 0;
            this.MOD_SRC_20.Name = "MOD_SRC_20";
            this.MOD_SRC_20.Step = 1;
            this.MOD_SRC_20.Tag = "MOD_SRC_20";
            this.MOD_SRC_20.Value = 27;
            this.MOD_SRC_20.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_20.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // VCF_VCA_MOD
            // 
            this.VCF_VCA_MOD.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.VCF_VCA_MOD, "VCF_VCA_MOD");
            this.VCF_VCA_MOD.Controls.Add(this.label3);
            this.VCF_VCA_MOD.Controls.Add(this.VCF_MOD_VIB);
            this.VCF_VCA_MOD.Controls.Add(this.VCF_MOD_LEV1);
            this.VCF_VCA_MOD.Controls.Add(this.VCF_MOD_LAG);
            this.VCF_VCA_MOD.Controls.Add(this.VCF_MOD_KEYB);
            this.VCF_VCA_MOD.Name = "VCF_VCA_MOD";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Name = "label3";
            // 
            // VCF_MOD_VIB
            // 
            resources.ApplyResources(this.VCF_MOD_VIB, "VCF_MOD_VIB");
            this.VCF_MOD_VIB.BackColor = System.Drawing.Color.Black;
            this.VCF_MOD_VIB.ForeColor = System.Drawing.Color.White;
            this.VCF_MOD_VIB.Maximum = 1;
            this.VCF_MOD_VIB.Minimum = 0;
            this.VCF_MOD_VIB.Name = "VCF_MOD_VIB";
            this.VCF_MOD_VIB.Step = 1;
            this.VCF_MOD_VIB.Tag = "VCF_MOD_VIB";
            this.VCF_MOD_VIB.UseVisualStyleBackColor = false;
            this.VCF_MOD_VIB.Value = 0;
            this.VCF_MOD_VIB.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCF_MOD_LEV1
            // 
            resources.ApplyResources(this.VCF_MOD_LEV1, "VCF_MOD_LEV1");
            this.VCF_MOD_LEV1.BackColor = System.Drawing.Color.Black;
            this.VCF_MOD_LEV1.ForeColor = System.Drawing.Color.White;
            this.VCF_MOD_LEV1.Maximum = 1;
            this.VCF_MOD_LEV1.Minimum = 0;
            this.VCF_MOD_LEV1.Name = "VCF_MOD_LEV1";
            this.VCF_MOD_LEV1.Step = 1;
            this.VCF_MOD_LEV1.Tag = "VCF_MOD_LEV1";
            this.VCF_MOD_LEV1.UseVisualStyleBackColor = false;
            this.VCF_MOD_LEV1.Value = 0;
            this.VCF_MOD_LEV1.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCF_MOD_LAG
            // 
            resources.ApplyResources(this.VCF_MOD_LAG, "VCF_MOD_LAG");
            this.VCF_MOD_LAG.BackColor = System.Drawing.Color.Black;
            this.VCF_MOD_LAG.ForeColor = System.Drawing.Color.White;
            this.VCF_MOD_LAG.Maximum = 1;
            this.VCF_MOD_LAG.Minimum = 0;
            this.VCF_MOD_LAG.Name = "VCF_MOD_LAG";
            this.VCF_MOD_LAG.Step = 1;
            this.VCF_MOD_LAG.Tag = "VCF_MOD_LAG";
            this.VCF_MOD_LAG.UseVisualStyleBackColor = false;
            this.VCF_MOD_LAG.Value = 0;
            this.VCF_MOD_LAG.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCF_MOD_KEYB
            // 
            resources.ApplyResources(this.VCF_MOD_KEYB, "VCF_MOD_KEYB");
            this.VCF_MOD_KEYB.BackColor = System.Drawing.Color.Black;
            this.VCF_MOD_KEYB.ForeColor = System.Drawing.Color.White;
            this.VCF_MOD_KEYB.Maximum = 1;
            this.VCF_MOD_KEYB.Minimum = 0;
            this.VCF_MOD_KEYB.Name = "VCF_MOD_KEYB";
            this.VCF_MOD_KEYB.Step = 1;
            this.VCF_MOD_KEYB.Tag = "VCF_MOD_KEYB";
            this.VCF_MOD_KEYB.UseVisualStyleBackColor = false;
            this.VCF_MOD_KEYB.Value = 0;
            this.VCF_MOD_KEYB.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // LAG_TIMING_LINEAR_EXPO
            // 
            this.LAG_TIMING_LINEAR_EXPO.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.LAG_TIMING_LINEAR_EXPO, "LAG_TIMING_LINEAR_EXPO");
            this.LAG_TIMING_LINEAR_EXPO.Controls.Add(this.rdExpo);
            this.LAG_TIMING_LINEAR_EXPO.Controls.Add(this.rdLinear);
            this.LAG_TIMING_LINEAR_EXPO.Maximum = 1;
            this.LAG_TIMING_LINEAR_EXPO.Minimum = 0;
            this.LAG_TIMING_LINEAR_EXPO.Name = "LAG_TIMING_LINEAR_EXPO";
            this.LAG_TIMING_LINEAR_EXPO.Step = 1;
            this.LAG_TIMING_LINEAR_EXPO.Tag = "LAG_TIMING_LINEAR_EXPO";
            this.LAG_TIMING_LINEAR_EXPO.Value = 0;
            this.LAG_TIMING_LINEAR_EXPO.ValueChanged += new MidiApp.UIControls.RadioButtonPanel.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            // 
            // rdExpo
            // 
            resources.ApplyResources(this.rdExpo, "rdExpo");
            this.rdExpo.BackColor = System.Drawing.Color.Black;
            this.rdExpo.ForeColor = System.Drawing.Color.White;
            this.rdExpo.Name = "rdExpo";
            this.rdExpo.Tag = "1";
            this.rdExpo.UseVisualStyleBackColor = false;
            this.rdExpo.CheckedChanged += new System.EventHandler(this.RadioExpoLinear_CheckedChanged);
            // 
            // rdLinear
            // 
            resources.ApplyResources(this.rdLinear, "rdLinear");
            this.rdLinear.BackColor = System.Drawing.Color.Black;
            this.rdLinear.Checked = true;
            this.rdLinear.ForeColor = System.Drawing.Color.White;
            this.rdLinear.Name = "rdLinear";
            this.rdLinear.TabStop = true;
            this.rdLinear.Tag = "0";
            this.rdLinear.UseVisualStyleBackColor = false;
            this.rdLinear.CheckedChanged += new System.EventHandler(this.RadioExpoLinear_CheckedChanged);
            // 
            // MOD_AMNT_SRC_20
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_20, "MOD_AMNT_SRC_20");
            this.MOD_AMNT_SRC_20.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_20.Maximum = 63;
            this.MOD_AMNT_SRC_20.Minimum = 0;
            this.MOD_AMNT_SRC_20.Name = "MOD_AMNT_SRC_20";
            this.MOD_AMNT_SRC_20.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_20.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_20.Step = 1;
            this.MOD_AMNT_SRC_20.Tag = "MOD_AMNT_SRC_20";
            this.MOD_AMNT_SRC_20.Value = 0;
            this.MOD_AMNT_SRC_20.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_20.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_20.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_19
            // 
            this.MOD_DEST_19.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_19.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_19, "MOD_DEST_19");
            this.MOD_DEST_19.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_19.FormattingEnabled = true;
            this.MOD_DEST_19.Maximum = 1;
            this.MOD_DEST_19.Minimum = 0;
            this.MOD_DEST_19.Name = "MOD_DEST_19";
            this.MOD_DEST_19.Step = 1;
            this.MOD_DEST_19.Tag = "MOD_DEST_19";
            this.MOD_DEST_19.Value = 0;
            this.MOD_DEST_19.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_19.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // LAG_LINEAR_EQUAL_TIME
            // 
            resources.ApplyResources(this.LAG_LINEAR_EQUAL_TIME, "LAG_LINEAR_EQUAL_TIME");
            this.LAG_LINEAR_EQUAL_TIME.BackColor = System.Drawing.Color.Black;
            this.LAG_LINEAR_EQUAL_TIME.ForeColor = System.Drawing.Color.White;
            this.LAG_LINEAR_EQUAL_TIME.Maximum = 1;
            this.LAG_LINEAR_EQUAL_TIME.Minimum = 0;
            this.LAG_LINEAR_EQUAL_TIME.Name = "LAG_LINEAR_EQUAL_TIME";
            this.LAG_LINEAR_EQUAL_TIME.Step = 1;
            this.LAG_LINEAR_EQUAL_TIME.Tag = "LAG_LINEAR_EQUAL_TIME";
            this.LAG_LINEAR_EQUAL_TIME.UseVisualStyleBackColor = false;
            this.LAG_LINEAR_EQUAL_TIME.Value = 0;
            this.LAG_LINEAR_EQUAL_TIME.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO2_MODS
            // 
            this.VCO2_MODS.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.VCO2_MODS, "VCO2_MODS");
            this.VCO2_MODS.Controls.Add(this.VCO2_WAVE_SYNC);
            this.VCO2_MODS.Controls.Add(this.label2);
            this.VCO2_MODS.Controls.Add(this.VCO2_MOD_VIB);
            this.VCO2_MODS.Controls.Add(this.VCO2_MOD_LEV1);
            this.VCO2_MODS.Controls.Add(this.VCO2_MOD_LAG);
            this.VCO2_MODS.Controls.Add(this.VCO2_MOD_KEYB);
            this.VCO2_MODS.Name = "VCO2_MODS";
            // 
            // VCO2_WAVE_SYNC
            // 
            resources.ApplyResources(this.VCO2_WAVE_SYNC, "VCO2_WAVE_SYNC");
            this.VCO2_WAVE_SYNC.BackColor = System.Drawing.Color.Black;
            this.VCO2_WAVE_SYNC.ForeColor = System.Drawing.Color.White;
            this.VCO2_WAVE_SYNC.Maximum = 1;
            this.VCO2_WAVE_SYNC.Minimum = 0;
            this.VCO2_WAVE_SYNC.Name = "VCO2_WAVE_SYNC";
            this.VCO2_WAVE_SYNC.Step = 1;
            this.VCO2_WAVE_SYNC.Tag = "VCO2_WAVE_SYNC";
            this.VCO2_WAVE_SYNC.UseVisualStyleBackColor = false;
            this.VCO2_WAVE_SYNC.Value = 0;
            this.VCO2_WAVE_SYNC.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Name = "label2";
            // 
            // VCO2_MOD_VIB
            // 
            resources.ApplyResources(this.VCO2_MOD_VIB, "VCO2_MOD_VIB");
            this.VCO2_MOD_VIB.BackColor = System.Drawing.Color.Black;
            this.VCO2_MOD_VIB.ForeColor = System.Drawing.Color.White;
            this.VCO2_MOD_VIB.Maximum = 1;
            this.VCO2_MOD_VIB.Minimum = 0;
            this.VCO2_MOD_VIB.Name = "VCO2_MOD_VIB";
            this.VCO2_MOD_VIB.Step = 1;
            this.VCO2_MOD_VIB.Tag = "VCO2_MOD_VIB";
            this.VCO2_MOD_VIB.UseVisualStyleBackColor = false;
            this.VCO2_MOD_VIB.Value = 0;
            this.VCO2_MOD_VIB.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO2_MOD_LEV1
            // 
            resources.ApplyResources(this.VCO2_MOD_LEV1, "VCO2_MOD_LEV1");
            this.VCO2_MOD_LEV1.BackColor = System.Drawing.Color.Black;
            this.VCO2_MOD_LEV1.ForeColor = System.Drawing.Color.White;
            this.VCO2_MOD_LEV1.Maximum = 1;
            this.VCO2_MOD_LEV1.Minimum = 0;
            this.VCO2_MOD_LEV1.Name = "VCO2_MOD_LEV1";
            this.VCO2_MOD_LEV1.Step = 1;
            this.VCO2_MOD_LEV1.Tag = "VCO2_MOD_LEV1";
            this.VCO2_MOD_LEV1.UseVisualStyleBackColor = false;
            this.VCO2_MOD_LEV1.Value = 0;
            this.VCO2_MOD_LEV1.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO2_MOD_LAG
            // 
            resources.ApplyResources(this.VCO2_MOD_LAG, "VCO2_MOD_LAG");
            this.VCO2_MOD_LAG.BackColor = System.Drawing.Color.Black;
            this.VCO2_MOD_LAG.ForeColor = System.Drawing.Color.White;
            this.VCO2_MOD_LAG.Maximum = 1;
            this.VCO2_MOD_LAG.Minimum = 0;
            this.VCO2_MOD_LAG.Name = "VCO2_MOD_LAG";
            this.VCO2_MOD_LAG.Step = 1;
            this.VCO2_MOD_LAG.Tag = "VCO2_MOD_LAG";
            this.VCO2_MOD_LAG.UseVisualStyleBackColor = false;
            this.VCO2_MOD_LAG.Value = 0;
            this.VCO2_MOD_LAG.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO2_MOD_KEYB
            // 
            resources.ApplyResources(this.VCO2_MOD_KEYB, "VCO2_MOD_KEYB");
            this.VCO2_MOD_KEYB.BackColor = System.Drawing.Color.Black;
            this.VCO2_MOD_KEYB.ForeColor = System.Drawing.Color.White;
            this.VCO2_MOD_KEYB.Maximum = 1;
            this.VCO2_MOD_KEYB.Minimum = 0;
            this.VCO2_MOD_KEYB.Name = "VCO2_MOD_KEYB";
            this.VCO2_MOD_KEYB.Step = 1;
            this.VCO2_MOD_KEYB.Tag = "VCO2_MOD_KEYB";
            this.VCO2_MOD_KEYB.UseVisualStyleBackColor = false;
            this.VCO2_MOD_KEYB.Value = 0;
            this.VCO2_MOD_KEYB.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_SRC_19
            // 
            this.MOD_SRC_19.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_19.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_19, "MOD_SRC_19");
            this.MOD_SRC_19.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_19.FormattingEnabled = true;
            this.MOD_SRC_19.Maximum = 27;
            this.MOD_SRC_19.Minimum = 0;
            this.MOD_SRC_19.Name = "MOD_SRC_19";
            this.MOD_SRC_19.Step = 1;
            this.MOD_SRC_19.Tag = "MOD_SRC_19";
            this.MOD_SRC_19.Value = 27;
            this.MOD_SRC_19.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_19.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_19
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_19, "MOD_AMNT_SRC_19");
            this.MOD_AMNT_SRC_19.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_19.Maximum = 63;
            this.MOD_AMNT_SRC_19.Minimum = 0;
            this.MOD_AMNT_SRC_19.Name = "MOD_AMNT_SRC_19";
            this.MOD_AMNT_SRC_19.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_19.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_19.Step = 1;
            this.MOD_AMNT_SRC_19.Tag = "MOD_AMNT_SRC_19";
            this.MOD_AMNT_SRC_19.Value = 0;
            this.MOD_AMNT_SRC_19.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_19.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_19.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCO1_MODS
            // 
            this.VCO1_MODS.BackColor = System.Drawing.Color.Magenta;
            resources.ApplyResources(this.VCO1_MODS, "VCO1_MODS");
            this.VCO1_MODS.Controls.Add(this.label1);
            this.VCO1_MODS.Controls.Add(this.VCO1_MOD_VIB);
            this.VCO1_MODS.Controls.Add(this.VCO1_MOD_LEV1);
            this.VCO1_MODS.Controls.Add(this.VCO1_MOD_LAG);
            this.VCO1_MODS.Controls.Add(this.VCO1_MOD_KEYB);
            this.VCO1_MODS.Name = "VCO1_MODS";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Name = "label1";
            // 
            // VCO1_MOD_VIB
            // 
            resources.ApplyResources(this.VCO1_MOD_VIB, "VCO1_MOD_VIB");
            this.VCO1_MOD_VIB.BackColor = System.Drawing.Color.Black;
            this.VCO1_MOD_VIB.ForeColor = System.Drawing.Color.White;
            this.VCO1_MOD_VIB.Maximum = 1;
            this.VCO1_MOD_VIB.Minimum = 0;
            this.VCO1_MOD_VIB.Name = "VCO1_MOD_VIB";
            this.VCO1_MOD_VIB.Step = 1;
            this.VCO1_MOD_VIB.Tag = "VCO1_MOD_VIB";
            this.VCO1_MOD_VIB.UseVisualStyleBackColor = false;
            this.VCO1_MOD_VIB.Value = 0;
            this.VCO1_MOD_VIB.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO1_MOD_LEV1
            // 
            resources.ApplyResources(this.VCO1_MOD_LEV1, "VCO1_MOD_LEV1");
            this.VCO1_MOD_LEV1.BackColor = System.Drawing.Color.Black;
            this.VCO1_MOD_LEV1.ForeColor = System.Drawing.Color.White;
            this.VCO1_MOD_LEV1.Maximum = 1;
            this.VCO1_MOD_LEV1.Minimum = 0;
            this.VCO1_MOD_LEV1.Name = "VCO1_MOD_LEV1";
            this.VCO1_MOD_LEV1.Step = 1;
            this.VCO1_MOD_LEV1.Tag = "VCO1_MOD_LEV1";
            this.VCO1_MOD_LEV1.UseVisualStyleBackColor = false;
            this.VCO1_MOD_LEV1.Value = 0;
            this.VCO1_MOD_LEV1.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO1_MOD_LAG
            // 
            resources.ApplyResources(this.VCO1_MOD_LAG, "VCO1_MOD_LAG");
            this.VCO1_MOD_LAG.BackColor = System.Drawing.Color.Black;
            this.VCO1_MOD_LAG.ForeColor = System.Drawing.Color.White;
            this.VCO1_MOD_LAG.Maximum = 1;
            this.VCO1_MOD_LAG.Minimum = 0;
            this.VCO1_MOD_LAG.Name = "VCO1_MOD_LAG";
            this.VCO1_MOD_LAG.Step = 1;
            this.VCO1_MOD_LAG.Tag = "VCO1_MOD_LAG";
            this.VCO1_MOD_LAG.UseVisualStyleBackColor = false;
            this.VCO1_MOD_LAG.Value = 0;
            this.VCO1_MOD_LAG.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO1_MOD_KEYB
            // 
            resources.ApplyResources(this.VCO1_MOD_KEYB, "VCO1_MOD_KEYB");
            this.VCO1_MOD_KEYB.BackColor = System.Drawing.Color.Black;
            this.VCO1_MOD_KEYB.ForeColor = System.Drawing.Color.White;
            this.VCO1_MOD_KEYB.Maximum = 1;
            this.VCO1_MOD_KEYB.Minimum = 0;
            this.VCO1_MOD_KEYB.Name = "VCO1_MOD_KEYB";
            this.VCO1_MOD_KEYB.Step = 1;
            this.VCO1_MOD_KEYB.Tag = "VCO1_MOD_KEYB";
            this.VCO1_MOD_KEYB.UseVisualStyleBackColor = false;
            this.VCO1_MOD_KEYB.Value = 0;
            this.VCO1_MOD_KEYB.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO2_WAVESHAPE_PULSE
            // 
            resources.ApplyResources(this.VCO2_WAVESHAPE_PULSE, "VCO2_WAVESHAPE_PULSE");
            this.VCO2_WAVESHAPE_PULSE.BackColor = System.Drawing.Color.Black;
            this.VCO2_WAVESHAPE_PULSE.ForeColor = System.Drawing.Color.White;
            this.VCO2_WAVESHAPE_PULSE.Maximum = 1;
            this.VCO2_WAVESHAPE_PULSE.Minimum = 0;
            this.VCO2_WAVESHAPE_PULSE.Name = "VCO2_WAVESHAPE_PULSE";
            this.VCO2_WAVESHAPE_PULSE.Step = 1;
            this.VCO2_WAVESHAPE_PULSE.Tag = "VCO2_WAVESHAPE_PULSE";
            this.VCO2_WAVESHAPE_PULSE.UseVisualStyleBackColor = false;
            this.VCO2_WAVESHAPE_PULSE.Value = 0;
            this.VCO2_WAVESHAPE_PULSE.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_DEST_18
            // 
            this.MOD_DEST_18.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_18.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_18, "MOD_DEST_18");
            this.MOD_DEST_18.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_18.FormattingEnabled = true;
            this.MOD_DEST_18.Maximum = 1;
            this.MOD_DEST_18.Minimum = 0;
            this.MOD_DEST_18.Name = "MOD_DEST_18";
            this.MOD_DEST_18.Step = 1;
            this.MOD_DEST_18.Tag = "MOD_DEST_18";
            this.MOD_DEST_18.Value = 0;
            this.MOD_DEST_18.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_18.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_18
            // 
            this.MOD_SRC_18.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_18.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_18, "MOD_SRC_18");
            this.MOD_SRC_18.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_18.FormattingEnabled = true;
            this.MOD_SRC_18.Maximum = 27;
            this.MOD_SRC_18.Minimum = 0;
            this.MOD_SRC_18.Name = "MOD_SRC_18";
            this.MOD_SRC_18.Step = 1;
            this.MOD_SRC_18.Tag = "MOD_SRC_18";
            this.MOD_SRC_18.Value = 27;
            this.MOD_SRC_18.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_18.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_18
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_18, "MOD_AMNT_SRC_18");
            this.MOD_AMNT_SRC_18.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_18.Maximum = 63;
            this.MOD_AMNT_SRC_18.Minimum = 0;
            this.MOD_AMNT_SRC_18.Name = "MOD_AMNT_SRC_18";
            this.MOD_AMNT_SRC_18.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_18.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_18.Step = 1;
            this.MOD_AMNT_SRC_18.Tag = "MOD_AMNT_SRC_18";
            this.MOD_AMNT_SRC_18.Value = 0;
            this.MOD_AMNT_SRC_18.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_18.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_18.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // PanelTRACK
            // 
            resources.ApplyResources(this.PanelTRACK, "PanelTRACK");
            this.PanelTRACK.Controls.Add(this.TRACK_1);
            this.PanelTRACK.Controls.Add(this.TRACK_2);
            this.PanelTRACK.Controls.Add(this.TRACK_3);
            this.PanelTRACK.Name = "PanelTRACK";
            // 
            // TRACK_1
            // 
            resources.ApplyResources(this.TRACK_1, "TRACK_1");
            this.TRACK_1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.TRACK_1.Checked = true;
            this.TRACK_1.ContextMenuStrip = this.PageContextMenuStrip;
            this.TRACK_1.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.TRACK_1.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_1.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_1.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.TRACK_1.Name = "TRACK_1";
            this.TRACK_1.TabStop = true;
            this.TRACK_1.Tag = "TRACK_1";
            this.TRACK_1.UseVisualStyleBackColor = false;
            this.TRACK_1.Click += new System.EventHandler(this.Radio_TRACK_X_Click);
            // 
            // TRACK_2
            // 
            resources.ApplyResources(this.TRACK_2, "TRACK_2");
            this.TRACK_2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.TRACK_2.ContextMenuStrip = this.PageContextMenuStrip;
            this.TRACK_2.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.TRACK_2.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_2.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_2.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.TRACK_2.Name = "TRACK_2";
            this.TRACK_2.Tag = "TRACK_2";
            this.TRACK_2.UseVisualStyleBackColor = false;
            this.TRACK_2.Click += new System.EventHandler(this.Radio_TRACK_X_Click);
            // 
            // TRACK_3
            // 
            resources.ApplyResources(this.TRACK_3, "TRACK_3");
            this.TRACK_3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.TRACK_3.ContextMenuStrip = this.PageContextMenuStrip;
            this.TRACK_3.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.TRACK_3.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_3.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_3.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.TRACK_3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.TRACK_3.Name = "TRACK_3";
            this.TRACK_3.Tag = "TRACK_3";
            this.TRACK_3.UseVisualStyleBackColor = false;
            this.TRACK_3.Click += new System.EventHandler(this.Radio_TRACK_X_Click);
            // 
            // VCO2_WAVESHAPE_NOISE
            // 
            resources.ApplyResources(this.VCO2_WAVESHAPE_NOISE, "VCO2_WAVESHAPE_NOISE");
            this.VCO2_WAVESHAPE_NOISE.BackColor = System.Drawing.Color.Black;
            this.VCO2_WAVESHAPE_NOISE.ForeColor = System.Drawing.Color.White;
            this.VCO2_WAVESHAPE_NOISE.Maximum = 1;
            this.VCO2_WAVESHAPE_NOISE.Minimum = 0;
            this.VCO2_WAVESHAPE_NOISE.Name = "VCO2_WAVESHAPE_NOISE";
            this.VCO2_WAVESHAPE_NOISE.Step = 1;
            this.VCO2_WAVESHAPE_NOISE.Tag = "VCO2_WAVESHAPE_NOISE";
            this.VCO2_WAVESHAPE_NOISE.UseVisualStyleBackColor = false;
            this.VCO2_WAVESHAPE_NOISE.Value = 0;
            this.VCO2_WAVESHAPE_NOISE.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_DEST_17
            // 
            this.MOD_DEST_17.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_17.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_17, "MOD_DEST_17");
            this.MOD_DEST_17.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_17.FormattingEnabled = true;
            this.MOD_DEST_17.Maximum = 1;
            this.MOD_DEST_17.Minimum = 0;
            this.MOD_DEST_17.Name = "MOD_DEST_17";
            this.MOD_DEST_17.Step = 1;
            this.MOD_DEST_17.Tag = "MOD_DEST_17";
            this.MOD_DEST_17.Value = 0;
            this.MOD_DEST_17.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_17.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // PanelRAMP
            // 
            resources.ApplyResources(this.PanelRAMP, "PanelRAMP");
            this.PanelRAMP.Controls.Add(this.RAMP_1);
            this.PanelRAMP.Controls.Add(this.RAMP_2);
            this.PanelRAMP.Controls.Add(this.RAMP_3);
            this.PanelRAMP.Controls.Add(this.RAMP_4);
            this.PanelRAMP.Name = "PanelRAMP";
            // 
            // RAMP_1
            // 
            resources.ApplyResources(this.RAMP_1, "RAMP_1");
            this.RAMP_1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.RAMP_1.Checked = true;
            this.RAMP_1.ContextMenuStrip = this.PageContextMenuStrip;
            this.RAMP_1.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_1.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_1.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_1.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_1.Name = "RAMP_1";
            this.RAMP_1.TabStop = true;
            this.RAMP_1.Tag = "RAMP_1";
            this.RAMP_1.UseVisualStyleBackColor = false;
            this.RAMP_1.Click += new System.EventHandler(this.Radio_RAMP_X_Click);
            // 
            // RAMP_2
            // 
            resources.ApplyResources(this.RAMP_2, "RAMP_2");
            this.RAMP_2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.RAMP_2.ContextMenuStrip = this.PageContextMenuStrip;
            this.RAMP_2.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_2.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_2.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_2.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_2.Name = "RAMP_2";
            this.RAMP_2.Tag = "RAMP_2";
            this.RAMP_2.UseVisualStyleBackColor = false;
            this.RAMP_2.Click += new System.EventHandler(this.Radio_RAMP_X_Click);
            // 
            // RAMP_3
            // 
            resources.ApplyResources(this.RAMP_3, "RAMP_3");
            this.RAMP_3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.RAMP_3.ContextMenuStrip = this.PageContextMenuStrip;
            this.RAMP_3.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_3.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_3.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_3.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_3.Name = "RAMP_3";
            this.RAMP_3.Tag = "RAMP_3";
            this.RAMP_3.UseVisualStyleBackColor = false;
            this.RAMP_3.Click += new System.EventHandler(this.Radio_RAMP_X_Click);
            // 
            // RAMP_4
            // 
            resources.ApplyResources(this.RAMP_4, "RAMP_4");
            this.RAMP_4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.RAMP_4.ContextMenuStrip = this.PageContextMenuStrip;
            this.RAMP_4.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_4.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_4.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_4.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.RAMP_4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.RAMP_4.Name = "RAMP_4";
            this.RAMP_4.Tag = "RAMP_4";
            this.RAMP_4.UseVisualStyleBackColor = false;
            this.RAMP_4.Click += new System.EventHandler(this.Radio_RAMP_X_Click);
            // 
            // MOD_SRC_17
            // 
            this.MOD_SRC_17.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_17.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_17, "MOD_SRC_17");
            this.MOD_SRC_17.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_17.FormattingEnabled = true;
            this.MOD_SRC_17.Maximum = 27;
            this.MOD_SRC_17.Minimum = 0;
            this.MOD_SRC_17.Name = "MOD_SRC_17";
            this.MOD_SRC_17.Step = 1;
            this.MOD_SRC_17.Tag = "MOD_SRC_17";
            this.MOD_SRC_17.Value = 27;
            this.MOD_SRC_17.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_17.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // VCO2_WAVESHAPE_SAW
            // 
            resources.ApplyResources(this.VCO2_WAVESHAPE_SAW, "VCO2_WAVESHAPE_SAW");
            this.VCO2_WAVESHAPE_SAW.BackColor = System.Drawing.Color.Black;
            this.VCO2_WAVESHAPE_SAW.ForeColor = System.Drawing.Color.White;
            this.VCO2_WAVESHAPE_SAW.Maximum = 1;
            this.VCO2_WAVESHAPE_SAW.Minimum = 0;
            this.VCO2_WAVESHAPE_SAW.Name = "VCO2_WAVESHAPE_SAW";
            this.VCO2_WAVESHAPE_SAW.Step = 1;
            this.VCO2_WAVESHAPE_SAW.Tag = "VCO2_WAVESHAPE_SAW";
            this.VCO2_WAVESHAPE_SAW.UseVisualStyleBackColor = false;
            this.VCO2_WAVESHAPE_SAW.Value = 0;
            this.VCO2_WAVESHAPE_SAW.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // VCO2_WAVESHAPE_TRI
            // 
            resources.ApplyResources(this.VCO2_WAVESHAPE_TRI, "VCO2_WAVESHAPE_TRI");
            this.VCO2_WAVESHAPE_TRI.BackColor = System.Drawing.Color.Black;
            this.VCO2_WAVESHAPE_TRI.ForeColor = System.Drawing.Color.White;
            this.VCO2_WAVESHAPE_TRI.Maximum = 1;
            this.VCO2_WAVESHAPE_TRI.Minimum = 0;
            this.VCO2_WAVESHAPE_TRI.Name = "VCO2_WAVESHAPE_TRI";
            this.VCO2_WAVESHAPE_TRI.Step = 1;
            this.VCO2_WAVESHAPE_TRI.Tag = "VCO2_WAVESHAPE_TRI";
            this.VCO2_WAVESHAPE_TRI.UseVisualStyleBackColor = false;
            this.VCO2_WAVESHAPE_TRI.Value = 0;
            this.VCO2_WAVESHAPE_TRI.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_AMNT_SRC_17
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_17, "MOD_AMNT_SRC_17");
            this.MOD_AMNT_SRC_17.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_17.Maximum = 63;
            this.MOD_AMNT_SRC_17.Minimum = 0;
            this.MOD_AMNT_SRC_17.Name = "MOD_AMNT_SRC_17";
            this.MOD_AMNT_SRC_17.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_17.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_17.Step = 1;
            this.MOD_AMNT_SRC_17.Tag = "MOD_AMNT_SRC_17";
            this.MOD_AMNT_SRC_17.Value = 0;
            this.MOD_AMNT_SRC_17.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_17.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_17.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCO1_WAVESHAPE_PULSE
            // 
            resources.ApplyResources(this.VCO1_WAVESHAPE_PULSE, "VCO1_WAVESHAPE_PULSE");
            this.VCO1_WAVESHAPE_PULSE.ForeColor = System.Drawing.Color.White;
            this.VCO1_WAVESHAPE_PULSE.Maximum = 1;
            this.VCO1_WAVESHAPE_PULSE.Minimum = 0;
            this.VCO1_WAVESHAPE_PULSE.Name = "VCO1_WAVESHAPE_PULSE";
            this.VCO1_WAVESHAPE_PULSE.Step = 1;
            this.VCO1_WAVESHAPE_PULSE.Tag = "VCO1_WAVESHAPE_PULSE";
            this.VCO1_WAVESHAPE_PULSE.UseVisualStyleBackColor = false;
            this.VCO1_WAVESHAPE_PULSE.Value = 0;
            this.VCO1_WAVESHAPE_PULSE.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_DEST_16
            // 
            this.MOD_DEST_16.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_16.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_16, "MOD_DEST_16");
            this.MOD_DEST_16.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_16.FormattingEnabled = true;
            this.MOD_DEST_16.Maximum = 1;
            this.MOD_DEST_16.Minimum = 0;
            this.MOD_DEST_16.Name = "MOD_DEST_16";
            this.MOD_DEST_16.Step = 1;
            this.MOD_DEST_16.Tag = "MOD_DEST_16";
            this.MOD_DEST_16.Value = 0;
            this.MOD_DEST_16.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_16.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_16
            // 
            this.MOD_SRC_16.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_16.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_16, "MOD_SRC_16");
            this.MOD_SRC_16.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_16.FormattingEnabled = true;
            this.MOD_SRC_16.Maximum = 27;
            this.MOD_SRC_16.Minimum = 0;
            this.MOD_SRC_16.Name = "MOD_SRC_16";
            this.MOD_SRC_16.Step = 1;
            this.MOD_SRC_16.Tag = "MOD_SRC_16";
            this.MOD_SRC_16.Value = 27;
            this.MOD_SRC_16.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_16.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // RAMP_X_RATE
            // 
            resources.ApplyResources(this.RAMP_X_RATE, "RAMP_X_RATE");
            this.RAMP_X_RATE.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.RAMP_X_RATE.Maximum = 63;
            this.RAMP_X_RATE.Minimum = 0;
            this.RAMP_X_RATE.Name = "RAMP_X_RATE";
            this.RAMP_X_RATE.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.RAMP_X_RATE.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.RAMP_X_RATE.Step = 1;
            this.RAMP_X_RATE.Tag = "RAMP_X_RATE";
            this.RAMP_X_RATE.Value = 0;
            this.RAMP_X_RATE.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.RAMP_X_RATE.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.RAMP_X_RATE.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_15
            // 
            this.MOD_DEST_15.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_15.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_15, "MOD_DEST_15");
            this.MOD_DEST_15.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_15.FormattingEnabled = true;
            this.MOD_DEST_15.Maximum = 1;
            this.MOD_DEST_15.Minimum = 0;
            this.MOD_DEST_15.Name = "MOD_DEST_15";
            this.MOD_DEST_15.Step = 1;
            this.MOD_DEST_15.Tag = "MOD_DEST_15";
            this.MOD_DEST_15.Value = 0;
            this.MOD_DEST_15.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_15.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_16
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_16, "MOD_AMNT_SRC_16");
            this.MOD_AMNT_SRC_16.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_16.Maximum = 63;
            this.MOD_AMNT_SRC_16.Minimum = 0;
            this.MOD_AMNT_SRC_16.Name = "MOD_AMNT_SRC_16";
            this.MOD_AMNT_SRC_16.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_16.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_16.Step = 1;
            this.MOD_AMNT_SRC_16.Tag = "MOD_AMNT_SRC_16";
            this.MOD_AMNT_SRC_16.Value = 0;
            this.MOD_AMNT_SRC_16.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_16.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_16.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_15
            // 
            this.MOD_SRC_15.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_15.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_15, "MOD_SRC_15");
            this.MOD_SRC_15.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_15.FormattingEnabled = true;
            this.MOD_SRC_15.Maximum = 27;
            this.MOD_SRC_15.Minimum = 0;
            this.MOD_SRC_15.Name = "MOD_SRC_15";
            this.MOD_SRC_15.Step = 1;
            this.MOD_SRC_15.Tag = "MOD_SRC_15";
            this.MOD_SRC_15.Value = 27;
            this.MOD_SRC_15.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_15.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // LFO_X_AMP
            // 
            resources.ApplyResources(this.LFO_X_AMP, "LFO_X_AMP");
            this.LFO_X_AMP.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.LFO_X_AMP.Maximum = 63;
            this.LFO_X_AMP.Minimum = 0;
            this.LFO_X_AMP.Name = "LFO_X_AMP";
            this.LFO_X_AMP.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.LFO_X_AMP.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.LFO_X_AMP.Step = 1;
            this.LFO_X_AMP.Tag = "LFO_X_AMP";
            this.LFO_X_AMP.Value = 0;
            this.LFO_X_AMP.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.LFO_X_AMP.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.LFO_X_AMP.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // LFO_X_RETRIG
            // 
            resources.ApplyResources(this.LFO_X_RETRIG, "LFO_X_RETRIG");
            this.LFO_X_RETRIG.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.LFO_X_RETRIG.Maximum = 127;
            this.LFO_X_RETRIG.Minimum = 0;
            this.LFO_X_RETRIG.Name = "LFO_X_RETRIG";
            this.LFO_X_RETRIG.PredefinedValues = new int[] {
        0,
        15,
        29,
        43,
        57,
        71,
        85,
        99,
        113,
        127};
            this.LFO_X_RETRIG.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.LFO_X_RETRIG.Step = 1;
            this.LFO_X_RETRIG.Tag = "LFO_X_RETRIG";
            this.LFO_X_RETRIG.Value = 0;
            this.LFO_X_RETRIG.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.LFO_X_RETRIG.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.LFO_X_RETRIG.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_15
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_15, "MOD_AMNT_SRC_15");
            this.MOD_AMNT_SRC_15.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_15.Maximum = 63;
            this.MOD_AMNT_SRC_15.Minimum = 0;
            this.MOD_AMNT_SRC_15.Name = "MOD_AMNT_SRC_15";
            this.MOD_AMNT_SRC_15.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_15.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_15.Step = 1;
            this.MOD_AMNT_SRC_15.Tag = "MOD_AMNT_SRC_15";
            this.MOD_AMNT_SRC_15.Value = 0;
            this.MOD_AMNT_SRC_15.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_15.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_15.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // LFO_X_SPEED
            // 
            resources.ApplyResources(this.LFO_X_SPEED, "LFO_X_SPEED");
            this.LFO_X_SPEED.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.LFO_X_SPEED.Maximum = 63;
            this.LFO_X_SPEED.Minimum = 0;
            this.LFO_X_SPEED.Name = "LFO_X_SPEED";
            this.LFO_X_SPEED.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.LFO_X_SPEED.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.LFO_X_SPEED.Step = 1;
            this.LFO_X_SPEED.Tag = "LFO_X_SPEED";
            this.LFO_X_SPEED.Value = 0;
            this.LFO_X_SPEED.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.LFO_X_SPEED.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.LFO_X_SPEED.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_14
            // 
            this.MOD_DEST_14.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_14.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_14, "MOD_DEST_14");
            this.MOD_DEST_14.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_14.FormattingEnabled = true;
            this.MOD_DEST_14.Maximum = 1;
            this.MOD_DEST_14.Minimum = 0;
            this.MOD_DEST_14.Name = "MOD_DEST_14";
            this.MOD_DEST_14.Step = 1;
            this.MOD_DEST_14.Tag = "MOD_DEST_14";
            this.MOD_DEST_14.Value = 0;
            this.MOD_DEST_14.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_14.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // VCO1_WAVESHAPE_SAW
            // 
            resources.ApplyResources(this.VCO1_WAVESHAPE_SAW, "VCO1_WAVESHAPE_SAW");
            this.VCO1_WAVESHAPE_SAW.BackColor = System.Drawing.Color.Black;
            this.VCO1_WAVESHAPE_SAW.ForeColor = System.Drawing.Color.White;
            this.VCO1_WAVESHAPE_SAW.Maximum = 1;
            this.VCO1_WAVESHAPE_SAW.Minimum = 0;
            this.VCO1_WAVESHAPE_SAW.Name = "VCO1_WAVESHAPE_SAW";
            this.VCO1_WAVESHAPE_SAW.Step = 1;
            this.VCO1_WAVESHAPE_SAW.Tag = "VCO1_WAVESHAPE_SAW";
            this.VCO1_WAVESHAPE_SAW.UseVisualStyleBackColor = false;
            this.VCO1_WAVESHAPE_SAW.Value = 0;
            this.VCO1_WAVESHAPE_SAW.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // MOD_SRC_14
            // 
            this.MOD_SRC_14.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_14.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_14, "MOD_SRC_14");
            this.MOD_SRC_14.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_14.FormattingEnabled = true;
            this.MOD_SRC_14.Maximum = 27;
            this.MOD_SRC_14.Minimum = 0;
            this.MOD_SRC_14.Name = "MOD_SRC_14";
            this.MOD_SRC_14.Step = 1;
            this.MOD_SRC_14.Tag = "MOD_SRC_14";
            this.MOD_SRC_14.Value = 27;
            this.MOD_SRC_14.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_14.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_14
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_14, "MOD_AMNT_SRC_14");
            this.MOD_AMNT_SRC_14.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_14.Maximum = 63;
            this.MOD_AMNT_SRC_14.Minimum = 0;
            this.MOD_AMNT_SRC_14.Name = "MOD_AMNT_SRC_14";
            this.MOD_AMNT_SRC_14.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_14.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_14.Step = 1;
            this.MOD_AMNT_SRC_14.Tag = "MOD_AMNT_SRC_14";
            this.MOD_AMNT_SRC_14.Value = 0;
            this.MOD_AMNT_SRC_14.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_14.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_14.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCO1_WAVESHAPE_TRI
            // 
            resources.ApplyResources(this.VCO1_WAVESHAPE_TRI, "VCO1_WAVESHAPE_TRI");
            this.VCO1_WAVESHAPE_TRI.BackColor = System.Drawing.Color.Black;
            this.VCO1_WAVESHAPE_TRI.ForeColor = System.Drawing.Color.White;
            this.VCO1_WAVESHAPE_TRI.Maximum = 1;
            this.VCO1_WAVESHAPE_TRI.Minimum = 0;
            this.VCO1_WAVESHAPE_TRI.Name = "VCO1_WAVESHAPE_TRI";
            this.VCO1_WAVESHAPE_TRI.Step = 1;
            this.VCO1_WAVESHAPE_TRI.Tag = "VCO1_WAVESHAPE_TRI";
            this._toolTip.SetToolTip(this.VCO1_WAVESHAPE_TRI, resources.GetString("VCO1_WAVESHAPE_TRI.ToolTip"));
            this.VCO1_WAVESHAPE_TRI.UseVisualStyleBackColor = false;
            this.VCO1_WAVESHAPE_TRI.Value = 0;
            this.VCO1_WAVESHAPE_TRI.CheckedChanged += new System.EventHandler(this.CheckBoxValuedControl_CheckedChanged);
            // 
            // FMLAG_RATE
            // 
            resources.ApplyResources(this.FMLAG_RATE, "FMLAG_RATE");
            this.FMLAG_RATE.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.FMLAG_RATE.Maximum = 63;
            this.FMLAG_RATE.Minimum = 0;
            this.FMLAG_RATE.Name = "FMLAG_RATE";
            this.FMLAG_RATE.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.FMLAG_RATE.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.FMLAG_RATE.Step = 1;
            this.FMLAG_RATE.Tag = "FMLAG_RATE";
            this.FMLAG_RATE.Value = 0;
            this.FMLAG_RATE.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.FMLAG_RATE.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.FMLAG_RATE.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_13
            // 
            this.MOD_DEST_13.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_13.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_13, "MOD_DEST_13");
            this.MOD_DEST_13.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_13.FormattingEnabled = true;
            this.MOD_DEST_13.Maximum = 1;
            this.MOD_DEST_13.Minimum = 0;
            this.MOD_DEST_13.Name = "MOD_DEST_13";
            this.MOD_DEST_13.Step = 1;
            this.MOD_DEST_13.Tag = "MOD_DEST_13";
            this.MOD_DEST_13.Value = 0;
            this.MOD_DEST_13.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_13.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_13
            // 
            this.MOD_SRC_13.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_13.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_13, "MOD_SRC_13");
            this.MOD_SRC_13.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_13.FormattingEnabled = true;
            this.MOD_SRC_13.Maximum = 27;
            this.MOD_SRC_13.Minimum = 0;
            this.MOD_SRC_13.Name = "MOD_SRC_13";
            this.MOD_SRC_13.Step = 1;
            this.MOD_SRC_13.Tag = "MOD_SRC_13";
            this.MOD_SRC_13.Value = 27;
            this.MOD_SRC_13.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_13.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // FM_AMP
            // 
            resources.ApplyResources(this.FM_AMP, "FM_AMP");
            this.FM_AMP.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.FM_AMP.Maximum = 63;
            this.FM_AMP.Minimum = 0;
            this.FM_AMP.Name = "FM_AMP";
            this.FM_AMP.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.FM_AMP.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.FM_AMP.Step = 1;
            this.FM_AMP.Tag = "FM_AMP";
            this.FM_AMP.Value = 0;
            this.FM_AMP.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.FM_AMP.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.FM_AMP.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // TRACK_X_PT5
            // 
            resources.ApplyResources(this.TRACK_X_PT5, "TRACK_X_PT5");
            this.TRACK_X_PT5.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.TRACK_X_PT5.Maximum = 63;
            this.TRACK_X_PT5.Minimum = 0;
            this.TRACK_X_PT5.Name = "TRACK_X_PT5";
            this.TRACK_X_PT5.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.TRACK_X_PT5.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.TRACK_X_PT5.Step = 1;
            this.TRACK_X_PT5.Tag = "TRACK_X_POINT_5";
            this.TRACK_X_PT5.Value = 0;
            this.TRACK_X_PT5.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.TRACK_X_PT5.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.TRACK_X_PT5.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_13
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_13, "MOD_AMNT_SRC_13");
            this.MOD_AMNT_SRC_13.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_13.Maximum = 63;
            this.MOD_AMNT_SRC_13.Minimum = 0;
            this.MOD_AMNT_SRC_13.Name = "MOD_AMNT_SRC_13";
            this.MOD_AMNT_SRC_13.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_13.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_13.Step = 1;
            this.MOD_AMNT_SRC_13.Tag = "MOD_AMNT_SRC_13";
            this.MOD_AMNT_SRC_13.Value = 0;
            this.MOD_AMNT_SRC_13.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_13.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_13.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_12
            // 
            this.MOD_DEST_12.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_12.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_12, "MOD_DEST_12");
            this.MOD_DEST_12.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_12.FormattingEnabled = true;
            this.MOD_DEST_12.Maximum = 1;
            this.MOD_DEST_12.Minimum = 0;
            this.MOD_DEST_12.Name = "MOD_DEST_12";
            this.MOD_DEST_12.Step = 1;
            this.MOD_DEST_12.Tag = "MOD_DEST_12";
            this.MOD_DEST_12.Value = 0;
            this.MOD_DEST_12.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_12.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_12
            // 
            this.MOD_SRC_12.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_12.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_12, "MOD_SRC_12");
            this.MOD_SRC_12.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_12.FormattingEnabled = true;
            this.MOD_SRC_12.Maximum = 27;
            this.MOD_SRC_12.Minimum = 0;
            this.MOD_SRC_12.Name = "MOD_SRC_12";
            this.MOD_SRC_12.Step = 1;
            this.MOD_SRC_12.Tag = "MOD_SRC_12";
            this.MOD_SRC_12.Value = 27;
            this.MOD_SRC_12.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_12.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // TRACK_X_PT4
            // 
            resources.ApplyResources(this.TRACK_X_PT4, "TRACK_X_PT4");
            this.TRACK_X_PT4.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.TRACK_X_PT4.Maximum = 63;
            this.TRACK_X_PT4.Minimum = 0;
            this.TRACK_X_PT4.Name = "TRACK_X_PT4";
            this.TRACK_X_PT4.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.TRACK_X_PT4.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.TRACK_X_PT4.Step = 1;
            this.TRACK_X_PT4.Tag = "TRACK_X_POINT_4";
            this.TRACK_X_PT4.Value = 0;
            this.TRACK_X_PT4.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.TRACK_X_PT4.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.TRACK_X_PT4.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // PanelLFO
            // 
            resources.ApplyResources(this.PanelLFO, "PanelLFO");
            this.PanelLFO.Controls.Add(this.LFO_5);
            this.PanelLFO.Controls.Add(this.LFO_1);
            this.PanelLFO.Controls.Add(this.LFO_4);
            this.PanelLFO.Controls.Add(this.LFO_2);
            this.PanelLFO.Controls.Add(this.LFO_3);
            this.PanelLFO.Name = "PanelLFO";
            // 
            // LFO_5
            // 
            resources.ApplyResources(this.LFO_5, "LFO_5");
            this.LFO_5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_5.ContextMenuStrip = this.PageContextMenuStrip;
            this.LFO_5.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_5.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_5.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_5.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_5.Name = "LFO_5";
            this.LFO_5.Tag = "LFO_5";
            this.LFO_5.UseVisualStyleBackColor = false;
            this.LFO_5.Click += new System.EventHandler(this.Radio_LFO_X_Click);
            // 
            // LFO_1
            // 
            resources.ApplyResources(this.LFO_1, "LFO_1");
            this.LFO_1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_1.Checked = true;
            this.LFO_1.ContextMenuStrip = this.PageContextMenuStrip;
            this.LFO_1.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_1.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_1.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_1.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_1.Name = "LFO_1";
            this.LFO_1.TabStop = true;
            this.LFO_1.Tag = "LFO_1";
            this.LFO_1.UseVisualStyleBackColor = false;
            this.LFO_1.Click += new System.EventHandler(this.Radio_LFO_X_Click);
            // 
            // LFO_4
            // 
            resources.ApplyResources(this.LFO_4, "LFO_4");
            this.LFO_4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_4.ContextMenuStrip = this.PageContextMenuStrip;
            this.LFO_4.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_4.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_4.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_4.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_4.Name = "LFO_4";
            this.LFO_4.Tag = "LFO_4";
            this.LFO_4.UseVisualStyleBackColor = false;
            this.LFO_4.Click += new System.EventHandler(this.Radio_LFO_X_Click);
            // 
            // LFO_2
            // 
            resources.ApplyResources(this.LFO_2, "LFO_2");
            this.LFO_2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_2.ContextMenuStrip = this.PageContextMenuStrip;
            this.LFO_2.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_2.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_2.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_2.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_2.Name = "LFO_2";
            this.LFO_2.Tag = "LFO_2";
            this.LFO_2.UseVisualStyleBackColor = false;
            this.LFO_2.Click += new System.EventHandler(this.Radio_LFO_X_Click);
            // 
            // LFO_3
            // 
            resources.ApplyResources(this.LFO_3, "LFO_3");
            this.LFO_3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.LFO_3.ContextMenuStrip = this.PageContextMenuStrip;
            this.LFO_3.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_3.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_3.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_3.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.LFO_3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.LFO_3.Name = "LFO_3";
            this.LFO_3.Tag = "LFO_3";
            this.LFO_3.UseVisualStyleBackColor = false;
            this.LFO_3.Click += new System.EventHandler(this.Radio_LFO_X_Click);
            // 
            // TRACK_X_PT3
            // 
            resources.ApplyResources(this.TRACK_X_PT3, "TRACK_X_PT3");
            this.TRACK_X_PT3.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.TRACK_X_PT3.Maximum = 63;
            this.TRACK_X_PT3.Minimum = 0;
            this.TRACK_X_PT3.Name = "TRACK_X_PT3";
            this.TRACK_X_PT3.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.TRACK_X_PT3.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.TRACK_X_PT3.Step = 1;
            this.TRACK_X_PT3.Tag = "TRACK_X_POINT_3";
            this.TRACK_X_PT3.Value = 0;
            this.TRACK_X_PT3.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.TRACK_X_PT3.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.TRACK_X_PT3.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_12
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_12, "MOD_AMNT_SRC_12");
            this.MOD_AMNT_SRC_12.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_12.Maximum = 63;
            this.MOD_AMNT_SRC_12.Minimum = 0;
            this.MOD_AMNT_SRC_12.Name = "MOD_AMNT_SRC_12";
            this.MOD_AMNT_SRC_12.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_12.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_12.Step = 1;
            this.MOD_AMNT_SRC_12.Tag = "MOD_AMNT_SRC_12";
            this.MOD_AMNT_SRC_12.Value = 0;
            this.MOD_AMNT_SRC_12.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_12.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_12.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // TRACK_X_PT2
            // 
            resources.ApplyResources(this.TRACK_X_PT2, "TRACK_X_PT2");
            this.TRACK_X_PT2.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.TRACK_X_PT2.Maximum = 63;
            this.TRACK_X_PT2.Minimum = 0;
            this.TRACK_X_PT2.Name = "TRACK_X_PT2";
            this.TRACK_X_PT2.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.TRACK_X_PT2.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.TRACK_X_PT2.Step = 1;
            this.TRACK_X_PT2.Tag = "TRACK_X_POINT_2";
            this.TRACK_X_PT2.Value = 0;
            this.TRACK_X_PT2.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.TRACK_X_PT2.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.TRACK_X_PT2.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_11
            // 
            this.MOD_DEST_11.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_11.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_11, "MOD_DEST_11");
            this.MOD_DEST_11.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_11.FormattingEnabled = true;
            this.MOD_DEST_11.Maximum = 1;
            this.MOD_DEST_11.Minimum = 0;
            this.MOD_DEST_11.Name = "MOD_DEST_11";
            this.MOD_DEST_11.Step = 1;
            this.MOD_DEST_11.Tag = "MOD_DEST_11";
            this.MOD_DEST_11.Value = 0;
            this.MOD_DEST_11.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_11.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // TRACK_X_PT1
            // 
            resources.ApplyResources(this.TRACK_X_PT1, "TRACK_X_PT1");
            this.TRACK_X_PT1.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.TRACK_X_PT1.Maximum = 63;
            this.TRACK_X_PT1.Minimum = 0;
            this.TRACK_X_PT1.Name = "TRACK_X_PT1";
            this.TRACK_X_PT1.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.TRACK_X_PT1.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.TRACK_X_PT1.Step = 1;
            this.TRACK_X_PT1.Tag = "TRACK_X_POINT_1";
            this.TRACK_X_PT1.Value = 0;
            this.TRACK_X_PT1.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.TRACK_X_PT1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.TRACK_X_PT1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCO2_PW
            // 
            resources.ApplyResources(this.VCO2_PW, "VCO2_PW");
            this.VCO2_PW.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO2_PW.Maximum = 63;
            this.VCO2_PW.Minimum = 0;
            this.VCO2_PW.Name = "VCO2_PW";
            this.VCO2_PW.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCO2_PW.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO2_PW.Step = 1;
            this.VCO2_PW.Tag = "VCO2_PW";
            this.VCO2_PW.Value = 0;
            this.VCO2_PW.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO2_PW.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO2_PW.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_11
            // 
            this.MOD_SRC_11.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_11.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_11, "MOD_SRC_11");
            this.MOD_SRC_11.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_11.FormattingEnabled = true;
            this.MOD_SRC_11.Maximum = 27;
            this.MOD_SRC_11.Minimum = 0;
            this.MOD_SRC_11.Name = "MOD_SRC_11";
            this.MOD_SRC_11.Step = 1;
            this.MOD_SRC_11.Tag = "MOD_SRC_11";
            this.MOD_SRC_11.Value = 27;
            this.MOD_SRC_11.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_11.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_11
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_11, "MOD_AMNT_SRC_11");
            this.MOD_AMNT_SRC_11.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_11.Maximum = 63;
            this.MOD_AMNT_SRC_11.Minimum = 0;
            this.MOD_AMNT_SRC_11.Name = "MOD_AMNT_SRC_11";
            this.MOD_AMNT_SRC_11.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_11.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_11.Step = 1;
            this.MOD_AMNT_SRC_11.Tag = "MOD_AMNT_SRC_11";
            this.MOD_AMNT_SRC_11.Value = 0;
            this.MOD_AMNT_SRC_11.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_11.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_11.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // ENV_X_DELAY
            // 
            resources.ApplyResources(this.ENV_X_DELAY, "ENV_X_DELAY");
            this.ENV_X_DELAY.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.ENV_X_DELAY.Maximum = 63;
            this.ENV_X_DELAY.Minimum = 0;
            this.ENV_X_DELAY.Name = "ENV_X_DELAY";
            this.ENV_X_DELAY.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.ENV_X_DELAY.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.ENV_X_DELAY.Step = 1;
            this.ENV_X_DELAY.Tag = "ENV_X_DELAY";
            this.ENV_X_DELAY.Value = 0;
            this.ENV_X_DELAY.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.ENV_X_DELAY.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.ENV_X_DELAY.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_10
            // 
            this.MOD_DEST_10.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_10.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_10, "MOD_DEST_10");
            this.MOD_DEST_10.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_10.FormattingEnabled = true;
            this.MOD_DEST_10.Maximum = 1;
            this.MOD_DEST_10.Minimum = 0;
            this.MOD_DEST_10.Name = "MOD_DEST_10";
            this.MOD_DEST_10.Step = 1;
            this.MOD_DEST_10.Tag = "MOD_DEST_10";
            this.MOD_DEST_10.Value = 0;
            this.MOD_DEST_10.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_10.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // VCO2_VOLUME
            // 
            resources.ApplyResources(this.VCO2_VOLUME, "VCO2_VOLUME");
            this.VCO2_VOLUME.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO2_VOLUME.Maximum = 63;
            this.VCO2_VOLUME.Minimum = 0;
            this.VCO2_VOLUME.Name = "VCO2_VOLUME";
            this.VCO2_VOLUME.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCO2_VOLUME.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO2_VOLUME.Step = 1;
            this.VCO2_VOLUME.Tag = "VCO2_VOLUME";
            this.VCO2_VOLUME.Value = 0;
            this.VCO2_VOLUME.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO2_VOLUME.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO2_VOLUME.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCO2_DETUNE
            // 
            resources.ApplyResources(this.VCO2_DETUNE, "VCO2_DETUNE");
            this.VCO2_DETUNE.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO2_DETUNE.Maximum = 31;
            this.VCO2_DETUNE.Minimum = -31;
            this.VCO2_DETUNE.Name = "VCO2_DETUNE";
            this.VCO2_DETUNE.PredefinedValues = new int[] {
        -31,
        -24,
        -18,
        -12,
        -6,
        0,
        7,
        13,
        19,
        25,
        31};
            this.VCO2_DETUNE.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO2_DETUNE.Step = 1;
            this.VCO2_DETUNE.Tag = "VCO2_DETUNE";
            this.VCO2_DETUNE.Value = 0;
            this.VCO2_DETUNE.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO2_DETUNE.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO2_DETUNE.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_10
            // 
            this.MOD_SRC_10.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_10.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_10, "MOD_SRC_10");
            this.MOD_SRC_10.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_10.FormattingEnabled = true;
            this.MOD_SRC_10.Maximum = 27;
            this.MOD_SRC_10.Minimum = 0;
            this.MOD_SRC_10.Name = "MOD_SRC_10";
            this.MOD_SRC_10.Step = 1;
            this.MOD_SRC_10.Tag = "MOD_SRC_10";
            this.MOD_SRC_10.Value = 27;
            this.MOD_SRC_10.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_10.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // ENV_X_VOLUME
            // 
            resources.ApplyResources(this.ENV_X_VOLUME, "ENV_X_VOLUME");
            this.ENV_X_VOLUME.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.ENV_X_VOLUME.Maximum = 63;
            this.ENV_X_VOLUME.Minimum = 0;
            this.ENV_X_VOLUME.Name = "ENV_X_VOLUME";
            this.ENV_X_VOLUME.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.ENV_X_VOLUME.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.ENV_X_VOLUME.Step = 1;
            this.ENV_X_VOLUME.Tag = "ENV_X_VOLUME";
            this.ENV_X_VOLUME.Value = 0;
            this.ENV_X_VOLUME.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.ENV_X_VOLUME.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.ENV_X_VOLUME.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_10
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_10, "MOD_AMNT_SRC_10");
            this.MOD_AMNT_SRC_10.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_10.Maximum = 63;
            this.MOD_AMNT_SRC_10.Minimum = 0;
            this.MOD_AMNT_SRC_10.Name = "MOD_AMNT_SRC_10";
            this.MOD_AMNT_SRC_10.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_10.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_10.Step = 1;
            this.MOD_AMNT_SRC_10.Tag = "MOD_AMNT_SRC_10";
            this.MOD_AMNT_SRC_10.Value = 0;
            this.MOD_AMNT_SRC_10.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_10.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_10.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_9
            // 
            this.MOD_DEST_9.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_9.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_9, "MOD_DEST_9");
            this.MOD_DEST_9.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_9.FormattingEnabled = true;
            this.MOD_DEST_9.Maximum = 1;
            this.MOD_DEST_9.Minimum = 0;
            this.MOD_DEST_9.Name = "MOD_DEST_9";
            this.MOD_DEST_9.Step = 1;
            this.MOD_DEST_9.Tag = "MOD_DEST_9";
            this.MOD_DEST_9.Value = 0;
            this.MOD_DEST_9.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_9.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // ENV_X_RELEASE
            // 
            resources.ApplyResources(this.ENV_X_RELEASE, "ENV_X_RELEASE");
            this.ENV_X_RELEASE.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.ENV_X_RELEASE.Maximum = 63;
            this.ENV_X_RELEASE.Minimum = 0;
            this.ENV_X_RELEASE.Name = "ENV_X_RELEASE";
            this.ENV_X_RELEASE.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.ENV_X_RELEASE.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.ENV_X_RELEASE.Step = 1;
            this.ENV_X_RELEASE.Tag = "ENV_X_RELEASE";
            this.ENV_X_RELEASE.Value = 0;
            this.ENV_X_RELEASE.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.ENV_X_RELEASE.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.ENV_X_RELEASE.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_9
            // 
            this.MOD_SRC_9.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_9.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_9, "MOD_SRC_9");
            this.MOD_SRC_9.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_9.FormattingEnabled = true;
            this.MOD_SRC_9.Maximum = 27;
            this.MOD_SRC_9.Minimum = 0;
            this.MOD_SRC_9.Name = "MOD_SRC_9";
            this.MOD_SRC_9.Step = 1;
            this.MOD_SRC_9.Tag = "MOD_SRC_9";
            this.MOD_SRC_9.Value = 27;
            this.MOD_SRC_9.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_9.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // ENV_X_SUSTAIN
            // 
            resources.ApplyResources(this.ENV_X_SUSTAIN, "ENV_X_SUSTAIN");
            this.ENV_X_SUSTAIN.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.ENV_X_SUSTAIN.Maximum = 63;
            this.ENV_X_SUSTAIN.Minimum = 0;
            this.ENV_X_SUSTAIN.Name = "ENV_X_SUSTAIN";
            this.ENV_X_SUSTAIN.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.ENV_X_SUSTAIN.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.ENV_X_SUSTAIN.Step = 1;
            this.ENV_X_SUSTAIN.Tag = "ENV_X_SUSTAIN";
            this.ENV_X_SUSTAIN.Value = 0;
            this.ENV_X_SUSTAIN.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.ENV_X_SUSTAIN.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.ENV_X_SUSTAIN.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_9
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_9, "MOD_AMNT_SRC_9");
            this.MOD_AMNT_SRC_9.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_9.Maximum = 63;
            this.MOD_AMNT_SRC_9.Minimum = 0;
            this.MOD_AMNT_SRC_9.Name = "MOD_AMNT_SRC_9";
            this.MOD_AMNT_SRC_9.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_9.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_9.Step = 1;
            this.MOD_AMNT_SRC_9.Tag = "MOD_AMNT_SRC_9";
            this.MOD_AMNT_SRC_9.Value = 0;
            this.MOD_AMNT_SRC_9.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_9.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_9.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // ENV_X_DECAY
            // 
            resources.ApplyResources(this.ENV_X_DECAY, "ENV_X_DECAY");
            this.ENV_X_DECAY.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.ENV_X_DECAY.Maximum = 63;
            this.ENV_X_DECAY.Minimum = 0;
            this.ENV_X_DECAY.Name = "ENV_X_DECAY";
            this.ENV_X_DECAY.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.ENV_X_DECAY.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.ENV_X_DECAY.Step = 1;
            this.ENV_X_DECAY.Tag = "ENV_X_DECAY";
            this.ENV_X_DECAY.Value = 0;
            this.ENV_X_DECAY.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.ENV_X_DECAY.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.ENV_X_DECAY.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // ENV_X_ATTACK
            // 
            resources.ApplyResources(this.ENV_X_ATTACK, "ENV_X_ATTACK");
            this.ENV_X_ATTACK.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.ENV_X_ATTACK.Maximum = 63;
            this.ENV_X_ATTACK.Minimum = 0;
            this.ENV_X_ATTACK.Name = "ENV_X_ATTACK";
            this.ENV_X_ATTACK.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.ENV_X_ATTACK.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.ENV_X_ATTACK.Step = 1;
            this.ENV_X_ATTACK.Tag = "ENV_X_ATTACK";
            this.ENV_X_ATTACK.Value = 0;
            this.ENV_X_ATTACK.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.ENV_X_ATTACK.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.ENV_X_ATTACK.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_8
            // 
            this.MOD_DEST_8.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_8.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_8, "MOD_DEST_8");
            this.MOD_DEST_8.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_8.FormattingEnabled = true;
            this.MOD_DEST_8.Maximum = 1;
            this.MOD_DEST_8.Minimum = 0;
            this.MOD_DEST_8.Name = "MOD_DEST_8";
            this.MOD_DEST_8.Step = 1;
            this.MOD_DEST_8.Tag = "MOD_DEST_8";
            this.MOD_DEST_8.Value = 0;
            this.MOD_DEST_8.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_8.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_8
            // 
            this.MOD_SRC_8.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_8.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_8, "MOD_SRC_8");
            this.MOD_SRC_8.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_8.FormattingEnabled = true;
            this.MOD_SRC_8.Maximum = 27;
            this.MOD_SRC_8.Minimum = 0;
            this.MOD_SRC_8.Name = "MOD_SRC_8";
            this.MOD_SRC_8.Step = 1;
            this.MOD_SRC_8.Tag = "MOD_SRC_8";
            this.MOD_SRC_8.Value = 27;
            this.MOD_SRC_8.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_8.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // VCO2_FREQ
            // 
            resources.ApplyResources(this.VCO2_FREQ, "VCO2_FREQ");
            this.VCO2_FREQ.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO2_FREQ.Maximum = 63;
            this.VCO2_FREQ.Minimum = 0;
            this.VCO2_FREQ.Name = "VCO2_FREQ";
            this.VCO2_FREQ.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCO2_FREQ.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO2_FREQ.Step = 1;
            this.VCO2_FREQ.Tag = "VCO2_FREQ";
            this.VCO2_FREQ.Value = 0;
            this.VCO2_FREQ.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO2_FREQ.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO2_FREQ.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCF_VCA2_VOLUME
            // 
            resources.ApplyResources(this.VCF_VCA2_VOLUME, "VCF_VCA2_VOLUME");
            this.VCF_VCA2_VOLUME.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCF_VCA2_VOLUME.Maximum = 63;
            this.VCF_VCA2_VOLUME.Minimum = 0;
            this.VCF_VCA2_VOLUME.Name = "VCF_VCA2_VOLUME";
            this.VCF_VCA2_VOLUME.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCF_VCA2_VOLUME.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCF_VCA2_VOLUME.Step = 1;
            this.VCF_VCA2_VOLUME.Tag = "VCF_VCA2_VOLUME";
            this.VCF_VCA2_VOLUME.Value = 0;
            this.VCF_VCA2_VOLUME.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCF_VCA2_VOLUME.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCF_VCA2_VOLUME.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_8
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_8, "MOD_AMNT_SRC_8");
            this.MOD_AMNT_SRC_8.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_8.Maximum = 63;
            this.MOD_AMNT_SRC_8.Minimum = 0;
            this.MOD_AMNT_SRC_8.Name = "MOD_AMNT_SRC_8";
            this.MOD_AMNT_SRC_8.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_8.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_8.Step = 1;
            this.MOD_AMNT_SRC_8.Tag = "MOD_AMNT_SRC_8";
            this.MOD_AMNT_SRC_8.Value = 0;
            this.MOD_AMNT_SRC_8.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_8.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_8.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_7
            // 
            this.MOD_DEST_7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_7.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_7, "MOD_DEST_7");
            this.MOD_DEST_7.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_7.FormattingEnabled = true;
            this.MOD_DEST_7.Maximum = 1;
            this.MOD_DEST_7.Minimum = 0;
            this.MOD_DEST_7.Name = "MOD_DEST_7";
            this.MOD_DEST_7.Step = 1;
            this.MOD_DEST_7.Tag = "MOD_DEST_7";
            this.MOD_DEST_7.Value = 0;
            this.MOD_DEST_7.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_7.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_7
            // 
            this.MOD_SRC_7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_7.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_7, "MOD_SRC_7");
            this.MOD_SRC_7.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_7.FormattingEnabled = true;
            this.MOD_SRC_7.Maximum = 27;
            this.MOD_SRC_7.Minimum = 0;
            this.MOD_SRC_7.Name = "MOD_SRC_7";
            this.MOD_SRC_7.Step = 1;
            this.MOD_SRC_7.Tag = "MOD_SRC_7";
            this.MOD_SRC_7.Value = 27;
            this.MOD_SRC_7.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_7.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // VCF_VCA1_VOLUME
            // 
            resources.ApplyResources(this.VCF_VCA1_VOLUME, "VCF_VCA1_VOLUME");
            this.VCF_VCA1_VOLUME.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCF_VCA1_VOLUME.Maximum = 63;
            this.VCF_VCA1_VOLUME.Minimum = 0;
            this.VCF_VCA1_VOLUME.Name = "VCF_VCA1_VOLUME";
            this.VCF_VCA1_VOLUME.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCF_VCA1_VOLUME.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCF_VCA1_VOLUME.Step = 1;
            this.VCF_VCA1_VOLUME.Tag = "VCF_VCA1_VOLUME";
            this.VCF_VCA1_VOLUME.Value = 0;
            this.VCF_VCA1_VOLUME.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCF_VCA1_VOLUME.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCF_VCA1_VOLUME.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_7
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_7, "MOD_AMNT_SRC_7");
            this.MOD_AMNT_SRC_7.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_7.Maximum = 63;
            this.MOD_AMNT_SRC_7.Minimum = 0;
            this.MOD_AMNT_SRC_7.Name = "MOD_AMNT_SRC_7";
            this.MOD_AMNT_SRC_7.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_7.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_7.Step = 1;
            this.MOD_AMNT_SRC_7.Tag = "MOD_AMNT_SRC_7";
            this.MOD_AMNT_SRC_7.Value = 0;
            this.MOD_AMNT_SRC_7.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_7.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_7.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_6
            // 
            this.MOD_DEST_6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_6.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_6, "MOD_DEST_6");
            this.MOD_DEST_6.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_6.FormattingEnabled = true;
            this.MOD_DEST_6.Maximum = 1;
            this.MOD_DEST_6.Minimum = 0;
            this.MOD_DEST_6.Name = "MOD_DEST_6";
            this.MOD_DEST_6.Step = 1;
            this.MOD_DEST_6.Tag = "MOD_DEST_6";
            this.MOD_DEST_6.Value = 0;
            this.MOD_DEST_6.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_6.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // VCF_RES
            // 
            resources.ApplyResources(this.VCF_RES, "VCF_RES");
            this.VCF_RES.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCF_RES.Maximum = 63;
            this.VCF_RES.Minimum = 0;
            this.VCF_RES.Name = "VCF_RES";
            this.VCF_RES.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCF_RES.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCF_RES.Step = 1;
            this.VCF_RES.Tag = "VCF_RES";
            this.VCF_RES.Value = 0;
            this.VCF_RES.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCF_RES.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCF_RES.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // PanelENV
            // 
            resources.ApplyResources(this.PanelENV, "PanelENV");
            this.PanelENV.Controls.Add(this.ENV_5);
            this.PanelENV.Controls.Add(this.ENV_1);
            this.PanelENV.Controls.Add(this.ENV_4);
            this.PanelENV.Controls.Add(this.ENV_2);
            this.PanelENV.Controls.Add(this.ENV_3);
            this.PanelENV.Name = "PanelENV";
            // 
            // ENV_5
            // 
            resources.ApplyResources(this.ENV_5, "ENV_5");
            this.ENV_5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ENV_5.ContextMenuStrip = this.PageContextMenuStrip;
            this.ENV_5.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_5.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_5.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_5.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_5.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_5.Name = "ENV_5";
            this.ENV_5.Tag = "ENV_5";
            this.ENV_5.UseVisualStyleBackColor = false;
            this.ENV_5.Click += new System.EventHandler(this.Radio_ENV_X_Click);
            // 
            // ENV_1
            // 
            resources.ApplyResources(this.ENV_1, "ENV_1");
            this.ENV_1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ENV_1.Checked = true;
            this.ENV_1.ContextMenuStrip = this.PageContextMenuStrip;
            this.ENV_1.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_1.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_1.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_1.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_1.Name = "ENV_1";
            this.ENV_1.TabStop = true;
            this.ENV_1.Tag = "ENV_1";
            this.ENV_1.UseVisualStyleBackColor = false;
            this.ENV_1.Click += new System.EventHandler(this.Radio_ENV_X_Click);
            // 
            // ENV_4
            // 
            resources.ApplyResources(this.ENV_4, "ENV_4");
            this.ENV_4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ENV_4.ContextMenuStrip = this.PageContextMenuStrip;
            this.ENV_4.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_4.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_4.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_4.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_4.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_4.Name = "ENV_4";
            this.ENV_4.Tag = "ENV_4";
            this.ENV_4.UseVisualStyleBackColor = false;
            this.ENV_4.Click += new System.EventHandler(this.Radio_ENV_X_Click);
            // 
            // ENV_2
            // 
            resources.ApplyResources(this.ENV_2, "ENV_2");
            this.ENV_2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ENV_2.ContextMenuStrip = this.PageContextMenuStrip;
            this.ENV_2.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_2.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_2.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_2.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_2.Name = "ENV_2";
            this.ENV_2.Tag = "ENV_2";
            this.ENV_2.UseVisualStyleBackColor = false;
            this.ENV_2.Click += new System.EventHandler(this.Radio_ENV_X_Click);
            // 
            // ENV_3
            // 
            resources.ApplyResources(this.ENV_3, "ENV_3");
            this.ENV_3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.ENV_3.ContextMenuStrip = this.PageContextMenuStrip;
            this.ENV_3.FlatAppearance.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_3.FlatAppearance.CheckedBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_3.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_3.FlatAppearance.MouseOverBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(77)))), ((int)(((byte)(77)))), ((int)(((byte)(85)))));
            this.ENV_3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(102)))), ((int)(((byte)(181)))), ((int)(((byte)(255)))));
            this.ENV_3.Name = "ENV_3";
            this.ENV_3.Tag = "ENV_3";
            this.ENV_3.UseVisualStyleBackColor = false;
            this.ENV_3.Click += new System.EventHandler(this.Radio_ENV_X_Click);
            // 
            // VCO1_PW
            // 
            resources.ApplyResources(this.VCO1_PW, "VCO1_PW");
            this.VCO1_PW.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO1_PW.Maximum = 63;
            this.VCO1_PW.Minimum = 0;
            this.VCO1_PW.Name = "VCO1_PW";
            this.VCO1_PW.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCO1_PW.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO1_PW.Step = 1;
            this.VCO1_PW.Tag = "VCO1_PW";
            this.VCO1_PW.Value = 0;
            this.VCO1_PW.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO1_PW.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO1_PW.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCF_FREQ
            // 
            resources.ApplyResources(this.VCF_FREQ, "VCF_FREQ");
            this.VCF_FREQ.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCF_FREQ.Maximum = 127;
            this.VCF_FREQ.Minimum = 0;
            this.VCF_FREQ.Name = "VCF_FREQ";
            this.VCF_FREQ.PredefinedValues = new int[] {
        0,
        15,
        29,
        43,
        57,
        71,
        85,
        99,
        113,
        127};
            this.VCF_FREQ.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCF_FREQ.Step = 1;
            this.VCF_FREQ.Tag = "VCF_FREQ";
            this.VCF_FREQ.Value = 0;
            this.VCF_FREQ.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCF_FREQ.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCF_FREQ.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_6
            // 
            this.MOD_SRC_6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_6.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_6, "MOD_SRC_6");
            this.MOD_SRC_6.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_6.FormattingEnabled = true;
            this.MOD_SRC_6.Maximum = 27;
            this.MOD_SRC_6.Minimum = 0;
            this.MOD_SRC_6.Name = "MOD_SRC_6";
            this.MOD_SRC_6.Step = 1;
            this.MOD_SRC_6.Tag = "MOD_SRC_6";
            this.MOD_SRC_6.Value = 27;
            this.MOD_SRC_6.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_6.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_6
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_6, "MOD_AMNT_SRC_6");
            this.MOD_AMNT_SRC_6.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_6.Maximum = 63;
            this.MOD_AMNT_SRC_6.Minimum = 0;
            this.MOD_AMNT_SRC_6.Name = "MOD_AMNT_SRC_6";
            this.MOD_AMNT_SRC_6.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_6.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_6.Step = 1;
            this.MOD_AMNT_SRC_6.Tag = "MOD_AMNT_SRC_6";
            this.MOD_AMNT_SRC_6.Value = 0;
            this.MOD_AMNT_SRC_6.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_6.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_6.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCO1_FREQ
            // 
            resources.ApplyResources(this.VCO1_FREQ, "VCO1_FREQ");
            this.VCO1_FREQ.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO1_FREQ.Maximum = 63;
            this.VCO1_FREQ.Minimum = 0;
            this.VCO1_FREQ.Name = "VCO1_FREQ";
            this.VCO1_FREQ.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCO1_FREQ.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO1_FREQ.Step = 1;
            this.VCO1_FREQ.Tag = "VCO1_FREQ";
            this.VCO1_FREQ.Value = 0;
            this.VCO1_FREQ.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO1_FREQ.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO1_FREQ.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // VCO1_VOLUME
            // 
            resources.ApplyResources(this.VCO1_VOLUME, "VCO1_VOLUME");
            this.VCO1_VOLUME.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO1_VOLUME.Maximum = 63;
            this.VCO1_VOLUME.Minimum = 0;
            this.VCO1_VOLUME.Name = "VCO1_VOLUME";
            this.VCO1_VOLUME.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.VCO1_VOLUME.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO1_VOLUME.Step = 1;
            this.VCO1_VOLUME.Tag = "VCO1_VOLUME";
            this.VCO1_VOLUME.Value = 0;
            this.VCO1_VOLUME.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO1_VOLUME.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO1_VOLUME.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_5
            // 
            this.MOD_DEST_5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_5.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_5, "MOD_DEST_5");
            this.MOD_DEST_5.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_5.FormattingEnabled = true;
            this.MOD_DEST_5.Maximum = 1;
            this.MOD_DEST_5.Minimum = 0;
            this.MOD_DEST_5.Name = "MOD_DEST_5";
            this.MOD_DEST_5.Step = 1;
            this.MOD_DEST_5.Tag = "MOD_DEST_5";
            this.MOD_DEST_5.Value = 0;
            this.MOD_DEST_5.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_5.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // VCO1_DETUNE
            // 
            resources.ApplyResources(this.VCO1_DETUNE, "VCO1_DETUNE");
            this.VCO1_DETUNE.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.VCO1_DETUNE.Maximum = 31;
            this.VCO1_DETUNE.Minimum = -31;
            this.VCO1_DETUNE.Name = "VCO1_DETUNE";
            this.VCO1_DETUNE.PredefinedValues = new int[] {
        -31,
        -24,
        -18,
        -12,
        -6,
        0,
        7,
        13,
        19,
        25,
        31};
            this.VCO1_DETUNE.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.VCO1_DETUNE.Step = 1;
            this.VCO1_DETUNE.Tag = "VCO1_DETUNE";
            this.VCO1_DETUNE.Value = 0;
            this.VCO1_DETUNE.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.AnyValuedControl_ValueChanged);
            this.VCO1_DETUNE.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.VCO1_DETUNE.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_5
            // 
            this.MOD_SRC_5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_5.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_5, "MOD_SRC_5");
            this.MOD_SRC_5.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_5.FormattingEnabled = true;
            this.MOD_SRC_5.Maximum = 27;
            this.MOD_SRC_5.Minimum = 0;
            this.MOD_SRC_5.Name = "MOD_SRC_5";
            this.MOD_SRC_5.Step = 1;
            this.MOD_SRC_5.Tag = "MOD_SRC_5";
            this.MOD_SRC_5.Value = 27;
            this.MOD_SRC_5.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_5.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_1
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_1, "MOD_AMNT_SRC_1");
            this.MOD_AMNT_SRC_1.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_1.Maximum = 63;
            this.MOD_AMNT_SRC_1.Minimum = 0;
            this.MOD_AMNT_SRC_1.Name = "MOD_AMNT_SRC_1";
            this.MOD_AMNT_SRC_1.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_1.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_1.Step = 1;
            this.MOD_AMNT_SRC_1.Tag = "MOD_AMNT_SRC_1";
            this.MOD_AMNT_SRC_1.Value = 0;
            this.MOD_AMNT_SRC_1.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_AMNT_SRC_5
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_5, "MOD_AMNT_SRC_5");
            this.MOD_AMNT_SRC_5.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_5.Maximum = 63;
            this.MOD_AMNT_SRC_5.Minimum = 0;
            this.MOD_AMNT_SRC_5.Name = "MOD_AMNT_SRC_5";
            this.MOD_AMNT_SRC_5.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_5.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_5.Step = 1;
            this.MOD_AMNT_SRC_5.Tag = "MOD_AMNT_SRC_5";
            this.MOD_AMNT_SRC_5.Value = 0;
            this.MOD_AMNT_SRC_5.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_5.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_5.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_1
            // 
            this.MOD_SRC_1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_1, "MOD_SRC_1");
            this.MOD_SRC_1.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_1.FormattingEnabled = true;
            this.MOD_SRC_1.Maximum = 27;
            this.MOD_SRC_1.Minimum = 0;
            this.MOD_SRC_1.Name = "MOD_SRC_1";
            this.MOD_SRC_1.Step = 1;
            this.MOD_SRC_1.Tag = "MOD_SRC_1";
            this.MOD_SRC_1.Value = 27;
            this.MOD_SRC_1.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_1.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_DEST_4
            // 
            this.MOD_DEST_4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_4.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_4, "MOD_DEST_4");
            this.MOD_DEST_4.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_4.FormattingEnabled = true;
            this.MOD_DEST_4.Maximum = 1;
            this.MOD_DEST_4.Minimum = 0;
            this.MOD_DEST_4.Name = "MOD_DEST_4";
            this.MOD_DEST_4.Step = 1;
            this.MOD_DEST_4.Tag = "MOD_DEST_4";
            this.MOD_DEST_4.Value = 0;
            this.MOD_DEST_4.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_4.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_DEST_1
            // 
            this.MOD_DEST_1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_1, "MOD_DEST_1");
            this.MOD_DEST_1.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_1.FormattingEnabled = true;
            this.MOD_DEST_1.Maximum = 1;
            this.MOD_DEST_1.Minimum = 0;
            this.MOD_DEST_1.Name = "MOD_DEST_1";
            this.MOD_DEST_1.Step = 1;
            this.MOD_DEST_1.Tag = "MOD_DEST_1";
            this.MOD_DEST_1.Value = 0;
            this.MOD_DEST_1.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_1.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_SRC_4
            // 
            this.MOD_SRC_4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_4.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_4, "MOD_SRC_4");
            this.MOD_SRC_4.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_4.FormattingEnabled = true;
            this.MOD_SRC_4.Maximum = 27;
            this.MOD_SRC_4.Minimum = 0;
            this.MOD_SRC_4.Name = "MOD_SRC_4";
            this.MOD_SRC_4.Step = 1;
            this.MOD_SRC_4.Tag = "MOD_SRC_4";
            this.MOD_SRC_4.Value = 27;
            this.MOD_SRC_4.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_4.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_4
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_4, "MOD_AMNT_SRC_4");
            this.MOD_AMNT_SRC_4.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_4.Maximum = 63;
            this.MOD_AMNT_SRC_4.Minimum = 0;
            this.MOD_AMNT_SRC_4.Name = "MOD_AMNT_SRC_4";
            this.MOD_AMNT_SRC_4.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_4.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_4.Step = 1;
            this.MOD_AMNT_SRC_4.Tag = "MOD_AMNT_SRC_4";
            this.MOD_AMNT_SRC_4.Value = 0;
            this.MOD_AMNT_SRC_4.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_4.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_4.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_3
            // 
            this.MOD_DEST_3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_3.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_3, "MOD_DEST_3");
            this.MOD_DEST_3.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_3.FormattingEnabled = true;
            this.MOD_DEST_3.Maximum = 1;
            this.MOD_DEST_3.Minimum = 0;
            this.MOD_DEST_3.Name = "MOD_DEST_3";
            this.MOD_DEST_3.Step = 1;
            this.MOD_DEST_3.Tag = "MOD_DEST_3";
            this.MOD_DEST_3.Value = 0;
            this.MOD_DEST_3.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_3.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_2
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_2, "MOD_AMNT_SRC_2");
            this.MOD_AMNT_SRC_2.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_2.Maximum = 63;
            this.MOD_AMNT_SRC_2.Minimum = 0;
            this.MOD_AMNT_SRC_2.Name = "MOD_AMNT_SRC_2";
            this.MOD_AMNT_SRC_2.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_2.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_2.Step = 1;
            this.MOD_AMNT_SRC_2.Tag = "MOD_AMNT_SRC_2";
            this.MOD_AMNT_SRC_2.Value = 0;
            this.MOD_AMNT_SRC_2.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_2.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_2.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_SRC_3
            // 
            this.MOD_SRC_3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_3.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_3, "MOD_SRC_3");
            this.MOD_SRC_3.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_3.FormattingEnabled = true;
            this.MOD_SRC_3.Maximum = 27;
            this.MOD_SRC_3.Minimum = 0;
            this.MOD_SRC_3.Name = "MOD_SRC_3";
            this.MOD_SRC_3.Step = 1;
            this.MOD_SRC_3.Tag = "MOD_SRC_3";
            this.MOD_SRC_3.Value = 27;
            this.MOD_SRC_3.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_3.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_SRC_2
            // 
            this.MOD_SRC_2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_SRC_2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_SRC_2, "MOD_SRC_2");
            this.MOD_SRC_2.ForeColor = System.Drawing.Color.White;
            this.MOD_SRC_2.FormattingEnabled = true;
            this.MOD_SRC_2.Maximum = 27;
            this.MOD_SRC_2.Minimum = 0;
            this.MOD_SRC_2.Name = "MOD_SRC_2";
            this.MOD_SRC_2.Step = 1;
            this.MOD_SRC_2.Tag = "MOD_SRC_2";
            this.MOD_SRC_2.Value = 27;
            this.MOD_SRC_2.DropDown += new System.EventHandler(this.ComboBoxValuedModSource_DropDown);
            this.MOD_SRC_2.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModSource_SelectionChangeCommitted);
            // 
            // MOD_AMNT_SRC_3
            // 
            resources.ApplyResources(this.MOD_AMNT_SRC_3, "MOD_AMNT_SRC_3");
            this.MOD_AMNT_SRC_3.KnobStyle = MidiApp.UIControls.EnumKnobStyle.Standard;
            this.MOD_AMNT_SRC_3.Maximum = 63;
            this.MOD_AMNT_SRC_3.Minimum = 0;
            this.MOD_AMNT_SRC_3.Name = "MOD_AMNT_SRC_3";
            this.MOD_AMNT_SRC_3.PredefinedValues = new int[] {
        0,
        7,
        14,
        21,
        28,
        35,
        42,
        49,
        56,
        63};
            this.MOD_AMNT_SRC_3.RotatingMode = MidiApp.UIControls.EnumRotatingMode.Linear;
            this.MOD_AMNT_SRC_3.Step = 1;
            this.MOD_AMNT_SRC_3.Tag = "MOD_AMNT_SRC_3";
            this.MOD_AMNT_SRC_3.Value = 0;
            this.MOD_AMNT_SRC_3.ValueChanged += new MidiApp.UIControls.KnobControl.ValueChangedEventHandler(this.KnobModSourceAmount_ValueChanged);
            this.MOD_AMNT_SRC_3.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseDown);
            this.MOD_AMNT_SRC_3.MouseUp += new System.Windows.Forms.MouseEventHandler(this.AnyKnobControl_MouseUp);
            // 
            // MOD_DEST_2
            // 
            this.MOD_DEST_2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.MOD_DEST_2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.MOD_DEST_2, "MOD_DEST_2");
            this.MOD_DEST_2.ForeColor = System.Drawing.Color.White;
            this.MOD_DEST_2.FormattingEnabled = true;
            this.MOD_DEST_2.Maximum = 1;
            this.MOD_DEST_2.Minimum = 0;
            this.MOD_DEST_2.Name = "MOD_DEST_2";
            this.MOD_DEST_2.Step = 1;
            this.MOD_DEST_2.Tag = "MOD_DEST_2";
            this.MOD_DEST_2.Value = 0;
            this.MOD_DEST_2.DropDown += new System.EventHandler(this.ComboBoxValuedModDest_DropDown);
            this.MOD_DEST_2.SelectionChangeCommitted += new System.EventHandler(this.ComboBoxValuedModDest_SelectionChangeCommitted);
            // 
            // _ledPanelControl
            // 
            resources.ApplyResources(this._ledPanelControl, "_ledPanelControl");
            this._ledPanelControl.LedBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(44)))), ((int)(((byte)(44)))), ((int)(((byte)(52)))));
            this._ledPanelControl.LedCount = 3;
            this._ledPanelControl.LedOffColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this._ledPanelControl.Name = "_ledPanelControl";
            // 
            // backupToolStripMenuItem
            // 
            this.backupToolStripMenuItem.Name = "backupToolStripMenuItem";
            resources.ApplyResources(this.backupToolStripMenuItem, "backupToolStripMenuItem");
            this.backupToolStripMenuItem.Click += new System.EventHandler(this.AllDataDumpBackupToolStripMenuItem_Click);
            // 
            // restoreToolStripMenuItem
            // 
            this.restoreToolStripMenuItem.Name = "restoreToolStripMenuItem";
            resources.ApplyResources(this.restoreToolStripMenuItem, "restoreToolStripMenuItem");
            this.restoreToolStripMenuItem.Click += new System.EventHandler(this.AllDataDumpRestoreToolStripMenuItem_Click);
            // 
            // MainForm
            // 
            this.AllowDrop = true;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(54)))), ((int)(((byte)(54)))), ((int)(((byte)(62)))));
            this.Controls.Add(this._ledPanelControl);
            this.Controls.Add(this.mainFormMenstrip);
            this.Controls.Add(this.btPatchStore);
            this.Controls.Add(this.btPatchPlus);
            this.Controls.Add(this.MOD_QUANTIZE_20);
            this.Controls.Add(this.MOD_QUANTIZE_19);
            this.Controls.Add(this.MOD_QUANTIZE_18);
            this.Controls.Add(this.btPatchSave);
            this.Controls.Add(this.MOD_QUANTIZE_17);
            this.Controls.Add(this.btPatchLoad);
            this.Controls.Add(this.btPatchRandom);
            this.Controls.Add(this.btPatchGoto);
            this.Controls.Add(this.btPatchMinus);
            this.Controls.Add(this.btSettings);
            this.Controls.Add(this.LFO_X_WAVESHAPE);
            this.Controls.Add(this.MOD_QUANTIZE_16);
            this.Controls.Add(this.lbSampleInput);
            this.Controls.Add(this.LFO_X_SAMPLE_INPUT);
            this.Controls.Add(this.MOD_QUANTIZE_15);
            this.Controls.Add(this.ENV_X_TRIG_SINGLE_MULTI);
            this.Controls.Add(this.MOD_QUANTIZE_14);
            this.Controls.Add(this.MOD_QUANTIZE_13);
            this.Controls.Add(this.MOD_QUANTIZE_12);
            this.Controls.Add(this.MOD_QUANTIZE_11);
            this.Controls.Add(this.labelRetrigMode);
            this.Controls.Add(this.LFO_X_RETRIG_MODE);
            this.Controls.Add(this.MOD_QUANTIZE_10);
            this.Controls.Add(this._vfdDisplay);
            this.Controls.Add(this.MOD_QUANTIZE_9);
            this.Controls.Add(this.MOD_QUANTIZE_8);
            this.Controls.Add(this.LAG_IN);
            this.Controls.Add(this.MOD_QUANTIZE_7);
            this.Controls.Add(this.PANEL_RAMP);
            this.Controls.Add(this.VCF_MODE);
            this.Controls.Add(this.LFO_X_LAG);
            this.Controls.Add(this.MOD_QUANTIZE_6);
            this.Controls.Add(this.MOD_QUANTIZE_5);
            this.Controls.Add(this.TRACK_X_IN);
            this.Controls.Add(this.MOD_QUANTIZE_4);
            this.Controls.Add(this.MOD_QUANTIZE_3);
            this.Controls.Add(this.MOD_QUANTIZE_2);
            this.Controls.Add(this.PANEL_ENV_X_MODE);
            this.Controls.Add(this.RAMP_X_TRIG_SINGLE_MULTI);
            this.Controls.Add(this.lbModQuantize);
            this.Controls.Add(this.Panel_ENV_X_TRIGGER);
            this.Controls.Add(this.MOD_QUANTIZE_1);
            this.Controls.Add(this.FM_DESTINATION);
            this.Controls.Add(this.LAG_MODE_LEGATO);
            this.Controls.Add(this.MOD_DEST_20);
            this.Controls.Add(this.MOD_SRC_20);
            this.Controls.Add(this.VCF_VCA_MOD);
            this.Controls.Add(this.LAG_TIMING_LINEAR_EXPO);
            this.Controls.Add(this.MOD_AMNT_SRC_20);
            this.Controls.Add(this.MOD_DEST_19);
            this.Controls.Add(this.LAG_LINEAR_EQUAL_TIME);
            this.Controls.Add(this.VCO2_MODS);
            this.Controls.Add(this.MOD_SRC_19);
            this.Controls.Add(this.MOD_AMNT_SRC_19);
            this.Controls.Add(this.VCO1_MODS);
            this.Controls.Add(this.VCO2_WAVESHAPE_PULSE);
            this.Controls.Add(this.MOD_DEST_18);
            this.Controls.Add(this.MOD_SRC_18);
            this.Controls.Add(this.MOD_AMNT_SRC_18);
            this.Controls.Add(this.PanelTRACK);
            this.Controls.Add(this.VCO2_WAVESHAPE_NOISE);
            this.Controls.Add(this.MOD_DEST_17);
            this.Controls.Add(this.PanelRAMP);
            this.Controls.Add(this.MOD_SRC_17);
            this.Controls.Add(this.VCO2_WAVESHAPE_SAW);
            this.Controls.Add(this.VCO2_WAVESHAPE_TRI);
            this.Controls.Add(this.MOD_AMNT_SRC_17);
            this.Controls.Add(this.VCO1_WAVESHAPE_PULSE);
            this.Controls.Add(this.MOD_DEST_16);
            this.Controls.Add(this.MOD_SRC_16);
            this.Controls.Add(this.RAMP_X_RATE);
            this.Controls.Add(this.MOD_DEST_15);
            this.Controls.Add(this.MOD_AMNT_SRC_16);
            this.Controls.Add(this.MOD_SRC_15);
            this.Controls.Add(this.LFO_X_AMP);
            this.Controls.Add(this.LFO_X_RETRIG);
            this.Controls.Add(this.MOD_AMNT_SRC_15);
            this.Controls.Add(this.LFO_X_SPEED);
            this.Controls.Add(this.MOD_DEST_14);
            this.Controls.Add(this.VCO1_WAVESHAPE_SAW);
            this.Controls.Add(this.MOD_SRC_14);
            this.Controls.Add(this.MOD_AMNT_SRC_14);
            this.Controls.Add(this.VCO1_WAVESHAPE_TRI);
            this.Controls.Add(this.FMLAG_RATE);
            this.Controls.Add(this.MOD_DEST_13);
            this.Controls.Add(this.MOD_SRC_13);
            this.Controls.Add(this.FM_AMP);
            this.Controls.Add(this.TRACK_X_PT5);
            this.Controls.Add(this.MOD_AMNT_SRC_13);
            this.Controls.Add(this.MOD_DEST_12);
            this.Controls.Add(this.MOD_SRC_12);
            this.Controls.Add(this.TRACK_X_PT4);
            this.Controls.Add(this.PanelLFO);
            this.Controls.Add(this.TRACK_X_PT3);
            this.Controls.Add(this.MOD_AMNT_SRC_12);
            this.Controls.Add(this.TRACK_X_PT2);
            this.Controls.Add(this.MOD_DEST_11);
            this.Controls.Add(this.TRACK_X_PT1);
            this.Controls.Add(this.VCO2_PW);
            this.Controls.Add(this.MOD_SRC_11);
            this.Controls.Add(this.MOD_AMNT_SRC_11);
            this.Controls.Add(this.ENV_X_DELAY);
            this.Controls.Add(this.MOD_DEST_10);
            this.Controls.Add(this.VCO2_VOLUME);
            this.Controls.Add(this.VCO2_DETUNE);
            this.Controls.Add(this.MOD_SRC_10);
            this.Controls.Add(this.ENV_X_VOLUME);
            this.Controls.Add(this.MOD_AMNT_SRC_10);
            this.Controls.Add(this.MOD_DEST_9);
            this.Controls.Add(this.ENV_X_RELEASE);
            this.Controls.Add(this.MOD_SRC_9);
            this.Controls.Add(this.ENV_X_SUSTAIN);
            this.Controls.Add(this.MOD_AMNT_SRC_9);
            this.Controls.Add(this.ENV_X_DECAY);
            this.Controls.Add(this.ENV_X_ATTACK);
            this.Controls.Add(this.MOD_DEST_8);
            this.Controls.Add(this.MOD_SRC_8);
            this.Controls.Add(this.VCO2_FREQ);
            this.Controls.Add(this.VCF_VCA2_VOLUME);
            this.Controls.Add(this.MOD_AMNT_SRC_8);
            this.Controls.Add(this.MOD_DEST_7);
            this.Controls.Add(this.MOD_SRC_7);
            this.Controls.Add(this.VCF_VCA1_VOLUME);
            this.Controls.Add(this.MOD_AMNT_SRC_7);
            this.Controls.Add(this.MOD_DEST_6);
            this.Controls.Add(this.VCF_RES);
            this.Controls.Add(this.PanelENV);
            this.Controls.Add(this.VCO1_PW);
            this.Controls.Add(this.VCF_FREQ);
            this.Controls.Add(this.MOD_SRC_6);
            this.Controls.Add(this.MOD_AMNT_SRC_6);
            this.Controls.Add(this.VCO1_FREQ);
            this.Controls.Add(this.VCO1_VOLUME);
            this.Controls.Add(this.MOD_DEST_5);
            this.Controls.Add(this.VCO1_DETUNE);
            this.Controls.Add(this.MOD_SRC_5);
            this.Controls.Add(this.lbSOURCE);
            this.Controls.Add(this.MOD_AMNT_SRC_1);
            this.Controls.Add(this.MOD_AMNT_SRC_5);
            this.Controls.Add(this.MOD_SRC_1);
            this.Controls.Add(this.MOD_DEST_4);
            this.Controls.Add(this.MOD_DEST_1);
            this.Controls.Add(this.MOD_SRC_4);
            this.Controls.Add(this.MOD_AMNT_SRC_4);
            this.Controls.Add(this.MOD_DEST_3);
            this.Controls.Add(this.lbDESTINATION);
            this.Controls.Add(this.MOD_AMNT_SRC_2);
            this.Controls.Add(this.lbAMOUNT);
            this.Controls.Add(this.MOD_SRC_3);
            this.Controls.Add(this.MOD_SRC_2);
            this.Controls.Add(this.MOD_AMNT_SRC_3);
            this.Controls.Add(this.MOD_DEST_2);
            this.DoubleBuffered = true;
            this.ForeColor = System.Drawing.SystemColors.ControlText;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MainMenuStrip = this.mainFormMenstrip;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Tag = "";
            this.PageContextMenuStrip.ResumeLayout(false);
            this.mainFormMenstrip.ResumeLayout(false);
            this.mainFormMenstrip.PerformLayout();
            this.ENV_X_TRIG_SINGLE_MULTI.ResumeLayout(false);
            this.ENV_X_TRIG_SINGLE_MULTI.PerformLayout();
            this.PANEL_RAMP.ResumeLayout(false);
            this.PANEL_RAMP.PerformLayout();
            this.PANEL_ENV_X_MODE.ResumeLayout(false);
            this.PANEL_ENV_X_MODE.PerformLayout();
            this.RAMP_X_TRIG_SINGLE_MULTI.ResumeLayout(false);
            this.RAMP_X_TRIG_SINGLE_MULTI.PerformLayout();
            this.Panel_ENV_X_TRIGGER.ResumeLayout(false);
            this.Panel_ENV_X_TRIGGER.PerformLayout();
            this.FM_DESTINATION.ResumeLayout(false);
            this.FM_DESTINATION.PerformLayout();
            this.VCF_VCA_MOD.ResumeLayout(false);
            this.VCF_VCA_MOD.PerformLayout();
            this.LAG_TIMING_LINEAR_EXPO.ResumeLayout(false);
            this.LAG_TIMING_LINEAR_EXPO.PerformLayout();
            this.VCO2_MODS.ResumeLayout(false);
            this.VCO2_MODS.PerformLayout();
            this.VCO1_MODS.ResumeLayout(false);
            this.VCO1_MODS.PerformLayout();
            this.PanelTRACK.ResumeLayout(false);
            this.PanelTRACK.PerformLayout();
            this.PanelRAMP.ResumeLayout(false);
            this.PanelRAMP.PerformLayout();
            this.PanelLFO.ResumeLayout(false);
            this.PanelLFO.PerformLayout();
            this.PanelENV.ResumeLayout(false);
            this.PanelENV.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private MidiApp.UIControls.KnobControl VCO1_FREQ;
        private MidiApp.UIControls.KnobControl VCO1_DETUNE;
        private MidiApp.UIControls.KnobControl VCO1_PW;
        private MidiApp.UIControls.KnobControl VCO1_VOLUME;
        private MidiApp.UIControls.KnobControl VCF_FREQ;
        private MidiApp.UIControls.KnobControl VCF_RES;
        private MidiApp.UIControls.KnobControl VCF_VCA1_VOLUME;
        private MidiApp.UIControls.KnobControl VCF_VCA2_VOLUME;
        private MidiApp.UIControls.KnobControl VCO2_VOLUME;
        private MidiApp.UIControls.KnobControl VCO2_PW;
        private MidiApp.UIControls.KnobControl VCO2_DETUNE;
        private MidiApp.UIControls.KnobControl VCO2_FREQ;
        private MidiApp.UIControls.KnobControl ENV_X_DELAY;
        private MidiApp.UIControls.KnobControl ENV_X_ATTACK;
        private MidiApp.UIControls.KnobControl ENV_X_DECAY;
        private MidiApp.UIControls.KnobControl ENV_X_SUSTAIN;
        private MidiApp.UIControls.KnobControl ENV_X_RELEASE;
        private MidiApp.UIControls.KnobControl ENV_X_VOLUME;
        private MidiApp.UIControls.KnobControl TRACK_X_PT5;
        private MidiApp.UIControls.KnobControl TRACK_X_PT4;
        private MidiApp.UIControls.KnobControl TRACK_X_PT3;
        private MidiApp.UIControls.KnobControl TRACK_X_PT2;
        private MidiApp.UIControls.KnobControl TRACK_X_PT1;
        private MidiApp.UIControls.KnobControl FMLAG_RATE;
        private MidiApp.UIControls.KnobControl FM_AMP;
        private MidiApp.UIControls.KnobControl LFO_X_SPEED;
        private MidiApp.UIControls.KnobControl LFO_X_RETRIG;
        private MidiApp.UIControls.KnobControl LFO_X_AMP;
        private MidiApp.UIControls.KnobControl RAMP_X_RATE;
        private MidiApp.UIControls.FakePanel PanelENV;
        private MidiApp.UIControls.FakePanel PanelLFO;
        private MidiApp.UIControls.FakePanel PanelRAMP;
        private MidiApp.UIControls.FakePanel PanelTRACK;
        private MidiApp.UIControls.CheckBoxValuedControl VCO1_WAVESHAPE_TRI;
        private MidiApp.UIControls.CheckBoxValuedControl VCO1_WAVESHAPE_SAW;
        private MidiApp.UIControls.CheckBoxValuedControl VCO1_WAVESHAPE_PULSE;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_WAVESHAPE_PULSE;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_WAVESHAPE_SAW;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_WAVESHAPE_TRI;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_WAVESHAPE_NOISE;
        private MidiApp.UIControls.FakePanel VCO1_MODS;
        private System.Windows.Forms.Label label1;
        private MidiApp.UIControls.CheckBoxValuedControl VCO1_MOD_VIB;
        private MidiApp.UIControls.CheckBoxValuedControl VCO1_MOD_LEV1;
        private MidiApp.UIControls.CheckBoxValuedControl VCO1_MOD_LAG;
        private MidiApp.UIControls.CheckBoxValuedControl VCO1_MOD_KEYB;
        private MidiApp.UIControls.FakePanel VCO2_MODS;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_WAVE_SYNC;
        private System.Windows.Forms.Label label2;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_MOD_VIB;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_MOD_LEV1;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_MOD_LAG;
        private MidiApp.UIControls.CheckBoxValuedControl VCO2_MOD_KEYB;
        private MidiApp.UIControls.FakePanel VCF_VCA_MOD;
        private System.Windows.Forms.Label label3;
        private MidiApp.UIControls.CheckBoxValuedControl VCF_MOD_VIB;
        private MidiApp.UIControls.CheckBoxValuedControl VCF_MOD_LEV1;
        private MidiApp.UIControls.CheckBoxValuedControl VCF_MOD_LAG;
        private MidiApp.UIControls.CheckBoxValuedControl VCF_MOD_KEYB;
        private MidiApp.UIControls.CheckBoxValuedControl LAG_LINEAR_EQUAL_TIME;
        private MidiApp.UIControls.CheckBoxValuedControl LAG_MODE_LEGATO;
        private MidiApp.UIControls.RadioButtonPanel LAG_TIMING_LINEAR_EXPO;
        private MidiApp.UIControls.RadioButtonPanel FM_DESTINATION;
        private MidiApp.UIControls.FakePanel PANEL_ENV_X_MODE;
        private System.Windows.Forms.Label label4;
        private MidiApp.UIControls.CheckBoxValuedControl ENV_X_MODE_DADR;
        private MidiApp.UIControls.CheckBoxValuedControl ENV_X_MODE_FREERUN;
        private MidiApp.UIControls.CheckBoxValuedControl ENV_X_MODE_RESET;
        private MidiApp.UIControls.ComboBoxValuedControl TRACK_X_IN;
        private MidiApp.UIControls.ComboBoxValuedControl LAG_IN;
        private MidiApp.UIControls.RadioButtonPanel ENV_X_TRIG_SINGLE_MULTI;
        private MidiApp.UIControls.FakePanel Panel_ENV_X_TRIGGER;
        private MidiApp.UIControls.ComboBoxValuedControl ENV_X_TRIG_LFO_SOURCE;
        private MidiApp.UIControls.CheckBoxValuedControl ENV_X_TRIG_GATED;
        private MidiApp.UIControls.CheckBoxValuedControl ENV_X_TRIG_LFOTRIG;
        private MidiApp.UIControls.CheckBoxValuedControl ENV_X_TRIG_EXTRIG;
        private MidiApp.UIControls.FakePanel PANEL_RAMP;
        private MidiApp.UIControls.ComboBoxValuedControl RAMP_X_TRIG_LFO_SOURCE;
        private MidiApp.UIControls.CheckBoxValuedControl RAMP_X_TRIG_GATED;
        private MidiApp.UIControls.CheckBoxValuedControl RAMP_X_TRIG_LFOTRIG;
        private MidiApp.UIControls.CheckBoxValuedControl RAMP_X_TRIG_EXTRIG;
        private MidiApp.UIControls.RadioButtonPanel RAMP_X_TRIG_SINGLE_MULTI;
        private MidiApp.UIControls.CheckBoxValuedControl LFO_X_LAG;
        private MidiApp.UIControls.ComboBoxValuedControl LFO_X_RETRIG_MODE;
        private System.Windows.Forms.Label labelRetrigMode;
        private MidiApp.UIControls.ComboBoxValuedControl VCF_MODE;
        private MidiApp.UIControls.ComboBoxValuedControl LFO_X_WAVESHAPE;
        private MidiApp.UIControls.ComboBoxValuedControl LFO_X_SAMPLE_INPUT;
        private System.Windows.Forms.Label lbSampleInput;
        private MidiApp.UIControls.VacuumFluoDisplayControl _vfdDisplay;
        private System.Windows.Forms.Timer _timer;
        private System.Windows.Forms.ContextMenuStrip PageContextMenuStrip;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_1;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_1;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_1;
        private System.Windows.Forms.Label lbSOURCE;
        private System.Windows.Forms.Label lbDESTINATION;
        private System.Windows.Forms.Label lbAMOUNT;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_2;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_2;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_2;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_3;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_3;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_3;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_4;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_4;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_4;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_5;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_5;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_5;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_6;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_6;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_6;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_7;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_7;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_7;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_8;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_8;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_8;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_9;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_9;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_9;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_10;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_10;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_10;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_11;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_11;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_11;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_12;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_12;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_12;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_13;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_13;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_13;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_14;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_14;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_14;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_15;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_15;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_15;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_16;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_16;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_16;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_17;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_17;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_17;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_18;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_18;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_18;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_19;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_19;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_19;
        private MidiApp.UIControls.KnobControl MOD_AMNT_SRC_20;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_SRC_20;
        private MidiApp.UIControls.ComboBoxValuedControl MOD_DEST_20;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_1;
        private System.Windows.Forms.Label lbModQuantize;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_2;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_3;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_4;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_5;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_6;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_7;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_8;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_9;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_10;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_11;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_12;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_13;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_14;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_15;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_16;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_17;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_18;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_19;
        private MidiApp.UIControls.CheckBoxValuedControl MOD_QUANTIZE_20;
        private MidiApp.UIControls.BackgroundImageButton btPatchPlus;
        private MidiApp.UIControls.BackgroundImageButton btPatchMinus;
        private MidiApp.UIControls.BackgroundImageButton btPatchGoto;
        private MidiApp.UIControls.BackgroundImageButton btPatchRandom;
        private MidiApp.UIControls.BackgroundImageButton btPatchLoad;
        private MidiApp.UIControls.BackgroundImageButton btSettings;
        private MidiApp.UIControls.BackgroundImageButton btPatchSave;
        private MidiApp.UIControls.BackgroundImageButton btPatchStore;
        private System.Windows.Forms.MenuStrip mainFormMenstrip;
        private System.Windows.Forms.ToolStripMenuItem FileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem patchToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem nextPatchStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem previousPatchToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem gotoPatchToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpXplorerToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem aboutdeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem randomizePatchToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripPageMenuItemCopy;
        private System.Windows.Forms.ToolStripMenuItem toolStripPageMenuItemPaste;
        private System.Windows.Forms.RadioButton ENV_5;
        private System.Windows.Forms.RadioButton ENV_1;
        private System.Windows.Forms.RadioButton ENV_4;
        private System.Windows.Forms.RadioButton ENV_2;
        private System.Windows.Forms.RadioButton ENV_3;
        private System.Windows.Forms.RadioButton LFO_5;
        private System.Windows.Forms.RadioButton LFO_1;
        private System.Windows.Forms.RadioButton LFO_4;
        private System.Windows.Forms.RadioButton LFO_2;
        private System.Windows.Forms.RadioButton LFO_3;
        private System.Windows.Forms.RadioButton RAMP_1;
        private System.Windows.Forms.RadioButton RAMP_2;
        private System.Windows.Forms.RadioButton RAMP_3;
        private System.Windows.Forms.RadioButton RAMP_4;
        private System.Windows.Forms.RadioButton TRACK_1;
        private System.Windows.Forms.RadioButton TRACK_2;
        private System.Windows.Forms.RadioButton TRACK_3;
        private BackgroundImageRadioButton rdExpo;
        private BackgroundImageRadioButton rdLinear;
        private BackgroundImageRadioButton rdFMDestVCO1;
        private BackgroundImageRadioButton rdFMDestVCF;
        private BackgroundImageRadioButton rdSINGLE;
        private BackgroundImageRadioButton rdMULTI;
        private BackgroundImageRadioButton rdRampXSingle;
        private BackgroundImageRadioButton rdRampXMulti;
        private System.Windows.Forms.ToolTip _toolTip;
        private System.Windows.Forms.ToolStripMenuItem renameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem storeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem synchronizeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem tuneRequestToolStripMenuItem;
        private LedPanelControl _ledPanelControl;
        private System.Windows.Forms.ToolStripMenuItem singlePatchesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem extractSingleTonesFromBankToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem getSingleTonesFromSynthToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem checkForNewReleaseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem goToWebsiteToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem allDataDumpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem backupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem restoreToolStripMenuItem;
    }
}

