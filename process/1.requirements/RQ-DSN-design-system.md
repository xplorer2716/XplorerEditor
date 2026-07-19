# RQ-DSN — JUCE UI Design System

Scope: cross-cutting presentation layer for the JUCE reimplementation (`juce/app/src/`) —
colour, typography, geometry and interaction-state tokens shared by every visual
component (background, knobs, toggles, combos, dialogs, indicators). This is a
**presentation-only** layer: it defines *how things look and react*, never *what
they do* (functional behaviour stays owned by RQ-GUI/RQ-CTL).

**Status**: Draft — awaiting owner validation before any ADR or implementation task starts.

Rationale (observed 2026-07, current `claude/xplorer-editor-juce-wl25q7` build):
visual constants are defined independently, file by file, with no shared source of
truth, which has already produced literal duplication and inconsistent per-component
conventions:
- `juce::Colour::fromRGB(30, 36, 44)` (panel/dropdown background) is hard-coded
  identically in four places: `XplorerLookAndFeel.cpp:8`, `XplorerLookAndFeel.cpp:10`,
  `ModMatrixPanel.cpp:22`, `SettingsDialog.cpp:44/599`.
- `juce::Colour::fromRGB(24, 28, 34)` (dialog/tick-box dark) is hard-coded
  independently in `XplorerLookAndFeel.cpp:48`, `SettingsDialog.cpp:674`,
  `ProgressWindow.cpp:32`.
- `BackgroundRenderer.cpp` (ADR-JUC-013) already centralises *its own* palette
  and a `FS_*` type scale as local `constexpr` — good practice, but scoped to
  that one file: `XplorerLookAndFeel.cpp:75` independently hard-codes a `12.0F`
  caption size that happens to match `BackgroundRenderer.cpp`'s `FS_CAPTION`,
  by coincidence, not by shared reference.
- Interaction-state handling exists only for the rotary knob (hover-brighten,
  `XplorerLookAndFeel.cpp:40`, `_ledColour.brighter(0.4F)`, ADR-JUC-009); toggle
  buttons, combo boxes and dialog list rows have no documented, reusable
  hover/pressed/disabled convention — each was decided ad hoc while building
  that specific screen.
- This is exactly the failure mode ADR-JUC-011 fixed once for the LED colour
  (single runtime source of truth, `XplorerLookAndFeel::ledColour()`); RQ-DSN
  generalises that pattern to every visual token instead of re-solving it
  component by component as new screens are built.

## Stakeholders
- **Owner**: Xplorer project owner.
- **Consumers**: every JUCE view component (`XplorerLookAndFeel`, `MainComponent`,
  `BackgroundRenderer`, `ModMatrixPanel`, `SettingsDialog`, `ProgressWindow`,
  `DisplayPanel`, `PageFamilyBlock`, `Dialogs`, `PianoWindow`) and every future
  ADR-JUC-* / TASK-JUC-* that touches visual presentation.

---

## Design tokens — colour palette

- **RQ-DSN-001** — The application shall define a single, headless (non-`Component`,
  non-JUCE-graphics-context-dependent) colour-token module in the JUCE app source
  tree, providing every colour value currently duplicated or scattered across
  `XplorerLookAndFeel.cpp`, `ModMatrixPanel.cpp`, `SettingsDialog.cpp`,
  `ProgressWindow.cpp`, `BackgroundRenderer.cpp` and `MainComponent.cpp` as a
  named constant, so that changing a colour is a one-place edit (extends the
  single-source-of-truth pattern already accepted for the LED colour,
  ADR-JUC-011).
- **RQ-DSN-002** — The application shall not contain a raw `juce::Colour::fromRGB`/
  `juce::Colour(0x...)` literal outside the colour-token module; every call site
  shall reference a named token instead (closes the duplication documented above
  as a Definition-of-Done gate for future Tier M/L visual tasks, per the
  existing "no duplicated literals" rule).
- **RQ-DSN-003** — The colour-token module shall expose colours by **semantic
  role** (e.g. panel surface, elevated surface / dropdown background, border,
  primary text, disabled text, the three MIDI-activity LED colours of
  RQ-GUI-022), not by raw RGB triplet name, so a component picks the role it
  means and the mapped RGB value can change without touching call sites.
- **RQ-DSN-004** — WHILE the user has customised the knob LED/border colour via
  the settings (RQ-SET-003), the design-system palette shall treat that value as
  a user-overridable token like any other — the token module supplies the
  *default*, `XplorerLookAndFeel::ledColour()` remains the single runtime
  accessor for the *effective* (possibly user-overridden) value (no behaviour
  change to ADR-JUC-011).

## Design tokens — typography

- **RQ-DSN-010** — The application shall define a single named type scale
  (module-level constants, e.g. section title / block label / caption / small
  label sizes) shared by every component that draws text, replacing the
  per-file `FS_*` constants currently local to `BackgroundRenderer.cpp` and the
  independent literal sizes in `XplorerLookAndFeel.cpp` and dialog code.
- **RQ-DSN-011** — WHEN a component's rendered text must fit a tight,
  measured bound (e.g. the combo-box shrink-to-fit rule of RQ-GUI-032, or a
  toggle-button caption), the computed size shall be clamped between the
  type-scale's defined minimum and maximum for that text role, so ad hoc
  legibility floors (currently a local `9.0F` in `XplorerLookAndFeel.cpp`) stay
  consistent with the same floor used elsewhere for the same role.
