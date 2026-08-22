# PLAN-BUG-002: No Synchronization at Startup or After a MIDI Settings Change

## Overview
Fix a defect reported by the owner: the editor never asks the synthesizer which
patch it is on, except when the user explicitly triggers Goto, Store or
Patch > Synchronize. At launch the editor therefore shows its own default tone
while the synth sits on whatever patch it was left on, and changing the MIDI
ports mid-session leaves the freshly opened port equally unasked.

The reference .NET implementation synchronized on both events. The port carried
over `applyMidiSettings` — device assignment, channel, delay, automation table —
but not the patch request that followed it.

No new mechanism is needed:
`sendProgramChangeAndGetSinglePatchFromSynth(currentProgramNumber())` already
exists and is exactly what the Patch > Synchronize menu item calls
(DEC-JUC-101). This plan adds that one call at the two sites that were missing
it.

## References
- **Requirements**: RQ-BUG-002. Related: RQ-CTL-006, RQ-CTL-021, RQ-GUI-008,
  RQ-GUI-025
- **ADRs**: None. The architecture is unchanged and the decision was already
  taken by ADR-JUC-032 (DEC-JUC-101), which established this call as *the*
  synchronization operation. Adding two more callers to it settles nothing new.

## Baseline (before any change)
Established on this toolchain, 2026-08-21, branch `feature/BUG`:

- `juce/app/src/MainComponent.cpp` — the constructor ends at
  `applyMidiSettings(...)`, with no patch request after it.
- `juce/app/src/SettingsDialog.cpp` — `SettingsContent::accept()` calls
  `applyMidiSettings(...)`, then commits the colour snapshots and closes. No
  patch request.
- `juce/app/src/MainComponent.cpp` — the Patch > Synchronize handler already
  makes the call this plan reuses, unchanged.

---

## Tasks

### TASK-BUG-003: Synchronize at startup and when settings are accepted
- **Tier**: M
- **Status**: Done (2026-08-21)
- **Description**: Call
  `sendProgramChangeAndGetSinglePatchFromSynth(currentProgramNumber())` at the
  end of the `MainComponent` constructor, immediately after
  `applyMidiSettings`, and in `SettingsContent::accept()` immediately after its
  own `applyMidiSettings`. Cancel keeps its existing path and stays silent.
- **Requirement refs**: RQ-BUG-002
- **ADR refs**: None (see References)
- **Acceptance Criteria** (Gherkin):
  - **Given** the application, **When** it starts, **Then** the synchronization
    call is made once, after the MIDI settings are applied
  - **Given** the Settings dialog, **When** the user accepts it, **Then** the
    same call is made, after the new settings reach the controller
  - **Given** the Settings dialog, **When** the user cancels it, **Then** no
    such call is made
  - **Given** the three trigger sites, **When** they are read, **Then** each
    makes the identical call, none with a variant of its own
- **Dependencies**: None
- **Assignee**: Human

---

## Verification
Run on this toolchain, 2026-08-21:

1. `XplorerApp` (`-DXPL_BUILD_APP=ON`) and every headless target build with
   warnings-as-errors on (`-Wall -Wextra -Wpedantic -Werror`) — no new
   diagnostic.
2. `ctest` still reports **117/117 pass**, 1 skipped (virtual MIDI cable
   absent). No pre-existing test modified.

## Note on testing
No unit test is added, and none is claimed. This task introduces no new
function or method — it adds two call statements, both inside
`juce/app/src/`, which is compiled only into the GUI target (`XPL_BUILD_APP`,
default `OFF`) and is not covered by any test target. The operation those
statements invoke is already exercised at controller level by
`XpanderControllerTests` (`[RQ-CTL-006]`, `[RQ-CTL-021]`); what is new here is
*when* it is called, which is observable only by running the application
against a synthesizer.

Owner verification, on hardware:
- Leave the synth on a patch other than the editor's default, start Xplorer,
  and confirm the editor shows that patch.
- With the editor running, change the synth output port in Settings, accept,
  and confirm the editor re-reads the patch over the new port.
- Open Settings, change nothing, cancel, and confirm no MIDI traffic follows.

## Out of scope
- The `Patch > Synchronize` handler, `Goto` and `Store` — already correct, they
  are the callers this plan copies.
- The controller's `start()` / `stop()` lifecycle, which the application also
  never invokes. Not required for this fix and not touched by it; see the note
  raised with the owner.
