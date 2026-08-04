# PLAN-GFX-003: ENV X / RAMP X Radio Row Alignment

## Overview

Fix the vertical alignment of the `SINGLE` / `MULTI` radios restored by
PLAN-TRG-001: they sit 3 px above the check boxes on their own row. Implements
RQ-GUI-053's clarified alignment clause and ADR-JUC-016 DEC-JUC-094.

Owner report (2026-08-04, session GFX): *"maintenant que l'on a restauré les radio
button single/multi pour ENVX et RAMPX, les radio ne sont pas verticalement
alignés avec les checkbox des sous groupes, ils sont légèrement plus haut […] on a
exactement le même problème d'alignement dans ENVX et RAMPX."*

## Diagnosis

`BoundRadioGroup::resized()`'s horizontal branch sizes each option button to
`semantic.controlRowHeight`, which pins it to the **top** of the panel instead of
letting it use the panel's own bounds.

Measured on the drawn indicator (`drawToggleButton` centres a 14 px indicator in
whatever bounds it receives), in canvas coordinates from `GeneratedControlTable.inc`:

| | Check boxes (`EXT.`/`LFO`/`GATED`) | Radio panel | Check-box indicator centre | Radio indicator centre — **today** | — **after fix** |
|---|---|---|---|---|---|
| ENV X | y=316 h=17 | y=313 **h=24** | 324.5 | **321.5** (−3.0) | 325.0 (+0.5) |
| RAMP X | y=722 h=17 | y=719 **h=22** | 730.5 | **727.5** (−3.0) | 730.0 (−0.5) |

Exactly 3 px on both, which is what "exactement le même problème" means: one rule,
two panels — not two coincidences.

**Root cause is a mixing of two geometry sources.** The design system owns shared
metrics; the control table owns extracted reference positions, which RQ-DSN §2
puts explicitly outside the token system. In the horizontal case the panel **is**
the row, its height is extracted geometry drawn deliberately taller than the 17 px
row and straddling it (ENV X: 3 px above, 4 px below), and imposing
`controlRowHeight` on the button throws that centring away. Full reasoning and the
three-use audit of `controlRowHeight` are in ADR-JUC-016, DEC-JUC-094.

## References

- **Requirements**: RQ-GUI-053 *(alignment clause clarified — this is a defect
  against its existing Gherkin, not new scope)*, RQ-GUI-040, RQ-GUI-038,
  RQ-GUI-032; RQ-DSN-021, RQ-DSN-052, RQ-DSN-061; RQ-TST-001
