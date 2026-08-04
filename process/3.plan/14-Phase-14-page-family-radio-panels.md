# PLAN-TRG-001: ENV X / RAMP X `SINGLE` / `MULTI` Trigger Radio Panels

## Overview

Restore the two `RadioButtonPanel` controls the JUCE port drops entirely — the
`SINGLE`/`MULTI` trigger mode of the ENV X and RAMP X blocks — reported as
[issue #31](https://github.com/xplorer2716/XplorerEditor/issues/31). Implements
RQ-GUI-053 and the ADR-JUC-016 extension (DEC-JUC-085/086/087).

Scope is three code sites and their tests. **No new widget, no new design token,
no new metadata field, no geometry change**: the radio widget, its skin and the
control-table coordinates all already exist — they were simply never reached on
the page-family path.

## References

- **Requirements**: RQ-GUI-053 *(new)*, RQ-GUI-038 (the shared radio widget),
  RQ-GUI-040, RQ-GUI-010, RQ-GUI-011, RQ-GUI-006, RQ-GUI-020, RQ-GUI-032,
  RQ-GUI-041, RQ-GUI-042, RQ-GUI-043; RQ-DSN-021, RQ-DSN-030, RQ-DSN-031,
  RQ-DSN-061; RQ-TST-001
- **ADRs**: ADR-JUC-016 *(extended by this plan — DEC-JUC-085, DEC-JUC-086,
  DEC-JUC-087)*; ADR-JUC-006 (headless core boundary), ADR-JUC-014 (tokens),
  ADR-JUC-017 (interaction states)

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = normal`.

**Root cause — three independent defects, each sufficient on its own:**

| # | Site | Defect |
|:-:|------|--------|
| 1 | `PageFamilyBlock::makeControl` | no `RadioButtonPanel` case → `default: return nullptr`, control silently dropped |
| 2 | `ControlMetadata::radioPanels()` | lists only the two fixed-block panels → options empty even once (1) is fixed |
| 3 | `BoundRadioGroup::resized()` | stacks unconditionally → wrong for the wide-and-short ENV (129×24) / RAMP (126×22) panels |

**Reference values (`MainForm.Designer.cs` / `MainForm.resx`):** `rdSINGLE` and
`rdRampXSingle` carry `Tag = "0"` and `.Text = SINGLE`; `rdMULTI` and
`rdRampXMulti` carry `Tag = "1"` and `.Text = MULTI`.

**DoR — owner approved the diagnosis, the geometric orientation rule and the
full artefact + execution sequence, 2026-08-04.**

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-TRG-001 | Add the two panels to `radioPanels()`; pin the table↔metadata invariant | M | RQ-GUI-053, DEC-JUC-087 |
| TASK-TRG-002 | `RadioButtonPanel` case in `PageFamilyBlock::makeControl` | M | RQ-GUI-053, DEC-JUC-085 |
| TASK-TRG-003 | Geometry-derived orientation in `BoundRadioGroup::resized()` + layout test — ***option height corrected 2026-08-04 by PLAN-GFX-003 / TASK-GFX-005: the side-by-side branch kept `controlRowHeight`, putting the radios 3 px above their row (DEC-JUC-094)*** | M | RQ-GUI-053, DEC-JUC-086 |

Sequential: 002 is inert without 001 (empty options), and 003 only becomes
observable once 002 places the controls.

---

### TASK-TRG-001: Add the two panels to `radioPanels()`; pin the table↔metadata invariant
- **Tier**: M
- **Status**: **Done** — the guard was proven to fail before it passes: with the
  two rows temporarily removed the suite reported `radio panel without options:
  RAMP_X_TRIG_SINGLE_MULTI` and 4 failed assertions; restored, 30/30 test cases.
  A test written after the fix that was never seen red is not a regression guard.
- **Description**: Add `ENV_X_TRIG_SINGLE_MULTI` and `RAMP_X_TRIG_SINGLE_MULTI`
  to `ControlMetadata.cpp`'s `radioPanels()` with the reference child-radio
  values (`SINGLE` = 0, `MULTI` = 1), and correct the table's comment, which
  claims it holds the panels "of the fixed blocks". Add the headless regression
  guard of DEC-JUC-087 to `ControlTableTests.cpp`: every control-table spec of
  kind `RadioButtonPanel` resolves to a non-empty option set — the test that
  would have caught defect 2, and that fails today.
- **Requirement refs**: RQ-GUI-053, RQ-GUI-032
- **ADR refs**: ADR-JUC-016 (DEC-JUC-087), ADR-JUC-006
- **Acceptance Criteria**:
  - *Given* `radioPanelOptions("ENV_X_TRIG_SINGLE_MULTI")`, *When* called,
    *Then* it returns exactly `{{"SINGLE", 0}, {"MULTI", 1}}` — the reference
    `Tag` integers, in reference order.
  - *Given* `radioPanelOptions("RAMP_X_TRIG_SINGLE_MULTI")`, *When* called,
    *Then* it returns the same pair set.
  - *Given* every `RadioButtonPanel` spec in the control table, *When* its id is
    passed to `radioPanelOptions()`, *Then* the result is non-empty — and the
    table holds exactly the four expected panels.
  - *Given* the two pre-existing panels, *When* queried, *Then* their options
    are unchanged.
  - *Given* `xpl_tests_app`, *When* it is built, *Then* it still links no JUCE
    module.
- **Dependencies**: None
- **Assignee**: AI

### TASK-TRG-002: `RadioButtonPanel` case in `PageFamilyBlock::makeControl`
- **Tier**: M
- **Status**: **Done** — verified in the running app, driven rather than
  eyeballed. Clicking `MULTI` in the ENV X trigger row deselected `SINGLE` and
  put `ENV1 TRIG:MULTI` on the VFD (transmit + RQ-GUI-020 label in one gesture).
  Switching ENV 1 → ENV 3 showed `SINGLE` (ENV 3's own value, not ENV 1's), and
  switching back showed `MULTI` again — the rebinding round trip, with **no
  page-family code beyond the one `switch` arm**, as DEC-JUC-085 predicted.
- **Description**: Add the `ControlKind::RadioButtonPanel` case to
  `PageFamilyBlock`'s `makeControl`, constructing the same `BoundRadioGroup` the
  fixed-block path uses, with options keyed on the shared `_X_` tag (as the
  `ComboBoxValuedControl` case already does for `comboLabelsForControl`) and
  bound to the concrete instance parameter. No other page-family change: the
  group is a `BoundControl`, so `_controls`, `rebindControlsToActiveInstance`
  and the registry handle instance switching unmodified.
- **Requirement refs**: RQ-GUI-053, RQ-GUI-010, RQ-GUI-011, RQ-GUI-032
- **ADR refs**: ADR-JUC-016 (DEC-JUC-085)
- **Acceptance Criteria**:
  - *Given* the ENV X and RAMP X blocks, *When* the window is built, *Then* a
    two-option radio group exists at the control-table bounds of each panel.
  - *Given* ENV 3 is the active instance, *When* `MULTI` is clicked, *Then*
    `ENV_3_TRIG_SINGLE_MULTI` = 1 is transmitted and the VFD shows `MULTI`.
  - *Given* the user switches ENV 3 → ENV 4, *When* the block rebinds, *Then*
    the radios display ENV 4's stored value and the registry holds the ENV 4
    binding.
  - *Given* an incoming synth/automation change, *When* it sets the parameter to
    0, *Then* the `SINGLE` radio becomes selected without echoing back.
  - *Given* the page-family source, *When* read, *Then* the new case adds no
    radio-specific branch anywhere outside `makeControl`.
- **Dependencies**: TASK-TRG-001
- **Assignee**: AI

### TASK-TRG-003: Geometry-derived orientation in `BoundRadioGroup::resized()` + layout test
- **Tier**: M
- **Status**: **Done** — suite 113/113 (110 at session start), build
  warning-clean. Both ENV X and RAMP X show `SINGLE`/`MULTI` side by side on the
  `GATED` check box's row in the running app.
  - **Found while wiring the test:** compiling `BoundControls.cpp` into the
    warnings-as-errors test target surfaced a pre-existing C4458 in
    `BoundComboBox`'s constructor — the structured binding `label` hides
    `juce::ComboBox::label`. Renamed to `itemLabel`; a rename, no behaviour
    change, and the only edit made outside this plan's three sites. The warning
    had been invisible because the app target does not build with `xpl::warnings`.
- **Description**: Lay the options out horizontally when the panel is too short
  to stack them (`height < optionCount × semantic::controlRowHeight`),
  vertically otherwise (DEC-JUC-086); button height stays `controlRowHeight` in
  both orientations so radios keep landing on their sibling check boxes' row.
  Compile `BoundControls.cpp` into the existing `xpl_tests_app_juce` target and
  pin **both** orientations at the real control-table bounds.
- **Requirement refs**: RQ-GUI-053, RQ-GUI-040
- **ADR refs**: ADR-JUC-016 (DEC-JUC-086), ADR-JUC-014
- **Acceptance Criteria**:
  - *Given* a two-option group sized 129×24 (ENV) or 126×22 (RAMP), *When*
    resized, *Then* the buttons are side by side — same `y`, disjoint `x`
    ranges, each `controlRowHeight` high, together spanning the full width.
  - *Given* a two-option group sized 82×42 (FM) or 79×47 (LAG), *When* resized,
    *Then* the layout is byte-identical to today's — stacked, full width, one
    slot per option.
  - *Given* the widget source, *When* read, *Then* no raw geometric literal was
    introduced and the orientation is decided from bounds alone, with no control
    id or metadata field consulted.
  - *Given* the full suite, *When* it runs, *Then* every pre-existing test passes
    unmodified.
- **Dependencies**: TASK-TRG-002
- **Assignee**: AI
