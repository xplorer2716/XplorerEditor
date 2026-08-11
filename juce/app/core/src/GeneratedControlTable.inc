// Originally extracted from Xplorer/View/MainForm.resx (geometry, captions)
// and MainForm.Designer.cs (types, tags) by
// juce/app/core/tools/extract_control_table.py. That script is now INERT —
// both its inputs left with the .NET tree — so this file is HAND-MAINTAINED
// and is itself the source of truth. Every deliberate deviation from the
// archived reference is listed below.
// Logical canvas coordinates (reference y minus the cropped 32 px
// menustrip band, see CANVAS_TOP_CROP). [RQ-GUI-001, ADR-JUC-013]
//
// Deviation (owner request, 2026-07-26, TASK-GUI-009): the reference
// MainForm.Designer.cs placed the VCO2_MOD_{KEYB,LAG,LEV1,VIB}/
// VCO2_WAVE_SYNC checkboxes 10 px left of their VCO1_MOD_* counterparts
// (same widths, columns did not line up between the two rows). The
// .NET source is archived (XplorerEditor-dotnet-archive), so this table
// is now the source of truth; the VCO2 row and its FakePanel backdrop
// were shifted +10 px to align every column with VCO1's, SYNC included.
// [RQ-GUI-001]
//
// Deviation (owner request, 2026-08-10, TASK-CLR-001): the LEFT column's LAG
// group (+13) and TRACK group (+21) are displaced downwards so each section
// separator sits tokens::component::sectionGapAbove below the section it
// terminates, instead of hugging the section below it. The displacements are
// COMPUTED, not authored: sectionGapAbove fixes each bar under its own group,
// TRACK X is pinned by RQ-CLR-003, and the leftover space is split evenly
// between the two below-separator gaps (45 and 46 px, RQ-CLR-004). Every moved
// control keeps its position relative to its own group, and BackgroundRenderer
// moves the matching diagram elements by the same amount — the two files hold
// one layout in two frames and RQ-CLR-005 is what keeps them in step.
// [RQ-CLR-001, RQ-CLR-004, RQ-CLR-005, ADR-CLR-001 (DEC-CLR-001-C)]
//
// Deviation (owner request, 2026-08-10, TASK-CLR-002): the CENTRE column's LFO
// group (+8) and RAMP group (+12) follow, same rule as the left column above.
// ENV does not move: with RAMP X pinned and sectionGapAbove applied, leaving it
// where it is already lands the three below-separator gaps at 21/20/20, inside
// the one-px tolerance of RQ-CLR-004 — moving a whole group to buy a single px
// of evenness is not worth the diff.
// [RQ-CLR-001, RQ-CLR-004, RQ-CLR-005, ADR-CLR-001 (DEC-CLR-001-C)]
//
// Deviation (owner request, 2026-08-10, TASK-GUI-002/003): the eight shortcut
// buttons go from 19x15 bitmaps to 27x27 vector keys, and the row is re-laid out
// to span the MODULATION GRID — from the MOD_SRC combo column's left edge (960)
// to the MOD_QUANTIZE tick-box column's right edge (1218), which is exactly the
// span the MOD MATRIX separator already takes (RQ-CLR-007). 8*27 + 7*6 = 258 is
// the solution; the row and the separator now derive from ONE reference instead
// of two. It moves from y=121 to y=132, which clears the MIDI LED strip it would
// otherwise overlap now that it reaches x=1218.
//
// The whole modulation matrix — four column labels and twenty rows — follows by
// +27 px. That figure is NOT chosen to make room: RQ-CLR-001 requires every
// section separator to clear its section by sectionGapAbove, the matrix column
// had escaped that rule entirely (43 px where the rule says 16), and 27 is the
// correction. It happens to free exactly the room the taller keys need.
// [RQ-GUI-063, RQ-GUI-065, RQ-GUI-066, RQ-CLR-001, ADR-GUI-001 (DEC-GUI-001-D, DEC-GUI-001-E)]
//
// Deviation (owner request, 2026-07-31, TASK-DSP-002): the display group —
// _vfdDisplay, _ledPanelControl and the eight btPatch*/btSettings buttons —
// is lifted so the display can be framed by a recessed bezel (RQ-GUI-050).
// The reference left ONE pixel between the glass (ending y=122) and the LED
// strip (starting y=123), so no bezel of any thickness fits without moving
// something; the reference had no bezel and therefore no reason to leave room.
// _vfdDisplay y 47 -> 30, which puts the bezel's top edge at canvas y=17.
// That is the VCF frame's VISIBLE top: its 2 px stroke is centred on y=18,
// so it starts being drawn at 17. Aligning on the geometric y instead left
// the bezel one pixel low at every scale (measured 1/2/3/4 device px at
// canvas scales 1.0/2.0/3.0/3.7 - a constant 1 logical px, TASK-DSP-007).
// The LEDs (123 -> 116) and buttons (128 -> 121) follow by 7 px so the gaps
// below the bezel are unchanged and the three keep reading as one assembly.
// Recorded here rather than as an offset in MainComponent, so the table keeps
// matching the screen.
// [RQ-GUI-050, ADR-JUC-024 (DEC-JUC-059)]
    {"ENV_1", ControlKind::RadioButton, 525, 180, 49, 26, "ENV_1", "ENV 1"},
    {"ENV_2", ControlKind::RadioButton, 578, 180, 49, 26, "ENV_2", "ENV 2"},
    {"ENV_3", ControlKind::RadioButton, 631, 180, 49, 26, "ENV_3", "ENV 3"},
    {"ENV_4", ControlKind::RadioButton, 684, 180, 49, 26, "ENV_4", "ENV 4"},
    {"ENV_5", ControlKind::RadioButton, 737, 180, 49, 26, "ENV_5", "ENV 5"},
    {"ENV_X_ATTACK", ControlKind::KnobControl, 575, 251, 32, 32, "ENV_X_ATTACK", ""},
    {"ENV_X_DECAY", ControlKind::KnobControl, 625, 251, 32, 32, "ENV_X_DECAY", ""},
    {"ENV_X_DELAY", ControlKind::KnobControl, 525, 251, 32, 32, "ENV_X_DELAY", ""},
    {"ENV_X_MODE_DADR", ControlKind::CheckBoxValuedControl, 843, 359, 54, 17, "ENV_X_MODE_DADR", "DADR"},
    {"ENV_X_MODE_FREERUN", ControlKind::CheckBoxValuedControl, 764, 359, 75, 17, "ENV_X_MODE_FREERUN", "FREERUN"},
    {"ENV_X_MODE_RESET", ControlKind::CheckBoxValuedControl, 700, 359, 59, 17, "ENV_X_MODE_RESET", "RESET"},
    {"ENV_X_RELEASE", ControlKind::KnobControl, 734, 251, 32, 32, "ENV_X_RELEASE", ""},
    {"ENV_X_SUSTAIN", ControlKind::KnobControl, 675, 251, 32, 32, "ENV_X_SUSTAIN", ""},
    {"ENV_X_TRIG_EXTRIG", ControlKind::CheckBoxValuedControl, 534, 316, 47, 17, "ENV_X_TRIG_EXTRIG", "EXT."},
    {"ENV_X_TRIG_GATED", ControlKind::CheckBoxValuedControl, 700, 316, 60, 17, "ENV_X_TRIG_GATED", "GATED"},
    {"ENV_X_TRIG_LFOTRIG", ControlKind::CheckBoxValuedControl, 580, 316, 43, 17, "ENV_X_TRIG_LFOTRIG", "LFO"},
    {"ENV_X_TRIG_LFO_SOURCE", ControlKind::ComboBoxValuedControl, 625, 314, 65, 21, "ENV_X_TRIG_LFO_SOURCE", ""},
    {"ENV_X_TRIG_SINGLE_MULTI", ControlKind::RadioButtonPanel, 764, 313, 129, 24, "ENV_X_TRIG_SINGLE_MULTI", ""},
    {"ENV_X_VOLUME", ControlKind::KnobControl, 819, 251, 32, 32, "ENV_X_VOLUME", ""},
    {"FMLAG_RATE", ControlKind::KnobControl, 199, 532, 32, 32, "FMLAG_RATE", ""},
    {"FM_AMP", ControlKind::KnobControl, 90, 229, 32, 32, "FM_AMP", ""},
    {"FM_DESTINATION", ControlKind::RadioButtonPanel, 191, 189, 82, 42, "FM_DESTINATION", ""},
    {"LAG_IN", ControlKind::ComboBoxValuedControl, 35, 532, 67, 21, "LAG_IN", ""},
    {"LAG_LINEAR_EQUAL_TIME", ControlKind::CheckBoxValuedControl, 352, 533, 88, 17, "LAG_LINEAR_EQUAL_TIME", "EQUAL TIME"},
    {"LAG_MODE_LEGATO", ControlKind::CheckBoxValuedControl, 352, 555, 66, 17, "LAG_MODE_LEGATO", "LEGATO"},
    {"LAG_TIMING_LINEAR_EXPO", ControlKind::RadioButtonPanel, 270, 532, 79, 47, "LAG_TIMING_LINEAR_EXPO", ""},
    {"LFO_1", ControlKind::RadioButton, 525, 412, 48, 26, "LFO_1", "LFO 1"},
    {"LFO_2", ControlKind::RadioButton, 578, 412, 48, 26, "LFO_2", "LFO 2"},
    {"LFO_3", ControlKind::RadioButton, 631, 412, 48, 26, "LFO_3", "LFO 3"},
    {"LFO_4", ControlKind::RadioButton, 684, 412, 48, 26, "LFO_4", "LFO 4"},
    {"LFO_5", ControlKind::RadioButton, 737, 412, 48, 26, "LFO_5", "LFO 5"},
    {"LFO_X_AMP", ControlKind::KnobControl, 818, 483, 32, 32, "LFO_X_AMP", ""},
    {"LFO_X_LAG", ControlKind::CheckBoxValuedControl, 525, 541, 44, 17, "LFO_X_LAG", "LAG"},
    {"LFO_X_RETRIG", ControlKind::KnobControl, 743, 483, 32, 32, "LFO_X_RETRIG", ""},
    {"LFO_X_RETRIG_MODE", ControlKind::ComboBoxValuedControl, 833, 539, 65, 21, "LFO_X_RETRIG_MODE", ""},
    {"LFO_X_SAMPLE_INPUT", ControlKind::ComboBoxValuedControl, 672, 539, 67, 21, "LFO_X_SAMPLE_INPUT", ""},
    {"LFO_X_SPEED", ControlKind::KnobControl, 530, 483, 32, 32, "LFO_X_SPEED", ""},
    {"LFO_X_WAVESHAPE", ControlKind::ComboBoxValuedControl, 611, 486, 100, 21, "LFO_X_WAVESHAPE", ""},
    {"MOD_AMNT_SRC_1", ControlKind::KnobControl, 1069, 208, 25, 25, "MOD_AMNT_SRC_1", ""},
    {"MOD_AMNT_SRC_10", ControlKind::KnobControl, 1069, 449, 25, 25, "MOD_AMNT_SRC_10", ""},
    {"MOD_AMNT_SRC_11", ControlKind::KnobControl, 1069, 489, 25, 25, "MOD_AMNT_SRC_11", ""},
    {"MOD_AMNT_SRC_12", ControlKind::KnobControl, 1069, 515, 25, 25, "MOD_AMNT_SRC_12", ""},
    {"MOD_AMNT_SRC_13", ControlKind::KnobControl, 1069, 541, 25, 25, "MOD_AMNT_SRC_13", ""},
    {"MOD_AMNT_SRC_14", ControlKind::KnobControl, 1069, 567, 25, 25, "MOD_AMNT_SRC_14", ""},
    {"MOD_AMNT_SRC_15", ControlKind::KnobControl, 1069, 593, 25, 25, "MOD_AMNT_SRC_15", ""},
    {"MOD_AMNT_SRC_16", ControlKind::KnobControl, 1069, 635, 25, 25, "MOD_AMNT_SRC_16", ""},
    {"MOD_AMNT_SRC_17", ControlKind::KnobControl, 1069, 660, 25, 25, "MOD_AMNT_SRC_17", ""},
    {"MOD_AMNT_SRC_18", ControlKind::KnobControl, 1069, 685, 25, 25, "MOD_AMNT_SRC_18", ""},
    {"MOD_AMNT_SRC_19", ControlKind::KnobControl, 1069, 710, 25, 25, "MOD_AMNT_SRC_19", ""},
    {"MOD_AMNT_SRC_2", ControlKind::KnobControl, 1069, 233, 25, 25, "MOD_AMNT_SRC_2", ""},
    {"MOD_AMNT_SRC_20", ControlKind::KnobControl, 1069, 735, 25, 25, "MOD_AMNT_SRC_20", ""},
    {"MOD_AMNT_SRC_3", ControlKind::KnobControl, 1069, 258, 25, 25, "MOD_AMNT_SRC_3", ""},
    {"MOD_AMNT_SRC_4", ControlKind::KnobControl, 1069, 283, 25, 25, "MOD_AMNT_SRC_4", ""},
    {"MOD_AMNT_SRC_5", ControlKind::KnobControl, 1069, 308, 25, 25, "MOD_AMNT_SRC_5", ""},
    {"MOD_AMNT_SRC_6", ControlKind::KnobControl, 1069, 349, 25, 25, "MOD_AMNT_SRC_6", ""},
    {"MOD_AMNT_SRC_7", ControlKind::KnobControl, 1069, 374, 25, 25, "MOD_AMNT_SRC_7", ""},
    {"MOD_AMNT_SRC_8", ControlKind::KnobControl, 1069, 399, 25, 25, "MOD_AMNT_SRC_8", ""},
    {"MOD_AMNT_SRC_9", ControlKind::KnobControl, 1069, 424, 25, 25, "MOD_AMNT_SRC_9", ""},
    {"MOD_DEST_1", ControlKind::ComboBoxValuedControl, 1100, 210, 103, 21, "MOD_DEST_1", ""},
    {"MOD_DEST_10", ControlKind::ComboBoxValuedControl, 1100, 451, 103, 21, "MOD_DEST_10", ""},
    {"MOD_DEST_11", ControlKind::ComboBoxValuedControl, 1100, 491, 103, 21, "MOD_DEST_11", ""},
    {"MOD_DEST_12", ControlKind::ComboBoxValuedControl, 1100, 517, 103, 21, "MOD_DEST_12", ""},
    {"MOD_DEST_13", ControlKind::ComboBoxValuedControl, 1100, 543, 103, 21, "MOD_DEST_13", ""},
    {"MOD_DEST_14", ControlKind::ComboBoxValuedControl, 1100, 569, 103, 21, "MOD_DEST_14", ""},
    {"MOD_DEST_15", ControlKind::ComboBoxValuedControl, 1100, 595, 103, 21, "MOD_DEST_15", ""},
    {"MOD_DEST_16", ControlKind::ComboBoxValuedControl, 1100, 637, 103, 21, "MOD_DEST_16", ""},
    {"MOD_DEST_17", ControlKind::ComboBoxValuedControl, 1100, 662, 103, 21, "MOD_DEST_17", ""},
    {"MOD_DEST_18", ControlKind::ComboBoxValuedControl, 1100, 687, 103, 21, "MOD_DEST_18", ""},
    {"MOD_DEST_19", ControlKind::ComboBoxValuedControl, 1100, 712, 103, 21, "MOD_DEST_19", ""},
    {"MOD_DEST_2", ControlKind::ComboBoxValuedControl, 1100, 235, 103, 21, "MOD_DEST_2", ""},
    {"MOD_DEST_20", ControlKind::ComboBoxValuedControl, 1100, 737, 103, 21, "MOD_DEST_20", ""},
    {"MOD_DEST_3", ControlKind::ComboBoxValuedControl, 1100, 260, 103, 21, "MOD_DEST_3", ""},
    {"MOD_DEST_4", ControlKind::ComboBoxValuedControl, 1100, 285, 103, 21, "MOD_DEST_4", ""},
    {"MOD_DEST_5", ControlKind::ComboBoxValuedControl, 1100, 310, 103, 21, "MOD_DEST_5", ""},
    {"MOD_DEST_6", ControlKind::ComboBoxValuedControl, 1100, 351, 103, 21, "MOD_DEST_6", ""},
    {"MOD_DEST_7", ControlKind::ComboBoxValuedControl, 1100, 376, 103, 21, "MOD_DEST_7", ""},
    {"MOD_DEST_8", ControlKind::ComboBoxValuedControl, 1100, 401, 103, 21, "MOD_DEST_8", ""},
    {"MOD_DEST_9", ControlKind::ComboBoxValuedControl, 1100, 426, 103, 21, "MOD_DEST_9", ""},
    {"MOD_QUANTIZE_1", ControlKind::CheckBoxValuedControl, 1206, 215, 12, 11, "MOD_QUANTIZE_1", ""},
    {"MOD_QUANTIZE_10", ControlKind::CheckBoxValuedControl, 1206, 456, 12, 11, "MOD_QUANTIZE_10", ""},
    {"MOD_QUANTIZE_11", ControlKind::CheckBoxValuedControl, 1206, 496, 12, 11, "MOD_QUANTIZE_11", ""},
    {"MOD_QUANTIZE_12", ControlKind::CheckBoxValuedControl, 1206, 522, 12, 11, "MOD_QUANTIZE_12", ""},
    {"MOD_QUANTIZE_13", ControlKind::CheckBoxValuedControl, 1206, 548, 12, 11, "MOD_QUANTIZE_13", ""},
    {"MOD_QUANTIZE_14", ControlKind::CheckBoxValuedControl, 1206, 574, 12, 11, "MOD_QUANTIZE_14", ""},
    {"MOD_QUANTIZE_15", ControlKind::CheckBoxValuedControl, 1206, 600, 12, 11, "MOD_QUANTIZE_15", ""},
    {"MOD_QUANTIZE_16", ControlKind::CheckBoxValuedControl, 1206, 642, 12, 11, "MOD_QUANTIZE_16", ""},
    {"MOD_QUANTIZE_17", ControlKind::CheckBoxValuedControl, 1206, 667, 12, 11, "MOD_QUANTIZE_17", ""},
    {"MOD_QUANTIZE_18", ControlKind::CheckBoxValuedControl, 1206, 692, 12, 11, "MOD_QUANTIZE_18", ""},
    {"MOD_QUANTIZE_19", ControlKind::CheckBoxValuedControl, 1206, 717, 12, 11, "MOD_QUANTIZE_19", ""},
    {"MOD_QUANTIZE_2", ControlKind::CheckBoxValuedControl, 1206, 240, 12, 11, "MOD_QUANTIZE_2", ""},
    {"MOD_QUANTIZE_20", ControlKind::CheckBoxValuedControl, 1206, 742, 12, 11, "MOD_QUANTIZE_20", ""},
    {"MOD_QUANTIZE_3", ControlKind::CheckBoxValuedControl, 1206, 265, 12, 11, "MOD_QUANTIZE_3", ""},
    {"MOD_QUANTIZE_4", ControlKind::CheckBoxValuedControl, 1206, 290, 12, 11, "MOD_QUANTIZE_4", ""},
    {"MOD_QUANTIZE_5", ControlKind::CheckBoxValuedControl, 1206, 315, 12, 11, "MOD_QUANTIZE_5", ""},
    {"MOD_QUANTIZE_6", ControlKind::CheckBoxValuedControl, 1206, 356, 12, 11, "MOD_QUANTIZE_6", ""},
    {"MOD_QUANTIZE_7", ControlKind::CheckBoxValuedControl, 1206, 381, 12, 11, "MOD_QUANTIZE_7", ""},
    {"MOD_QUANTIZE_8", ControlKind::CheckBoxValuedControl, 1206, 406, 12, 11, "MOD_QUANTIZE_8", ""},
    {"MOD_QUANTIZE_9", ControlKind::CheckBoxValuedControl, 1206, 431, 12, 11, "MOD_QUANTIZE_9", ""},
    {"MOD_SRC_1", ControlKind::ComboBoxValuedControl, 960, 210, 103, 21, "MOD_SRC_1", ""},
    {"MOD_SRC_10", ControlKind::ComboBoxValuedControl, 960, 451, 103, 21, "MOD_SRC_10", ""},
    {"MOD_SRC_11", ControlKind::ComboBoxValuedControl, 960, 491, 103, 21, "MOD_SRC_11", ""},
    {"MOD_SRC_12", ControlKind::ComboBoxValuedControl, 960, 517, 103, 21, "MOD_SRC_12", ""},
    {"MOD_SRC_13", ControlKind::ComboBoxValuedControl, 960, 543, 103, 21, "MOD_SRC_13", ""},
    {"MOD_SRC_14", ControlKind::ComboBoxValuedControl, 960, 569, 103, 21, "MOD_SRC_14", ""},
    {"MOD_SRC_15", ControlKind::ComboBoxValuedControl, 960, 595, 103, 21, "MOD_SRC_15", ""},
    {"MOD_SRC_16", ControlKind::ComboBoxValuedControl, 960, 637, 103, 21, "MOD_SRC_16", ""},
    {"MOD_SRC_17", ControlKind::ComboBoxValuedControl, 960, 662, 103, 21, "MOD_SRC_17", ""},
    {"MOD_SRC_18", ControlKind::ComboBoxValuedControl, 960, 687, 103, 21, "MOD_SRC_18", ""},
    {"MOD_SRC_19", ControlKind::ComboBoxValuedControl, 960, 712, 103, 21, "MOD_SRC_19", ""},
    {"MOD_SRC_2", ControlKind::ComboBoxValuedControl, 960, 235, 103, 21, "MOD_SRC_2", ""},
    {"MOD_SRC_20", ControlKind::ComboBoxValuedControl, 960, 737, 103, 21, "MOD_SRC_20", ""},
    {"MOD_SRC_3", ControlKind::ComboBoxValuedControl, 960, 260, 103, 21, "MOD_SRC_3", ""},
    {"MOD_SRC_4", ControlKind::ComboBoxValuedControl, 960, 285, 103, 21, "MOD_SRC_4", ""},
    {"MOD_SRC_5", ControlKind::ComboBoxValuedControl, 960, 310, 103, 21, "MOD_SRC_5", ""},
    {"MOD_SRC_6", ControlKind::ComboBoxValuedControl, 960, 351, 103, 21, "MOD_SRC_6", ""},
    {"MOD_SRC_7", ControlKind::ComboBoxValuedControl, 960, 376, 103, 21, "MOD_SRC_7", ""},
    {"MOD_SRC_8", ControlKind::ComboBoxValuedControl, 960, 401, 103, 21, "MOD_SRC_8", ""},
    {"MOD_SRC_9", ControlKind::ComboBoxValuedControl, 960, 426, 103, 21, "MOD_SRC_9", ""},
    {"PANEL_ENV_X_MODE", ControlKind::FakePanel, 596, 352, 302, 30, "", ""},
    {"PANEL_RAMP", ControlKind::FakePanel, 528, 728, 236, 32, "", ""},
    {"PanelENV", ControlKind::FakePanel, 505, 176, 290, 32, "", ""},
    {"PanelLFO", ControlKind::FakePanel, 505, 410, 290, 32, "", ""},
    {"PanelRAMP", ControlKind::FakePanel, 505, 594, 270, 32, "", ""},
    {"PanelTRACK", ControlKind::FakePanel, 80, 637, 269, 32, "", ""},
    {"Panel_ENV_X_TRIGGER", ControlKind::FakePanel, 531, 309, 231, 32, "", ""},
    {"RAMP_1", ControlKind::RadioButton, 525, 596, 57, 26, "RAMP_1", "RAMP 1"},
    {"RAMP_2", ControlKind::RadioButton, 586, 596, 57, 26, "RAMP_2", "RAMP 2"},
    {"RAMP_3", ControlKind::RadioButton, 647, 596, 57, 26, "RAMP_3", "RAMP 3"},
    {"RAMP_4", ControlKind::RadioButton, 708, 596, 57, 26, "RAMP_4", "RAMP 4"},
    {"RAMP_X_RATE", ControlKind::KnobControl, 641, 667, 32, 32, "RAMP_X_RATE", ""},
    {"RAMP_X_TRIG_EXTRIG", ControlKind::CheckBoxValuedControl, 531, 734, 47, 17, "RAMP_X_TRIG_EXTRIG", "EXT."},
    {"RAMP_X_TRIG_GATED", ControlKind::CheckBoxValuedControl, 700, 734, 60, 17, "RAMP_X_TRIG_GATED", "GATED"},
    {"RAMP_X_TRIG_LFOTRIG", ControlKind::CheckBoxValuedControl, 580, 734, 43, 17, "RAMP_X_TRIG_LFOTRIG", "LFO"},
    {"RAMP_X_TRIG_LFO_SOURCE", ControlKind::ComboBoxValuedControl, 629, 732, 65, 21, "RAMP_X_TRIG_LFO_SOURCE", ""},
    {"RAMP_X_TRIG_SINGLE_MULTI", ControlKind::RadioButtonPanel, 767, 731, 126, 22, "RAMP_X_TRIG_SINGLE_MULTI", ""},
    {"TRACK_1", ControlKind::RadioButton, 83, 638, 63, 26, "TRACK_1", "TRACK 1"},
    {"TRACK_2", ControlKind::RadioButton, 150, 638, 63, 26, "TRACK_2", "TRACK 2"},
    {"TRACK_3", ControlKind::RadioButton, 217, 638, 63, 26, "TRACK_3", "TRACK 3"},
    {"TRACK_X_IN", ControlKind::ComboBoxValuedControl, 35, 718, 67, 21, "TRACK_X_IN", ""},
    {"TRACK_X_PT1", ControlKind::KnobControl, 110, 718, 32, 32, "TRACK_X_POINT_1", ""},
    {"TRACK_X_PT2", ControlKind::KnobControl, 154, 718, 32, 32, "TRACK_X_POINT_2", ""},
    {"TRACK_X_PT3", ControlKind::KnobControl, 198, 718, 32, 32, "TRACK_X_POINT_3", ""},
    {"TRACK_X_PT4", ControlKind::KnobControl, 242, 718, 32, 32, "TRACK_X_POINT_4", ""},
    {"TRACK_X_PT5", ControlKind::KnobControl, 286, 718, 32, 32, "TRACK_X_POINT_5", ""},
    {"VCF_FREQ", ControlKind::KnobControl, 525, 67, 32, 32, "VCF_FREQ", ""},
    {"VCF_MODE", ControlKind::ComboBoxValuedControl, 612, 68, 127, 21, "VCF_MODE", ""},
    {"VCF_MOD_KEYB", ControlKind::CheckBoxValuedControl, 605, 126, 51, 17, "VCF_MOD_KEYB", "KEYB"},
    {"VCF_MOD_LAG", ControlKind::CheckBoxValuedControl, 664, 126, 44, 17, "VCF_MOD_LAG", "LAG"},
    {"VCF_MOD_LEV1", ControlKind::CheckBoxValuedControl, 716, 126, 49, 17, "VCF_MOD_LEV1", "LEV1"},
    {"VCF_MOD_VIB", ControlKind::CheckBoxValuedControl, 773, 126, 40, 17, "VCF_MOD_VIB", "VIB"},
    {"VCF_RES", ControlKind::KnobControl, 575, 67, 32, 32, "VCF_RES", ""},
    {"VCF_VCA1_VOLUME", ControlKind::KnobControl, 743, 67, 32, 32, "VCF_VCA1_VOLUME", ""},
    {"VCF_VCA2_VOLUME", ControlKind::KnobControl, 818, 67, 32, 32, "VCF_VCA2_VOLUME", ""},
    {"VCF_VCA_MOD", ControlKind::FakePanel, 525, 119, 302, 32, "", ""},
    {"VCO1_DETUNE", ControlKind::KnobControl, 154, 67, 32, 32, "VCO1_DETUNE", ""},
    {"VCO1_FREQ", ControlKind::KnobControl, 66, 67, 32, 32, "VCO1_FREQ", ""},
    {"VCO1_MODS", ControlKind::FakePanel, 61, 119, 311, 32, "", ""},
    {"VCO1_MOD_KEYB", ControlKind::CheckBoxValuedControl, 154, 126, 51, 17, "VCO1_MOD_KEYB", "KEYB"},
    {"VCO1_MOD_LAG", ControlKind::CheckBoxValuedControl, 213, 126, 44, 17, "VCO1_MOD_LAG", "LAG"},
    {"VCO1_MOD_LEV1", ControlKind::CheckBoxValuedControl, 265, 126, 49, 17, "VCO1_MOD_LEV1", "LEV1"},
    {"VCO1_MOD_VIB", ControlKind::CheckBoxValuedControl, 322, 126, 40, 17, "VCO1_MOD_VIB", "VIB"},
    {"VCO1_PW", ControlKind::KnobControl, 243, 67, 32, 32, "VCO1_PW", ""},
    {"VCO1_VOLUME", ControlKind::KnobControl, 416, 67, 32, 32, "VCO1_VOLUME", ""},
    {"VCO1_WAVESHAPE_PULSE", ControlKind::CheckBoxValuedControl, 329, 104, 58, 17, "VCO1_WAVESHAPE_PULSE", "PULSE"},
    {"VCO1_WAVESHAPE_SAW", ControlKind::CheckBoxValuedControl, 329, 87, 48, 17, "VCO1_WAVESHAPE_SAW", "SAW"},
    {"VCO1_WAVESHAPE_TRI", ControlKind::CheckBoxValuedControl, 329, 69, 38, 18, "VCO1_WAVESHAPE_TRI", "TRI"},
    {"VCO2_DETUNE", ControlKind::KnobControl, 153, 347, 32, 32, "VCO2_DETUNE", ""},
    {"VCO2_FREQ", ControlKind::KnobControl, 66, 347, 32, 32, "VCO2_FREQ", ""},
    {"VCO2_MODS", ControlKind::FakePanel, 61, 402, 380, 32, "", ""},
    {"VCO2_MOD_KEYB", ControlKind::CheckBoxValuedControl, 154, 409, 51, 17, "VCO2_MOD_KEYB", "KEYB"},
    {"VCO2_MOD_LAG", ControlKind::CheckBoxValuedControl, 213, 409, 44, 17, "VCO2_MOD_LAG", "LAG"},
    {"VCO2_MOD_LEV1", ControlKind::CheckBoxValuedControl, 265, 409, 49, 17, "VCO2_MOD_LEV1", "LEV1"},
    {"VCO2_MOD_VIB", ControlKind::CheckBoxValuedControl, 322, 409, 40, 17, "VCO2_MOD_VIB", "VIB"},
    {"VCO2_PW", ControlKind::KnobControl, 244, 347, 32, 32, "VCO2_PW", ""},
    {"VCO2_VOLUME", ControlKind::KnobControl, 414, 245, 32, 32, "VCO2_VOLUME", ""},
    {"VCO2_WAVESHAPE_NOISE", ControlKind::CheckBoxValuedControl, 331, 301, 56, 17, "VCO2_WAVESHAPE_NOISE", "NOISE"},
    {"VCO2_WAVESHAPE_PULSE", ControlKind::CheckBoxValuedControl, 331, 284, 58, 17, "VCO2_WAVESHAPE_PULSE", "PULSE"},
    {"VCO2_WAVESHAPE_SAW", ControlKind::CheckBoxValuedControl, 331, 267, 48, 17, "VCO2_WAVESHAPE_SAW", "SAW"},
    {"VCO2_WAVESHAPE_TRI", ControlKind::CheckBoxValuedControl, 331, 250, 41, 17, "VCO2_WAVESHAPE_TRI", "TRI"},
    {"VCO2_WAVE_SYNC", ControlKind::CheckBoxValuedControl, 371, 409, 52, 17, "VCO2_WAVE_SYNC", "SYNC"},
    {"_ledPanelControl", ControlKind::LedPanelControl, 1190, 116, 32, 8, "", ""},
    {"_vfdDisplay", ControlKind::VacuumFluoDisplayControl, 951, 30, 267, 75, "", ""},
    {"btPatchGoto", ControlKind::BackgroundImageButton, 1026, 132, 27, 27, "", ""},
    {"btPatchLoad", ControlKind::BackgroundImageButton, 1092, 132, 27, 27, "", ""},
    {"btPatchMinus", ControlKind::BackgroundImageButton, 960, 132, 27, 27, "", ""},
    {"btPatchPlus", ControlKind::BackgroundImageButton, 993, 132, 27, 27, "", ""},
    {"btPatchRandom", ControlKind::BackgroundImageButton, 1059, 132, 27, 27, "", ""},
    {"btPatchSave", ControlKind::BackgroundImageButton, 1125, 132, 27, 27, "", ""},
    {"btPatchStore", ControlKind::BackgroundImageButton, 1158, 132, 27, 27, "", ""},
    {"btSettings", ControlKind::BackgroundImageButton, 1191, 132, 27, 27, "", ""},
    {"label1", ControlKind::Label, 67, 128, 72, 13, "", "VCO1 MOD ="},
    {"label2", ControlKind::Label, 67, 411, 72, 13, "", "VCO2 MOD ="},
    {"label3", ControlKind::Label, 531, 128, 64, 13, "", "VCF MOD ="},
    {"label4", ControlKind::Label, 599, 361, 94, 13, "", "CYCLING MODE="},
    {"labelRetrigMode", ControlKind::Label, 744, 543, 83, 13, "", "RETRIG MODE"},
    {"lbAMOUNT", ControlKind::Label, 1062, 185, 38, 13, "", "AMNT"},
    {"lbDESTINATION", ControlKind::Label, 1129, 185, 39, 13, "", "DEST."},
    {"lbModQuantize", ControlKind::Label, 1195, 185, 29, 13, "", "QTZ"},
    {"lbSOURCE", ControlKind::Label, 985, 185, 52, 13, "", "SOURCE"},
    {"lbSampleInput", ControlKind::Label, 583, 543, 86, 13, "", "SAMPLE INPUT"},
    {"rdExpo", ControlKind::BackgroundImageRadioButton, 274, 555, 53, 17, "1", "EXPO"},
    {"rdFMDestVCF", ControlKind::BackgroundImageRadioButton, 225, 210, 44, 17, "1", "VCF"},
    {"rdFMDestVCO1", ControlKind::BackgroundImageRadioButton, 225, 192, 52, 17, "0", "VCO1"},
    {"rdLinear", ControlKind::BackgroundImageRadioButton, 274, 535, 63, 17, "0", "LINEAR"},
    {"rdMULTI", ControlKind::BackgroundImageRadioButton, 833, 316, 57, 17, "1", "MULTI"},
    {"rdRampXMulti", ControlKind::BackgroundImageRadioButton, 836, 734, 57, 17, "1", "MULTI"},
    {"rdRampXSingle", ControlKind::BackgroundImageRadioButton, 770, 734, 63, 17, "0", "SINGLE"},
    {"rdSINGLE", ControlKind::BackgroundImageRadioButton, 767, 316, 63, 17, "0", "SINGLE"},