- **ADRs**: ADR-JUC-016 *(this plan implements DEC-JUC-094, which corrects the
  height clause of DEC-JUC-086)*; ADR-JUC-014 (tokens), ADR-JUC-006 (headless core)

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = chat-eco`.
Branch: `feature/GFX`.

## What must NOT change

- **The stacked branch.** `FM_DESTINATION` (82×42) and `LAG_TIMING_LINEAR_EXPO`
  (79×47) are correct and were not reported. There a panel spans several rows, so
  each option must be one `controlRowHeight` at its slot top — TASK-JUC-108 made
  that change for a real defect (the lower radio drifting below its check box).
- **The orientation test** `height < optionCount × controlRowHeight` — DEC-JUC-086,
  unchanged. This is the one place the token legitimately answers a design-system
  question.
- **`drawToggleButton`.** Its indicator centring (`(height − boxSize) × 0.5`) is
  what makes the fix work; it is read, not modified.
- **Widths, orientation, bindings, VFD behaviour, control-table data.**

## Test impact — an assertion that encoded the defect

`BoundRadioGroupTests.cpp` (TASK-TRG-003) asserts, for the side-by-side panels:

```cpp
CHECK(first.getY() == 0);            CHECK(second.getY() == 0);
CHECK(first.getHeight() == rowHeight); CHECK(second.getHeight() == rowHeight);
```

The two height assertions state exactly the behaviour being corrected, so they
**must** change. This is the DoD's permitted case — *"a test may only be changed
if the change correctly reflects new or corrected expected behavior"* — and it is
recorded here rather than done quietly.

The replacement is deliberately **stronger, not weaker**. The current test checks
an implementation detail (a height equals a token); it would have passed with the
radios anywhere vertically, which is why it did not catch this. The new assertion
checks the property the owner actually reported: the radio indicator's vertical
centre matches the sibling check box's, both derived from the real control table.
That is a regression guard the old test was not.

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GFX-005 | Side-by-side radio options fill the panel height; replace the height assertions with a check-box alignment guard | M | RQ-GUI-053, DEC-JUC-094 |

---

### TASK-GFX-005: Side-by-side radio options fill the panel height
- **Tier**: M
- **Status**: **Done** — commit 50cb5f3. `.withHeight(controlRowHeight)` dropped
  from the horizontal branch; both header comments corrected to state which
  geometry source governs each branch. Height assertions replaced by an
  indicator-centre guard read from the real control table (ENV X / RAMP X
  trigger radios vs. their `GATED` check box), proven red first: reverting the
  fix and rebuilding failed exactly 4 assertions at `3.0 <= 1.0` — the predicted
  3 px, both panels — then passed once restored. Full rebuild clean; all 6
  suites green, 0 test modified beyond the justified replacement:
  `xpl_tests_app_juce` 323/323 (+9 from the new guard), `xpl_tests_app`
  2014/2014, `xpl_tests_framework` 123/123, `xpl_tests_model` 333/333,
  `xpl_tests_midi` 68/68, `xpl_tests_settings` 31/31, `xpl_tests_controller`
  99/99.
- **Description**: In `BoundRadioGroup::resized()`, drop `.withHeight(
  tokens::semantic::controlRowHeight)` from the horizontal branch so each option
  takes the full height of its slot (the panel's height); update the comment to
  state which geometry source governs each branch. The vertical branch, the
  orientation test and the width split are untouched.
  In `BoundRadioGroupTests.cpp`, replace the two side-by-side `getHeight() ==
  rowHeight` assertions with: (a) the options fill the panel height, and (b) the
  indicator-centre guard — for `ENV_X_TRIG_SINGLE_MULTI` against
  `ENV_X_TRIG_GATED`, and `RAMP_X_TRIG_SINGLE_MULTI` against
  `RAMP_X_TRIG_GATED`, both read from the control table, the drawn indicator
  centres agree within 1 px, computed with the same rule `drawToggleButton`
  uses (`min(14, height)` indicator centred in the button's bounds). The stacked
  scenario keeps its assertions unchanged.
- **Requirement refs**: RQ-GUI-053, RQ-GUI-040, RQ-GUI-032, RQ-DSN-021, RQ-DSN-052
- **ADR refs**: ADR-JUC-016 (DEC-JUC-094; corrects DEC-JUC-086's height clause),
  ADR-JUC-014
- **Acceptance Criteria** (Gherkin):
  - *Given* the ENV X trigger row, *When* it is rendered, *Then* the `SINGLE` and
    `MULTI` indicators are on the same vertical centre as the `EXT.` / `LFO` /
    `GATED` indicators, within 1 px.
  - *Given* the RAMP X trigger row, *When* it is rendered, *Then* the same holds.
  - *Given* `FM_DESTINATION` and `LAG_TIMING_LINEAR_EXPO`, *When* they are
    rendered, *Then* their radios are unchanged — stacked, one `controlRowHeight`
    each, at their slot tops.
  - *Given* a side-by-side panel, *When* its option bounds are read, *Then* each
    lies inside the panel (the invariant TASK-TRG-003 added, still held).
  - *Given* the horizontal branch, *When* it is read, *Then* it consumes only
    extracted control-table geometry — no design-system row height.
  - *Given* the suite, *When* it runs, *Then* it passes, with the two changed
    assertions justified above and no other test modified.
- **Dependencies**: None
- **Assignee**: AI

---

## Definition of Ready

- [x] Task has a description, Gherkin acceptance criteria and a tier.
- [x] Task references its requirement and ADR IDs.
- [x] Design-system IDs listed (RQ-DSN-021, RQ-DSN-052; ADR-JUC-014). The task
      introduces no visual literal — it removes a token application from a branch
      governed by extracted reference geometry.
- [x] **Owner approval** — granted 2026-08-04 after review of the diagnosis and
      the test-change impact.
- [x] Task Done; full test suite green; ADR-JUC-016's DEC-JUC-094 ready to move
      from Proposed-by-addition to Accepted alongside the ADR's existing status.
