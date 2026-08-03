# PLAN-DGM-001: Diagram Stroke Weight, Fill Contrast and Paint Order

## Overview

Make the eight colour-coded functional blocks legible at a glance: one stroke
weight for the whole background diagram, a stronger block fill, and blocks
painted over the lines that run into them instead of under their end-caps.
Implements ADR-JUC-027.

Scope is the vector background (`BackgroundRenderer`) and its SVG prototype.
Control widgets are explicitly untouched — the stroke-role split (DEC-JUC-074)
exists precisely so they stay as they are.

## References

- **Requirements**: RQ-GUI-051 *(new)*, RQ-DSN-099 *(new)*, RQ-GUI-037,
  RQ-GUI-044, RQ-DSN-092, RQ-DSN-093, RQ-DSN-094, RQ-DSN-095
- **ADRs**: ADR-JUC-027 *(this plan implements it)*; ADR-JUC-013 (the vector
  background and its mockup pipeline), ADR-JUC-014 / ADR-JUC-015 (token
  structure and codegen), ADR-JUC-018 (block identity colours)

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = chat-eco`.

**Mockup-first, per ADR-JUC-013:** every value was reviewed in
`juce/tools/background-mockup.svg` before `BackgroundRenderer.cpp` was touched.
Two intermediates were rejected on review and are recorded in ADR-JUC-027
Context: `blockFillAlpha = 0.24`, and a 1.5px block frame against 2.0px lines.

**DoR — owner approved the ADR + plan + execute sequence, 2026-08-03.**

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GUI-010 | Split the stroke role and raise the block fill in `design-tokens.yaml` | M | RQ-DSN-099, RQ-GUI-051, DEC-JUC-074/066 |
| TASK-GUI-011 | Layer the SVG mockup generator and align its stroke widths | M | RQ-GUI-051, DEC-JUC-076 |
| TASK-GUI-012 | Layer `BackgroundRenderer` and retire the border re-stroke pass | M | RQ-GUI-051, DEC-JUC-074/064 |
| TASK-GUI-013 | Regression tests: token ordering and paint order | M | RQ-DSN-099, RQ-GUI-051 |

---

### TASK-GUI-010: Split the stroke role and raise the block fill in `design-tokens.yaml`
- **Tier**: M
- **Status**: **Done** — `DesignTokens.hpp` regenerated, `--check` idempotent.
  Two intermediates were reviewed in the mockup and rejected before landing
  here: `blockFillAlpha = 0.24` (owner: too close to the 0.18 original) and a
  block-only 1.5px frame against unchanged 2.0px lines (see TASK-GUI-011).
- **Description**: Add `global.stroke15` and the `semantic.strokeDiagram` role
  that aliases it; rewrite `semantic.strokeLine`'s note to state its remaining
  widget-only consumers; raise `global.blockFillAlpha` 0.18 → 0.30 with a note
  recording that 0.18 was measured and 0.30 chosen. Regenerate
  `DesignTokens.hpp`.
- **Requirement refs**: RQ-DSN-099, RQ-GUI-051
- **ADR refs**: ADR-JUC-027 (DEC-JUC-074, DEC-JUC-077)
- **Acceptance Criteria**:
  - *Given* the token file, *When* the generator runs, *Then* `strokeDiagram`
    and `strokeLine` both resolve and `--check` is idempotent.
  - *Given* `strokeLine`, *When* its value is read, *Then* it is unchanged at
    2.0 — no control widget moves as a result of this task.
  - *Given* `blockFillAlpha`'s note, *When* it is read, *Then* it distinguishes
    the measured origin from the chosen current value.
- **Dependencies**: None
- **Assignee**: AI

### TASK-GUI-011: Layer the SVG mockup generator and align its stroke widths
- **Tier**: M
- **Status**: **Done** — verified structurally: the regenerated SVG's diagram
  group parses as one lines-row, one boxes-row, one text-row (90/22/92
  elements), and the only stroke widths outside 1.5 are the randomised
  0.5–1.6 wood-grain hairlines, which are texture, not diagram.
- **Description**: Point every diagram stroke in
  `generate_background_mockup.py` at `strokeDiagram` (one `LW`, no per-element
  width), and re-order the emitted diagram into lines → boxes → text with a
  per-element layer marker so the geometry call sites stay untouched. Assert at
  generation time that no layer is empty.
- **Requirement refs**: RQ-GUI-051, RQ-DSN-099
- **ADR refs**: ADR-JUC-027 (DEC-JUC-076)
- **Acceptance Criteria**:
  - *Given* the regenerated SVG, *When* its diagram strokes are collected,
    *Then* they are all one width (the wood-grain hairlines, which are
    randomised texture, excepted).
  - *Given* the regenerated SVG, *When* its diagram is read in document order,
    *Then* every line element precedes every box element, which precedes every
    text element.
  - *Given* the generator, *When* it runs, *Then* the output parses as XML and
    contains no layer marker.
- **Dependencies**: TASK-GUI-010
- **Assignee**: AI

### TASK-GUI-012: Layer `BackgroundRenderer` and retire the border re-stroke pass
- **Tier**: M
- **Status**: **Done** — `XplorerApp` builds warning-clean on the touched file.
  **Correction against the original premise:** the shipped painter did not
  actually show the bled-line defect — its re-stroke pass already covered every
  case TASK-GUI-013's test samples, confirmed by running that test against the
  pre-change file (git-restored, then reverted back). Only the SVG generator
  had the visible symptom. This task is therefore parity with ADR-JUC-013 (the
  generator must predict the painter) and removal of a per-symptom patch ahead
  of the fill-alpha increase making a future gap visible — not a fix for an
  observed defect in the product. ADR-JUC-027 Context/Consequences were
  corrected to say this rather than imply a shipped bug.
- **Description**: Repoint `LINE_WIDTH` at `semantic::strokeDiagram`; append
  every primitive to a line / box / text layer and replay the layers in that
  order; move the rotated `NOISE` label's transform inside its deferred closure;
  delete the `colouredBorders` final pass and its struct, whose purpose the
  layering now serves by construction.
- **Requirement refs**: RQ-GUI-051, RQ-GUI-037, RQ-GUI-044
- **ADR refs**: ADR-JUC-027 (DEC-JUC-074, DEC-JUC-075)
- **Acceptance Criteria**:
  - *Given* the painter, *When* it is read, *Then* no stroke width resolves to
    `strokeLine` and the `colouredBorders` pass is gone.
  - *Given* a rendered panel, *When* a block edge touched by a knob stub is
    examined, *Then* no part of the stub is visible over the block.
  - *Given* the rotated `NOISE` label, *When* the panel is painted, *Then* it is
    still rotated.
  - *Given* the build, *When* it compiles, *Then* it is warning-clean on the
    touched file.
- **Dependencies**: TASK-GUI-010
- **Assignee**: AI

### TASK-GUI-013: Regression tests — token ordering and paint order
- **Tier**: M
- **Status**: **Done** — `xpl_tests_app_juce` gains `BackgroundRendererTests.cpp`
  (`BackgroundRenderer.cpp`/`BlockPalette.cpp` now compiled into that target);
  full suite 108/108 (1 pre-existing hardware-dependent skip, unrelated). The
  paint-order test's discriminative power was checked, not assumed: inverting
  the layer replay order made it fail with `distanceTo(neutralLine) == 0.0`
  (exact match on the neutral colour) before the fix was re-applied.
- **Description**: Add Catch2 coverage in the JUCE-enabled app suite for the two
  things that can silently regress: the stroke-role ordering
  (`strokeBorder` < `strokeDiagram` < `strokeLine`) that RQ-DSN-099 makes the
  actual design decision, and the paint order, by rendering
  `paintVectorBackground` into an image and sampling a block interior a stub
  runs into. Compile `BackgroundRenderer.cpp` into `xpl_tests_app_juce`.
- **Requirement refs**: RQ-DSN-099, RQ-GUI-051
- **ADR refs**: ADR-JUC-027 (DEC-JUC-074, DEC-JUC-075)
- **Acceptance Criteria**:
  - *Given* the token header, *When* the ordering test runs, *Then* it asserts
    `strokeBorder` < `strokeDiagram` < `strokeLine` and fails if any future edit
    inverts them.
  - *Given* a rendered background, *When* a pixel just inside a block edge that a
    stub ends on is sampled, *Then* it carries the block's tint and not the
    neutral line colour.
  - *Given* the suite, *When* it runs, *Then* every pre-existing test still
    passes unmodified.
- **Dependencies**: TASK-GUI-012
- **Assignee**: AI
