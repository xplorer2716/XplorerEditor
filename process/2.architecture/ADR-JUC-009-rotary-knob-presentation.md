# ADR-JUC-009: Rotary Knob Presentation & Interaction — LED-Ring-Only, VFD-Backed Value Entry

## Status
Accepted (owner, 2026-07-13)

**Amended 2026-08-06 (owner report, session GUI)** — DEC-JUC-106 closes the
"Not in scope / deferred" item below: the Standard/vintage knob-style switch,
confirmed unwired to any rendering, is removed rather than left unaffected.

## Requirements
RQ-GUI-030, RQ-GUI-031, RQ-GUI-034, RQ-SET-003

## Context
Owner review of the M2/M3 build flagged the rotary knobs (`BoundKnob` +
`XplorerLookAndFeel::drawRotarySlider`) as visually heavier than the reference:

1. A pointer line is drawn from the knob centre to the coloured ring
   (`drawRotarySlider` "Pointer" path). The reference `KnobControl` only draws
   such a tick in the *Standard* style; the owner wants it gone entirely —
   just the coloured LED ring.
2. The reference brightens the LED ring while the mouse is over the knob
   (`_isMouseEntered ? UIService.GetLightColor(_ledBorderColor) : _ledBorderColor`);
   the port does not.
3. The port added a permanent numeric text box under each knob
   (`setTextBoxStyle(TextBoxBelow, …)`). The reference never had one — the value
   lives on the VFD — so this is redundant clutter and it shrinks the rotary
   (JUCE carves the text row out of the slider bounds).

## Decision
1. **LED-ring-only rendering.** `drawRotarySlider` drops the centre pointer
   path; it paints **no filled knob body** — the interior is left transparent
   so the (non-opaque) `Slider`'s parent, i.e. the vector panel background with
   its shading, shows through the ring — plus the background ring track and the
   coloured value arc from start to the current position. Deviation from the
   reference Standard style (which keeps a tick and a body) — a deliberate
   owner simplification (RQ-NFR-009 allows look changes; recorded here per
   sequencing rule 3). *(Owner review 2026-07: the earlier dark-filled body
   read as an ugly black disc; removed so only the crown remains.)*
2. **Hover-brightened ring.** `drawRotarySlider` receives the `juce::Slider&`;
   when `slider.isMouseOverOrDragging(true)` it strokes the arc with
   `_ledColour.brighter(f)` instead of `_ledColour` — the JUCE equivalent of the
   reference's `_isMouseEntered` light-colour. No per-knob state; the
   LookAndFeel reads the live component state, JUCE repaints on enter/exit.
3. **No text box; value on the VFD + on demand.**
   - `BoundKnob` uses `NoTextBox`, so the rotary fills its full extracted
     bounds (visibly larger, closer to the reference proportions) with no
     control-table change.
   - `setPopupDisplayEnabled(true, true, topLevel)` shows the value as a
     transient bubble **while turning** the knob (RQ-GUI-034).
   - A **double-click** opens a transient inline numeric entry (a temporary
     `Slider` text box shown via `showTextBox()` for the edit, or a small
     `TextEditor` overlay) committing an exact value through the normal
     `onControlEdited` path; it dismisses on Enter/focus-loss. Exact mechanism
     is an implementation detail; observable contract: double-click → type →
     value set like any edit (anti-echo, CC-disable, VFD update all apply).
4. The VFD remains the primary continuous value readout (RQ-GUI-020); the
   bubble/entry are momentary, so the screen stays uncluttered.

## Consequences
- Lighter, larger knobs matching the reference silhouette; one less always-on
  widget per knob (49 knobs).
- Value still discoverable three ways: VFD (continuous), drag bubble
  (momentary), double-click entry (precise) — strictly more than the reference,
  which had only the VFD + keyboard-preset entry.
- `drawRotarySlider` gains a branch on hover state; no new asset, no layout
  change.
- **(Amendment, DEC-JUC-106)** The User interface settings page's "Knob
  behaviour" group now has one row (movement) instead of two; the persisted
  settings schema loses one field, tolerantly (old and .NET-imported files
  keep loading). No user loses a working choice, since the removed choice
  never did anything.

