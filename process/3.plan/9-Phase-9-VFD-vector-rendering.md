# PLAN-VFD-001: VFD Vector 16-Segment Rendering

## Overview

Replace the VFD's inherited 12×16 sprite-sheet blit with 16-segment glyphs drawn
vectorially at the physical pixel scale, so the display stays crisp on HiDPI and
at any window size, covers the whole printable ASCII range, and stops depending
on an asset that can no longer be regenerated in this repository.

Scope is the *rendering* of `DisplayPanel` only. The content pipeline
(`VfdDisplayHelper`), the grid formula, the centering and the caching strategy
are carried over unchanged from ADR-JUC-007 (DEC-JUC-055).

## References

- **Requirements**: RQ-GUI-020, RQ-GUI-031, RQ-GUI-033 *(amended, session VFD)*,
  RQ-GUI-049 *(new)*, RQ-DSN-097 *(new)*, RQ-GUI-005
- **ADRs**: ADR-JUC-023 *(this plan implements it)*, superseding ADR-JUC-007;
  ADR-JUC-014 / ADR-JUC-015 (tokens and their codegen), ADR-JUC-013 (the rest of
  the panel is already vector)

Session state: `unit_tests = true`, `platform = linux`, `chat_mode = normal`.
Tier M and L tasks therefore carry unit tests, and functional verification runs
through the existing Xvfb/screenshot pipeline.

**DoR — this plan is presented for owner approval before any task starts.**

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-VFD-001 | Vendor the 16-segment ASCII table | M | RQ-GUI-033, RQ-GUI-049, DEC-JUC-051 |
| TASK-VFD-002 | Fitting tool deriving the VFD parameters from the baseline sheet | M | RQ-DSN-097, DEC-JUC-054, DEC-JUC-056 |
| TASK-VFD-003 | VFD token group in `design-tokens.yaml` | M | RQ-DSN-097, DEC-JUC-056 |
| TASK-VFD-004 | Vector segment renderer | L | RQ-GUI-033, DEC-JUC-053, DEC-JUC-054 |
| TASK-VFD-005 | Off-model primitives and their override table | M | RQ-GUI-033, DEC-JUC-052 |
| TASK-VFD-006 | Full ASCII 32–126 coverage | M | RQ-GUI-049 |
| TASK-VFD-007 | Wire into `DisplayPanel`, retire the sprite blit | M | RQ-GUI-033, DEC-JUC-055 |
| TASK-VFD-008 | Verification at scale 1.0 / 2.0 / 3.0 | S | RQ-GUI-005, RQ-GUI-033, RQ-GUI-049 |

---

### TASK-VFD-001: Vendor the 16-segment ASCII table
- **Tier**: M
- **Status**: Not Started
- **Description**: Bring `dmadison/LED-Segment-ASCII`'s 16-segment character map
  into the tree as generated data (95 masks, ASCII 32–126, one `uint16_t` each),
  preserving the upstream MIT notice, and pin the bit→geometry mapping with
  tests rather than assuming it.
- **Requirement refs**: RQ-GUI-033, RQ-GUI-049
- **ADR refs**: ADR-JUC-023 (DEC-JUC-051)
- **Acceptance Criteria**:
  - *Given* the generated table, *When* `'0'`, `'1'`, `'8'`, `'A'`, `'E'`, `'H'`,
    `'M'`, `'W'`, `'X'`, `'+'`, `'-'` and `'T'` are decoded, *Then* each lights
    exactly the segment set its shape requires, pinning the outer ring `A`–`H`
    clockwise from top-left and the inner `K,M,N,P,R,S,T,U`.
  - *Given* the table, *When* its length is checked, *Then* it holds 95 entries
    covering ASCII 32–126 with no gap.
  - *Given* the repository, *When* the generated header is read, *Then* the
    upstream MIT copyright notice is present and the dependency is declared as a
    third party.
- **Dependencies**: None
- **Assignee**: AI