- **RQ-DSN-012** — The design-system typography module shall not alter the
  computed, per-instance sizing behaviour already specified by RQ-GUI-032
  (combo-box full-label fit) — it supplies the shared floor/ceiling and default
  sizes only; the fitting algorithm itself is unchanged.

## Design tokens — geometry & interaction states

- **RQ-DSN-020** — The application shall define shared geometry tokens for
  corner radius and stroke width used across controls and panels (currently
  independent literals: `2.0F`/`2.4F` in `XplorerLookAndFeel.cpp`,
  `2.0F` `CORNER` / `LINE_WIDTH` in `BackgroundRenderer.cpp`), so the same
  visual "roundedness" and line weight read as one coherent system across the
  vector background and the JUCE-native controls drawn over it.
- **RQ-DSN-021** — Every interactive control (rotary knob, toggle button, combo
  box, dialog list row) shall define an explicit, documented state set — idle,
  hovered, pressed/dragging, disabled, focused — instead of a per-component ad
  hoc subset (today only the rotary knob implements a hover state,
  ADR-JUC-009; toggle buttons, combo boxes and `SettingsDialog` list rows have
  none).
- **RQ-DSN-022** — WHEN a control's state changes to hovered, the design system
  shall apply the same shared brighten/darken factor used everywhere a
  "hover" affordance is shown (currently a local `brighter(0.4F)` literal in
  `XplorerLookAndFeel.cpp`), so hover feedback reads identically across knobs,
  toggles and combo/list rows once each adopts a hover state under RQ-DSN-021.
- **RQ-DSN-023** — WHEN a control is disabled, the design system shall apply a
  single shared desaturation/opacity rule across all control types, so a
  disabled knob, toggle or combo box is visually recognisable as disabled by
  the same convention everywhere (no such shared rule exists today).
- **RQ-DSN-024** — Transient/timed visual feedback (the knob value popup
  bubble of RQ-GUI-034, the ≈100 ms MIDI-activity LED hold of RQ-GUI-022 /
  ADR-JUC-008, the modulation-matrix hover highlight of RQ-GUI-018 /
  ADR-JUC-010) shall draw its timing constants (durations, fade curves) from
  the shared token module rather than a local literal per component, so a
  future timing change (e.g. "make all hold/fade timings 20% longer") is a
  one-place edit.

## Governance & drift prevention

- **RQ-DSN-030** — Any future ADR-JUC-* that decides a visual presentation
  detail (colour, size, corner radius, state behaviour) shall reference the
  RQ-DSN token(s) it uses or introduces, instead of recording a new standalone
  literal — new visual decisions extend the token module rather than
  bypassing it (mirrors how ADR-JUC-011 is already referenced by RQ-GUI-018).
- **RQ-DSN-031** — WHEN a design-system token's value changes, no call site
  shall require an accompanying literal edit — verifying this (e.g. a grep for
  raw colour/size literals outside the token module returning zero results)
  shall be part of the Definition-of-Done for any Tier M/L task that touches
  visual presentation code, alongside the existing "no duplicated literals"
  rule.
- **RQ-DSN-032** — The `ADR-JUC-013-mockup-generator.py` SVG prototyping tool
  (and any future design mockup script) shall keep its palette and type-scale
  values in sync with the token module's values — manually mirrored and
  reviewed together on any token change is acceptable, but a mockup and the
  shipped app shall never silently diverge on a shared token's value (the risk
  ADR-JUC-013 already flags for its geometry, extended here to tokens).

---

## Non-Functional Requirements

- **RQ-DSN-050** — The colour/typography/geometry token module shall be
  headless — plain constants with no dependency on a live `juce::Graphics`
  context or `Component` — consistent with the existing core/app split
  (`xpl_app_core` pattern) and testable under `session.unit_tests = true`
  without a display.
- **RQ-DSN-051** — Adopting the design system for an existing component shall
  be a pure refactor: it shall not change that component's functional
  behaviour or any RQ-GUI/RQ-CTL requirement it fulfils — only the literal
  values' origin (inline vs. token reference) changes.
- **RQ-DSN-052** — The design-system requirements (this file) define the token
  *structure* and *governance rules* only; the specific palette/type-scale
  *values* (owner-approved colours, exact pixel sizes) and the migration of
  existing components onto the token module are out of scope here and shall be
  recorded in a follow-up ADR-JUC-* and implementation plan, per the mandated
  requirements → ADR → plan → implementation sequence.

---

## Traceability

| RQ-DSN | Depends on / relates to |
|---|---|
| 001–004 | RQ-GUI-022, RQ-GUI-031, RQ-SET-003, ADR-JUC-011 |
| 010–012 | RQ-GUI-032, RQ-GUI-033, ADR-JUC-013 |
| 020–024 | RQ-GUI-018, RQ-GUI-022, RQ-GUI-031, RQ-GUI-034, ADR-JUC-008, ADR-JUC-009, ADR-JUC-010, ADR-JUC-011 |
| 030–032 | ADR-JUC-011, ADR-JUC-013 |
| 050–052 | RQ-NFR-007, RQ-TST (process) |
