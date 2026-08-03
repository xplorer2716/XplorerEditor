# PLAN-MTX-001: Modulation-Matrix Block Identity and Frame Highlight

## Overview

Carry the functional-block colour identity into the modulation matrix, and
re-render the RQ-GUI-018 hover cross-reference as a frame treatment so it stops
overwriting the information the new colours carry. Implements ADR-JUC-028.

Scope is the matrix's two combo boxes per row. The amount knob and quantize
check box are deliberately untouched, and no geometry, list content or
controller behaviour changes.

## References

- **Requirements**: RQ-GUI-052 *(new)*, RQ-DSN-100 *(new)*, RQ-GUI-018
  *(rendering amended)*, RQ-DSN-004 *(amended — matrix no longer an accent
  consumer)*, RQ-GUI-015, RQ-GUI-016, RQ-GUI-041, RQ-GUI-044, RQ-GUI-045,
  RQ-DSN-023, RQ-DSN-092, RQ-DSN-094, RQ-DSN-095, RQ-DSN-099
- **ADRs**: ADR-JUC-028 *(this plan implements it)*; ADR-JUC-006 (headless core
  boundary), ADR-JUC-010 (modulation cross-reference), ADR-JUC-011 (accent
  single source of truth), ADR-JUC-014 (tokens), ADR-JUC-017 (interaction
  states), ADR-JUC-018 / ADR-JUC-020 (block palette), ADR-JUC-027
  (`strokeDiagram`)

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = chat-eco`.

**No new design token.** Everything reuses `blockFillAlpha`, `strokeBorder`,
`strokeDiagram` and `hoverBrighten` — RQ-DSN-100 exists precisely to say that a
third consumer of the block family invents nothing.

**DoR — owner approved the requirement, the highlight design and the ID plan,
2026-08-03 ("c'est bon pour moi").**

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-MTX-001 | Move `BlockId` to core; add the headless block-lookup tables + tests | M | RQ-GUI-052, DEC-JUC-079 |
| TASK-MTX-002 | `ModMatrixComboBox` carrying tint + highlight state | M | RQ-GUI-052, DEC-JUC-080 |
| TASK-MTX-003 | Render tint and frame highlight in `drawComboBox` | M | RQ-GUI-052, RQ-DSN-100, DEC-JUC-078/081 |
| TASK-MTX-004 | Wire `ModMatrixPanel`, retire the background-repaint highlight | M | RQ-GUI-018, RQ-GUI-052, DEC-JUC-082 |

---

### TASK-MTX-001: Move `BlockId` to core; add the headless block-lookup tables
- **Tier**: M
- **Status**: **Done** — 28 sources and 47 destinations covered exhaustively;
  suite 110/110.
- **Description**: Extract `enum class BlockId` from `BlockPalette.hpp` into a
  JUCE-free `xplorer/app/BlockIdentity.hpp` in `xpl_app_core`, included by
  `BlockPalette.hpp` so existing users are unaffected. Add
  `modulationSourceBlock()` / `modulationDestinationBlock()` to
  `ModulationHighlight`, returning `std::optional<BlockId>`, as constexpr tables
  in the style of the existing `SELECTOR_SOURCES` / `FIXED_DESTINATIONS`.
- **Requirement refs**: RQ-GUI-052, RQ-DSN-100
- **ADR refs**: ADR-JUC-028 (DEC-JUC-079)
- **Acceptance Criteria**:
  - *Given* every value of `EnumModulationSourcesModMatrix`, *When* mapped,
    *Then* `ENV1..5`→Env, `LFO1..5`→Lfo, `RMP1..4`→Ramp, `TRK1..3`→Track,
    `LAG`→Lag, and `KBD`/`VEL`/`RVEL`/`PRES`/`PED1`/`PED2`/`VIB`/`LEV1`/`LEV2`/
    `NONE`→nullopt.
  - *Given* every value of `EnumModulationDestinations`, *When* mapped, *Then*
    each resolves to a block and none returns nullopt (VCO/VCF/LFO/ENV/LAG only
    — the model has no TRACK/RAMP/MATRIX destination).
  - *Given* `xpl_app_core`, *When* it is built, *Then* it still links no JUCE
    module, and `xpl_tests_app` covers the new functions without JUCE.
  - *Given* `BlockId`'s declaration order, *When* compared with before, *Then*
    it is byte-identical — it is the RQ-SET-007 persistence contract.
- **Dependencies**: None
- **Assignee**: AI

### TASK-MTX-002: `ModMatrixComboBox` carrying tint + highlight state
- **Tier**: M
- **Status**: **Done** — with a correction made in TASK-MTX-004: the control
  first stored an `std::optional<juce::Colour>`, which is the cached copy
  RQ-DSN-095 forbids. It now stores the `BlockId` and the hue is resolved from
  the live palette at paint time, so a user re-theme reaches the matrix through
  the existing `sendLookAndFeelChange` with no invalidation code at all.
- **Description**: Add a `ModMatrixComboBox : HoverRepaintingComboBox` holding an
  `std::optional<BlockId>` and a `bool` highlight flag, each with a setter that
  repaints only on an actual change. Use it for the matrix's source and
  destination combos. *(The identity, not the colour — see the status note.)*
- **Requirement refs**: RQ-GUI-052
- **ADR refs**: ADR-JUC-028 (DEC-JUC-080)
- **Acceptance Criteria**:
  - *Given* the new class, *When* it is used, *Then* it still inherits the
    issue-#21 mouseEnter/mouseExit repaint of `HoverRepaintingComboBox`.
  - *Given* a setter called with the value already held, *When* it runs, *Then*
    no repaint is triggered.
  - *Given* a combo whose value has no block, *When* its state is read, *Then*
    the block hue is `nullopt` rather than a sentinel colour.
- **Dependencies**: TASK-MTX-001
- **Assignee**: AI

### TASK-MTX-003: Render tint and frame highlight in `drawComboBox`
- **Tier**: M
- **Status**: **Done** — build warning-clean.
- **Description**: Extend `XplorerLookAndFeel::drawComboBox` to resolve the
  optional block state: fill = `surfaceRecessed.overlaidWith(hue.withAlpha(
  blockFillAlpha))` (opaque, DEC-JUC-081), frame = hue at `strokeBorder`, and
  when highlighted, frame = `hue.brighter(hoverBrighten)` at `strokeDiagram`
  with the fill left alone. Untinted combos keep today's exact appearance, and
  RQ-GUI-041's direct-hover brighten and RQ-GUI-042's focus ring are preserved.
- **Requirement refs**: RQ-GUI-052, RQ-DSN-100, RQ-GUI-041, RQ-GUI-042
- **ADR refs**: ADR-JUC-028 (DEC-JUC-078, DEC-JUC-081)
- **Acceptance Criteria**:
  - *Given* a combo with no block state, *When* drawn, *Then* its appearance is
    unchanged from before this plan — every other combo box in the app.
  - *Given* a tinted combo at rest, *When* drawn, *Then* its frame is the pure
    block hue at `strokeBorder` and its fill is opaque.
  - *Given* the same combo highlighted, *When* drawn, *Then* only the frame
    differs — brighter and at `strokeDiagram`.
  - *Given* a tinted combo that is also focused, *When* drawn, *Then* the focus
    ring is still visible. *(Written expecting the block frame to remain visible
    beside it; the running app showed the focus ring covers it entirely, being
    drawn last and thicker. The criterion is corrected to what was actually
    accepted — the focus affordance wins — rather than marked met as written.
    Reasoning in ADR-JUC-028 Consequences.)*
  - *Given* the renderer source, *When* read, *Then* no raw colour, alpha or
    width literal was introduced.
- **Dependencies**: TASK-MTX-002
- **Assignee**: AI

### TASK-MTX-004: Wire `ModMatrixPanel`, retire the background-repaint highlight
- **Tier**: M
- **Status**: **Done** — verified in the running app, measured rather than eyeballed.
  Row 1 set to `LFO5`: resting frame is exactly `#A087C9` (the `blockLfo`
  token); on hovering the `LFO 5` selector the frame pixel goes to
  (187,169,216) (brighter) and the pixel one row inside flips from fill
  (69,65,91) to frame (99,91,122) (thicker), while the fill itself is
  byte-identical — the two cues of DEC-JUC-078, and the untouched background.
  `_defaultComboBackground` and `highlightColour()` are gone, and with them the
  last `setColour(backgroundColourId, ...)` call. Suite 110/110.
  - **Found while verifying:** a focused matrix combo hides both the block frame
    and the highlight, because RQ-GUI-042's focus ring is drawn last at 2.0 px
    over their 1.0/1.5 px. Accepted and recorded in ADR-JUC-028 Consequences
    rather than worked around — see the reasoning there.
