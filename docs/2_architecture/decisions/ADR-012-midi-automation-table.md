# ADR-012: MIDI CC Automation Table — Load Path, Editor UI & CC-Name Port

## Status
Accepted (owner, 2026-07-13 — reference behaviour is authoritative; editable
parameter set and CC range mirror the reference exactly). Implemented: CC-name
extraction, headless parser + load path in `applyMidiSettings`, and the
`TableListBox` editor on the MIDI settings page.

## Requirements
RQ-GUI-036, RQ-SET-002, RQ-FMW-032, RQ-FMW-050..052

## Context
The reference lets the user map each tone parameter to a MIDI Continuous
Controller: incoming CCs on the automation-input device then drive those
parameters (`AbstractController` CC handler → `ControlChangeAutomationTable`).
Two reference pieces implement this:

- **Load** (`SettingsManager.LoadSettings`): parse each persisted
  `"NAME;CC"` entry, clamp CC to 1..128, and fill
  `XController.ControlChangeAutomationTable` (the `StringIntDualDictionary`) —
  run at startup and after every settings change.
- **Edit** (`Settings/MidiPage`): a `LvAutomation` list of
  {parameter friendly name, CC name}; an inline combo picks a CC from
  `ControlChangesNames.Names` (129 entries, index = CC number, last = "None");
  a context action resets all rows to unassigned; on OK the rows are written
  back to settings.

The JUCE port has **neither**:
- `applyMidiSettings` sets devices/channel/delay but never parses
  `midiConfig.automationTable` into `controller.controlChangeAutomationTable()`
  — so the dictionary is empty at runtime: CC automation does nothing and the
  VFD `MIDI CC:` line is always blank, even though the table is persisted and
  reloaded from XML.
- The MIDI settings page has no mapping editor.
- There is no CC-name list in the C++ tree.

## Decision
Three coordinated parts.

1. **CC-name list (single source, generated).** Extend
   `extract_control_table.py` to emit `GeneratedControlChangeNames.inc` from the
   reference `Sanford.Multimedia.Midi/.../ControlChangesNames.cs` (129 names),
   surfaced by a small headless accessor in `xpl_app_core`
   (`controlChangeName(int)` / `controlChangeNameCount()`). Same
   extract-from-reference discipline as the other generated tables; no
   hand-copied list.

2. **Load path (the functional gap).** Add automation-table loading to the
   startup/settings-apply path (`applyMidiSettings`, shared by `MainComponent`
   startup and the settings dialog accept): clear
   `controller.controlChangeAutomationTable()`, then for each persisted
   `"NAME;CC"` entry parse on the last `;`, clamp CC to 1..128, and `add`.
   Parsing lives in a headless helper (unit-tested) so it is not tangled with
   JUCE. This alone makes CC automation and the VFD CC line work.

3. **Editor UI.** Add a `juce::TableListBox` (or a scrolled list of rows) to
   `MidiSettingsPage`: one row per parameter (friendly name from the existing
   `GeneratedParameterNames` table) with a CC-name column edited by a combo
   populated from the generated CC names; a "reset all to unassigned" button.
   The page seeds from `midiConfig.automationTable`, and on accept writes the
   rows back as `"NAME;CC"` and applies them (part 2). Parameter set / order =
   the persisted table's keys (reference parity).

4. **HTML export.** A headless `buildMidiMappingHtml(rows, generatedOn)` in
   `xpl_app_core` produces a self-contained (inline-CSS) light, compact,
   print-friendly document (Parameter / Control Change columns, title, project
   link, timestamp; HTML-escaped cells); the injected timestamp keeps it
   deterministic and unit-tested. An "Export as HTML" button on the page writes
   it via a `FileChooser` and opens it in the default browser
   (`File::startAsProcess`). Compact by design — tight cell padding, no
   decorative whitespace — per the owner's preference over the reference's
   more spacious layout.

## Consequences
- CC automation actually functions (input CCs move parameters; VFD shows the
  mapped CC) — a latent functional gap closed, independent of the UI.
- The mapping is user-editable as in the reference; friendly names and CC names
  both come from generated tables, so nothing drifts from the .NET source.
- One new generated asset; the parse helper and CC-name accessor are
  headless-tested.

## Open questions (owner)
- **Scope of the editable parameter set**: the reference lists exactly the
  persisted table's parameters (≈ all tone parameters). Proposed: same set,
  same order. Confirm if you want a filtered/grouped presentation instead.
- **CC display range**: the reference shows CCs 0..128 with 128 = "None" and
  clamps stored values to 1..128. Proposed: mirror exactly. Confirm.
