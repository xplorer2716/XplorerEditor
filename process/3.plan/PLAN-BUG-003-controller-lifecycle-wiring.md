# PLAN-BUG-003: The Application Never Starts or Stops the Controller

## Overview

`XpanderController::start()` and `stop()` are implemented and unit-tested, and
nothing in `juce/app/` calls either. The reference drives both from its view
layer, in three places the port never carried over; the controller-internal
`stop()`/`start()` pairs were all ported correctly, so the gap is exactly the
view layer.

Three defects follow, analysed in ADR-BUG-002: no transmission until an
unrelated operation happens to start the worker; the once-only first-start
patch request firing at the wrong moment and overwriting the session's first
loaded or randomized tone; and no smart all-notes-off at exit.

This plan adds the missing callers and supersedes the startup half of
TASK-BUG-003.

## References
- **Requirements**: RQ-BUG-003, RQ-BUG-002 (amended). Related: RQ-CTL-001,
  RQ-CTL-005, RQ-CTL-021, RQ-CTL-050, RQ-CTL-060, RQ-FMW-041, RQ-GUI-025,
  RQ-MID-006
- **ADRs**: ADR-BUG-002 (DEC-BUG-005 … DEC-BUG-009). Related: ADR-JUC-005
- **Supersedes**: TASK-BUG-003's startup call (PLAN-BUG-002), replaced per
  DEC-BUG-006. TASK-BUG-003's settings-accepted call is kept, relocated per
  DEC-BUG-008.

## Baseline (before any change)
Established on this toolchain, 2026-08-21, branch `feature/BUG` at `e3883d4`:

- `grep -rnw "start" juce/app/src/` — no call to the controller's `start()`;
  the only match is `ExtractFlow::start()`, unrelated.
- `git log -S"controller->start"` over `juce/app/` — empty: the call was never
  present and later removed, it was never written.
- Reference call sites, from `xplorer2716/XplorerEditor-dotnet-archive` at
  `38abfb2` and its `MidiApp` submodule at `0e5850f`:
  `MainForm.Overrides.cs:413`, `AbstractControllerMainForm.Events.cs:84-98`,
  `SettingsManager.cs:60/132` and `144/206`.
- Test suite — 117/117 pass.

---

## Tasks

### TASK-BUG-004: Wire the controller lifecycle into the application
- **Tier**: L
- **Status**: Done (2026-08-21)
- **Description**: Call `start()` at the end of the `MainComponent`
  constructor, replacing TASK-BUG-003's explicit startup patch request; call
  `stop()` in its destructor; call `stop()` in `SettingsContent`'s constructor
  and `start()` in its destructor, with the RQ-BUG-002 resynchronization moved
  after that restart and kept conditional on acceptance.
- **Requirement refs**: RQ-BUG-002, RQ-BUG-003
- **ADR refs**: ADR-BUG-002 (DEC-BUG-005 … DEC-BUG-009)
- **Acceptance Criteria** (Gherkin):
  - **Given** the main component, **When** it is constructed, **Then**
    `start()` is called once, after `applyMidiSettings`, and no explicit
    startup patch request remains
  - **Given** the main component, **When** it is destroyed, **Then** `stop()`
    is called
  - **Given** the settings content, **When** it is constructed, **Then**
    `stop()` is called; **When** it is destroyed, **Then** `start()` is called
    on every close path
  - **Given** an accepted Settings dialog, **When** it closes, **Then** the
    resynchronization is issued after `start()`, not before
  - **Given** a cancelled Settings dialog, **When** it closes, **Then** the
    controller is restarted and no resynchronization is issued
  - **Given** `applyMidiSettings`, **When** it is read, **Then** it contains no
    `isRunning()` guard (DEC-BUG-009)
- **Dependencies**: None
- **Assignee**: AI

