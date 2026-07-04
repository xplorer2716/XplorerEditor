# RQ-GUI — User Interface (JUCE)

Scope: JUCE reimplementation of the WinForms View layer (MainForm, dialogs, MidiApp.UIControls). Functional parity first; pixel-fidelity to the current skin is desirable but secondary to behavior.

## Main window

- **RQ-GUI-001** — The main window shall present all single-patch parameters simultaneously (single-screen overview: VCOs, VCF/VCA, FM/Lag, envelopes, LFOs, tracks, ramps, name, modulation matrix overview) as the reference layout does.
- **RQ-GUI-002** — The UI shall bind every control to its model parameter by name via a registration mechanism, so control changes call `SetParameter` and controller events update controls, with no per-control hand-written glue.
- **RQ-GUI-003** — When a parameter-change event arrives from the controller (automation or synth), the affected control shall update without echoing the change back to the synth.
- **RQ-GUI-004** — While a control is being edited from the UI, its mapped CC automation shall be temporarily disabled (RQ-FMW-033).
- **RQ-GUI-005** — The UI shall scale with system font/DPI settings, preserving the auto-sizing behavior of the reference.
- **RQ-GUI-006** — The UI shall run on the JUCE message thread; controller events arriving from other threads shall be marshalled to it (RQ-FMW-061).

## Custom controls

- **RQ-GUI-010** — The UI shall provide JUCE equivalents of the custom controls: rotary knob (linear/rotary mouse modes, LED ring/border color options), valued combo box, valued checkbox, blue track bar, radio-button panel, LED panel, and VFD-style display.
- **RQ-GUI-011** — The VFD display component shall render parameter feedback like the hardware display (current page, parameter name and value on change) reproducing the reference `VfdDisplayHelper` behavior, including scrolled/typewriter text.

## Dialogs & workflows

- **RQ-GUI-020** — The UI shall provide the workflows of the reference dialogs: settings (MIDI, UI, randomizer pages), rename patch, store & goto patch, extract single tones from a bank file, tone morphing, progress display for long operations, about box, and splash screen.
- **RQ-GUI-021** — Long operations (all-data dump backup/restore, get-all-patches) shall show progression and keep the UI responsive without any `DoEvents`-style pumping, with cancellation where the reference offers it.
- **RQ-GUI-022** — The UI shall expose the modulation matrix editing view with source/amount/quantize/destination editing per entry, enforcing availability rules from the controller (RQ-CTL-030).
- **RQ-GUI-023** — The UI shall provide the copy/paste page actions for TRACK, ENV, LFO and RAMP page families with paste-eligibility feedback (RQ-CTL-040).
- **RQ-GUI-024** — The UI shall provide a piano-keyboard window sending Note On/Off to the synth (RQ-MID-010).
- **RQ-GUI-025** — The UI shall display MIDI activity indicators driven by the MIDI-activity event (RQ-CTL-027).

## Error handling

- **RQ-GUI-030** — When an unhandled exception reaches the UI layer, the application shall show a top-level error dialog offering a bug report containing the diagnostic payload (RQ-FMW-071) instead of terminating silently.
