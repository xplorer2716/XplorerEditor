# PLAN-GUI-002: Section Separator — One Rule Interrupted by Its Label

## Overview

Collapse the eight functional-block section headers from two rows (label above a
rule) onto one (a rule interrupted by its label), in the Xpander's silkscreen
idiom, freeing ~11 px per section **without moving anything else**. Implements
RQ-GUI-062 and RQ-DSN-101; architecture ADR-JUC-034 (DEC-JUC-107/108/109/110).

Owner request (2026-08-08, session GUI): *"Il serait nécessaire de donner plus
'd'air' dans l'interface, et ce, sans toucher aux structures des blocs
fonctionnels. Pour cela on pourrait redessiner les libellés de séparation des
blocs [...] afin de gagner de la hauteur."*

The reclaimed height is **not** redistributed by this plan — that is separate
work the owner has begun scoping (middle column: raise the VCF/VCA separator,
lower the RAMP X controls, spread ENV X / LFO X into the result).

## References

- **Requirements**: RQ-GUI-062 *(new — this plan implements it)*, RQ-DSN-101
  *(new)*, RQ-GUI-037, RQ-GUI-044, RQ-GUI-001, RQ-DSN-063, RQ-DSN-092
- **ADRs**: ADR-JUC-034 *(new — this plan implements it)*, ADR-JUC-013
  (mockup-first pipeline), ADR-JUC-014 (token module), ADR-JUC-018

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = normal`.
Branch: `feature/GUI`.

## Working method

Every visual iteration went through the ADR-JUC-013 pipeline — **token →
generator → SVG → owner review → painter** — and the owner reviewed four times.
Two implemented versions were rejected on review; both rejections are recorded
in ADR-JUC-034 and in the task notes below, because each is why the final
implementation reads the way it does.

## What must NOT change

- **The rule's y and its right end** — block geometry and, for the matrix, a
  control column align to them.
- **Every control position and the control table** — this plan touches the
  background painter and the mockup generator only.
- **The fade on the run after the label** (RQ-GUI-037, RQ-DSN-092), unchanged in
  rule and in value.

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GUI-020 | Header token group; label inline with the rule, bottom-aligned | M | RQ-GUI-062, RQ-DSN-101, DEC-JUC-107 |
| TASK-GUI-021 | Rule thickness = measured cap height | M | RQ-GUI-062, DEC-JUC-109 |
| TASK-GUI-022 | Lead-in stub; `MOD MATRIX`; matrix rule stops at the quantize column | M | RQ-GUI-062, DEC-JUC-108, DEC-JUC-110 |

Sequential: each task's owner review gated the next. 021 and 022 both re-enter
`section()` on both sides, so they were kept apart to keep each review's subject
unambiguous.

---

### TASK-GUI-020: Header token group; label inline with the rule, bottom-aligned
- **Tier**: M
- **Status**: **Done** — owner-validated ("visuellement cela a l'air ok").
- **Description**: Add `sectionBarHeight` (4.5) and `sectionLabelGap` (8) to
  `design-tokens.yaml` (`global` + `component` alias) and regenerate
  `DesignTokens.hpp`. Rewrite `section()` in **both**
  `generate_background_mockup.py` and `BackgroundRenderer.cpp` so the label is
  drawn at the section's x with its baseline on the rule's bottom edge, and the
  rule starts after the label (measured width) and keeps its right end. Remove
  `SECTION_TITLE_RISE`.
- **Requirement refs**: RQ-GUI-062, RQ-DSN-101, RQ-GUI-037, RQ-GUI-044
- **ADR refs**: ADR-JUC-034 (DEC-JUC-107), ADR-JUC-013, ADR-JUC-014
- **Acceptance Criteria** (Gherkin):
  - *Given* a section header, *When* rendered, *Then* label and rule share one
    row and the bottom of the letters is at the same y as the bottom of the rule.
  - *Given* the rule, *When* its right end is compared with the previous layout,
    *Then* it is unchanged.
  - *Given* the generator and the painter, *When* read, *Then* neither holds a
    literal for the two values and both resolve them from the same tokens.
- **Notes / findings**:
  - **Centring was implemented first and rejected by the owner**: the rule read
    as crossing the text and the letters hung ~3 px below it. Bottom alignment
    replaced it in the same task (DEC-JUC-107).
  - `sectionBarHeight` changes meaning here — from "rule thickness" to "the
    label's baseline offset" — which is what makes it load-bearing enough to be
    a token rather than a bare constant (RQ-DSN-101).
  - The label now has to be **measured**; the stacked layout never needed its
    width. `Font::getStringWidthFloat` is deprecated in the vendored JUCE and
    `build-win-local` treats warnings as errors, so
    `GlyphArrangement::getStringWidth` is used. **`build-app` does not have
    warnings-as-errors and compiled the deprecated call silently** —
    `build-win-local` is the build that holds.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GUI-021: Rule thickness = measured cap height
- **Tier**: M
- **Status**: **Done** — owner-validated.
- **Description**: Draw the rule as tall as the label's capitals instead of a
  fixed thin strip, keeping its bottom edge (and therefore everything below it)
  where it is. In the painter, add `capHeight(const juce::Font&)` measuring the
  outline of a reference `'H'` via `GlyphArrangement::createPath` + path bounds.
  In the generator, which has no font engine, use the published
  Helvetica/Arial Bold ratio (0.716 em) and note the substitution at the site.
- **Requirement refs**: RQ-GUI-062, RQ-DSN-101
- **ADR refs**: ADR-JUC-034 (DEC-JUC-109)
- **Acceptance Criteria** (Gherkin):
  - *Given* the eight headers, *When* their rule thicknesses are compared,
    *Then* all eight are identical and equal the section typeface's cap height.
  - *Given* the rule, *When* its bottom edge is compared with the previous
    iteration, *Then* it is unchanged and nothing below it moved.
  - *Given* the painter, *When* searched for a stored thickness, *Then* there is
    none.
- **Notes / findings**:
  - **First implementation used `GlyphArrangement::getStringBounds().getHeight()`
    and was wrong**: `PositionedGlyph::getBounds()` is built from
    `font.getHeight()` — the full line box, ascent **plus** descent — so it
    returned ~15 px where the cap height is ~10.7 px. The owner reported it on
    sight ("la barre est bien plus haute que le texte"). Nothing in the API name
    distinguishes the two; the corrected version measures the glyph *outline*.
  - **One fixed reference glyph, not each label's own ink**: measuring the label
    would make `VCF/VCA` (whose `/` overshoots the cap height) taller than `LAG`.
  - Between this task and the next, the working tree was rolled back to the
    thin-rule state while the built executable still carried the tall rule —
    which read as a token-propagation failure and was not one (the 229 tokens
    were verified aligned at that moment). Cap height was never a token: it is a
    run-time measurement. The owner confirmed the restore.
- **Dependencies**: TASK-GUI-020
- **Assignee**: AI

---

### TASK-GUI-022: Lead-in stub; `MOD MATRIX`; matrix rule stops at the quantize column
- **Tier**: M
- **Status**: **Done** — owner-validated ("visuellement c'est OK").
- **Description**: Add `sectionLeadStub` (16) to the tokens and draw a short rule
  **before** the label — flat block hue at full opacity — so the separator reads
  as one rule interrupted by its name rather than as an underline starting after
  it. Only the run after the label keeps the fade. Rename
  `MODULATION MATRIX` → `MOD MATRIX`, and end the matrix rule flush with the
  quantize check-box column (`MATRIX_SECTION_BAR_WIDTH` = 260, from
  `MOD_QUANTIZE_n` x=1206 + width=12 = 1218, less the section's x=958) instead of
  overrunning it. Both sides, tokens shared.
- **Requirement refs**: RQ-GUI-062, RQ-DSN-101
- **ADR refs**: ADR-JUC-034 (DEC-JUC-108, DEC-JUC-110)
- **Acceptance Criteria** (Gherkin):
  - *Given* a section header, *When* rendered, *Then* stub, label and the
    remainder of the rule appear in that order on one row.
  - *Given* the stub, *When* its colour is sampled, *Then* it matches the label
    and the first pixel of the run after it, at full opacity.
  - *Given* the eight sections, *When* their stubs are compared, *Then* all are
    the same length and each starts at its section's x.
  - *Given* the matrix section, *When* its rule's right end is compared with the
    quantize column, *Then* the two are flush (x=1218).
- **Notes / findings**:
  - **One fixed stub length for all sections**, not 1.5 of each label's own
    characters: a per-label stub would leave the eight rules starting at eight
    different x offsets. Owner decision.
  - The stub is a flat fill rather than part of a continuous gradient: over
    16 px of a 370 px header the fade would span 1.00 → 0.97, and a continuous
    gradient would have required per-section user-space gradients in the SVG for
    no visible gain. Owner confirmed independently.
  - The matrix rule's end is a **named constant with its derivation documented at
    both call sites**, not a control-table lookup: the painter transcribes mockup
    geometry and does not query the table (ADR-JUC-013), and the generator
    cannot — a run-time lookup on one side only would break the lock-step.
- **Dependencies**: TASK-GUI-021
- **Assignee**: AI

---

## Verification

- **Existing suite stays green, unmodified**: 127/127 after the final task, 0
  tests modified.
- **Token sync**: `generate_design_tokens.py --check` reports the generated
  header in sync; an independent name-and-value cross-check of all three tiers
  (229 tokens) reports 0 missing, 0 orphan, 0 value divergence.
- **Geometry, mechanically checked on the generated SVG** rather than eyeballed:
  for each of the eight sections — stub start and length, label x, rule right
  end, rule thickness. All eight right ends unchanged (423 / 896 / 897) and the
  matrix at 1218.
- **Diff scope**: each iteration's SVG diff stayed at **one line** (the text
  layer), proving the 90 signal lines and 22 block boxes were untouched.
- **Visual/interactive verification by the owner** at every task — there is no
  pixel baseline for the panel, matching every other visual surface in this
  codebase.

## Definition of Ready

- [x] Each task has a description, Gherkin acceptance criteria and a tier.
- [x] Each task references its requirement and ADR IDs.
- [x] Design-system IDs listed (RQ-DSN-101, ADR-JUC-014) — UI task rule.
- [x] No visual literal introduced: the three geometry values are tokens, the
      rule thickness is measured — DoD applies at Tier M.
- [x] **Owner approval** — granted 2026-08-08, iteratively through the mockup.

## Outcome

Each section header now occupies ~11 px instead of ~22 px — ~11 px freed per
section, eight sections — with the rule's y, its right end, every block boundary
and every control position unchanged. The separator reads as the Xpander's
silkscreen does: one rule running into its section name.

Three geometry values entered the design system (`sectionBarHeight`,
`sectionLabelGap`, `sectionLeadStub`) and are consumed by the painter and the
mockup generator alike, so the prototype and the shipped panel can no longer
diverge on one. The rule's thickness deliberately did **not** become a token: it
is measured from the typeface at paint time, so a font or size change cannot
leave it stale.

*Deliberately not done here:* redistributing the reclaimed height between the
blocks. The owner has started scoping it for the middle column (raise the
VCF/VCA separator, lower the RAMP X controls, spread ENV X / LFO X into the
result); it needs its own requirement, because unlike this plan it moves
controls and therefore deviates from the extracted reference geometry.