### TASK-BUG-005: Pin the first-start contract with a unit test
- **Tier**: M
- **Status**: Done (2026-08-21)
- **Description**: Add a headless scenario asserting that the first `start()`
  aligns the current program number on the editing one and emits the program
  change and dump request for it, and that a second `start()` emits neither.
  This is the contract every consequence in ADR-BUG-002 rests on, and it was
  untested.
- **Requirement refs**: RQ-BUG-003
- **ADR refs**: ADR-BUG-002 (DEC-BUG-006)
- **Acceptance Criteria** (Gherkin):
  - **Given** a controller that has never been started with a known editing
    program number, **When** `start()` is called, **Then** a program change and
    a single-patch dump request are sent for that number **And**
    `currentProgramNumber()` equals it
  - **Given** that same controller, **When** `start()` is called a second time,
    **Then** no further message is sent
- **Dependencies**: None
- **Assignee**: AI

---

## Verification
Run on this toolchain, 2026-08-21:

1. `XplorerApp` (`-DXPL_BUILD_APP=ON`) and every headless target build with
   warnings-as-errors on (`-Wall -Wextra -Wpedantic -Werror`) — **no new
   diagnostic**.
2. `ctest` reports **118/118 pass** (117 before, plus TASK-BUG-005's scenario),
   1 skipped as always (virtual MIDI cable absent). No pre-existing test
   modified. Re-run after merging `feature/QLT` (PR #67) into this branch:
   **122/122 pass**, same skip.
3. The reference-parity analysis was re-run after the change. All four
   view-layer call sites of the reference are now matched, and the two frames
   the first-start branch emits are asserted by TASK-BUG-005. Result recorded
   in "Post-change parity check" below.

## Post-change parity check

| Reference site | Port site | Status |
|---|---|:-:|
| `MainForm.OnLoad:413` → `Start()` | `MainComponent.cpp` ctor, after `applyMidiSettings` | matched |
| `AbstractControllerMainForm` `DoCleanupBeforeClosing` → `Stop()` | `MainComponent::~MainComponent` | matched |
| … → `CloseMidiDevices()` | `~AbstractController()` | already covered |
| … → `UnRegisterForControllerEvents()` | handlers are `std::function` members of the controller | already covered |
| `SettingsManager.ShowSettingsDialog:144` → `Stop()` | `SettingsContent` ctor | matched |
| `SettingsManager.ShowSettingsDialog:206` → `finally { Start(); }` | `~SettingsContent` | matched (DEC-BUG-007) |
| `SettingsManager.LoadSettings:60/132` → guarded `Stop()`/`Start()` | not ported | intentional (DEC-BUG-009) |
| six controller-internal `Stop()`/`Start()` pairs | `loadTone`, `randomizeTone`, `morphTones`, `backupAllDataDumpToFile`, `getSingleTonesFromSynth`, program-dump handler | already matched before this plan |

Divergence from the reference, deliberate: the resynchronization on accepted
settings (RQ-BUG-002, DEC-BUG-008). The reference re-applies its MIDI settings
without re-requesting the patch.

## Note on testing
TASK-BUG-005 covers the controller contract, which is headless and testable.
The four call sites of TASK-BUG-004 are in `juce/app/src/`, compiled only into
the GUI target (`XPL_BUILD_APP`, default `OFF`) and covered by no test target;
they are verified by the source-level checks in the acceptance criteria and by
the owner's verification below.

Owner verification, on hardware:
- Start Xplorer, move a knob before doing anything else — the synth follows.
  Before this change nothing was transmitted until a load or a randomize.
- Start Xplorer, then randomize — the randomized tone stays. Before this
  change the first randomize of a session was overwritten by the synth's patch.
- Open Settings, cancel — no MIDI traffic beyond the all-notes-off that
  stopping the controller sends, and the editor keeps working afterwards.
- Quit while holding a note — the note stops.

## Out of scope
- The six controller-internal `stop()`/`start()` pairs — already faithful to
  the reference, listed in ADR-BUG-002's Context.
- `closeMidiDevices()` and handler release at teardown — already covered by
  `~AbstractController()` (DEC-BUG-005).
