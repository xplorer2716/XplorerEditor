# PLAN-GUI-003: Randomizer Page Checkbox Column Alignment

## Overview
Align the Randomizer page's VCO2-random and Matrix-random checkbox rows into
one shared three-column grid, and shorten the "Sources & destinations" label
that made the widest column costlier than it needed to be.

## References
- **Requirements**: RQ-GUI-069; carries RQ-GUI-061
- **ADRs**: ADR-JUC-033 (referenced, not amended — no new decision)
- **Design system**: RQ-DSN-020 (dialog component sizes, content-driven tier)

---

## Tasks

### TASK-GUI-024: Shared column widths for the two checkbox rows
- **Tier**: S
- **Status**: Done (2026-08-15)
- **Description**: Add `dialogRandomizerCol1/2/3Width` tokens and use them for
  both the VCO2-random and Matrix-random rows in
  `RandomizerSettingsPage::resized()`; rename "Sources & destinations" to
  "source & dest.".
- **Requirement refs**: RQ-GUI-069
- **ADR refs**: ADR-JUC-033 (referenced)
- **Acceptance Criteria** (Gherkin):
  - **Given** the Randomizer page, **When** displayed, **Then** FM/Amount,
    Noise/Quantize and Sync/"source & dest." each share a left edge
  - **Given** the Matrix-random row, **When** read, **Then** its third
    checkbox reads "source & dest."
  - **Given** the source tree, **When** the two rows' checkbox bounds are
    read, **Then** both use the same three named tokens, no raw literal
- **Dependencies**: None
- **Assignee**: AI

---

## Out of scope
Checkbox/label font size (RQ-GUI-061, unchanged), what the checkboxes control
(`RandomizerSettingsPage::applyTo`/`setFromConfig`, unchanged), every other
settings page.
