# Implementation Plan — Xplorer JUCE Migration

> Bottom-up execution: MIDI base → MidiApp framework → Model → Controller → View.
> Every task = one commit series `TASK-JUCE-NNN: <summary> [RQ-…]`, tier per the project process (M = tests required, L = tests + ADR + full delivery checklist).
> Non-UI tasks are TDD: failing Catch2 scenario first (tags = RQ IDs), then implementation, then refactor.

## Phase 0 — Bootstrap

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUCE-010 | `juce/` CMake skeleton: FetchContent JUCE 8 + Catch2, layer targets, CTest wiring, warning-clean empty libs, GPL headers template | L | RQ-BLD-001..006 |
| TASK-JUCE-011 | CI workflow: Linux configure/build/ctest on push | M | RQ-BLD-007 |
| TASK-JUCE-012 | Test fixtures: reference `.syx` files (1 single patch min., 1 all-data dump) + expected byte vectors extracted from the C# reference | M | RQ-TST-002, RQ-TST-003 |

## Phase 1 — MIDI layer (`xpl_midi`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUCE-020 | `MidiMessage` value type + backend interfaces (`MidiBackend`, `MidiInputPort`, `MidiOutputPort`) | M | RQ-MID-040, RQ-MID-010..012 (types) |
| TASK-JUCE-021 | `MockMidiBackend`: scriptable input, captured output, loopback wiring | M | RQ-MID-041 |
| TASK-JUCE-022 | `SysexStreamIterator` (byte-stream splitter, reference-identical) | M | RQ-MID-030 |
| TASK-JUCE-023 | `JuceMidiBackend` adapters (enumeration, open-by-name, hot-swap, send/receive incl. large SysEx, error callback) | L | RQ-MID-001..006, 010..012, 020..024 |
| TASK-JUCE-024 | Optional loopback integration test (skipped without virtual MIDI cable) | M | RQ-TST-005 |

## Phase 2 — Framework (`xpl_framework`, port of MidiApp.MidiController)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUCE-030 | `AbstractParameter` (quantize/clamp/changed/clone, thread-safe) | M | RQ-FMW-001..003 |
| TASK-JUCE-031 | `AbstractTone` (ordered parameter map, channel, randomize, morph) | M | RQ-FMW-010..012 |
| TASK-JUCE-032 | `DualDictionary` (name⇄CC), `ToneReader/Writer` interfaces | M | RQ-FMW-032, RQ-FMW-020 |
| TASK-JUCE-033 | `AbstractController` core: tone ownership, Start/Stop, SetParameter, devices by name via backend, transmit delay | L | RQ-FMW-030..034 |
| TASK-JUCE-034 | Transmit worker (jthread + cv per ADR-005) with mock-backend pacing tests | L | RQ-FMW-040..042, RQ-NFR-001 |
| TASK-JUCE-035 | Automation input behavior (CC scaling, disable-one-CC, forwarding) + events via `EventDispatcher` | L | RQ-FMW-050..052, 060..061, RQ-FMW-033 |
| TASK-JUCE-036 | Services: logger, bug-report payload, single-instance guard | M | RQ-FMW-070..072, RQ-NFR-008 |

## Phase 3 — Model (`xpl_model`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUCE-040 | Constants & enums (`XpanderConstants` port, page/subpage tables) | M | RQ-MOD-001..003 |
| TASK-JUCE-041 | Parameter types (base, signed, mod-matrix, full-tone) with byte-exact SysEx generation tests | L | RQ-MOD-010..013, RQ-TST-003 |
| TASK-JUCE-042 | Nibble packetization reader/writer | M | RQ-MOD-040 |
| TASK-JUCE-043 | `XpanderSinglePatch` binary mapping (399-byte layout) | M | RQ-MOD-022 (layout) |
| TASK-JUCE-044 | `XpanderTone`: full 226-parameter map + program numbers + name handling | L | RQ-MOD-020, 021, 023 |
| TASK-JUCE-045 | Tone ⇄ bytes round-trip (`FromByteArray`/`ToByteArray`) against fixtures | L | RQ-MOD-022, RQ-MOD-050, RQ-TST-002 |
| TASK-JUCE-046 | Modulation matrix (entries, rules, edit ops, randomize) | L | RQ-MOD-030..033 |
| TASK-JUCE-047 | Tone reader/writer + sysex file type detection + bank enumeration | L | RQ-MOD-041..043 |

## Phase 4 — Controller (`xpl_controller` + `xpl_settings`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUCE-050 | Settings service (interface + file persistence + .NET import + defaults) | L | RQ-SET-001..006 |
| TASK-JUCE-051 | `XpanderController` base: load/save/extract, page-select-before-edit worker override, events | L | RQ-CTL-001..003, 020, 070 |
| TASK-JUCE-052 | Synth-input handlers: patch dump, panel edits, page select, prog up/down, mod edit, multi-patch ignore, MIDI-activity | L | RQ-CTL-021..027 |
| TASK-JUCE-053 | Dump machinery: backup/restore with pacing+progression, get-all-patches, store/goto, programmer mode | L | RQ-CTL-004..008, RQ-NFR-002 |
| TASK-JUCE-054 | Mod-matrix ops, clipboard page families, randomizer, morphing | L | RQ-CTL-030, 040, 050, 051 |
| TASK-JUCE-055 | Synth utilities: tune request, smart all-notes-off, VFD text (0x05/0x06), Matrix-12 variants | L | RQ-CTL-060..062 |

## Phase 5 — View (JUCE app)

