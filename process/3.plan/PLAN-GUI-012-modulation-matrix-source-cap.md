# PLAN-GUI-012: Modulation-Matrix 6-Source Cap — UI Enforcement

## Overview
Fix a functional defect reported by the owner: the UI lets a destination
collect more than the hardware's 6-source-per-destination limit. The
controller-layer capability that expresses this rule was already ported
faithfully (RQ-CTL-030) but never called from the UI. See ADR-JUC-036 for
the full comparison against the .NET reference and the mechanism chosen.

## References
- **Requirements**: RQ-GUI-016 (defect closed + amended), RQ-GUI-020
  (amended — the "MAX SRC COUNT REACHED FOR" VFD notice)
- **ADRs**: ADR-JUC-036 (DEC-JUC-122, DEC-JUC-123)

---

## Tasks

### TASK-GUI-040: Enforce the 6-source cap in the UI, with a VFD notice
- **Tier**: M
- **Status**: Done (2026-08-18)
- **Description**:
  1. `ModMatrixComboBox`: add a `showPopup()` override and a
     `setOnAboutToShowPopup(std::function<void()>)` hook (DEC-JUC-123).
  2. `ModMatrixPanel`: add `refreshComboAvailability()` (rebuilds every row's
     source and destination item lists from `getAvailableModulationDestinationsForEntry`/
     `sourceAvailabilityForEntry`, preserving each combo's current selected
     value, `dontSendNotification` throughout) — called at the end of
     `refreshRow` (covers construction via `refreshAll` and external
     single-entry sync) and at the end of `onSourceChanged`/
     `onDestinationChanged` (local edits; DEC-JUC-122). Add
     `setMaxSourceReachedHandler(std::function<void(int)>)`, fired from the
     source combo's `onAboutToShowPopup` hook when
     `sourceAvailabilityForEntry` is false.
  3. `MainComponent`: wire the new handler to
     `_vfd->showModulationEntry(entry, true)` — the VFD content builder
     already produces the exact reference text; only the trigger was
     missing.
- **Requirement refs**: RQ-GUI-016, RQ-GUI-020
- **ADR refs**: ADR-JUC-036 (DEC-JUC-122, DEC-JUC-123)
- **Acceptance Criteria** (Gherkin):
  - **Given** a destination already at 6 active sources, **When** any other
    row's source combo is opened (mouse) or nudged (arrow keys) while its own
    source is `NONE`, **Then** `NONE` is the only value it can show or select
  - **Given** the same state, **When** the source combo's dropdown is opened,
    **Then** the VFD shows `MAX SRC COUNT REACHED FOR` / `<destination>`
  - **Given** a destination already at 6 active sources, **When** any row's
    destination combo is opened or nudged, **Then** that destination is
    absent from the list unless it is already this row's own destination
  - **Given** a row that is itself one of a saturated destination's 6
    sources, **When** its own source combo is opened, **Then** the full
    source list is offered (changing which source it uses does not add a
    7th)
  - **Given** a matrix state where no destination is saturated, **When**
    any combo is opened, **Then** behaviour is unchanged from before this
    task (full lists, normal selection)
- **Dependencies**: None
- **Assignee**: AI

---

## Note on testing
Controller/model-level coverage for `isMaxSourceCountForDestinationReached`/
`sourceAvailabilityForEntry`/`getAvailableModulationDestinationsForEntry`
already exists in part (model layer) and is extended here (controller
layer) to also cover `sourceAvailabilityForEntry` and
`getAvailableModulationDestinationsForEntry` directly, which had no test at
all despite being ported. `ModMatrixPanel`'s own combo-repopulation logic is
new, UI-facing code (Tier M): covered by constructing a real `ModMatrixPanel`
against a controller driven to a saturated destination and asserting on the
resulting `juce::ComboBox` item lists/selections, the same headless-Component
technique already used elsewhere in this test target.

## Out of scope
Hardening `XpanderTone::addModulationSource`'s local-state write when the cap
is hit (ADR-JUC-036, "Alternatives Considered" — byte-identical reference
behaviour, left as-is); any change to `XpanderController.ModulationMatrix.cs`
comparison beyond what motivated this fix.
