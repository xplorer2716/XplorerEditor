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
- **Status**: Done (2026-08-15) — colour value corrected by TASK-GUI-025
- **Description**: Set `PopupMenu::highlightedBackgroundColourId`/
  `highlightedTextColourId` in `XplorerLookAndFeel`'s constructor so a hovered
  popup-list item reads in the control accent (`ledColour()`) instead of
  `LookAndFeel_V4`'s default. *(As first delivered, the background was
  `ledColour.withAlpha(component.popupHighlightAlpha)` — a translucent tint,
  not the exact colour. See TASK-GUI-025.)*
- **Requirement refs**: RQ-GUI-068
- **ADR refs**: ADR-JUC-017 (DEC-JUC-111)
- **Acceptance Criteria** (Gherkin): superseded by TASK-GUI-025's, below.
- **Dependencies**: None
- **Assignee**: AI

### TASK-GUI-025: Correct the popup-list highlight to the exact accent colour
- **Tier**: S
- **Status**: Done (2026-08-15)
- **Description**: Owner report on the built app — the popup-list highlight
  "n'est pas la même couleur [que Knob LED color], elle est plus foncée" (a
  different, darker colour than the knob-LED swatch). Root cause: TASK-GUI-023
  filled with `ledColour.withAlpha(component.popupHighlightAlpha)` (0.35) to
  guarantee text contrast, which is a *tint* of the accent, not the accent
  itself. Changed to opaque `ledColour` — matching `drawTickBox`/
  `drawRadioBox`'s ticked-fill treatment exactly — and removed the now-unused
  `component.popupHighlightAlpha` token. Verified in the built app, including
  a live re-theme (LED colour changed in Settings → OK → popup reopened):
  highlight tracks the new colour, both before and after this fix.
- **Requirement refs**: RQ-GUI-068 (acceptance criterion corrected)
- **ADR refs**: ADR-JUC-017 (DEC-JUC-111, correction note)
- **Acceptance Criteria** (Gherkin):
  - **Given** an open combo box popup list, **When** the pointer moves over an
    item, **Then** that item's background highlights in the exact, opaque
    `ledColour()` and its text uses `textPrimary`
  - **Given** the user changes the knob LED colour in settings, **When** a
    popup is next opened and hovered, **Then** the highlight uses the new
    colour (LookAndFeel rebuild, no cached copy) — verified live, not just
    read from the code
  - **Given** the source tree, **When** searched for `popupHighlightAlpha`,
    **Then** no occurrence remains (token and its one call site both removed)
- **Dependencies**: TASK-GUI-023
- **Assignee**: AI

---

## Out of scope
`drawPopupMenuItem`'s layout (icon, tick, submenu arrow, text sizing, the
separator rule) — untouched, still `LookAndFeel_V4`'s own. The combo box's own
idle/hover fill (`drawComboBox`, DEC-JUC-021) and the `SettingsDialog`
automation-table row hover (DEC-JUC-023) — both already accent-driven, not
part of this gap.
