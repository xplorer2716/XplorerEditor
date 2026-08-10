# RQ-CLR — Section Separator Vertical Rhythm

Scope: the vertical placement of the section separator bars (`VCO1/VCO2/FM`,
`LAG`, `TRACK X`, `VCF/VCA`, `ENV X`, `LFO X`, `RAMP X`) relative to the
diagram blocks and controls around them, in the left and centre columns of the
main canvas.

The separator's *appearance* — inline label, lead-in stub, faded rule — is
already specified by RQ-GUI-062 and decided by ADR-JUC-034. This file
specifies only **where it sits vertically**, and how the surrounding blocks are
distributed to make that placement possible.

Origin: owner-supplied mockup `CLR-mockup/xplorer-clear.jpg`, measured against
`CLR-mockup/xplorer-feature-gui.JPG` (the state before this change). Both
screenshots were calibrated to canvas coordinates via the 20 modulation-matrix
source combos, whose positions are fixed and known; residual 0.33 canvas px.

---

## Functional Requirements

### RQ-CLR-001: A separator bar belongs to the section above it

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The **gap between the bottom of the lowest visible element of
  a section and the label baseline of that section's separator bar SHALL equal
  `tokens::component::sectionGapAbove`**, for every section of the left and
  centre columns.
- **Rationale**: the separator is a *terminator* — its label names the section
  that just ended (ADR-JUC-034, Xpander silkscreen convention). Before this
  change the bar sat 17–43 px below its own section but only 9–11 px above the
  next one, so proximity bound it to the wrong group and it read as a heading
  for the section below.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** any section of the left or centre column
  - **When** the canvas geometry is evaluated
  - **Then** `separatorBaselineY - lastVisibleElementBottomY` equals
    `tokens::component::sectionGapAbove` for that section
- **Dependencies**: RQ-GUI-062, RQ-DSN-020, ADR-JUC-034, ADR-CLR-001

### RQ-CLR-002: The next section starts farther away than the section ended

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The **gap between a separator's label baseline and the top of
  the first visible element of the following section SHALL be greater than or
  equal to `tokens::component::sectionGapBelowMin`**, which SHALL itself be
  strictly greater than `tokens::component::sectionGapAbove`.
- **Rationale**: the grouping is only legible if the bar is measurably closer
  to its own section than to the next. A floor expressed as a token, rather
  than a fixed value, lets each column absorb its own residual slack
  (RQ-CLR-004) without breaking the relation the design depends on.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** two consecutive sections of the same column
  - **When** the canvas geometry is evaluated
  - **Then** `nextSectionFirstElementTopY - separatorBaselineY >=
    tokens::component::sectionGapBelowMin`
  - **And** `tokens::component::sectionGapBelowMin >
    tokens::component::sectionGapAbove`
- **Dependencies**: RQ-CLR-001, RQ-DSN-020, ADR-CLR-001

### RQ-CLR-003: Bottom anchors and canvas height are invariant

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The `TRACK X`, `RAMP X` and `MOD MATRIX` separators SHALL keep
  a common label baseline at canvas y = 776, and `LOGICAL_CANVAS_HEIGHT` SHALL
  remain 786.
- **Rationale**: the three columns terminate on one line; that alignment is the
  canvas's visual floor and is what makes the layout read as three columns of
  one instrument rather than three independent stacks. Changing the canvas
  height would also change the window's aspect ratio, which RQ-SCL-001 derives
  from it.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** the rendered canvas
  - **When** the three bottom separators are located
  - **Then** all three share label baseline y = 776
  - **And** `LOGICAL_CANVAS_HEIGHT == 786`
- **Dependencies**: RQ-SCL-001, RQ-GUI-037, ADR-CLR-001

### RQ-CLR-004: Residual slack is distributed evenly within a column

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: WHILE a column's section content heights and its two anchors
  (top content, bottom separator baseline) are fixed, the space left over after
  applying `sectionGapAbove` to every separator SHALL be divided equally among
  that column's below-separator gaps, to within one canvas pixel.
- **Rationale**: the slack differs greatly between columns — the left column has
  ~45 px per gap, the centre only ~20 — because their content heights differ.
  Distributing it evenly is the only rule that produces a repeatable layout;
  the owner mockup let it fall wherever the hand-drag landed (41 vs 26 px in
  the left column), which is not reproducible.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** a column with N below-separator gaps
  - **When** the layout is computed
  - **Then** the largest and smallest of those N gaps differ by at most 1 px
- **Dependencies**: RQ-CLR-001, RQ-CLR-002, RQ-CLR-003

### RQ-CLR-005: Diagram and controls move together

- **Category**: Functional
- **EARS Type**: Unwanted-behavior
- **Statement**: IF a section's vertical position changes, THEN both its
  `BackgroundRenderer` draw calls and its `GeneratedControlTable.inc` entries
  SHALL be displaced by the same amount, so that no control is left detached
  from the diagram element that frames it.
- **Rationale**: the two files hold the same layout in two different coordinate
  frames (`BackgroundRenderer` is offset by `CANVAS_TOP_CROP` = 27). They are
  now independently maintained — `GeneratedControlTable.inc` is no longer
  generated from the .NET `MainForm.Designer.cs` — so nothing but this
  requirement keeps them consistent.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** a section displaced by `d` canvas px
  - **When** its renderer coordinates and its control-table coordinates are compared
  - **Then** both changed by exactly `d`
- **Dependencies**: RQ-CLR-001, ADR-JUC-013, ADR-CLR-001

---

## Non-Functional Requirements

### RQ-CLR-010: The rhythm is verifiable without rendering

- **Category**: Non-Functional
- **NFR Type**: Maintainability
- **EARS Type**: Ubiquitous
- **Statement**: The rhythm specified by RQ-CLR-001..004 SHALL be asserted by
  headless unit tests that read the layout constants directly, so that a later
  edit to any single coordinate fails the build rather than degrading silently.
- **Metric**: every section of the left and centre columns covered by at least
  one assertion on its above-gap and one on its below-gap.
- **Measurement Method**: `ctest` on the `xpl_tests` target; the layout suite
  runs headless (no `juce::Graphics` instantiation required).
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** a coordinate edited so a section's above-gap ceases to equal
    `sectionGapAbove`
  - **When** the test suite runs
  - **Then** it fails, naming the offending section
- **Dependencies**: RQ-CLR-001, RQ-TST, ADR-JUC-003
