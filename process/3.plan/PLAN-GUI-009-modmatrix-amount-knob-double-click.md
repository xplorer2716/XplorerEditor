# PLAN-GUI-009: Modulation-Matrix Amount Knob — Double-Click Numeric Entry

## Overview
Fix a defect reported by the owner: double-clicking a modulation-matrix
amount knob does not open the inline numeric entry that every other rotary
knob offers. Root cause: the amount control (`ModMatrixPanel::buildRow`) is
built as a plain `juce::Slider`, while the double-click behaviour lives only
on `BoundKnob`, the parameter-knob wrapper class the matrix row does not use
(matrix edits are dedicated controller operations, not parameter bindings).

## References
- **Requirements**: RQ-GUI-034 (amended — defect closed), RQ-GUI-015
  (defines the amount knob)
- **ADRs**: None. No new architectural decision: this reapplies the shared
  base-class pattern already established for combo boxes
  (`HoverRepaintingComboBox`, ADR-JUC-017 DEC-JUC-040) to knobs. Tier M.

---

## Tasks

### TASK-GUI-036: Extract NumericEntryKnob and wire it to the matrix amount knob
- **Tier**: M
- **Status**: Done (2026-08-18)
- **Description**: Extract `BoundKnob`'s double-click → inline numeric entry
  behaviour (`mouseDoubleClick`/`applyTextEntry`/`dismissTextEntry`) into a
  new `NumericEntryKnob : juce::Slider` base class. `BoundKnob` inherits it
  instead of duplicating it. `ModMatrixPanel::Row::amount` changes from
  `juce::Slider` to `NumericEntryKnob`, closing the gap.
- **Requirement refs**: RQ-GUI-034, RQ-GUI-015
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** a `NumericEntryKnob` with a parent component, **When** it is
    double-clicked, **Then** an inline `TextEditor` opens over its bounds,
    pre-filled with its current integer value
  - **Given** the inline editor is open, **When** Return is pressed with a
    typed value, **Then** the knob's value is set to it (`sendNotificationSync`)
    and the editor closes
  - **Given** the inline editor is open, **When** Escape is pressed, **Then**
    the editor closes and the knob's value is unchanged
  - **Given** a modulation-matrix amount knob, **When** it is double-clicked,
    **Then** it behaves identically to a parameter `BoundKnob` (same class,
    one implementation)
- **Dependencies**: None
- **Assignee**: AI

### TASK-GUI-043: Unify the drag-time value bubble the same way as the double-click entry
- **Tier**: M
- **Status**: Done (2026-08-26)
- **Description**: TASK-GUI-036 unified the double-click → inline numeric
  entry behaviour into `NumericEntryKnob`, but `setPopupDisplayEnabled(true,
  true, nullptr)` — the transient value bubble shown while a knob is dragged
  (RQ-GUI-034's other clause) — was left called only from `BoundKnob`'s own
  constructor (`BoundControls.cpp`), so the modulation-matrix amount knob
  (`ModMatrixPanel::buildRow`, built directly as a `NumericEntryKnob`) never
  got it — the identical gap shape TASK-GUI-036 closed for the double-click,
  reappearing for the bubble because the fix was applied to one consumer
  instead of the shared base. Fixed by moving the call into
  `NumericEntryKnob`'s own constructor (delegating the no-arg overload to the
  `(SliderStyle, TextEntryBoxPosition)` one, so it runs exactly once) and
  removing the now-redundant call from `BoundKnob`.
- **Requirement refs**: RQ-GUI-034, RQ-GUI-015
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** a modulation-matrix amount knob, **When** the user drags it,
    **Then** a transient popup bubble shows its current value, identical to
    a parameter `BoundKnob`
  - **Given** `NumericEntryKnob`'s two constructors, **When** either is used,
    **Then** the popup bubble is enabled — one implementation, not one per
    consumer
- **Dependencies**: TASK-GUI-036
- **Assignee**: AI

---

## Note on testing
`NumericEntryKnob` is new code (Tier M): unit tests cover its double-click
open, Return-commit and Escape-dismiss paths directly against the class,
since both `BoundKnob` and the matrix row now get identical behaviour from
it — testing the base class once is testing both call sites.

## Out of scope
`BoundKnob`'s registry wiring (`onDragStart`/`onDragEnd`/`onValueChange` →
`ParameterBindingRegistry`) and `ModMatrixPanel::onAmountChanged`'s
controller call — both unchanged, already correct, already reachable
through the same `setValue()` path the new class already used.
