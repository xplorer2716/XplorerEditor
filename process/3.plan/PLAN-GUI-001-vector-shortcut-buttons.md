# PLAN-GUI-001: Vector Shortcut Buttons

## Overview
Replace the eight bitmap shortcut buttons with vector keys and a hand-authored
icon library, resize the row so it spans the VFD exactly, and bring the
modulation matrix column into the section rhythm it had escaped.

## References
- **Requirements**: RQ-GUI-063, RQ-GUI-064, RQ-GUI-065, RQ-GUI-066; carries RQ-GUI-021, RQ-GUI-022
- **ADRs**: ADR-GUI-001 (DEC-GUI-001-A..E); depends on ADR-JUC-014, ADR-JUC-017, ADR-JUC-024, ADR-CLR-001
- **Design system**: RQ-DSN-010 (type/colour scale), RQ-DSN-020 (4 px spacing scale)

## Geometry, as decided

| | Value | Source |
|---|--:|---|
| Key size | 29 px | only integer solution spanning the VFD |
| Gap between keys | 5 px | idem — 8×29 + 7×5 = 267 |
| Row x | 951 → 1218 | VFD's own edges |
| Row y | 132 → 161 | 8 px below the LED strip |
| Gap to matrix labels | 24 px | remainder, falls below (RQ-CLR-001 proximity) |
| Matrix displacement | +27 px | derived from RQ-CLR-001, not chosen |

---

## Tasks

### TASK-GUI-001: Icon library and flat key, behind the existing buttons
- **Tier**: L
- **Status**: Not Started
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
- **Status**: Not Started
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
- **Status**: Not Started
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
