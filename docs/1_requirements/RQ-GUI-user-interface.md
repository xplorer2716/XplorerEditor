# RQ-GUI — User Interface (JUCE)

Scope: JUCE reimplementation of the WinForms View layer (MainForm, dialogs, MidiApp.UIControls).
Inputs: reference screenshot + owner's textual description (2026-07), `MainForm.Designer.cs` (4487 lines, 234 custom controls), `MainForm.resx` (background bitmap + button icons).
Owner decisions (2026-07): reuse the existing background bitmap; **resizable** main window; functional parity with standard JUCE controls first, custom skin afterwards.

## Main window — global structure

- **RQ-GUI-001** — The main window shall present all single-patch parameters simultaneously on one screen, laid out over the reference background image (block diagram of the synth signal path), organized as: LEFT column = VCO1, VCO2, FM/VCA, LAG, TRACK X blocks; CENTER column = MULTIMODE VCF/VCA1/VCA, ENV X, LFO X, RAMP X blocks; RIGHT column = VFD display, shortcut buttons, modulation matrix.
- **RQ-GUI-002** — The UI shall bind every control to its model parameter by name through a registration mechanism (control identifier → parameter name), so control changes call `SetParameter` and controller events update controls, with no per-control hand-written glue.
- **RQ-GUI-003** — When a parameter-change event arrives from the controller (automation or synth), the affected control shall update without echoing the change back to the synth.
- **RQ-GUI-004** — While a control is being edited from the UI, its mapped CC automation shall be temporarily disabled (RQ-FMW-033).
- **RQ-GUI-005** — The main window shall be freely **resizable**: the whole content (background bitmap and controls, positioned on a fixed logical canvas) scales uniformly with the window, preserving aspect ratio (owner decision; supersedes the reference's fixed launch-time DPI sizing).
- **RQ-GUI-006** — The UI shall run on the JUCE message thread; controller events arriving from other threads shall be marshalled to it (RQ-FMW-061) via the `EventDispatcher` seam.
- **RQ-GUI-007** — The main window shall use the reference background bitmap extracted from `MainForm.resx`, rendered scaled; a vector redraw is a post-parity evolution (ADR-006).
- **RQ-GUI-008** — The main window shall provide the reference menu bar: **File** (New, Open, Save, Save As, Exit), **Patch** (Next, Previous, Goto, Rename, Store, Randomize, Copy page, Paste page, Synchronize, Visualize), **Tools** (Settings, Tune Request, Single patches: Extract from bank / Get from synth, All data dump: Backup / Restore), **Help** (Help, Check for new release, Go to website, About).

## Page families (shared control blocks)

- **RQ-GUI-010** — The ENV X, LFO X, RAMP X and TRACK X areas shall each be a **single shared control block** whose controls edit the currently selected family instance; selector buttons (ENV 1–5, LFO 1–5, RAMP 1–4, TRACK 1–3) switch the active instance.
- **RQ-GUI-011** — When a selector button is clicked, the block's controls shall rebind to the selected instance's parameters (name = family prefix + digit, per `PageFamilyDescriptor`), refresh their displayed values from the model, and the controller shall send the matching page-select to the synth (reference `TriggerRuleManager`/`PageRefreshManager` behavior).
- **RQ-GUI-012** — When the synth selects an ENV/LFO/RAMP/TRACK page (page-change event, RQ-CTL-023), the corresponding selector shall activate and the block shall refresh, mirroring the synth panel.

## Modulation matrix panel

- **RQ-GUI-015** — The matrix panel shall present the 20 modulation entries as 4 groups of 5 rows; each row = source combo box, amount knob (−63..+63), destination combo box, quantize checkbox (reference tags `MOD_SOURCE_n`, `MOD_AMNT_SRC_n`, `MOD_DEST_n`, `MOD_QUANTIZE_n`).
- **RQ-GUI-016** — Row edits shall call the controller matrix operations (RQ-CTL-030); destination combos shall only offer destinations allowed by the max-6-sources rule, and source editability shall follow `sourceAvailabilityForEntry`.
- **RQ-GUI-017** — Modulation-entry-change and full-tone-change events shall refresh the affected row(s)/whole matrix without echoing to the synth.

## VFD display & shortcut buttons

- **RQ-GUI-020** — The UI shall show a display area at top-right presenting at minimum: current program number and patch name, and the last edited parameter on change. The parameter line shall follow the reference `VfdDisplayHelper` content: the parameter's end-user display name (Resources.resx string, tag as fallback) and its value formatted by control type — combo/radio selection **label**, checkbox `Y`/`N`, knob numeric — plus the mapped `MIDI CC:` number when one exists; a modulation-matrix edit shall instead show `SRC TO DEST:` with `AMNT:/QTZ:`. The display updates both on local panel edits and on incoming automation/synth changes. Initial implementation = multiline text with a fixed-width font (owner decision); hardware-accurate VFD rendering (dot-matrix glyphs, reference `VacuumFluoDisplayControl`) is specified by RQ-GUI-033/ADR-007. *Not yet ported: the trailing `.` active-modulation-destination marker (needs live page-family state).*
- **RQ-GUI-021** — Under the display, the UI shall provide the 8 reference shortcut buttons: program − / program +, goto patch, randomize, load patch, save patch, store patch, MIDI settings — each triggering the same controller operation as its menu counterpart.
- **RQ-GUI-022** — The UI shall display MIDI activity indication driven by the MIDI-activity event (RQ-CTL-027).

## Dialogs & workflows

- **RQ-GUI-025** — The UI shall provide the reference dialog workflows: settings (MIDI / UI / randomizer pages), rename patch, store & goto patch, extract single tones from a bank file, tone morphing, progress display for long operations, about box, and splash screen.
- **RQ-GUI-026** — Long operations (all-data dump backup/restore, get-all-patches) shall run off the message thread with progression display and shall keep the UI responsive (no event pumping); cancellation where the reference offers it.
- **RQ-GUI-027** — The UI shall provide the copy/paste page actions for ENV/LFO/RAMP/TRACK families (menu Patch → Copy/Paste) with paste-eligibility feedback (RQ-CTL-040).
- **RQ-GUI-028** — The UI shall provide a piano-keyboard window sending Note On/Off to the synth (RQ-MID-010).
- **RQ-GUI-029** — The main window shall accept a dropped `.syx` file (reference `MainForm` `AllowDrop`): the first dropped `.syx` is classified (RQ-MOD-043) and loaded by type — a single tone replaces and transmits the edited tone (RQ-CTL-001), an all-data dump is restored after an explicit confirmation (with progress, RQ-GUI-026), an unknown file raises a warning. The File → Open menu shares the same load-by-type path.

## Controls & look

- **RQ-GUI-030** — Initial phase: standard JUCE controls (rotary `Slider`, `ComboBox`, `ToggleButton`, buttons) positioned on the logical canvas; behavior complete (owner decision).
- **RQ-GUI-031** — Skin phase (after functional parity): a custom `LookAndFeel` shall reproduce the reference control appearance — knobs with configurable LED ring/border color, linear/rotary mouse modes and standard/vintage styles (UiConfiguration, RQ-SET-003), image shortcut buttons (existing GIF assets), custom checkboxes/radio panels, VFD-style display (detailed by RQ-GUI-033).
- **RQ-GUI-032** — Valued controls shall map integer parameter ranges exactly (min/max/step from the model), including signed ranges and enum-labeled combos (filter modes, wave shapes, sources/destinations) with the reference label sets.
- **RQ-GUI-033** — The display shall render its text as bitmap glyphs from the reference `MATRIXTINY` sprite sheet (96 glyphs of 12×16 px in a 32×3 grid, ASCII 32–126; characters outside that range render as a space, source cell = column `(c−32) mod 32`, row `(c−32) div 32`), on a black background, with the glyph block centered in the control bounds and the grid derived from those bounds (`cols = width/12`, `lines = height/16`) — replacing the interim fixed-width-font rendering of RQ-GUI-020. The sheet shall be extracted mechanically from the .NET resources into the committed assets. Implementation choices (paint strategy, caching, scaling quality) per ADR-007.

## Error handling

- **RQ-GUI-035** — When an unhandled exception reaches the UI layer, the application shall show a top-level error dialog offering a bug report containing the diagnostic payload (RQ-FMW-071) instead of terminating silently.
