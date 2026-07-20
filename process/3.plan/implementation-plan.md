# Implementation Plan — Xplorer JUCE Migration

> Bottom-up execution: MIDI base → MidiApp framework → Model → Controller → View.
> Every task = one commit series `TASK-JUC-NNN: <summary> [RQ-…]`, tier per the project process (M = tests required, L = tests + ADR + full delivery checklist).
> Non-UI tasks are TDD: failing Catch2 scenario first (tags = RQ IDs), then implementation, then refactor.

## Phase 0 — Bootstrap

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-010 | `juce/` CMake skeleton: FetchContent JUCE 8 + Catch2, layer targets, CTest wiring, warning-clean empty libs, GPL headers template | L | RQ-BLD-001..006 |
| TASK-JUC-011 | CI workflow: Linux configure/build/ctest on push | M | RQ-BLD-007 |
| TASK-JUC-012 | Test fixtures: reference `.syx` files (1 single patch min., 1 all-data dump) + expected byte vectors extracted from the C# reference | M | RQ-TST-002, RQ-TST-003 |

## Phase 1 — MIDI layer (`xpl_midi`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-020 | `MidiMessage` value type + backend interfaces (`MidiBackend`, `MidiInputPort`, `MidiOutputPort`) | M | RQ-MID-040, RQ-MID-010..012 (types) |
| TASK-JUC-021 | `MockMidiBackend`: scriptable input, captured output, loopback wiring | M | RQ-MID-041 |
| TASK-JUC-022 | `SysexStreamIterator` (byte-stream splitter, reference-identical) | M | RQ-MID-030 |
| TASK-JUC-023 | `JuceMidiBackend` adapters (enumeration, open-by-name, hot-swap, send/receive incl. large SysEx, error callback) | L | RQ-MID-001..006, 010..012, 020..024 |
| TASK-JUC-024 | Optional loopback integration test (skipped without virtual MIDI cable) | M | RQ-TST-005 |

## Phase 2 — Framework (`xpl_framework`, port of MidiApp.MidiController)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-030 | `AbstractParameter` (quantize/clamp/changed/clone, thread-safe) | M | RQ-FMW-001..003 |
| TASK-JUC-031 | `AbstractTone` (ordered parameter map, channel, randomize, morph) | M | RQ-FMW-010..012 |
| TASK-JUC-032 | `DualDictionary` (name⇄CC), `ToneReader/Writer` interfaces | M | RQ-FMW-032, RQ-FMW-020 |
| TASK-JUC-033 | `AbstractController` core: tone ownership, Start/Stop, SetParameter, devices by name via backend, transmit delay | L | RQ-FMW-030..034 |
| TASK-JUC-034 | Transmit worker (jthread + cv per ADR-JUC-005) with mock-backend pacing tests | L | RQ-FMW-040..042, RQ-NFR-001 |
| TASK-JUC-035 | Automation input behavior (CC scaling, disable-one-CC, forwarding) + events via `EventDispatcher` | L | RQ-FMW-050..052, 060..061, RQ-FMW-033 |
| TASK-JUC-036 | Services: logger, bug-report payload, single-instance guard | M | RQ-FMW-070..072, RQ-NFR-008 |

## Phase 3 — Model (`xpl_model`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-040 | Constants & enums (`XpanderConstants` port, page/subpage tables) | M | RQ-MOD-001..003 |
| TASK-JUC-041 | Parameter types (base, signed, mod-matrix, full-tone) with byte-exact SysEx generation tests | L | RQ-MOD-010..013, RQ-TST-003 |
| TASK-JUC-042 | Nibble packetization reader/writer | M | RQ-MOD-040 |
| TASK-JUC-043 | `XpanderSinglePatch` binary mapping (399-byte layout) | M | RQ-MOD-022 (layout) |
| TASK-JUC-044 | `XpanderTone`: full 226-parameter map + program numbers + name handling | L | RQ-MOD-020, 021, 023 |
| TASK-JUC-045 | Tone ⇄ bytes round-trip (`FromByteArray`/`ToByteArray`) against fixtures | L | RQ-MOD-022, RQ-MOD-050, RQ-TST-002 |
| TASK-JUC-046 | Modulation matrix (entries, rules, edit ops, randomize) | L | RQ-MOD-030..033 |
| TASK-JUC-047 | Tone reader/writer + sysex file type detection + bank enumeration | L | RQ-MOD-041..043 |

