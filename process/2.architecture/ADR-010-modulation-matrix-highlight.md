# ADR-010: Modulation-Matrix Hover Highlight & Active-Destination Marker — Shared Knob→Destination Map

## Status
Accepted (owner, 2026-07-13). Implemented: `ModulationHighlight` resolver
(headless-tested), `ModMatrixPanel` highlight API, `MainComponent` hover
wiring, and the VFD "." marker in `VfdDisplayHelper`.

## Requirements
RQ-GUI-018, RQ-GUI-020

## Context
The reference cross-references the panel and the modulation matrix on hover
(`MainForm` `#region ModulationMatrixHighlight`):

- Hovering a **page-family selector** that is a modulation source (ENV/LFO/
  RAMP/TRACK n) highlights the background of every **source** combo in the 20
  matrix rows currently set to that source
  (`PageRadioButton_MouseEnter`, map `_radioButtonToModSourceMap`).
- Hovering a **knob** that is a modulation destination highlights the
  background of every **destination** combo currently set to that destination
  whose row also has an active source (≠ NONE) (`KnobControl_MouseEnter`, maps
  `_knobTagToModDestMap` for fixed knobs and `_knobTagToPagedModDestMap` +
  `ResolvePagedModDest` for paged ENV/LFO knobs).
- Highlight colour = the knob LED-border colour; mouse-leave restores the
  default combo background.

The JUCE port has none of this. Crucially, the same knob→destination resolution
also drives the **VFD active-modulation-destination "." marker**
(`IsParameterModulatedForTag`), which was deferred earlier (architecture-
analysis §10-12) for exactly the reason that this map did not exist yet.

## Decision
1. **One headless resolver, `ModulationDestinationResolver`** (in
   `xpl_app_core`, so it is unit-tested): ports `_knobTagToModDestMap`,
   `_knobTagToPagedModDestMap` and `ResolvePagedModDest`. Given a control tag +
   the current family-instance selection, it returns the
   `EnumModulationDestinations` (or none). The page-selector→source map
   (`_radioButtonToModSourceMap`) is a second small static table beside it.
   These are the *only* new data; both are pure functions over the constants,
   headless-testable, and shared by the highlight and the VFD marker.
2. **`ModMatrixPanel` gains a highlight API**:
   `highlightSources(sourceValue)` / `highlightDestinations(destValue)` set the
   matching combos' `ComboBox::backgroundColourId` to the LED-border colour and
   repaint; `clearHighlight()` restores them. The panel already knows each
   row's current source/destination from the model, so matching is a scan of
   the 20 entries (same predicate as the reference, including the "source ≠
   NONE" guard for destinations).
3. **Wiring (`MainComponent`)**: page-family selector components and bound
   knobs get mouse enter/exit hooks (JUCE `MouseListener`) that call the
   resolver then the panel's highlight API. Paged knobs pass the active
   instance from the owning `PageFamilyBlock` (the resolver needs the current
   ENV/LFO page), matching `ResolvePagedModDest`.
4. **VFD marker unlocked**: `VfdDisplayHelper::showControlEdit` appends `.` to
   the parameter display name when the resolver says the tag is an active
   modulation destination (a matrix entry targets it with source ≠ NONE) —
   closing the deferred §10-12 gap with the same resolver, at no extra data
   cost.

## Consequences
- The panel↔knob/selector cross-highlight of the reference returns, aiding
  patch comprehension; colour tracks the user's LED setting.
- The knob→destination map is written once and serves both the highlight and
  the long-deferred VFD "." marker (removes gap §10-12).
- `ModulationDestinationResolver` + the source map are headless-tested;
  the hover wiring is thin (mouse listener → resolver → panel), visual-only.
- Highlight is a transient background colour swap; no model or MIDI effect,
  cleared on mouse-leave and re-derived on each hover (so it reflects the
  current matrix even after edits).

## Open question (owner)
The reference highlights on the raw `MouseEnter`/`MouseLeave` of each control.
Under the scaled canvas this maps directly to JUCE `mouseEnter`/`mouseExit`;
no arbitration needed unless you want a hover delay — proposed: none (match the
reference's immediate response).