### TASK-VFD-002: Fitting tool deriving the VFD parameters from the baseline sheet
- **Tier**: M
- **Status**: Not Started
- **Description**: Add `juce/tools/fit_vfd_tokens.py`, which measures
  `vfd-matrix.png` and fits the geometry and photometry parameters against it,
  so the token values are *derived and reproducible* rather than eyeballed. The
  tool reports the RMSE it reaches, and offers a `--check` mode consistent with
  `generate_design_tokens.py`.
- **Requirement refs**: RQ-DSN-097
- **ADR refs**: ADR-JUC-023 (DEC-JUC-054, DEC-JUC-056)
- **Acceptance Criteria**:
  - *Given* the baseline sheet, *When* the tool runs, *Then* it emits a parameter
    set and its RMSE over the 51 glyphs the sheet actually draws.
  - *Given* the committed token values, *When* the tool runs with `--check`,
    *Then* it exits non-zero if they no longer match what it derives.
  - *Given* the measured unlit level, *When* it is reported, *Then* it is taken
    from the sheet's own blank cell rather than from a free fit parameter.
- **Dependencies**: None
- **Assignee**: AI

### TASK-VFD-003: VFD token group in `design-tokens.yaml`
- **Tier**: M
- **Status**: Not Started
- **Description**: Add the VFD token group — segment geometry, italic shear,
  stroke weight, segment gap, the two glow radii and amplitudes, the unlit level
  and the phosphor hue — in cell-normalised units, each carrying a `note`
  recording its provenance, and regenerate `DesignTokens.hpp`.
- **Requirement refs**: RQ-DSN-097
- **ADR refs**: ADR-JUC-023 (DEC-JUC-056), ADR-JUC-014, ADR-JUC-015
- **Acceptance Criteria**:
  - *Given* the YAML, *When* a VFD geometry token is read, *Then* its value is a
    fraction of the glyph cell and never a pixel count.
  - *Given* the generator, *When* it runs with `--check`, *Then* it is idempotent
    against the committed `DesignTokens.hpp`.
  - *Given* a token fitted against the reference, *When* it is read, *Then* a
    `note` records that provenance.
- **Dependencies**: TASK-VFD-002
- **Assignee**: AI

### TASK-VFD-004: Vector segment renderer
- **Tier**: L
- **Status**: Not Started
- **Description**: Build the 16 segment outlines once as `juce::Path` in a unit
  cell; accumulate a scalar radiance field (lit core + two-component glow over an
  always-drawn unlit bed) and tone-map it once, preserving the phosphor hue below
  1.0 and lifting toward white above it. Rasterise at the physical pixel scale
  read from the graphics context, scaling the glow radii by that same factor.
- **Requirement refs**: RQ-GUI-033, RQ-GUI-005
- **ADR refs**: ADR-JUC-023 (DEC-JUC-053, DEC-JUC-054)
- **Acceptance Criteria**:
  - *Given* a render scale of 1.0 and one of 2.0, *When* a glyph is produced,
    *Then* the second is rasterised at twice the resolution and is not a
    magnification of the first.
  - *Given* the render scale is doubled, *When* the glow is applied, *Then* its
    radius in device pixels doubles with it.
  - *Given* a glyph cell, *When* it is painted, *Then* the unlit segments are
    present at the token's low intensity.
  - *Given* the renderer source, *When* it is read, *Then* it contains no numeric
    visual literal — every value resolves to a token (RQ-DSN-097).
- **Dependencies**: TASK-VFD-001, TASK-VFD-003
- **Assignee**: AI

### TASK-VFD-005: Off-model primitives and their override table
- **Tier**: M
- **Status**: Not Started
- **Description**: A pure 16-segment renderer cannot draw the reference's `:`
  (two separated dots) or `_` (a bar below the glyph body). Add explicit
  primitives for them, selected through one small, data-driven per-character
  override table so the divergence from the vendored table stays auditable in a
  single place. `:` is the priority: it appears in every `NAME:VALUE` and every
  `MIDI CC:` line.
