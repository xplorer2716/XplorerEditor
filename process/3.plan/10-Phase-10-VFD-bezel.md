# PLAN-DSP-001: VFD Bezel and Display-Group Alignment

## Overview

Frame the display with a recessed bezel and lift the display group (glass, MIDI
LEDs, shortcut buttons) so the bezel's top edge lines up with the VCF block row.
Implements ADR-JUC-024.

Scope is the display's *surround* and *placement*. The glyph rendering of
ADR-JUC-023 is untouched.

## References

- **Requirements**: RQ-GUI-050 *(new)*, RQ-DSN-098 *(new)*, RQ-GUI-001,
  RQ-GUI-022, RQ-GUI-033, RQ-GUI-037, RQ-DSN-094
- **ADRs**: ADR-JUC-024 *(this plan implements it)*; ADR-JUC-006 (control-table
  deviation policy), ADR-JUC-013 (vector background), ADR-JUC-014 / ADR-JUC-015
  (tokens), ADR-JUC-023 (the renderer this sits around)

Session state: `unit_tests = false`, `platform = linux`, `chat_mode = chat-eco`.
No unit tests are written; verification is the Xvfb/screenshot pipeline against
the owner-approved mockup.

**Approved mockup:** `full_bezelB_aligned.png`, owner-validated 2026-07-31. The
throwaway patch that produced it is `vfd-bezel-B-aligned-mockup.patch` — both in
the session scratchpad, i.e. **not durable**; the numbers that matter are
restated in the tasks below so the plan stands on its own.

**DoR — presented for owner approval before any task starts.**

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-DSP-001 | Bezel token group in `design-tokens.yaml` | M | RQ-DSN-098, DEC-JUC-057/060/061 |
| TASK-DSP-002 | Record the display-group lift in the control table | M | RQ-GUI-050, DEC-JUC-059 |
| TASK-DSP-003 | Draw the bezel in `DisplayPanel` | M | RQ-GUI-050, DEC-JUC-057/058 |
| TASK-DSP-004 | Inset the glyph grid so wrapping matches the glass | S | RQ-GUI-020, DEC-JUC-058 |
| TASK-DSP-005 | Verify against the approved mockup | S | RQ-GUI-050, RQ-GUI-022 |

---

### TASK-DSP-001: Bezel token group in `design-tokens.yaml`
- **Tier**: M
- **Status**: **Done** — 7 scalars in `global`, 9 aliases in `component`,
  generator `--check` idempotent, build clean, suite 106/106.
  - **Lateral margin set to 4, not the mockup's 6.** The mockup used one value
    for both axes and the result crowded the modulation matrix on the right; the
    plan left the number open rather than reconducting a defect it had itself
    flagged.
  - **The band aliases the plate stops instead of restating their hex** —
    `BandTop → plateBot`, `BandBottom → plateTop`. The bezel is the panel's own
    metal, inverted, and aliasing keeps that relationship visible: retune the
    panel material and the bezel follows.
  - **Lengths are logical pixels here, not cell fractions.** The RQ-DSN-097
    glyph tokens had to be cell-relative to survive rescaling; the bezel is
    panel furniture drawn in canvas coordinates, so the same choice would have
    been cargo-culting.
- **Description**: Add the `vfdBezel*` tokens — band gradient stops, corner
  radius, vertical and lateral margins, inner-shadow / top-rim / bottom-rim
  alphas — in the `global` tier with `component` aliases, and regenerate
  `DesignTokens.hpp`. Every note records that the value is a **design choice**,
  not a measurement: there is no reference artwork, the .NET display had no
  bezel.
