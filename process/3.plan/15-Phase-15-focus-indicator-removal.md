# PLAN-GFX-001: Removal of the Keyboard-Focus Visual Indicator

## Overview

Remove the keyboard-focus ring from the four controls that draw one, and delete
the design-system vocabulary that existed only to support it. Implements
RQ-GUI-054 and ADR-JUC-029 (DEC-JUC-088/089/090/091).

Owner report (2026-08-04, session GFX): *"Nous avons aujourd'hui un mécanisme de
focus sur les contrôles: checkbox, radio, combo. C'est visuellement pas bon et
rentre graphiquement en conflit avec le reste du GUI: je voudrais supprimer cette
notion de focus."* Scope extended to the page-family selector buttons on the
owner's confirmation — they use the same token and the same helper, and leaving
them would keep half a rule alive.

**This is a rendering removal, not an accessibility removal.** No
`setWantsKeyboardFocus` call is touched, tab traversal keeps working, and the
Ctrl+C / Ctrl+V page gesture (RQ-GUI-027) — which is *why* `PageSelectorButton`
requests focus at all — is unaffected. The residual gap (a user tabbing without
acting now has no cue; the VFD's last-action readout of RQ-GUI-020 covers the
acting user only) is accepted knowingly and recorded in ADR-JUC-029.

**No replacement indicator.** Two options were put to the owner — reuse the hover
brighten, or a side/underline marker — and both were declined: the objection is
to the notion of a focus cue on these controls, not only to its current shape.

## References

- **Requirements**: RQ-GUI-054 *(new — the removal)*; RQ-GUI-042 *(withdrawn)*,
  RQ-DSN-033 *(withdrawn)*; RQ-GUI-041 and RQ-GUI-043 *(must survive untouched)*;
  RQ-GUI-045, RQ-GUI-052 *(the block-identity borders the ring was competing
  with)*; RQ-GUI-020, RQ-GUI-027; RQ-DSN-030, RQ-DSN-062, RQ-DSN-063, RQ-DSN-099
