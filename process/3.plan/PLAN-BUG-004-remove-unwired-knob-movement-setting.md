# PLAN-BUG-004: Remove the Unwired Knob-Movement Setting

## Overview

`uiConfig.knobMovementIsLinear` was persisted, defaulted to `true`, asserted in
the settings tests and offered as a "Knob movement" (`Linear`/`Circular`) radio
pair on the User interface page — and read by nothing. `BoundKnob`'s
constructor calls `setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag)`
unconditionally, so the pair changed nothing on screen whatever its state.

This is the exact sibling of the knob-style switch removed by TASK-GUI-018
(ADR-JUC-009, DEC-JUC-106) and it is removed the same way, under DEC-JUC-124.
The owner has confirmed that `RotaryHorizontalVerticalDrag` is the intended
behaviour and that no choice is wanted, so the switch is removed rather than
wired.

`knobStyleIsStandard` needed no work: TASK-GUI-018 already removed it in
2026-08. The only remaining mentions are historical notes in process artifacts
and the deliberately-kept `<KnobStyleIsStandard>` line in the .NET-import test
fixture.

## References
- **Requirements**: RQ-GUI-031 (amended 2026-08-21), RQ-SET-003 (amended
  2026-08-21). Related: RQ-SET-006, RQ-SET-007
- **ADRs**: ADR-JUC-009 (DEC-JUC-124; sibling of DEC-JUC-106)
- **Precedent**: TASK-GUI-018 (`process/3.plan/5-Phase-5-View.md`)

## Baseline (before any change)
Established on this toolchain, 2026-08-21, branch `feature/BUG`:

- `AllUsersSettings.hpp:40` — the field.
- `XmlSettingsService.cpp:203/254` — XML read and write.
- `AllUsersSettingsDefaults.cpp:49` — default `true`.
- `SettingsDialog.cpp:429-430, 474-475, 481, 543-546, 550, 688-695` — group,
  radio pair, `applyTo`, layout, group id, members.
- `SettingsServiceTests.cpp:44` — default assertion; `:208` — the .NET-import
  fixture element.
- Consumers: **none**. `BoundControls.cpp:11` fixes the drag mode.
- Test suite — 122/122 pass.

---

## Tasks

### TASK-BUG-006: Remove the setting, its UI, its persistence and its default
- **Tier**: M
- **Status**: Done (2026-08-21)
- **Description**: Delete `knobMovementIsLinear` and everything that fed it:
  the field, its XML element, its default, the "Knob movement" radio pair, the
  now-empty "Knob behaviour" group, the `setupRadioPair`/`layoutRadioRow`
  helpers left without a caller, and the settings-test assertion on the
  default. Update the user manual's User interface page accordingly.
- **Requirement refs**: RQ-GUI-031, RQ-SET-003
- **ADR refs**: ADR-JUC-009 (DEC-JUC-124)
- **Acceptance Criteria** (Gherkin):
  - **Given** the source tree, **When** it is searched for
    `knobMovementIsLinear`, **Then** there is no match outside the .NET-import
    test fixture and the process artifacts that record the removal
  - **Given** the User interface settings page, **When** it is displayed,
    **Then** it shows the Colours group only — no "Knob behaviour" group
  - **Given** a settings file written after this change, **When** it is read
    back, **Then** it carries no `KnobMovementIsLinear` element
  - **Given** a legacy or .NET-imported settings file that still carries
    `<KnobMovementIsLinear>`, **When** it is loaded, **Then** it loads without
    error and every other value imports unchanged (RQ-SET-006)
  - **Given** a knob, **When** it is dragged, **Then** it behaves exactly as
    before — `RotaryHorizontalVerticalDrag`, unchanged
- **Dependencies**: None
- **Assignee**: AI

---

## Verification
Run on this toolchain, 2026-08-21:

1. `XplorerApp` (`-DXPL_BUILD_APP=ON`) and every headless target build with
   warnings-as-errors on (`-Wall -Wextra -Wpedantic -Werror`) — no new
   diagnostic. The two now-callerless helpers were removed in the same step,
   so no unused-function diagnostic is possible.
2. `ctest` reports **122/122 pass**, 1 skipped as always. No test was modified
   to force a pass: the single edited assertion tested a field that no longer
   exists.

## Note on testing
No test is added. This task removes code and adds no function or method; the
behaviour it leaves behind — the fixed drag mode — is a `juce::Slider` feature,
not this project's logic. The `.NET`-import scenario keeps its
`<KnobMovementIsLinear>` fixture line and now demonstrates the RQ-SET-006
tolerance for it, exactly as it already does for `<KnobStyleIsStandard>`.

Owner verification: open Settings → User interface and confirm the page shows
the Colours group alone; drag a knob and confirm nothing about its feel has
changed.

## Out of scope
- Wiring a linear/circular choice. JUCE implements both natively
  (`Slider::RotaryVerticalDrag`, `Slider::Rotary`), and DEC-JUC-124 records why
  the option is dropped rather than connected.
- The 2012 line in the manual's Release history that mentions "linear knob
  movement" — a historical record of the .NET release, left as-is.