- **Requirement refs**: RQ-DSN-098
- **ADR refs**: ADR-JUC-024 (DEC-JUC-057, DEC-JUC-060, DEC-JUC-061)
- **Acceptance Criteria**:
  - *Given* the token group, *When* the margins are read, *Then* the vertical
    and lateral ones are distinct entries (mockup values: 6 vertical, lateral to
    be chosen — the mockup's 6 crowded the modulation matrix).
  - *Given* any bezel token, *When* its note is read, *Then* it says the value
    is chosen rather than fitted, so nobody looks for a baseline that never
    existed.
  - *Given* the bezel and the raised-plate tokens, *When* compared, *Then* the
    bezel's relief runs dark-top/light-bottom and RQ-DSN-094's the other way.
  - *Given* the generator, *When* run with `--check`, *Then* it is idempotent.
- **Dependencies**: None
- **Assignee**: AI

### TASK-DSP-002: Record the display-group lift in the control table
- **Tier**: M
- **Status**: **Done** — `_vfdDisplay` y 47→31, `_ledPanelControl` 123→117, the
  eight buttons 128→122, with the rationale in the table header beside the
  TASK-GUI-009 note. Build clean, suite 106/106.
  - **One acceptance criterion of mine was written on a false premise.** It said
    the deviation should be documented in `extract_control_table.py` "alongside
    the existing TASK-GUI-009 one" — but TASK-GUI-009's deviation is *not* in
    that script; it was applied by hand to the `.inc`. Worse, the script is
    **inert**: both its `.NET` inputs left with the archive, so running it would
    fail rather than overwrite anything. Recorded that fact in the script header
    and in the table header instead of inventing a mechanism to satisfy a
    criterion that assumed the wrong thing.
  - **The lift is in the table; the bezel expansion is not.** `MainComponent`
    applies no offset (DEC-JUC-059) but does expand the glass rect by the margin
    *tokens* to get the panel bounds. Baking the expanded size into the table
    would have coupled it to the tokens: retuning a margin would then silently
    desynchronise the coordinates from the drawing.
- **Description**: Move `_vfdDisplay`, `_ledPanelControl` and the eight
  `btPatch*`/`btSettings` entries up in `GeneratedControlTable.inc`, with a
  header note giving the rationale — as the TASK-GUI-009 VCO2 deviation did. The
  mockup's numbers: display content lifted so the **bezel top** lands at canvas
  `y=18` (level with the VCF row, drawn at mockup `y=45` minus the 27 px crop),
  LEDs and buttons lifted 6 px so the gaps below the bezel are unchanged.
- **Requirement refs**: RQ-GUI-050, RQ-GUI-001, RQ-GUI-022
- **ADR refs**: ADR-JUC-024 (DEC-JUC-059), ADR-JUC-006
- **Acceptance Criteria**:
  - *Given* the control table, *When* the display-group entries are read,
    *Then* their lifted coordinates are there, with a header note explaining the
    deviation from the archived reference.
  - *Given* `MainComponent`, *When* the group is placed, *Then* it applies **no**
    offset of its own — the table is what the screen shows.
  - ~~*Given* the extractor script, *When* it is inspected, *Then* the deviation
    is documented alongside the existing TASK-GUI-009 one rather than silently
    overwritten on a future run.~~ **Withdrawn during implementation:** written
    on a false premise. TASK-GUI-009's deviation is not in the script either —
    it was hand-applied to the `.inc` — and the script is inert, both its `.NET`
    inputs having left with the archive. Replaced by: *Given* the extractor
    script, *When* it is read, *Then* it states that it can no longer run and
    that the `.inc` is hand-maintained and authoritative.
- **Dependencies**: None
- **Assignee**: AI

### TASK-DSP-003: Draw the bezel in `DisplayPanel`
- **Tier**: M
- **Status**: **Done** — band, rim hairlines and inner shadow, all from tokens.
  Build clean, suite 106/106. Measured from the running app (canvas coords):
  bezel top edge `(24,24,28)` at y=18, bottom edge `(101,102,109)` at y=111,
  surrounding plate `(68,69,78)` — the top reads darker than the plate and the
  bottom lighter, which is the recess cue of DEC-JUC-057. The top edge sits on
  the VCF block frame, so the alignment RQ-GUI-050 asks for holds.
  - **Left deliberately for TASK-DSP-004:** `maxCharsPerLine()` and
    `lineCount()` still derive from the raw bounds. They return 22×5 — the same
    as the glass-based computation in `paint()` — but **only by coincidence**:
    `275/12` and `267/12` both floor to 22, `94/16` and `82/16` both to 5. A
    larger margin would separate them and text would wrap under the bezel. This
    is the sort of accident that hides a bug until someone retunes a token,
    which is exactly why the plan gave it its own task instead of a line here.
- **Description**: Paint the band, its two rim hairlines and the glass's inner
  shadow, then the glass, then the glyph block — all from tokens, no literal.
  The component's bounds now cover bezel **and** glass.
- **Requirement refs**: RQ-GUI-050, RQ-DSN-098
- **ADR refs**: ADR-JUC-024 (DEC-JUC-057, DEC-JUC-058)
- **Acceptance Criteria**:
  - *Given* the painted panel, *When* its edges are compared, *Then* the top rim
    is darker than the band and the bottom rim lighter.
  - *Given* the renderer source, *When* it is read, *Then* no bezel colour,
    radius, margin or alpha appears as a literal.
  - *Given* any render scale, *When* the panel is painted, *Then* the glass's
    inner edge stays aligned with the bezel — the reason DEC-JUC-058 keeps both
    in one component.
- **Dependencies**: TASK-DSP-001, TASK-DSP-002
- **Assignee**: AI

### TASK-DSP-004: Inset the glyph grid so wrapping matches the glass
- **Tier**: S
- **Status**: **Done** — both accessors now measure `glassBounds()`. No
  computation in the file reads the raw bounds any more. Suite 106/106.
  - **The fix is invisible at today's values and provable at others.** With
    margins 4/6 both the old and new forms give 22×5. Double them and the old
    accessor claims **23×6** for a glass that holds 22×5 — a line wrapping to 23
    characters would put text under the band, and a sixth row would be requested
    that does not exist. That divergence is the verification; the current
    agreement is the trap.
- **Description**: `maxCharsPerLine()` and `lineCount()` derive the grid from
  the **inset** rectangle, not the full bounds. `VfdDisplayHelper` uses
  `maxCharsPerLine()` as its wrap threshold, so leaving it on the outer bounds
  would wrap text to a width the glass does not have — text would be clipped by
  the bezel. This is the one place where DEC-JUC-058's cost bites, hence its own
  task rather than a line buried in TASK-DSP-003.
- **Requirement refs**: RQ-GUI-020, RQ-GUI-033
- **ADR refs**: ADR-JUC-024 (DEC-JUC-058)
- **Acceptance Criteria**:
  - *Given* the panel bounds, *When* the grid is computed, *Then* it comes from
    the bounds minus the margins, and still yields 22×5 at the launch geometry.
  - *Given* a parameter-edit line long enough to wrap, *When* it is shown,
    *Then* it wraps inside the glass with no character under the bezel.
- **Dependencies**: TASK-DSP-003
- **Assignee**: AI

### TASK-DSP-005: Verify against the approved mockup
- **Tier**: S
- **Status**: Not Started
- **Description**: Build, capture the full window under Xvfb at 1:1, and compare
  with the approved mockup. Check the group's gaps and the alignment with the
  VCF row. `unit_tests = false`, so this is the verification.
- **Requirement refs**: RQ-GUI-050, RQ-GUI-022, RQ-GUI-005
- **ADR refs**: ADR-JUC-024
- **Acceptance Criteria**:
  - *Given* the built application, *When* captured at 1:1, *Then* the bezel's top
    edge is level with the top of the VCF block row.
  - *Given* the capture, *When* the display group is measured, *Then* the gaps
    between bezel, LED strip and buttons match the approved mockup.
  - *Given* an enlarged window, *When* captured, *Then* the bezel scales with the
    canvas and stays aligned to the glass.
  - *Given* the build, *When* it completes, *Then* it is warning-clean and the
    existing suite (106) still passes.
- **Dependencies**: TASK-DSP-004
- **Assignee**: AI
