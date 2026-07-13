# ADR-011: Single Runtime Source of Truth for the Knob LED Colour

## Status
Proposed (awaiting owner validation before implementation)

## Requirements
RQ-GUI-018, RQ-GUI-031, RQ-SET-003

## Context
The knob LED-border colour (UiConfiguration `knobLedBorderColor`) is a
user-editable setting. At runtime it currently exists as **three copies**:

- `XplorerLookAndFeel::_ledColour` — used to paint knob rings and tick boxes;
- `ModMatrixPanel::_highlightColour` — used for the matrix hover highlight;
- read twice from settings in `MainComponent`'s constructor (once for the
  LookAndFeel, once passed to the matrix panel).

When the user changes the colour, `MainComponent::updateLedColour` rebuilds the
LookAndFeel but does **not** refresh the matrix panel's cached copy, so the
highlight keeps the old colour — the bug the owner reported. This is a
duplicated-state defect: the same value is stored in more than one place and
the update path only refreshes one.

## Decision
Make the **LookAndFeel the single runtime owner** of the LED colour; every
consumer derives from it rather than caching a copy.

1. `XplorerLookAndFeel` exposes `juce::Colour ledColour() const` (it already
   holds `_ledColour` and is the object rebuilt on change).
2. `ModMatrixPanel` drops `_highlightColour` and `setHighlightColour`; its
   `highlight*` methods read the colour from the effective LookAndFeel of their
   combos (`combo.getLookAndFeel()` → `dynamic_cast<XplorerLookAndFeel*>`),
   falling back to a neutral tint if the cast fails.
3. `MainComponent`'s constructor no longer reads the colour a second time nor
   calls `setHighlightColour`; `updateLedColour` keeps rebuilding the
   LookAndFeel and that alone now fixes every consumer, because the highlight is
   re-derived from the live LookAndFeel on each hover.
4. The **persisted** source of truth stays `UiConfiguration.knobLedBorderColor`
   (with the one default literal `DEFAULT_KNOB_COLOR` in
   `AllUsersSettingsDefaults`); the runtime source is the LookAndFeel derived
   from it. No colour literal is duplicated.

## Consequences
- Changing the knob colour updates knobs, tick boxes and the matrix highlight
  in one step (the reported bug is gone by construction, not by adding a second
  update call).
- One fewer piece of mutable UI state; impossible to leave a stale copy.
- The matrix panel gains a dependency on the concrete `XplorerLookAndFeel`
  type for the getter — acceptable (it is the app's own LookAndFeel); the
  fallback keeps it robust if the default LookAndFeel is ever swapped.

## Alternative rejected
"Also call `setHighlightColour` inside `updateLedColour`." — fixes the symptom
but keeps the duplicated state and the latent class of bug (any future consumer
must remember to subscribe). Rejected per the single-source-of-truth principle.