## Not in scope / deferred
- ~~The reference keyboard **preset-value** entry (number keys while hovering a
  knob) is a separate feature, not requested here.~~
  **Closed 2026-08-27 by ADR-JUC-037** (RQ-GUI-079, RQ-GUI-080, owner request,
  session GUI). It was indeed a separate feature and it has now been specified
  and built as one. Two decisions there bear directly on this ADR and are noted
  so a reader of the knobs' presentation finds them: the gesture takes **no
  keyboard focus** (DEC-JUC-125), because decision 2's hover brighten is the
  only state this ADR ever renders and ADR-JUC-029 later removed focus
  rendering entirely; and the value it sets travels the **same path as
  decision 3's double-click entry** (one shared `applyValue()` on
  `NumericEntryKnob`), so the two ways of setting an exact value cannot diverge
  in what they notify. Nothing in decisions 1–4 changes.
- ~~The knob "Standard vs vintage" style switch (UiConfiguration) is unaffected;
  only the pointer removal and hover brighten apply to the current default.~~
  **Closed 2026-08-06 by DEC-JUC-106** — see Decision below.

- **DEC-JUC-124 — Remove the Linear/Circular knob-movement switch too; the
  `RotaryHorizontalVerticalDrag` mouse mode is the sole, unconditional
  behaviour.** *(2026-08-21, owner decision, session BUG.)* The sibling of
  DEC-JUC-106, found the same way and closed the same way. Every consumer of
  `knobMovementIsLinear` was read before touching anything: `UiSettingsPage`'s
  "Knob movement" radio pair (`Linear`/`Circular`) read and wrote it,
  `XmlSettingsService` persisted it, `AllUsersSettingsDefaults` set it to
  `true`, and `SettingsServiceTests` asserted that default — but no knob ever
  read it. `BoundKnob`'s constructor calls
  `setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag)` unconditionally,
  so the pair had **zero observable effect** since the port.

  Wiring it was possible and cheap — JUCE implements both behaviours natively,
  `Slider::Rotary` being the angular mode the reference computed by hand with
  `Math.Atan` and `Slider::RotaryVerticalDrag` its vertical-delta `Linear`
  mode. It was rejected: the owner confirms `RotaryHorizontalVerticalDrag`
  meets the need, and a setting nobody asked for is not worth the live-apply
  machinery it would need (the knobs are built once, so a wired setting would
  otherwise only take effect after a restart, unlike the colours beside it,
  which preview live).

  Removed: the `bool knobMovementIsLinear` field
  (`AllUsersSettings::UiConfiguration`), its XML read/write
  (`XmlSettingsService`, element `KnobMovementIsLinear`), its default
  (`AllUsersSettingsDefaults`), the radio pair and its layout row
  (`UiSettingsPage`), the now-empty "Knob behaviour" `GroupComponent`, and the
  `setupRadioPair`/`layoutRadioRow` helpers that had no other caller left.
  With DEC-JUC-106 this empties `UiConfiguration` of everything but colour.
  **Kept as-is:** the `.NET`-file-import fixture's `<KnobMovementIsLinear>`
  element, for the same reason DEC-JUC-106 kept `<KnobStyleIsStandard>` — it
  demonstrates that a real legacy file still imports cleanly once the element
  is no longer read (RQ-SET-006).

  **Not a divergence to re-litigate:** `RotaryHorizontalVerticalDrag` accepts
  horizontal drag as well as vertical, where the reference's `Linear` used the
  vertical delta alone. That is the behaviour the JUCE port has shipped from
  the start, and it is now the specified one.

- **DEC-JUC-106 — Remove the Standard/vintage (Standard/Flat) knob-style
  switch entirely; the ring-only rendering of decision 1 above is the sole,
  unconditional style.** Re-reading every consumer of `knobStyleIsStandard`
  before touching anything: `UiSettingsPage`'s "Knob style" radio pair
  (`Standard`/`Flat`) read and wrote the field, `XmlSettingsService`
  persisted it, and `SettingsServiceTests` round-tripped it — but no
  `XplorerLookAndFeel` painter, nor any other rendering code, ever read the
  field. It has been a user-facing choice with **zero observable effect**
  since this ADR's decision 1 made the ring-only style unconditional in
  2026-07. Removed: the `bool knobStyleIsStandard` field
  (`AllUsersSettings::UiConfiguration`), its XML read/write
  (`XmlSettingsService`), the radio pair and its layout row
  (`UiSettingsPage`), and the corresponding round-trip assertions
  (`SettingsServiceTests`). **Kept as-is:** the `.NET`-file-import test
  fixture's `<KnobStyleIsStandard>` element (RQ-SET-006) — a real legacy file
  contains it, and the point of that test is that such a file still imports
  cleanly when the element is simply no longer read, which is exactly what
  now happens.
