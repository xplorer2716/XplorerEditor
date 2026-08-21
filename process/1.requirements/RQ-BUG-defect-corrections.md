# FTR-BUG-001: Defect Corrections

## Overview
Corrections of defects found in the shipped JUCE build. Each requirement here
states the behaviour the application SHALL have after the fix, so the defect and
its resolution stay greppable by ID like any other requirement.

Requirements in this file may deliberately depart from the reference .NET
implementation. That is not the default for this port (RQ-MOD-050 and the
"bit-exact versus the reference" scope note of RQ-MOD apply everywhere else) and
each departure SHALL be justified by an ADR — see ADR-BUG-001.

## Stakeholders
- **Owner**: project owner (defect reports)
- **Consumers**: `xpl_model` (`XpanderTone` randomizer helpers), `xpl_controller`
  (RQ-CTL-050), the Randomizer settings page, the user manual

---

## Functional Requirements

### RQ-BUG-001: Randomizer "Octave" VCO frequency strategy
- **Category**: Functional
- **EARS Type**: Event-driven
- **Statement**: WHEN the VCO frequency strategy is applied with the value
  `Octave`, the model SHALL set `VCO1_FREQ` to 0 and `VCO2_FREQ` to 12, so that
  VCO2 sounds exactly one octave above VCO1.
- **Rationale**: The reference implementation has no `case` for `Octave`, so the
  option falls through to `Free` and leaves both oscillator pitches random —
  the one strategy value in the list that is indistinguishable from doing
  nothing. The port carried the omission over faithfully
  (`XpanderTone.cpp`, "reference has no case: falls through unchanged"), which
  means a user selecting "Octave" on the Randomizer settings page gets no
  octave and no indication that the setting was ignored.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - **Given** a tone, **When** the VCO frequency strategy `Octave` is applied,
    **Then** `VCO1_FREQ` is 0 **And** `VCO2_FREQ` is 12
  - **Given** a tone, **When** the strategy `Free` is applied, **Then** neither
    `VCO1_FREQ` nor `VCO2_FREQ` is modified — `Octave` no longer shares its
    behaviour
  - **Given** the interval strategies as a family, **When** each is applied,
    **Then** `Octave` sits between `Seventh` (11) and `Ninth` (14) in the
    semitone distance it produces, matching its position in the option list
- **Dependencies**: RQ-CTL-050, RQ-MOD-033, RQ-SET-003; ADR-BUG-001

### RQ-BUG-002: Synchronization with the synthesizer at startup and after a MIDI settings change
- **Category**: Functional
- **EARS Type**: Event-driven
- **Statement**: WHEN the application finishes applying its persisted MIDI
  settings at startup, and WHEN the user accepts a change in the Settings
  dialog, the application SHALL synchronize with the synthesizer so that the
  patch shown in the editor is the patch the synthesizer holds.
- **Correction (2026-08-21, TASK-BUG-004)**: this requirement was first
  delivered as two explicit `sendProgramChangeAndGetSinglePatchFromSynth(
  currentProgramNumber())` calls. Analysis of the reference implementation
  (see ADR-BUG-002) showed the startup half belongs to the controller's own
  first-start behaviour, which the application had never triggered, and which
  synchronizes on the **editing** program number — the "Default patch number"
  setting — not the current one. The startup call is therefore replaced by
  `start()` (RQ-BUG-003) and only the settings-accepted resync remains an
  explicit call, made after the controller has been restarted so the reply
  dump is not received on stopped input ports.
- **Rationale**: The reference .NET implementation synchronized on both events.
  The JUCE port applies the MIDI settings (`applyMidiSettings`) but never
  follows them with the patch request, so the editor opens on its own default
  tone while the synthesizer sits on whatever patch it was left on — and the two
  stay out of step until the user happens to trigger a Goto, Store or the
  Patch > Synchronize menu item. Changing the MIDI ports mid-session has the
  same effect: the newly opened port has never been asked for a patch.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - **Given** persisted MIDI settings naming a synth output port, **When** the
    application starts, **Then** a program change and a single-patch dump
    request are sent for the program number those settings designate
  - **Given** an open Settings dialog with a changed MIDI port, **When** the
    user accepts it, **Then** a program change and dump request are sent on the
    newly applied port, for the patch the user is currently editing
  - **Given** the Settings dialog, **When** the user accepts it, **Then** the
    resynchronization happens after the controller has been restarted, so the
    synthesizer's reply reaches running input ports
  - **Given** the Settings dialog, **When** the user cancels it, **Then** no
    synchronization is triggered
- **Dependencies**: RQ-BUG-003, RQ-CTL-006, RQ-CTL-021, RQ-GUI-008, RQ-GUI-025;
  ADR-BUG-002

### RQ-BUG-003: The application drives the controller lifecycle
- **Category**: Functional
- **EARS Type**: Complex
- **Statement**: The application SHALL own the controller's run state:
  - WHEN the main window has applied its persisted MIDI settings at startup,
    the application SHALL start the controller;
  - WHEN the main window is destroyed, the application SHALL stop the
    controller before releasing it;
  - WHEN the Settings dialog opens, the application SHALL stop the controller,
    and WHEN that dialog closes by any route — accept, cancel, Escape or the
    title bar — the application SHALL start it again.
- **Rationale**: `XpanderController::start()` and `stop()` are fully
  implemented and unit-tested, but nothing in `juce/app/` has ever called
  either. Three defects follow. (a) The transmit worker is only started by the
  trailing `start()` inside `loadTone`, `randomizeTone`, `morphTones`,
  `backupAllDataDumpToFile`, `getSingleTonesFromSynth` and the program-dump
  handler, so until one of those runs, panel edits are queued and never sent.
  (b) Because `start()` has never run, its once-only first-start branch is
  still armed, and the first of those operations fires a patch request whose
  reply overwrites the tone the user just loaded or randomized. (c) `stop()` is
  never reached at exit, so the smart all-notes-off of RQ-CTL-060 is never
  sent. All three are consequences of the same omission: the reference drives
  this lifecycle from its view layer, and the port carried over the controller
  but not its callers.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - **Given** a controller that has never been started, **When** `start()` is
    called, **Then** the current program number is aligned on the editing
    program number **And** a program change and a single-patch dump request are
    sent for it
  - **Given** a controller already started once, **When** `start()` is called
    again, **Then** no program change and no dump request are sent
  - **Given** the application at startup, **When** the main window is
    constructed, **Then** the controller is started once, after its MIDI
    settings are applied
  - **Given** a running application, **When** the main window is destroyed,
    **Then** the controller is stopped before it is released
  - **Given** the Settings dialog, **When** it opens, **Then** the controller is
    stopped, **And When** it closes by any route, **Then** the controller is
    started again exactly once
  - **Given** the first tone-mutating operation of a session (load, randomize,
    morph, backup, get-single-tones), **When** it completes, **Then** its
    trailing `start()` sends no patch request, because the first-start branch
    was already consumed at application startup
- **Dependencies**: RQ-BUG-002, RQ-CTL-001, RQ-CTL-005, RQ-CTL-021, RQ-CTL-050,
  RQ-CTL-060, RQ-FMW-041, RQ-GUI-025, RQ-MID-006; ADR-BUG-002, ADR-JUC-005

---

## Traceability note
The persisted settings value is unaffected: `Octave` already round-trips through
`XmlSettingsService` under the name `"Octave"` and keeps its enumerator position
(`EnumRandomVCOFreq`), so settings files written before this correction select
the corrected behaviour without migration. Only what the value *does* changes.
