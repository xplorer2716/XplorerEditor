# PLAN-GUI-001: Vector Shortcut Buttons

## Overview
Replace the eight bitmap shortcut buttons with vector keys and a hand-authored
icon library, size the row so it spans the modulation grid exactly, and bring the
modulation matrix column into the section rhythm it had escaped.

## References
- **Requirements**: RQ-GUI-063, RQ-GUI-064, RQ-GUI-065, RQ-GUI-066, RQ-GUI-067; carries RQ-GUI-021, RQ-GUI-022
- **ADRs**: ADR-GUI-001 (DEC-GUI-001-A..F); depends on ADR-JUC-014, ADR-JUC-017, ADR-JUC-024, ADR-CLR-001
- **Design system**: RQ-DSN-010 (type/colour scale), RQ-DSN-020 (4 px spacing scale)

## Geometry, as decided

| | Value | Source |
|---|--:|---|
| Key size | 27 px | 8×27 + 7×6 = 258, the modulation grid's span |
| Gap between keys | 6 px | idem |
| Row x | 960 → 1218 | `MOD_SRC` left edge to `MOD_QUANTIZE` right edge — the `MOD MATRIX` separator's own span (RQ-CLR-007) |
| Row y | 132 → 159 | 8 px below the LED strip |
| Gap to matrix labels | 26 px | remainder, falls below (RQ-CLR-001 proximity) |
| Matrix displacement | +27 px | derived from RQ-CLR-001, not chosen |

TASK-GUI-002 first sized the row against the VFD (29 px keys spanning 951→1218);
TASK-GUI-004 re-referenced it to the grid on owner review. The table above is the
delivered state.

---

## Tasks

### TASK-GUI-001: Icon library and flat key, behind the existing buttons
- **Tier**: L
- **Status**: Done (2026-08-10)
- **Description**: Add the button tokens, create `ShortcutIcons` (eight
  `juce::Path` builders) and the flat key painting, and unit-test the icon
  geometry. The buttons are not yet swapped — this task delivers the parts.
- **Requirement refs**: RQ-GUI-063, RQ-GUI-064
- **ADR refs**: ADR-GUI-001 (DEC-GUI-001-A, DEC-GUI-001-B, DEC-GUI-001-C)
- **Design-system refs**: RQ-DSN-010, ADR-JUC-014, ADR-JUC-017
- **Acceptance Criteria**:
  - **Given** each of the eight icon builders
  - **When** its path is built into a 29 px box
  - **Then** the path is non-empty and its bounds sit inside that box
  - **And** no icon's bounds are identical to another's, so no two render alike
- **Dependencies**: None
- **Assignee**: AI

### TASK-GUI-002: Swap the buttons, delete the GIFs
- **Tier**: M
- **Status**: Done (2026-08-10)
- **Description**: Replace the eight `juce::ImageButton`s with vector buttons
  driving the same actions, resize and reposition them per RQ-GUI-065, and
  remove the 24 GIF resources from the assets and the binary data.
- **Requirement refs**: RQ-GUI-063, RQ-GUI-065; preserves RQ-GUI-021
- **ADR refs**: ADR-GUI-001 (DEC-GUI-001-A, DEC-GUI-001-D)
- **Acceptance Criteria**:
  - **Given** the control table, **When** the eight button rects are read,
    **Then** each is 29×29, they are 5 px apart, and the row runs 951→1218
  - **And** the row does not intersect the LED strip's bounds
  - **Given** the built binary, **When** its resources are listed, **Then** no
    button GIF remains
  - **And** every button still triggers the action RQ-GUI-021 assigns it
- **Dependencies**: TASK-GUI-001
- **Assignee**: AI

### TASK-GUI-003: Matrix descent and the missing rhythm assertions
- **Tier**: M
- **Status**: Done (2026-08-10)
- **Description**: Displace the modulation matrix block by +27 px and extend
  `SectionRhythmTests` to cover the matrix column, which it never asserted.
- **Requirement refs**: RQ-GUI-066, RQ-CLR-001, RQ-CLR-003
- **ADR refs**: ADR-GUI-001 (DEC-GUI-001-E); ADR-CLR-001
- **Acceptance Criteria**:
  - **Given** the matrix column, **When** the gap between its lowest element and
    the `MOD MATRIX` label baseline is measured, **Then** it equals
    `tokens::component::sectionGapAbove`
  - **And** the separator's own baseline is still 776 (RQ-CLR-003)
  - **And** `SectionRhythmTests` fails if either ceases to hold
- **Dependencies**: TASK-GUI-002
- **Assignee**: AI

### TASK-GUI-004: Row on the modulation grid, and the accent hover
- **Tier**: S
- **Status**: Done (2026-08-10)
- **Description**: Re-reference the row from the VFD to the modulation grid, and
  add the hover highlight in the control accent.
- **Requirement refs**: RQ-GUI-065 (amended), RQ-GUI-067, RQ-CLR-007
- **ADR refs**: ADR-GUI-001 (DEC-GUI-001-D amended, DEC-GUI-001-F)
- **Origin**: owner review of the TASK-GUI-002 build. Two points: the row should
  span the modulation grid rather than the VFD — "the same calculation as the MOD
  MATRIX separator", which it is — and a hovered key should light in the
  highlight colour. A first pass read that as the MIDI lamp blue; the owner
  clarified it is the interaction accent of the knobs, tick boxes and radios,
  which unlike the lamp colours is user-themeable.
- **Change**: keys 29→27, gaps 5→6, row 951→960 at its left end; both ends now
  equal the separator's, so the right column hangs off one reference instead of
  two. Hover reads the accent from the LookAndFeel at paint time — a token was
  tried first and removed, see DEC-GUI-001-F.
- **Acceptance Criteria**:
  - **Given** the control table, **When** the row's ends are compared with the
    `MOD MATRIX` separator's, **Then** they coincide (960 and 1218)
  - **And** the keys are 27×27 with 6 px gaps
  - **Given** a hovered key other than store, **When** it is painted, **Then**
    its ink is the LookAndFeel's `ledColour()` brightened by `hoverBrighten`
  - **And** given a different accent, **Then** the highlight follows it — the
    test renders with an off-palette magenta so a hard-coded colour cannot pass
- **Dependencies**: TASK-GUI-003
- **Assignee**: AI

---

## Verification note
`xpl_tests_app_juce` does not link under this container's GCC 13 — two
pre-existing `-Werror` failures unrelated to this work (`BoundRadioGroupTests`
and JUCE's vendored SheenBidi). Tests added here are compiled but run in CI on
macOS and Windows. Visual acceptance is by screenshot of the built application
at 1x, which is also RQ-GUI-064's legibility floor.

## Out of scope
The MIDI LED strip keeps its position and treatment (RQ-GUI-022, ADR-JUC-031).
The VFD, its bezel and the display group's other gaps (ADR-JUC-024) are
untouched.
