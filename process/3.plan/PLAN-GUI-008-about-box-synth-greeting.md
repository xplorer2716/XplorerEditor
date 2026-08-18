# PLAN-GUI-008: About Box Synth Greeting — Missing UI Wiring

## Overview
Fix a defect reported by the owner: opening the About box does not greet the
synth, unlike the .NET reference. Root cause: the controller-layer capability
(`XpanderController::sendGreetingsToSynth()`) was already ported and already
tested, faithfully — nothing in the JUCE UI ever called it.

## References
- **Requirements**: RQ-GUI-076 (new); RQ-CTL-061 (corrected — was "at
  startup", the reference trigger is the About box)
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

---

## Note on testing
No new unit test was added for the call site itself: it is one line of UI
glue in a menu-command switch, the same shape as every other case in that
switch (none of which carry an isolated test — they are thin pass-through
calls exercised through the app, not through a JUCE `ApplicationCommandManager`
test harness this codebase does not have). `sendGreetingsToSynth()`'s actual
behaviour — message content, padding, off/on sequence, Xpander vs Matrix-12 —
was already covered before this task and is unchanged.

## Verification note
The owner explicitly cannot be shown a passing automated check for "the
Xpander's display shows the greeting" from this environment (no synth
hardware attached to CI or to this session) and will verify on real hardware.

## Out of scope
The greeting's text content (unchanged, and see RQ-GUI-076's note on the
"make some noize!" discrepancy — not implemented, needs its own decision);
the About dialog's visual content (RQ-GUI-025); `sendGreetingsToSynth()`'s
internals (already correct).