> Refined 2026-07 after owner review of the reference UI (screenshot + description; ADR-006).
> Order: assets → shell → binding → blocks → matrix → display → menus/dialogs → skin.
> UI logic (registry, family substitution, matrix glue) is headless-tested; visual validation on Windows by the owner at each milestone (M1 = after 064, M2 = after 067, M3 = after 069).

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUCE-060 | Asset & layout extraction: background PNG + button GIFs out of `MainForm.resx`; mechanical extraction of the 234-control table (id, type, bounds, tag) from `MainForm.Designer.cs` into a declarative C++ table | M | RQ-GUI-007, RQ-GUI-001 |
| TASK-JUCE-061 | App shell: JUCE app target (`XPL_BUILD_APP`), main window with logical-canvas scaling + background bitmap, `JuceEventDispatcher`, single-instance guard, splash, top-level exception dialog | L | RQ-GUI-005..007, RQ-GUI-035 |
| TASK-JUCE-062 | `ParameterBindingRegistry`: id→parameter wiring, anti-echo guard, automation-disable on edit, full-tone refresh — headless tests against mock backend | L | RQ-GUI-002..004 |
| TASK-JUCE-063 | Fixed blocks (VCO1, VCO2, FM, LAG, VCF/VCA): standard JUCE controls placed from the extracted table, bound via registry | L | RQ-GUI-001, RQ-GUI-030, RQ-GUI-032 |
| TASK-JUCE-064 | `PageFamilyBlock` ×4 (ENV/LFO/RAMP/TRACK): shared controls, selector buttons, digit substitution, synth page-change sync — logic headless-tested. **Milestone M1: owner Windows build** | L | RQ-GUI-010..012 |
| TASK-JUCE-065 | Modulation matrix panel: 20 rows, availability rules, entry/full-tone refresh | L | RQ-GUI-015..017 |
| TASK-JUCE-066 | Display panel (fixed-font text: program, name, last parameter, typewriter) + 8 shortcut buttons + MIDI activity indicator. *M2 owner review (#9): `VfdDisplayHelper` ported so the last-parameter line shows the friendly name + value-by-control-type (combo/radio label, checkbox Y/N, knob numeric) + MIDI CC, and modulation-matrix edits display SRC/DEST/AMNT/QTZ; the `.` active-mod-destination marker is deferred.* | L | RQ-GUI-020..022 |
| TASK-JUCE-067 | Menu bar + dialogs: settings (3 pages: MIDI/UI/randomizer), rename, store/goto, extract single tones, backup/restore + get-all-patches with progress, about; window-wide `.syx` drag & drop + shared load-by-type. **Milestone M2**. *Morphing deferred: the reference `ToneMorphingForm` is flagged work-in-progress (empty OK/Cancel, not wired to any menu) — see architecture-analysis_juce §11.* | L | RQ-GUI-008, RQ-GUI-025..027, RQ-GUI-029 |
| TASK-JUCE-068 | Piano-keyboard window | M | RQ-GUI-028 |
| TASK-JUCE-069 | Skin pass: `XplorerLookAndFeel` (knobs + LED ring per UiConfiguration, image buttons, custom checks/radios). **Milestone M3**. *The VFD rendering piece is split out into TASK-JUCE-073.* | L | RQ-GUI-031 |
| TASK-JUCE-073 | VFD bitmap glyph rendering (added 2026-07 after M2 review): extract `MATRIXTINY` sheet to assets, `DisplayPanel` paints 12×16 glyph cells per ADR-007 (direct sprite-sheet paint + `setBufferedToImage`, reference grid formula, nearest-neighbour scaling); `VfdDisplayHelper` wrap width unified with the computed grid; display grown to 5 rows (82 px, upward) per the owner-arbitrated option (b). | L | RQ-GUI-033 |
| TASK-JUCE-074 | MIDI LED panel (added 2026-07 after M2 review): replace the single activity LED by the reference 3-LED `LedPanelControl` port — green automation-in / blue synth-in / red synth-out, reference geometry and colours, ≈100 ms retriggerable hold, event-driven timer per ADR-008. | M | RQ-GUI-022 |

## Phase 6 — Integration & cut-over

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUCE-070 | Windows build validation + packaging: native MSVC `windows-latest` CI (`juce-windows.yml`) building `XplorerApp` (x64) + tests, uploading `Xplorer.exe` as an artifact. *MinGW cross-compile ruled out — JUCE `#error`s on it (RQ-BLD-008).* | L | RQ-BLD-002, RQ-BLD-008 |
| TASK-JUCE-071 | Manual hardware validation checklist executed by owner (real Xpander/Matrix-12) | — | RQ-TST-006 |
| TASK-JUCE-072 | Cross-compat campaign: patch libraries and settings exchanged between .NET and JUCE builds | M | RQ-MOD-050, RQ-SET-006, RQ-NFR-003 |

## Traceability matrix

Generated mechanically: Catch2 tags `[RQ-…]` ↔ requirement files; per-task RQ column above; commit messages carry task + RQ IDs. A snapshot table will be added to this file at each phase end (phase, tasks done, RQ covered by passing tests, RQ remaining).

## Sequencing rules

1. A phase starts only when the previous phase's tests are green in CI (Linux) — except Phase 5+ (GUI), validated on Windows by the owner.
2. Behavior questions are always answered by reading the C# reference; ambiguities that the reference cannot settle are logged and asked to the owner, never guessed.
3. Deviations from reference behavior (allowed only for RQ-NFR-009 defects) each get an ADR.
