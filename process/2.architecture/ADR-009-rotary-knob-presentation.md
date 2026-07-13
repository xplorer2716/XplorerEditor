# ADR-009: Rotary Knob Presentation & Interaction — LED-Ring-Only, VFD-Backed Value Entry

## Status
Accepted (owner, 2026-07-13)

## Requirements
RQ-GUI-030, RQ-GUI-031, RQ-GUI-034

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
   path; it paints the knob body, the background ring track, and the coloured
   value arc from start to the current position. Deviation from the reference
   Standard style (which keeps a tick) — a deliberate owner simplification
   (RQ-NFR-009 allows look changes; recorded here per sequencing rule 3).
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

## Not in scope / deferred
- The reference keyboard **preset-value** entry (number keys while hovering a
  knob) is a separate feature, not requested here.
- The knob "Standard vs vintage" style switch (UiConfiguration) is unaffected;
  only the pointer removal and hover brighten apply to the current default.
