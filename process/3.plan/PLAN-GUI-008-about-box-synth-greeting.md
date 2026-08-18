# PLAN-GUI-008: About Box Synth Greeting — Missing UI Wiring

## Overview
Fix a defect reported by the owner: opening the About box does not greet the
synth, unlike the .NET reference. Root cause: the controller-layer capability
(`XpanderController::sendGreetingsToSynth()`) was already ported and already
tested, faithfully — nothing in the JUCE UI ever called it.

## References
- **Requirements**: RQ-GUI-076 (new); RQ-CTL-061 (corrected twice — see
  TASK-GUI-035 and TASK-GUI-039 below)
- **ADRs**: None. No new architectural decision: this wires an
  already-decided, already-implemented, already-tested capability to its
  documented reference trigger. Tier S.

---

## Tasks

### TASK-GUI-035: Call sendGreetingsToSynth() when the About box opens
- **Tier**: S
- **Status**: Done (2026-08-18)
- **Description**: Add `_controller->sendGreetingsToSynth();` to
  `MainComponent`'s About menu handler (case 30), right after
  `showAboutDialog(...)` — matching the reference's `AboutForm.OnLoad()`
  timing. No change to `sendGreetingsToSynth()` itself (already correct,
  already covered by `XpanderControllerTests.cpp`).
- **Requirement refs**: RQ-GUI-076, RQ-CTL-061
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** a connected synth output, **When** the About box is opened,
    **Then** the greeting SysEx sequence is sent (already-tested content,
    now reachable from the UI)
  - **Given** the sources, **When** `MainComponent.cpp`'s About case is read,
    **Then** it calls `sendGreetingsToSynth()` alongside `showAboutDialog()`
- **Dependencies**: None
- **Assignee**: AI
  - *Correction (2026-08-18, owner report, session GUI):* this task's own
    claim above — "No change to `sendGreetingsToSynth()` itself (already
    correct...)" — was inaccurate. The wiring was correct; the message
    *content* was not (TASK-GUI-039).

### TASK-GUI-039: Fix the greeting's missing version and unbounded line length
- **Tier**: S
- **Status**: Done (2026-08-18)
- **Description**: Two defects in the greeting's actual text, found once the
  synth greeting was reachable at all (TASK-GUI-035): (a) `MainComponent`
  constructed `XpanderController` with the literal `"XPLORER"` as its
  `productNameAndVersion` — no version number ever reached the synth, despite
  `sendGreetingsToSynth()` faithfully building line 1 from that value. Fixed
  by reusing the same build-derived string the About dialog already shows
  (`productNameAndVersion()`, extracted as a shared helper so the two call
  sites cannot drift). (b) `sendGreetingsToSynth()`'s line-1 construction
  (`padRight`) only ever grows a string, never shrinks it — a name/version
  that does not fit the 40-character line would have silently overrun onto
  line 2. Fixed by truncating at the first `-` (the `-<stream>` suffix —
  canary/preprod/...) only when the full string does not fit; the
  name+version alone is expected to always fit.
- **Requirement refs**: RQ-CTL-061 (amended)
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** the built application, **When** the synth is greeted, **Then**
    line 1 reads "XPLORER" followed by the same version string the About
    dialog shows
  - **Given** a product name/version that fits the 40-character line,
    **When** greeted, **Then** it is sent whole, unchanged
  - **Given** a product name/version that does NOT fit the line but does
    once its `-<stream>` suffix is dropped, **When** greeted, **Then** line 1
    is the truncated, padded string, and line 2 (the GitHub URL) is
    untouched by the overflow that would otherwise have occurred
- **Dependencies**: TASK-GUI-035
- **Assignee**: AI

---

## Note on testing
No new unit test was added for TASK-GUI-035's call site itself: it is one
line of UI glue in a menu-command switch, the same shape as every other case
in that switch (none of which carry an isolated test — they are thin
pass-through calls exercised through the app, not through a JUCE
`ApplicationCommandManager` test harness this codebase does not have).

TASK-GUI-039's truncation branch is new decision logic inside
`sendGreetingsToSynth()` itself, so it IS covered: a new `XpanderControllerTests.cpp`
scenario greets with a product name/version engineered to exceed the
40-character line only before its `-<stream>` suffix is dropped, and asserts
line 1 is truncated/padded correctly and line 2 is untouched. The
`productNameAndVersion()` helper (`MainComponent.cpp`) is UI glue with no
independent behaviour (string concatenation of two build-time values) and is
not separately tested, matching the rest of this file.

## Verification note
The owner explicitly cannot be shown a passing automated check for "the
Xpander's display shows the greeting" from this environment (no synth
hardware attached to CI or to this session) and will verify on real hardware.

## Out of scope
The greeting's text content (unchanged, and see RQ-GUI-076's note on the
"make some noize!" discrepancy — not implemented, needs its own decision);
the About dialog's visual content (RQ-GUI-025); `sendGreetingsToSynth()`'s
internals (already correct).