- **Description**: Resolve each combo's hue from the selected value through the
  runtime palette in `refreshRow` and after every user edit (DEC-JUC-082);
  replace `highlightSources`/`highlightDestinations`/`clearHighlight`'s
  `setColour(backgroundColourId, ...)` with the highlight flag; delete
  `_defaultComboBackground` and `highlightColour()`, now unused. The
  `MainComponent::onControlHovered` trigger and the row-matching rules are
  untouched.
- **Requirement refs**: RQ-GUI-018, RQ-GUI-052, RQ-DSN-095
- **ADR refs**: ADR-JUC-028 (DEC-JUC-082), ADR-JUC-011
- **Acceptance Criteria**:
  - *Given* a loaded patch, *When* `refreshRow` runs, *Then* each combo's tint
    matches its newly selected value — the load path, not just construction.
  - *Given* a user changes a source or destination, *When* the edit completes,
    *Then* that combo's tint updates immediately.
  - *Given* the panel source, *When* read, *Then* no `setColour(
    backgroundColourId, ...)` call remains and `highlightColour()` is gone.
  - *Given* the user changes a block colour in settings, *When* the matrix
    repaints, *Then* it shows the new hue (no cached copy, RQ-DSN-095).
  - *Given* the full suite, *When* it runs, *Then* every pre-existing test passes
    unmodified.
- **Dependencies**: TASK-MTX-003
- **Assignee**: AI
