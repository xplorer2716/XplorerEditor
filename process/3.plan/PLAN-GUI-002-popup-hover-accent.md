# PLAN-GUI-002: Combo Box Popup Hover in the Control Accent

## Overview
Give the hovered item of an open combo box's popup list the same control-accent
highlight every other interactive control already uses, closing the one
surface DEC-JUC-021 left on JUCE's stock, barely-visible default.

## References
- **Requirements**: RQ-GUI-068; carries RQ-GUI-041
- **ADRs**: ADR-JUC-017 (DEC-JUC-111, extends — no supersession)
- **Design system**: RQ-DSN-023, RQ-DSN-031, RQ-DSN-061, RQ-DSN-062; ADR-JUC-011, ADR-JUC-014

---

## Tasks

### TASK-GUI-023: Tokenise the popup-list hover colour
- **Tier**: S
- **Status**: Done (2026-08-15)
- **Description**: Add `component.popupHighlightAlpha` and set
  `PopupMenu::highlightedBackgroundColourId`/`highlightedTextColourId` in
  `XplorerLookAndFeel`'s constructor so a hovered popup-list item reads in the
  control accent (`ledColour()`) instead of `LookAndFeel_V4`'s default.
- **Requirement refs**: RQ-GUI-068
- **ADR refs**: ADR-JUC-017 (DEC-JUC-111)
- **Acceptance Criteria** (Gherkin):
  - **Given** an open combo box popup list, **When** the pointer moves over an
    item, **Then** that item's background highlights in `ledColour()` at
    `component.popupHighlightAlpha` and its text uses `textPrimary`
  - **Given** the user changes the knob LED colour in settings, **When** a
    popup is next opened and hovered, **Then** the highlight uses the new
    colour (LookAndFeel rebuild, no cached copy)
  - **Given** the source tree, **When** searched for a raw alpha literal on
    this colour, **Then** none exists outside `DesignTokens.hpp`
- **Dependencies**: None
- **Assignee**: AI

---

## Out of scope
`drawPopupMenuItem`'s layout (icon, tick, submenu arrow, text sizing, the
separator rule) — untouched, still `LookAndFeel_V4`'s own. The combo box's own
idle/hover fill (`drawComboBox`, DEC-JUC-021) and the `SettingsDialog`
automation-table row hover (DEC-JUC-023) — both already accent-driven, not
part of this gap.