- **ADRs**: ADR-JUC-029 *(this plan implements it — DEC-JUC-088, DEC-JUC-089,
  DEC-JUC-090, DEC-JUC-091)*; ADR-JUC-017 *(partially superseded: DEC-JUC-022)*,
  ADR-JUC-028 *(partially superseded: DEC-JUC-083 in full, DEC-JUC-084 in part)*,
  ADR-JUC-014 / ADR-JUC-015 *(token module and its generator)*, ADR-JUC-019
  *(page-family selector painting)*

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = chat-eco`.
Branch: `feature/GFX`.

---

## The four sites, and why the order matters

| # | Site | What it draws today |
|:-:|------|---------------------|
| 1 | `XplorerLookAndFeel::drawTickBox` | `drawRoundedRectangle(box.expanded(1.0F), …, strokeFocusRing)` — ring **outside** the border |
| 2 | `XplorerLookAndFeel::drawRadioBox` | `drawEllipse(box.expanded(1.0F), strokeFocusRing)` — ring **outside** |
| 3 | `XplorerLookAndFeel::drawComboBox` | `focusRingInside(frameBounds, corner, frameWidth)` — ring **inside** |
| 4 | `PageFamilyBlock::PageSelectorButton::paintButton` | `focusRingInside(bounds, corner, strokeBorder)` — ring **inside** |

`focusRingInside()` has two callers (3 and 4) in two different files, and
`strokeFocusRing` has four. Each intermediate state must build, so the sequence is
**consumers first, vocabulary last**: TASK-GFX-001 (sites 1–3) → TASK-GFX-002
(site 4) → TASK-GFX-003 (helper + token). Deleting the token first would break the
build; deleting the helper before site 4 would too.

## What must NOT change

Recorded here because each is a plausible over-reach of a deletion task:

- **`bounds.reduced(frameWidth * 0.5F)`** in `drawComboBox` (line ~277). This is
  DEC-JUC-084's *frame* rule — a stroke is centred on its path — and it is what
  stops RQ-GUI-052's 1.5 px highlight being clipped into square corners. It was
  found while fixing the focus ring; it is not part of it. Reverting it to a
  constant `0.5F` would reintroduce a matrix rendering bug.
- **Hover and disabled** in all four sites: `hoverBrighten`, `disabledAlpha`,
  `disabledMul`, `hovered`. RQ-GUI-041/043 are untouched by this plan.
- **`setWantsKeyboardFocus(true)`** in `PageSelectorButton`'s constructor
  (RQ-GUI-027, DEC-JUC-091).
- **`laf`** in `paintButton` — also used for `blockPalette()`, so it survives the
  removal of the `laf != nullptr` focus branch.
- **`strokeBorder`, `strokeDiagram`, `strokeLine`** and `global.stroke15`: values
  and remaining consumers unchanged. The existing ordering guard in
  `BackgroundRendererTests.cpp` (`strokeBorder < strokeDiagram < strokeLine`, and
  `strokeLine == 2.0F`) must still pass, untouched.

## Verification strategy — why no new unit test

`session.unit_tests = true`, and the Tier M/L rule is that *every generated
function or method* has a unit test. This plan generates none: it deletes four
render branches, one struct, one inline function and one token. The rule is
vacuous here rather than skipped, and the guarantees are mechanical instead:

- **The compiler is the token guard.** After DEC-JUC-089, any surviving reference
  to `tokens::semantic::strokeFocusRing` or `focusRingInside` fails to compile.
  An absence enforced by the build needs no assertion.
- **`generate_design_tokens.py --check` is the sync guard** (ADR-JUC-015): it
  exits 1 if `DesignTokens.hpp` is stale, so the YAML edit and the regeneration
  cannot be half-applied in either direction.
- **A grep is the coverage report**, per RQ-DSN-071's DoD pattern: `strokeFocusRing`,
  `focusRingInside`, `FocusRingGeometry` and `hasKeyboardFocus` must return no
  match in `juce/app/src` and `juce/tools` after TASK-GFX-003.
- **The visual outcome is verified in the running app** against RQ-GUI-054's
  Gherkin. There is no pixel baseline for combo boxes, check boxes or radios —
  ADR-JUC-028 recorded the same limitation for the work being undone here.
- **The existing suite must stay green** at every task, with no test modified
  (DoD). Expected: unchanged pass count.

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GFX-001 | Remove the focus branch from `drawTickBox`, `drawRadioBox`, `drawComboBox` | M | RQ-GUI-054, DEC-JUC-088 |
| TASK-GFX-002 | Remove the focus branch from `PageSelectorButton::paintButton` | M | RQ-GUI-054, DEC-JUC-088, DEC-JUC-091 |
| TASK-GFX-003 | Delete `focusRingInside`/`FocusRingGeometry` and the `strokeFocusRing` token; regenerate | L | RQ-GUI-054, DEC-JUC-089, DEC-JUC-090 |

Strictly sequential — see "why the order matters" above.

---

### TASK-GFX-001: Remove the focus branch from the three `XplorerLookAndFeel` draw overrides
- **Tier**: M
- **Status**: **Done** — commit c9a9226. `XplorerApp` (Debug) built clean; the
  three focus branches and both header comments removed as specified;
  `drawTickBox`/`drawRadioBox`'s now-unused `component` parameter left unnamed.
- **Description**: In `XplorerLookAndFeel.cpp`, delete the three
  `hasKeyboardFocus(true)` blocks — `drawTickBox` (~121–126), `drawRadioBox`
  (~157–161), `drawComboBox` (~291–303, including its explanatory comment) — and
  correct the three header comments that describe the focus state as one of the
  composed states (~96–98, ~132–134, ~201–205). `drawTickBox`'s
  `juce::Component& component` parameter becomes unused: it is an override of
  `LookAndFeel_V4::drawTickBox`, so the signature stays and the parameter is left
  unnamed. `drawRadioBox`'s is unnamed for the same reason of symmetry — removing
  it from a signature we own would touch `drawToggleButton`'s call sites for no
  behavioural gain, which is outside this task's scope. `focusRingInside` and
  `strokeFocusRing` are **not** touched here: `PageFamilyBlock` still uses both,
  and this task must leave a building tree.
- **Requirement refs**: RQ-GUI-054, RQ-GUI-041, RQ-GUI-043, RQ-GUI-052
- **ADR refs**: ADR-JUC-029 (DEC-JUC-088); supersedes ADR-JUC-017 (DEC-JUC-022),
  ADR-JUC-028 (DEC-JUC-083)
- **Acceptance Criteria** (Gherkin):
  - *Given* a focused check box, radio button or combo box, *When* it is
    rendered, *Then* it is indistinguishable from the same control unfocused.
  - *Given* a focused modulation-matrix combo, *When* it is rendered, *Then* its
    block-identity frame (RQ-GUI-052) is the only frame it carries, at
    `strokeBorder` at rest and `strokeDiagram` while cross-referenced.
  - *Given* an enabled control under the pointer, *When* it is rendered, *Then*
    the RQ-GUI-041 hover brighten is unchanged; *Given* a disabled control,
    *Then* the RQ-GUI-043 treatment is unchanged.
  - *Given* `drawComboBox`, *When* its frame rectangle is read, *Then* it is
    still `bounds.reduced(frameWidth * 0.5F)` (DEC-JUC-084, preserved).
  - *Given* the build, *When* it runs, *Then* it compiles with no warning and the
    suite passes with no test modified.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GFX-002: Remove the focus branch from `PageSelectorButton::paintButton`
- **Tier**: M
- **Status**: **Done** — commit 7e2a45a. `XplorerApp` (Debug) rebuilt clean;
  `setWantsKeyboardFocus(true)` and `laf`'s other use (block palette) untouched.
- **Description**: In `PageFamilyBlock.cpp`, delete the
  `isEnabled() && hasKeyboardFocus(true) && laf != nullptr` block (~76–86) and its
  comment. `laf` stays — it also resolves the live block palette (~50–52).
  `setWantsKeyboardFocus(true)` in the constructor (~37) **stays**: it exists so
  Ctrl+C / Ctrl+V reach `keyPressed` for the page copy/paste gesture, and removing
  it would break RQ-GUI-027 (DEC-JUC-091). After this task `focusRingInside()` has
  no caller left, which is what makes TASK-GFX-003 possible.
- **Requirement refs**: RQ-GUI-054, RQ-GUI-045, RQ-GUI-027, RQ-GUI-041
- **ADR refs**: ADR-JUC-029 (DEC-JUC-088, DEC-JUC-091); supersedes ADR-JUC-028
  (DEC-JUC-083, second site)
- **Acceptance Criteria** (Gherkin):
  - *Given* a focused ENV X / LFO X / RAMP X / TRACK X selector button, *When* it
    is rendered, *Then* its block-hue border (RQ-GUI-045) is the only border it
    carries and no ring is drawn inside it.
  - *Given* the active instance of a family, *When* it is rendered, *Then* its
    block-hue fill at `blockFillAlpha` is unchanged, and hover still brightens
    border and fill (RQ-GUI-041).
  - *Given* a selector button, *When* the user presses Ctrl+C then Ctrl+V on
    another instance, *Then* the page is copied and pasted exactly as before
    (RQ-GUI-027) — proving focusability was not removed with the ring.
  - *Given* the build, *When* it runs, *Then* it compiles with no warning and the
    suite passes with no test modified.
- **Dependencies**: TASK-GFX-001
- **Assignee**: AI

---

### TASK-GFX-003: Delete the focus-ring helper and the `strokeFocusRing` token; regenerate
- **Tier**: L
- **Status**: **Done** — commit 56122e7. `focusRingInside`/`FocusRingGeometry`
  removed from `XplorerLookAndFeel.hpp`; `strokeFocusRing` removed from
  `design-tokens.yaml`; `DesignTokens.hpp` regenerated, `--check` clean;
  `BackgroundRendererTests.cpp` comment corrected (assertions untouched). Full
  rebuild clean; all 6 suites green, 0 test modified: `xpl_tests_app_juce`
  314/314, `xpl_tests_app` 2014/2014, `xpl_tests_framework` 123/123,
  `xpl_tests_model` 333/333, `xpl_tests_midi` 68/68, `xpl_tests_settings`
  31/31, `xpl_tests_controller` 99/99.
- **Description**: Remove the now-orphaned design-system vocabulary, in the right
  order and through the generator, never by hand-editing the generated header:
  1. Delete `FocusRingGeometry` and `focusRingInside()` from
     `XplorerLookAndFeel.hpp` (~16–50), with their doc comment.
  2. Delete the `strokeFocusRing` row from `juce/tools/design-tokens.yaml` (~270)
     — the single source of truth (RQ-DSN-063). `global.stroke15` **stays**:
     `strokeDiagram` still aliases it, and DEC-JUC-083 was explicit that the two
     roles coincide by value only.
  3. Amend the `strokeLine` note (~271), which lists "focus rings" among its
     consumers — inaccurate since DEC-JUC-083 and now void.
  4. Regenerate with `python3 juce/tools/generate_design_tokens.py`, then verify
     with `--check` (ADR-JUC-015). `DesignTokens.hpp` is never hand-edited.
  5. Correct the one stale word in `BackgroundRendererTests.cpp` (~47), whose
     comment cites "combo arrow, focus ring or selector outline" as the strokes
     `strokeLine` must not move. The **assertions are not touched** — this is a
     comment correction inside a test whose subject (the stroke-role ordering) is
     unchanged, not a test modified to pass.
  This task removes a public inline function and a public token, and spans the
  app layer, the token source of truth and its generated output — hence Tier L.
- **Requirement refs**: RQ-GUI-054, RQ-DSN-063, RQ-DSN-099, RQ-DSN-030
- **ADR refs**: ADR-JUC-029 (DEC-JUC-089, DEC-JUC-090); ADR-JUC-014, ADR-JUC-015;
  supersedes ADR-JUC-028 (DEC-JUC-083 in full, DEC-JUC-084 in part)
- **Acceptance Criteria** (Gherkin):
  - *Given* `design-tokens.yaml` and `DesignTokens.hpp`, *When* they are searched
    for `strokeFocusRing`, *Then* there is no match in either.
  - *Given* `juce/app/src`, *When* it is searched for `focusRingInside`,
    `FocusRingGeometry` and `hasKeyboardFocus`, *Then* there is no match.
  - *Given* the generator, *When* `--check` is run after regeneration, *Then* it
    exits 0 (header in sync with the YAML).
  - *Given* the design system, *When* `strokeBorder`, `strokeDiagram` and
    `strokeLine` are read, *Then* their values are unchanged and the existing
    ordering assertions in `BackgroundRendererTests.cpp` still hold, unmodified.
  - *Given* the whole build, *When* it runs, *Then* it compiles with no warning
    and the suite passes with no test modified.
- **Dependencies**: TASK-GFX-002
- **Assignee**: AI

---

## Definition of Ready

- [x] Each task has a description, Gherkin acceptance criteria and a tier.
- [x] Each task references its requirement and ADR IDs.
- [x] UI tasks list the design-system IDs they depend on (RQ-DSN-063, RQ-DSN-099,
      RQ-DSN-030; ADR-JUC-014, ADR-JUC-015). No task introduces a visual literal —
      all three only remove tokens and token consumers.
- [x] **Owner approval** — granted 2026-08-04, session GFX.
- [x] All three tasks Done; full test suite green; ADR-JUC-029 ready to move
      from Proposed to Accepted.
