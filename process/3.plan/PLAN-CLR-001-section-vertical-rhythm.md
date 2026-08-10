# PLAN-CLR-001: Section Separator Vertical Rhythm

## Overview
Re-place the section separator bars of the left and centre columns so each bar
sits close under the section it terminates, and redistribute the surrounding
diagram blocks and controls to make that placement fit between the fixed
anchors. Introduces the two spacing tokens the rhythm is expressed in.

Owner instruction: proceed **column by column**, left first, centre second.

## References
- **Requirements**: RQ-CLR-001, RQ-CLR-002, RQ-CLR-003, RQ-CLR-004, RQ-CLR-005, RQ-CLR-010
- **ADRs**: ADR-CLR-001 (DEC-CLR-001-A..D); depends on ADR-JUC-034, ADR-JUC-014, ADR-JUC-013
- **Design system**: RQ-DSN-020 (4 px spacing scale), RQ-GUI-062 (separator appearance)

## Source-of-truth note
`GeneratedControlTable.inc` is **no longer generated** from the .NET
`MainForm.Designer.cs` (owner confirmation, 2026-08-10). It is an autonomous
hand-maintained source, so control coordinates may be edited directly. Its
frame is canvas coordinates; `BackgroundRenderer.cpp` is offset by
`CANVAS_TOP_CROP` = 27.

---

## Tasks

### TASK-CLR-001: Rhythm tokens, headless test harness, and the left column
- **Tier**: L
- **Status**: Done (2026-08-10)
- **Description**: Add `sectionGapAbove` (16) and `sectionGapBelowMin` (20) to
  `tokens::component`, create `SectionRhythmTests.cpp` asserting RQ-CLR-001..004
  for every section of both columns, and apply the computed displacements to the
  left column (VCO / LAG / TRACK) in both layout files.
- **Requirement refs**: RQ-CLR-001, RQ-CLR-002, RQ-CLR-003, RQ-CLR-004, RQ-CLR-005, RQ-CLR-010
- **ADR refs**: ADR-CLR-001 (DEC-CLR-001-A, DEC-CLR-001-B, DEC-CLR-001-C, DEC-CLR-001-D)
- **Design-system refs**: RQ-DSN-020, ADR-JUC-014 (DEC-JUC-014-C, narrowed), RQ-GUI-062, ADR-JUC-034
- **Displacements** (canvas px):
  - VCO group `0` · `VCO1/VCO2/FM` bar `487 → 465` · LAG group `+13`
  - `LAG` bar `629 → 615` · TRACK group `+21` · `TRACK X` bar unchanged
  - resulting below-gaps: 45 / 46
- **Acceptance Criteria**:
  - **Given** the left column after the change
  - **When** `SectionRhythmTests` runs
  - **Then** each of `VCO1/VCO2/FM`, `LAG`, `TRACK X` has an above-gap equal to
    `tokens::component::sectionGapAbove`
  - **And** each below-gap is `>= tokens::component::sectionGapBelowMin`
  - **And** the two below-gaps differ by at most 1 px
  - **And** the `TRACK X` label baseline is still y = 776
  - **And** the centre column's assertions still pass unchanged (it has not moved yet)
- **Dependencies**: None
- **Assignee**: AI
- **Verification note**: `xpl_tests_app_juce` only builds under `XPL_BUILD_APP=ON`,
  which Linux CI never sets (`linux-headless-release.yml` runs the default OFF);
  the app target is exercised on macOS/AppleClang and Windows/MSVC. Forcing it
  under this container's GCC 13 surfaces two failures that predate this task and
  belong to neither of its files — `-Wdangling-reference` in
  `BoundRadioGroupTests.cpp:154` (a GCC 13 diagnostic) and `-Wmaybe-uninitialized`
  in JUCE's vendored SheenBidi C source. Left untouched: modifying them to green
  a build is exactly what the DoD forbids. `SectionRhythmTests.cpp` itself
  compiles clean under `-Werror`, the 101-test headless suite passes, and the
  rhythm arithmetic was re-checked against the real control table.
- **Defect found and fixed during verification**: measuring a section's bottom
  from control *bounds* put LAG's bar 13 px above its section instead of 16.
  `LAG_TIMING_LINEAR_EXPO` declares 79x47 but `BoundRadioGroup::resized` paints
  its last row at `y + (N-1)*(h/N) + controlRowHeight`, so seven of those px are
  never drawn. RQ-CLR-001 measures *visible* elements, so the test now mirrors
  that rule (`paintedBottom`). The layout was right; the measurement was wrong.

### TASK-CLR-002: Centre column
- **Tier**: M
- **Status**: Not Started
- **Description**: Apply the computed displacements to the centre column
  (VCF / ENV / LFO / RAMP) in both layout files, so the whole canvas satisfies
  the rhythm.
- **Requirement refs**: RQ-CLR-001, RQ-CLR-002, RQ-CLR-003, RQ-CLR-004, RQ-CLR-005
- **ADR refs**: ADR-CLR-001 (DEC-CLR-001-B, DEC-CLR-001-C)
- **Design-system refs**: RQ-DSN-020, RQ-GUI-062, ADR-JUC-034
- **Displacements** (canvas px):
  - VCF group `0` · `VCF/VCA` bar `194 → 182` · ENV group `−1` · `ENV X` bar `416 → 414`
  - LFO group `+8` · `LFO X` bar `597 → 599` · RAMP group `+12` · `RAMP X` bar unchanged
  - resulting below-gaps: 20 / 21 / 20
- **Acceptance Criteria**:
  - **Given** the centre column after the change
  - **When** `SectionRhythmTests` runs
  - **Then** each of `VCF/VCA`, `ENV X`, `LFO X`, `RAMP X` has an above-gap equal
    to `tokens::component::sectionGapAbove`
  - **And** each below-gap is `>= tokens::component::sectionGapBelowMin`
  - **And** the three below-gaps differ by at most 1 px
  - **And** the `RAMP X` label baseline is still y = 776
- **Dependencies**: TASK-CLR-001
- **Assignee**: AI

---

## Out of scope
The right column's button mockup (`CLR-mockup/boutons.jpg`,
`CLR-mockup/boutons2.JPG`) — owner deferred it to a later session. The mockup's
flattened background is an artefact of the owner's image edit; the original
gradient is kept (owner confirmation, 2026-08-10).
