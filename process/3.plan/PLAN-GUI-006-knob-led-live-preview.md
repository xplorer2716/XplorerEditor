# PLAN-GUI-006: Knob LED Colour Live Preview

## Overview
Give the settings dialog's knob LED colour control the same live-preview
behaviour the eight functional-block colours already have: showing the pick
immediately in the panel behind the dialog, with no restart and no OK click
required, and reverting on any non-accept close.

## References
- **Requirements**: RQ-GUI-073; carries RQ-GUI-046, RQ-SET-003, RQ-DSN-095
- **ADRs**: ADR-JUC-020 (amended — DEC-JUC-113); ADR-JUC-011 (referenced,
  pointer note only, no new decision)

---

## Tasks

### TASK-GUI-031: Live-preview the knob LED colour in place
- **Tier**: M
- **Status**: Done (2026-08-17)
- **Description**: Add `XplorerLookAndFeel::setLedColour(juce::Colour)`,
  mutating `_ledColour` in place and re-applying the one JUCE colour-ID baked
  from it at construction (`PopupMenu::highlightedBackgroundColourId`,
  RQ-GUI-068). Simplify `MainComponent::updateLedColour` from a full
  `LookAndFeel` rebuild to `setLedColour` + `sendLookAndFeelChange()`. Wire
  `SettingsDialog`'s knob-LED `ColourSelector` and "Reset to defaults" to
  preview live through the existing `onLedColourChanged` callback (now fired
  on every edit, not only on accept), and revert it on any non-accept close —
  mirroring the block palette's existing `_originalPalette` snapshot/restore
  shape (DEC-JUC-038) exactly.
- **Requirement refs**: RQ-GUI-073, RQ-GUI-046, RQ-GUI-068, RQ-SET-003,
  RQ-DSN-095
- **ADR refs**: ADR-JUC-020 (DEC-JUC-113), ADR-JUC-011
- **Acceptance Criteria** (Gherkin):
  - **Given** the settings User-interface page, **When** the user picks a new
    colour for the knob LED, **Then** every knob ring, tick box and radio
    button in the panel behind the dialog shows the new colour immediately,
    before OK is clicked
  - **Given** a customised knob LED colour, **When** "Reset to defaults" is
    activated, **Then** the knob LED colour returns to its design-system
    default and the panel updates immediately, alongside the eight block
    colours
  - **Given** a live-previewed knob LED colour, **When** the dialog is closed
    via Cancel, Escape or the title bar, **Then** the panel reverts to the
    colour in effect before the dialog opened
  - **Given** a live-previewed knob LED colour, **When** the user clicks OK,
    **Then** the colour is persisted and remains in effect
  - **Given** `XplorerLookAndFeel::setLedColour`, **When** called, **Then**
    `ledColour()` reflects the new colour AND the PopupMenu highlight colour
    it seeded at construction (RQ-GUI-068) reflects it too — no stale cached
    copy
  - **Given** a LED-only colour change, **When** `setLedColour` is called,
    **Then** the block palette held by the same `LookAndFeel` is left
    untouched
- **Dependencies**: None
- **Assignee**: AI

---

## Out of scope
The eight block-colour controls and their preview wiring (unchanged, already
live per ADR-JUC-020); the `ColourSelector` interaction itself; the dialog's
apply-on-accept/discard-on-cancel contract (kept, not changed); any design
system token (no new or retuned visual literal).