## Phase 4 — Controller (`xpl_controller` + `xpl_settings`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-050 | Settings service (interface + file persistence + .NET import + defaults) | L | RQ-SET-001..006 |
| TASK-JUC-051 | `XpanderController` base: load/save/extract, page-select-before-edit worker override, events | L | RQ-CTL-001..003, 020, 070 |
| TASK-JUC-052 | Synth-input handlers: patch dump, panel edits, page select, prog up/down, mod edit, multi-patch ignore, MIDI-activity | L | RQ-CTL-021..027 |
| TASK-JUC-053 | Dump machinery: backup/restore with pacing+progression, get-all-patches, store/goto, programmer mode | L | RQ-CTL-004..008, RQ-NFR-002 |
| TASK-JUC-054 | Mod-matrix ops, clipboard page families, randomizer, morphing | L | RQ-CTL-030, 040, 050, 051 |
| TASK-JUC-055 | Synth utilities: tune request, smart all-notes-off, VFD text (0x05/0x06), Matrix-12 variants | L | RQ-CTL-060..062 |

## Phase 5 — View (JUCE app)

> Refined 2026-07 after owner review of the reference UI (screenshot + description; ADR-JUC-006).
> Order: assets → shell → binding → blocks → matrix → display → menus/dialogs → skin.
> UI logic (registry, family substitution, matrix glue) is headless-tested; visual validation on Windows by the owner at each milestone (M1 = after 064, M2 = after 067, M3 = after 069).

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-060 | Asset & layout extraction: background PNG + button GIFs out of `MainForm.resx`; mechanical extraction of the 234-control table (id, type, bounds, tag) from `MainForm.Designer.cs` into a declarative C++ table | M | RQ-GUI-007, RQ-GUI-001 |
| TASK-JUC-061 | App shell: JUCE app target (`XPL_BUILD_APP`), main window with logical-canvas scaling + background bitmap, `JuceEventDispatcher`, single-instance guard, splash, top-level exception dialog | L | RQ-GUI-005..007, RQ-GUI-035 |
| TASK-JUC-062 | `ParameterBindingRegistry`: id→parameter wiring, anti-echo guard, automation-disable on edit, full-tone refresh — headless tests against mock backend | L | RQ-GUI-002..004 |
| TASK-JUC-063 | Fixed blocks (VCO1, VCO2, FM, LAG, VCF/VCA): standard JUCE controls placed from the extracted table, bound via registry | L | RQ-GUI-001, RQ-GUI-030, RQ-GUI-032 |
| TASK-JUC-064 | `PageFamilyBlock` ×4 (ENV/LFO/RAMP/TRACK): shared controls, selector buttons, digit substitution, synth page-change sync — logic headless-tested. **Milestone M1: owner Windows build** | L | RQ-GUI-010..012 |
| TASK-JUC-065 | Modulation matrix panel: 20 rows, availability rules, entry/full-tone refresh | L | RQ-GUI-015..017 |
| TASK-JUC-066 | Display panel (fixed-font text: program, name, last parameter, typewriter) + 8 shortcut buttons + MIDI activity indicator. *M2 owner review (#9): `VfdDisplayHelper` ported so the last-parameter line shows the friendly name + value-by-control-type (combo/radio label, checkbox Y/N, knob numeric) + MIDI CC, and modulation-matrix edits display SRC/DEST/AMNT/QTZ; the `.` active-mod-destination marker is deferred.* | L | RQ-GUI-020..022 |
| TASK-JUC-067 | Menu bar + dialogs: settings (3 pages: MIDI/UI/randomizer), rename, store/goto, extract single tones, backup/restore + get-all-patches with progress, about; window-wide `.syx` drag & drop + shared load-by-type. **Milestone M2**. *Morphing deferred: the reference `ToneMorphingForm` is flagged work-in-progress (empty OK/Cancel, not wired to any menu) — see architecture-analysis_juce §11.* | L | RQ-GUI-008, RQ-GUI-025..027, RQ-GUI-029 |
| TASK-JUC-068 | Piano-keyboard window | M | RQ-GUI-028 |
| TASK-JUC-069 | Skin pass: `XplorerLookAndFeel` (knobs + LED ring per UiConfiguration, image buttons, custom checks/radios). **Milestone M3**. *The VFD rendering piece is split out into TASK-JUC-073.* | L | RQ-GUI-031 |
| TASK-JUC-073 | VFD bitmap glyph rendering (added 2026-07 after M2 review): extract `MATRIXTINY` sheet to assets, `DisplayPanel` paints 12×16 glyph cells per ADR-JUC-007 (direct sprite-sheet paint + `setBufferedToImage`, reference grid formula, nearest-neighbour scaling); `VfdDisplayHelper` wrap width unified with the computed grid; display grown to 5 rows (82 px, upward) per the owner-arbitrated option (b). | L | RQ-GUI-033 |
| TASK-JUC-074 | MIDI LED panel (added 2026-07 after M2 review): replace the single activity LED by the reference 3-LED `LedPanelControl` port — green automation-in / blue synth-in / red synth-out, reference geometry and colours, ≈100 ms retriggerable hold, event-driven timer per ADR-JUC-008. | M | RQ-GUI-022 |
| TASK-JUC-075 | Rotary knob cosmetics + page-selector captions (added 2026-07 after M2/M3 review): selector buttons show `ENV 1`…/`TRACK 3` (extracted `.label`, instance tracked separately from the caption); rotary rendered as LED-ring-only (no centre pointer) with a hover-brightened ring; knob text box removed → value as a drag popup bubble + double-click inline numeric entry, rotary fills its full bounds (larger). Per ADR-JUC-009. *Awaiting owner go-ahead.* | M | RQ-GUI-011, RQ-GUI-031, RQ-GUI-034 |
| TASK-JUC-076 | Modulation-matrix hover highlight + VFD active-destination marker (added 2026-07 after M2/M3 review): headless `ModulationDestinationResolver` + page-selector→source map (`xpl_app_core`, unit-tested); `ModMatrixPanel` highlight API; mouse enter/exit wiring from selectors and knobs; and the deferred VFD `.` marker resolved with the same resolver. Per ADR-JUC-010 (closes analysis gap §10-12). | L | RQ-GUI-018, RQ-GUI-020 |
| TASK-JUC-077 | LED-colour single source of truth (added 2026-07 after M3 review): the LookAndFeel becomes the sole runtime owner of the knob LED colour (`ledColour()` accessor); `ModMatrixPanel` derives the highlight colour from it instead of caching a copy, so changing the colour in the settings updates the matrix highlight live. Removes the duplicated colour state. Per ADR-JUC-011. | M | RQ-GUI-018, RQ-GUI-031 |
| TASK-JUC-078 | MIDI CC automation table — load path + editor (added 2026-07 after M3 review): (1) generate `GeneratedControlChangeNames.inc` (129 CC names) from the reference; (2) load the persisted `"NAME;CC"` table into the controller dictionary on startup/settings-apply (headless-tested parser) — currently missing, so CC automation and the VFD CC line do not work; (3) add the parameter↔CC mapping editor (list + per-row CC combo + reset) to the MIDI settings page; (4) an "Export as HTML" action writing a compact light print-friendly mapping document (headless `buildMidiMappingHtml`, unit-tested) opened in the browser. Per ADR-JUC-012. | L | RQ-GUI-036, RQ-SET-002 |

## Phase 6 — Integration & cut-over

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-070 | Windows build validation + packaging: native MSVC `windows-latest` CI (`juce-windows.yml`) building `XplorerApp` (x64) + tests, uploading `Xplorer.exe` as an artifact. *MinGW cross-compile ruled out — JUCE `#error`s on it (RQ-BLD-008).* | L | RQ-BLD-002, RQ-BLD-008 |
| TASK-JUC-071 | Manual hardware validation checklist executed by owner (real Xpander/Matrix-12) | — | RQ-TST-006 |
| TASK-JUC-080 | Vector background (added 2026-07, session goal): replace `main-background.jpg` by a JUCE-drawn vector background per ADR-JUC-013 — `paintVectorBackground` (`BackgroundRenderer.cpp`) transcribing the owner-validated mockup 1:1 (boxes/lines/stubs/captions/sections/out-labels), direct `Graphics` paint (no image buffering under the canvas transform), procedural brushed-metal/wood textures with fixed seed; JPEG dropped from BinaryData and the splash now renders the vector background to an image. See ADR-JUC-013 Implementation Notes for the two recorded deviations. **Status: Done — built (XplorerApp, exit 0) and visually verified against the mockup under Xvfb; controls overlay their stubs exactly.** | L | RQ-GUI-037, RQ-GUI-005 |
| TASK-JUC-081 | Vector background corner finish (added 2026-07, owner review of TASK-JUC-080): stroke the diagram signal lines as `juce::Path` segments with rounded joint + end-cap style (`frameStroke`) instead of `drawLine`, so perpendicular 90° junctions get the same softly-rounded corner as the block frames (`drawRoundedRectangle`) rather than a notched butt-cap. Cosmetic only, no geometry change. **Status: Done — rebuilt (XplorerApp, exit 0) and verified under Xvfb; all line corners now match the frames.** | S | RQ-GUI-037, ADR-JUC-013 |
| TASK-JUC-082 | Vector background finish pass (added 2026-07, owner review): (1) drop the procedural brushed-metal streaks that read as unwanted horizontal lines — keep only the gentle vertical luminance gradient (light shading); (2) re-sync the SVG prototype `ADR-JUC-013-mockup-generator.py` / `ADR-JUC-013-mockup.svg` with the JUCE code (streaks removed + round line caps, so the script keeps prototyping the real rendering). Cosmetic only. **Status: Done — rebuilt (XplorerApp, exit 0) and verified under Xvfb; SVG regenerated.** | S | RQ-GUI-037, ADR-JUC-013 |
| TASK-JUC-083 | Canvas top-band crop (added 2026-07, owner review): remove the 32 px dead band the reference bitmap reserved for the WinForms menustrip (measured on the original: 14 px dark strip + 18 px empty plate, first content at y=32). `extract_control_table.py` gains `CANVAS_TOP_CROP = 32` (every control y shifts up; the script also no longer re-extracts the retired background bitmap), `GeneratedControlTable.inc` regenerated, `LOGICAL_CANVAS_HEIGHT` 813 → 781, and the diagram geometry (kept in reference/mockup coordinates) is translated up by the crop in both `BackgroundRenderer.cpp` and the SVG prototype generator. **Status: Done — rebuilt (XplorerApp, exit 0), verified under Xvfb: diagram starts right under the menu bar, controls stay aligned on their stubs, bottom row intact.** | M | RQ-GUI-037, RQ-GUI-001, ADR-JUC-013 |
| TASK-JUC-084 | Rotary knob transparent interior (added 2026-07, owner review): `drawRotarySlider` no longer fills a dark knob body — the interior is left transparent so the (non-opaque) `Slider`'s panel background shows through; only the ring track + coloured crown are drawn. **Status: Done — rebuilt (XplorerApp, exit 0), verified under Xvfb: knobs show the shaded background inside the ring.** | S | RQ-GUI-031, ADR-JUC-009 |
| TASK-JUC-085 | Vector background padding + flat section bars (added 2026-07, owner review): (1) keep a 5 px black margin at the top of the panel and add a matching one at the bottom (`CANVAS_PADDING`), so a full-screen window clears the menu bar and OS taskbar — `CANVAS_TOP_CROP` becomes 32−5=27, `LOGICAL_CANVAS_HEIGHT` 781→786, table regenerated, painter + SVG generator inset the panel and translate the diagram by the new crop; (2) repaint the section-title bars as flat rects with a left-to-right blue luminance gradient (bright at the label) instead of the vertical tube gradient. **Status: Done — rebuilt (XplorerApp, exit 0) and full test suite green (81/81); verified under Xvfb.** | M | RQ-GUI-037, ADR-JUC-013 |
| TASK-JUC-086 | Vector background alignment pass (added 2026-07, owner review): (1) fill the top/bottom panel margins with panel material (full-height metal + wood rails) instead of black — the black bands made the menu-bar edge stand out; (2) snap every diagram stub tick and its caption to the exact control-table knob centre (the mockup values had drifted, worst on TRACK X ~3–7 px), so all rotaries are centred under their ticks; (3) left-align the LAG IN / TRACK IN labels with their selector combos (x=combo left) at a common 9 px offset. Painter + SVG generator updated together. **Status: Done — rebuilt (XplorerApp, exit 0), test suite green (81/81), verified under Xvfb.** | S | RQ-GUI-037, ADR-JUC-013 |
| TASK-JUC-087 | Combo box full-text labels (added 2026-07, issue #12): `XplorerLookAndFeel::getComboBoxFont` override — per combo box, measures its widest item label against the text area `LookAndFeel_V4::positionComboBoxText` lays out (box width − 30px arrow zone − 10px label border) and shrinks the base font (down to a 9px floor) so it always fits, instead of JUCE's default 70%-squish-then-ellipsis. **Status: Done — rebuilt (XplorerApp, exit 0), test suite green (81/81); verified under Xvfb: no ellipsis anywhere (KEYBD, VCO1, TRIANGLE, 1 POLE LOW, etc. all shown in full).** | M | RQ-GUI-032, issue #12 |
| TASK-JUC-088 | Page copy/paste UI (added 2026-07, issue #13): new `PageSelectorButton` (`PageFamilyBlock.hpp/.cpp`) — right-click opens a "Copy Page"/"Paste Page" context menu (Paste greyed out per `XpanderController::canClipboardPasteTo`), Ctrl+C/Ctrl+V do the same while the button is focused; both call the already-ported controller clipboard (`setClipboardSource`/`pasteClipboardTo`/`clearClipboard`, RQ-CTL-040 — no controller change needed). Left-click keeps the normal radio-selection behavior. **Status: Done — rebuilt (XplorerApp, exit 0), test suite green (81/81); verified interactively under Xvfb with xdotool: right-click ENV_2 → Copy Page → right-click ENV_4 shows Paste Page now enabled → click → app stable.** | M | RQ-GUI-027, RQ-CTL-040, issue #13 |
| TASK-JUC-089 | VFD unassigned-CC sentinel (added 2026-07, issue #11): `VfdDisplayHelper::showControlEdit` now excludes `unassignedControlChange()` (128, the CC-name list's last "None" entry) from the shown `MIDI CC:` line, matching the reference's `ccNumber != Length-1` check — previously every unassigned parameter (the loaded defaults are all `NAME;128`) showed "MIDI CC: 128". **Status: Done — rebuilt (XplorerApp, exit 0), test suite green (81/81).** | S | RQ-GUI-020, issue #11 |
| TASK-JUC-079 | AGNOS process conformance (added 2026-07): write `process/_sessionstate/session.yaml` (platform=linux, unit_tests=false); rename artifact identifiers to the AGNOS `<TYPE>-<TRI>-NNN` format under the single migration trigram `JUC` — `ADR-NNN` → `ADR-JUC-NNN` and `TASK-JUCE-NNN` → `TASK-JUC-NNN` (numbers preserved) — across ADR files, plan, requirements, analysis and all source-code/CMake traceability comments. RQ IDs keep their per-area trigrams (already conformant). Comments/docs only; no behaviour change. | S | RQ-TST (process) |
| TASK-JUC-072 | Cross-compat campaign: patch libraries and settings exchanged between .NET and JUCE builds | M | RQ-MOD-050, RQ-SET-006, RQ-NFR-003 |

## Design System (RQ-DSN, ADR-JUC-014 / ADR-JUC-015)

Unified graphical design system: a single source of truth for colour,
typography, geometry and motion, so the app's visual maintenance goes through
tokens instead of scattered literals in the JUCE code.

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-JUC-090 | `DesignTokens.hpp` three-tier token module (global→semantic→component). **Status: Done — superseded as *generated* by TASK-JUC-095.** | M | RQ-DSN-001..024, ADR-JUC-014 |
| TASK-JUC-091 | Migrate `XplorerLookAndFeel` to tokens (value-preserving). **Status: Done — build 0 err, 81/81 tests.** | M | RQ-DSN-061 |
| TASK-JUC-092 | Migrate `BackgroundRenderer` palette + font scale to tokens. **Status: Done.** | M | RQ-DSN-061, ADR-JUC-013 |
| TASK-JUC-093 | Migrate `ModMatrixPanel`, `SettingsDialog`, `ProgressWindow`, `MainComponent` LED panel to tokens. **Status: Done.** | M | RQ-DSN-061 |
| TASK-JUC-094 | Verify: build + 81/81 functional tests + Xvfb smoke + grep (no raw colour literal outside the module) + value-identity by construction. **Status: Done.** | S | RQ-DSN-071, RQ-DSN-090 |
| TASK-JUC-095 | `juce/tools/design-tokens.yaml` single source + `generate_design_tokens.py` emitting `DesignTokens.hpp` (generated, `--check` idempotent). Value-identical to the hand-written header (27 colours, 0 ARGB diff). **Status: Done — regenerated, build 0 err, 81/81 tests.** | L | RQ-DSN-060, RQ-DSN-063, ADR-JUC-015 |
| TASK-JUC-096 | Move + refactor the mockup generator to `juce/tools/generate_background_mockup.py`, reading palette/fonts/line-width from the YAML via the shared `resolve()`; geometry unchanged. Regenerated SVG byte-identical. **Status: Done.** | M | RQ-DSN-063, ADR-JUC-013, ADR-JUC-015 |
| TASK-JUC-097 | Move `background-mockup.svg` out of `process/` to `juce/tools/`; update path/mechanism references in ADR-JUC-013, ADR-JUC-014, RQ-DSN-060/063; folder-layering rule (process=spec, tools=scripts, src=impl). **Status: Done.** | S | ADR-JUC-015 |
| TASK-JUC-102 | Rotary knob unlit-ring colour fix (added 2026-07, owner visual review): the `controlTrack` token was an opaque `#282E36` slate, reading as a distinctly dark ring — the reference `KnobControl.cs` (`DEFAULT_KNOB_LED_BACKGROUND_COLOR`, Standard style) draws it as `ARGB(5,255,255,255)`, a ~2% white wash over the panel, effectively near-invisible. Token corrected: `semantic.controlTrack` repointed from `global.slate700` (now unused, removed) to `global.white`, new `global.knobTrackAlpha = 0.0196` (5/255) aliased in `component.knobTrackAlpha`; `XplorerLookAndFeel::drawRotarySlider` now does `controlTrack.withAlpha(knobTrackAlpha)`. **Status: Done — regenerated `DesignTokens.hpp` (`--check` clean), rebuilt (XplorerApp, exit 0), test suite green (81/81), verified under Xvfb: ring now near-invisible like the reference.** | S | RQ-DSN-061, ADR-JUC-014 |

### Design-system backlog (phase 2 — owner-reasoned, NOT value-preserving)

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-JUC-098 | Spacing / layout token scale (needs an owner-approved base unit; `RQ-DSN-020` deferred it). | L | RQ-DSN-020 |
| TASK-JUC-099 | Font-size consolidation (e.g. 13.5 vs 13) — changes pixels, owner sign-off. | M | RQ-DSN-010 |
| TASK-JUC-100 | Missing interaction states: shared hover/disabled/focus for toggle/combo/list rows. | M | RQ-DSN-030..033 |
| TASK-JUC-101 | Accessibility gaps: colour-alone LED/highlight meaning, contrast audit, focus ring. | M | RQ-DSN-050..052 |

## Traceability matrix

Generated mechanically: Catch2 tags `[RQ-…]` ↔ requirement files; per-task RQ column above; commit messages carry task + RQ IDs. A snapshot table will be added to this file at each phase end (phase, tasks done, RQ covered by passing tests, RQ remaining).

## Sequencing rules

1. A phase starts only when the previous phase's tests are green in CI (Linux) — except Phase 5+ (GUI), validated on Windows by the owner.
2. Behavior questions are always answered by reading the C# reference; ambiguities that the reference cannot settle are logged and asked to the owner, never guessed.
3. Deviations from reference behavior (allowed only for RQ-NFR-009 defects) each get an ADR.