- **Requirement refs**: RQ-GUI-033
- **ADR refs**: ADR-JUC-023 (DEC-JUC-052)
- **Acceptance Criteria**:
  - *Given* `':'`, *When* it is rendered, *Then* it produces two vertically
    separated dots and not a continuous vertical bar.
  - *Given* the override table, *When* it is read, *Then* every character that
    diverges from the vendored table is listed there and nowhere else.
  - *Given* a character with no override, *When* it is rendered, *Then* the
    vendored segment mask is used unchanged.
- **Dependencies**: TASK-VFD-004
- **Assignee**: AI

### TASK-VFD-006: Full ASCII 32–126 coverage
- **Tier**: M
- **Status**: Not Started
- **Description**: Close the inherited gap where 44 of 95 cells — every lowercase
  letter among them — rendered blank, silently dropping characters that reach the
  display from a synthesizer patch name or a `.syx` file.
- **Requirement refs**: RQ-GUI-049
- **ADR refs**: ADR-JUC-023 (DEC-JUC-051)
- **Acceptance Criteria**:
  - *Given* each code point in 32–126, *When* it is rendered, *Then* its cell
    differs from the cell produced for a space — except for the space itself.
  - *Given* a tone name containing lowercase letters, *When* it is shown, *Then*
    every letter is legible.
  - *Given* a code point outside 32–126, *When* it is rendered, *Then* it renders
    as a space.
- **Dependencies**: TASK-VFD-004, TASK-VFD-005
- **Assignee**: AI

### TASK-VFD-007: Wire into `DisplayPanel`, retire the sprite blit
- **Tier**: M
- **Status**: Not Started
- **Description**: Replace `DisplayPanel::paint`'s per-cell `drawImage` with the
  vector renderer, keeping the grid formula, the centering, the `setLines`
  early-out and `setBufferedToImage(true)` exactly as they are (DEC-JUC-055).
  Drop the sprite from the binary-data target; **keep `vfd-matrix.png` in the
  repository** as the fitting reference and regression baseline.
- **Requirement refs**: RQ-GUI-033, RQ-GUI-020
- **ADR refs**: ADR-JUC-023 (DEC-JUC-055)
- **Acceptance Criteria**:
  - *Given* the panel, *When* it is painted, *Then* no sprite sheet is read for
    glyph artwork.
  - *Given* the panel bounds, *When* the grid is computed, *Then* it is still
    `cols = floor(w/12)`, `lines = floor(h/16)` from the logical bounds, and the
    block is still centered — 22×5 at the launch geometry.
  - *Given* text set twice with identical content, *When* `setLines` is called
    the second time, *Then* no repaint is triggered.
  - *Given* the repository, *When* it is inspected, *Then* `vfd-matrix.png` is
    still present and referenced by the fitting tool.
- **Dependencies**: TASK-VFD-004, TASK-VFD-005, TASK-VFD-006
- **Assignee**: AI

### TASK-VFD-008: Verification at scale 1.0 / 2.0 / 3.0
- **Tier**: S
- **Status**: Not Started
- **Description**: Build, run the suite, and verify the rendered panel under Xvfb
  at the three physical pixel scales that occur in practice — launch at 100 % DPI
  (scale exactly 1.0), launch on HiDPI (2.0), enlarged window on HiDPI (3.0) —
  against the baseline sheet.
- **Requirement refs**: RQ-GUI-005, RQ-GUI-033, RQ-GUI-049
- **ADR refs**: ADR-JUC-023
- **Acceptance Criteria**:
  - *Given* the app under Xvfb, *When* it is launched and a parameter is edited,
    *Then* the display shows the same five content lines as before the change.
  - *Given* the window enlarged, *When* the display is captured, *Then* the
    glyphs are crisp at the new size, with no block magnification.
  - *Given* the build, *When* it completes, *Then* it is warning-clean at the
    project's warning level and the test suite is green.
- **Dependencies**: TASK-VFD-007
- **Assignee**: AI
