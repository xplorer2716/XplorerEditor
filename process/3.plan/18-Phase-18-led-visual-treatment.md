# PLAN-GFX-004: Refined MIDI LED Appearance

## Overview

Replace the flat 5 px squares of the MIDI activity strip by round lamps with a
recessed rim and a radial glow when lit. Implements RQ-GUI-056 and ADR-JUC-031
(DEC-JUC-095/096/097).

Owner request (2026-08-04, session GFX): the LEDs under the VFD are still the
direct .NET port and look unfinished next to the reworked panel. Owner decisions
taken during specification: a **dedicated lamp** treatment rather than a
check-box/radio-derived one, **7 px in a first version** subject to visual review,
and the LEDs must stay **smaller than a check box**.

## References

- **Requirements**: RQ-GUI-056 *(new)*, RQ-GUI-022 *(per-device meaning and
  timing — unchanged)*, RQ-GUI-050 *(display-group alignment — must be
  preserved)*; RQ-DSN-021, RQ-DSN-052, RQ-DSN-061, RQ-DSN-063; RQ-TST-001
- **ADRs**: ADR-JUC-031 *(this plan implements it)*; ADR-JUC-008 *(behaviour half
  unchanged)*, ADR-JUC-014 / ADR-JUC-015 *(tokens and generator)*, ADR-JUC-024
  *(DEC-JUC-059 display group — constrains this work)*

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = chat-eco`.
Branch: `feature/GFX`.

## The size envelope, and why it is the interesting part

Three independent ceilings, all binding on the same value:

| Limit | Value | Source |
|---|---|---|
| Owner allowance | ≤ 10 px | "jusqu'à 2× la taille de l'existant" |
| Smaller than a check box | < 14 px | `min(14, height)` in `drawToggleButton` |
| Fits the 32×8 panel with 3 LEDs | ≤ 8 px | `_ledPanelControl` control-table bounds |

**7 px** clears all three. Note that the *panel*, not the owner's ceiling, is the
binding limit — the allowance of 10 px is not reachable, which is why the panel
bounds were checked before accepting the brief.

## What must NOT change

- **`_ledPanelControl` and every display-group entry in the control table.** They
  are extracted reference geometry and part of the computed assembly of
  ADR-JUC-024 (DEC-JUC-059: VFD 47→30, LEDs 123→116, buttons 128→121). The glow
  gets its room from inflated *painting* bounds instead (DEC-JUC-097).
- **The LED centres.** Derived from the original spec rectangle, so no lamp moves.
- **ADR-JUC-008's behaviour half**: device→LED mapping, the 100 ms retriggerable
  hold, and the 30 ms decay timer that runs only while lit.
- **The colours.** The three ON hues and the OFF fill/border keep their token
  values and their per-device meaning (RQ-GUI-022).

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GFX-006 | Add the three LED tokens; regenerate | M | RQ-GUI-056, DEC-JUC-096 |
| TASK-GFX-007 | Round lamp + rim + glow in `LedPanelComponent::paint`, on inflated bounds | M | RQ-GUI-056, DEC-JUC-095, DEC-JUC-097 |

Sequential: 007 consumes the tokens 006 adds.

---

### TASK-GFX-006: Add the LED tokens and regenerate
- **Tier**: M
- **Status**: Not Started
- **Description**: Add to `juce/tools/design-tokens.yaml` — the single source of
  truth (RQ-DSN-063) — three globals and their component-tier aliases, following
  the existing `disabledAlpha` / `knobTrackAlpha` pattern:
  `indicatorSize` (int, **7**, note recording that it is a first version pending
  owner visual review and naming the three ceilings of DEC-JUC-096),
  `indicatorGlowAlpha` (float) and `indicatorGlowRadius` (float, a multiplier of
  the diameter). Regenerate `DesignTokens.hpp` with
  `python juce/tools/generate_design_tokens.py`, then verify with `--check`
  (ADR-JUC-015 — never hand-edit the header). No consumer yet; this task only
  makes the vocabulary exist.
- **Requirement refs**: RQ-GUI-056, RQ-DSN-021, RQ-DSN-061, RQ-DSN-063
- **ADR refs**: ADR-JUC-031 (DEC-JUC-096), ADR-JUC-014, ADR-JUC-015
- **Acceptance Criteria** (Gherkin):
  - *Given* `design-tokens.yaml`, *When* the generator runs, *Then* the three
    tokens resolve and `--check` exits 0.
  - *Given* `component.indicatorSize`, *When* it is read, *Then* it is 7, is
    strictly less than 14 (the check-box indicator), is at most 10 (twice the
    5 px reference) and at most 8 (the panel height).
  - *Given* the build, *When* it runs, *Then* it compiles with no warning and the
    suite passes with no test modified.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GFX-007: Round lamps with rim and glow, on inflated painting bounds
- **Tier**: M
- **Status**: Not Started
- **Description**: Rework `MainComponent::LedPanelComponent`:
  1. Remove `static constexpr int LED_SIZE = 5` — the last visual literal in this
     component — and read `tokens::component::indicatorSize` instead.
  2. `paint()` draws each LED in three layers (DEC-JUC-095): radial glow in the ON
     hue beneath the body **only while lit**, then the filled circle (`fillEllipse`)
     in the ON hue or `indicatorOffFill`, then the 1 px rim (`drawEllipse`) in
     `indicatorOffBorder`, always.
  3. The component is given bounds inflated by the glow radius around its
     control-table spec, and keeps that spec rectangle as a member so the LED
     positions are computed from it, never from `getLocalBounds()` (DEC-JUC-097).
     Update the placement call site in `MainComponent` accordingly, and state at
     the site that this component's bounds deliberately differ from its spec —
     the only such component in the app.
  4. The horizontal spacing formula (`(w − n·size)/(n+1)`) is kept, now fed the
     token size.
  Behaviour (`flash`, `timerCallback`, hold, mapping) is untouched.
- **Requirement refs**: RQ-GUI-056, RQ-GUI-022, RQ-GUI-050, RQ-DSN-061, RQ-DSN-052
- **ADR refs**: ADR-JUC-031 (DEC-JUC-095, DEC-JUC-097), ADR-JUC-008 *(behaviour
  half unchanged)*, ADR-JUC-024 *(DEC-JUC-059 alignment preserved)*
- **Acceptance Criteria** (Gherkin):
  - *Given* the LED area under the VFD, *When* the UI is displayed, *Then* the
    LEDs are round, rimmed and visually consistent with the rest of the interface.
  - *Given* an idle LED, *When* it is rendered, *Then* it shows the OFF fill and
    the rim, and no glow.
  - *Given* MIDI traffic on one source, *When* its LED lights, *Then* it shows a
    radial glow in that source's existing hue and the other two stay dark.
  - *Given* the painter, *When* it is read, *Then* every size, alpha and radius
    resolves to a token and no `LED_SIZE` literal remains.
  - *Given* the control table, *When* it is compared with the previous revision,
    *Then* `_ledPanelControl`, `_vfdDisplay` and the eight buttons are unchanged.
  - *Given* the LED positions, *When* they are computed, *Then* each centre is the
    one the spec rectangle yields, independent of the inflation.
  - *Given* the build, *When* it runs, *Then* it compiles with no warning and the
    suite passes with no test modified.
- **Dependencies**: TASK-GFX-006
- **Assignee**: AI

---

## Verification

- **Headless geometry guard** (`session.unit_tests = true`): the risk worth
  pinning is not the appearance but the **size envelope** — a later retune of
  `indicatorSize` (which DEC-JUC-096 explicitly invites) silently breaking one of
  its three ceilings. A test asserts, from the tokens and the real control-table
  spec, that the diameter is < 14, ≤ 10, and that three LEDs plus spacing fit the
  panel's 32×8 bounds. That is a guard the current code has no equivalent of, and
  it fails loudly the day someone types 12.
- **Visual verification by the owner**, per RQ-GUI-056's Gherkin: there is no
  pixel baseline for this component, and 7 px is explicitly a first iteration
  awaiting that review.
- **The existing suite stays green**, unmodified.

## Definition of Ready

- [x] Both tasks have descriptions, Gherkin acceptance criteria and tiers.
- [x] Both reference their requirement and ADR IDs.
- [x] Design-system IDs listed (RQ-DSN-021, RQ-DSN-061, RQ-DSN-063; ADR-JUC-014,
      ADR-JUC-015). Every new visual value is a token; the task removes the one
      remaining literal.
- [x] **Owner approval** — treatment, size and first-version framing agreed
      2026-08-04.
