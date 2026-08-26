# PLAN-GUI-013: Keyboard Preset-Value Entry for Rotary Knobs

## Overview
Ports the .NET reference's knob preset-value gesture — hover a rotary knob,
press a number-row key, the knob jumps to that preset — to the JUCE
application, resolved from **physical key positions** so it works on the
owner's AZERTY keyboard as well as on QWERTY. Closes the "Not in scope /
deferred" item ADR-JUC-009 left open when the knobs were first ported.

The layout-independent machinery is not built here: it already exists
(ADR-JUC-035's per-platform `KeyboardLayoutQuery`), and this plan gives it a
second consumer by extending its position table.

## References
- **Requirements**: RQ-GUI-079, RQ-GUI-080 — with RQ-GUI-074, RQ-GUI-034,
  RQ-GUI-015, RQ-GUI-030, RQ-GUI-003, RQ-GUI-004, RQ-GUI-020, RQ-GUI-054,
  RQ-GUI-008, RQ-GUI-027, RQ-BLD-025 constraining it
- **ADRs**: ADR-JUC-037 (DEC-JUC-125…131); amends ADR-JUC-009; extends
  ADR-JUC-035 (DEC-JUC-116); constrained by ADR-JUC-029 (DEC-JUC-088);
  ADR-BUG-001 for the distribution correction; ADR-JUC-006 for the layering
- **Design system**: not applicable, and this is deliberate — the feature
  introduces no colour, spacing, metric or typographic value. Its constants are
  semitones and parameter units, which the design system does not own
  (RQ-DSN §2). Recorded here so the absence reads as a decision, not an
  omission.

## Session variables
`unit_tests: true` (raised from `false` at the owner's instruction mid-session:
*"active les tests unitaires c'est important, il faut tester"*),
`platform: windows`, `chat_mode: normal`.

---

## Tasks

### TASK-GUI-046: Extract the KeyboardLayoutQuery interface into its own header
- **Tier**: S
- **Status**: Done
- **Description**: Move the `KeyboardLayoutQuery` abstract class out of
  `PianoKeyMapping.hpp` into `xplorer/app/KeyboardLayoutQuery.hpp`, so a second
  consumer need not include the piano's header to reach the seam. The
  implementation files keep their `Piano*` names (DEC-JUC-131).
- **Requirement refs**: RQ-GUI-080, RQ-BLD-025
- **ADR refs**: ADR-JUC-037 (DEC-JUC-131), ADR-JUC-035 (DEC-JUC-116)
- **Acceptance Criteria**: *Given* the headless build, *When* it is configured
  with `-DXPL_BUILD_APP=OFF`, *Then* the new header compiles with no OS
  dependency. *Given* `PianoKeyMapping.hpp`, *When* it is read, *Then* it
  includes the extracted header and no longer declares the interface, and every
  existing piano consumer builds unchanged.
- **Dependencies**: None
- **Assignee**: AI

### TASK-GUI-047: Preset values and preset keymap in the headless core
- **Tier**: M
- **Status**: Done
- **Description**: New `xpl_app_core` module `KnobPresetValues`:
  `presetKeyPositions()` (the eleven physical positions, eleventh slot bound
  twice), `resolvePresetKeyMapping()`, `presetIndexForCharacter()`,
  `presetValuesForRange()` (ten or eleven values, mirrored on a symmetric
  range) and `harmonicPresetValuesFor()` (the VCO override). Pure functions,
  no JUCE, no OS.
- **Requirement refs**: RQ-GUI-079, RQ-GUI-080, RQ-BLD-025
- **ADR refs**: ADR-JUC-037 (DEC-JUC-126, DEC-JUC-127, DEC-JUC-128,
  DEC-JUC-129, DEC-JUC-130), ADR-BUG-001
- **Acceptance Criteria**: *Given* a 0..63 range, *When* presets are derived,
  *Then* they are `0 7 14 … 63` — identical to the reference's own serialised
  table. *Given* a −63..63 range, *When* presets are derived, *Then* there are
  eleven, the middle is exactly 0, and the set is a mirror about it. *Given*
  `VCO1_FREQ`, *When* its override is looked up, *Then* it is the reference's
  semitone table and every value lies inside 0..63. *Given* a null query,
  *When* the keymap is resolved, *Then* it is empty.
- **Dependencies**: TASK-GUI-046
- **Assignee**: AI

### TASK-GUI-048: Extend the three per-platform position tables with the number row
- **Tier**: M
- **Status**: Done
- **Description**: Add the ten number-row positions plus both eleventh-slot
  candidates to `referenceScanCodes()` (Windows, PC/AT scancodes 0x02–0x0C and
  0x1A), `referenceKeycodes()` (Linux, evdev `KEY_*`+8) and
  `referenceKeyCodes()` (macOS, `kVK_ANSI_*` SDK constants). Update each file's
  header comment to state the table now serves two features, and correct the
  macOS dead-key note, which the new `[` position invalidates.
- **Requirement refs**: RQ-GUI-080
- **ADR refs**: ADR-JUC-037 (DEC-JUC-126, DEC-JUC-127, DEC-JUC-131),
  ADR-JUC-035 (DEC-JUC-116, DEC-JUC-118)
- **Acceptance Criteria**: *Given* the Windows table, *When* it is compared
  against a live fr-FR layout probe, *Then* each listed scancode resolves to the
  character that layout actually produces. *Given* any platform file, *When* it
  is read, *Then* the piano positions and the preset positions are separated by
  comment and each cites its requirement.
- **Dependencies**: TASK-GUI-047
- **Assignee**: AI

### TASK-GUI-049: Preset application on NumericEntryKnob
- **Tier**: M
- **Status**: Done
- **Description**: Give `NumericEntryKnob` `setPresetValues()`,
  `presetValues()` (derived lazily from the live range) and
  `applyPresetValue(index)`, and factor the value-commit path into a shared
  private `applyValue()` used by both the preset key and the existing
  double-click text entry.
- **Requirement refs**: RQ-GUI-079, RQ-GUI-034, RQ-GUI-015
- **ADR refs**: ADR-JUC-037 (DEC-JUC-128), ADR-JUC-009 (amended)
- **Acceptance Criteria**: *Given* a knob whose range is set after
  construction, *When* its presets are asked for, *Then* they span the range
  that was set. *Given* a unipolar knob, *When* the eleventh preset is applied,
  *Then* it returns false and the value is unchanged. *Given* a knob carrying
  the harmonic override, *When* the fourth preset is applied, *Then* it lands on
  12, not on a linear quarter of its range.
- **Dependencies**: TASK-GUI-047
- **Assignee**: AI

### TASK-GUI-050: Central key dispatch and the VCO harmonic override wiring
- **Tier**: M
- **Status**: Done
- **Description**: Resolve the keymap once in `MainComponent`'s constructor;
  add `applyPresetKeyToHoveredKnob()` and call it first from
  `keyPressed()`; apply `harmonicPresetValuesFor()` where `BoundKnob`s are
  built from the control table.
- **Requirement refs**: RQ-GUI-079, RQ-GUI-080, RQ-GUI-008, RQ-GUI-027,
  RQ-GUI-054
- **ADR refs**: ADR-JUC-037 (DEC-JUC-125, DEC-JUC-126, DEC-JUC-130),
  ADR-JUC-029 (DEC-JUC-088), ADR-JUC-032 (DEC-JUC-099)
- **Acceptance Criteria**: *Given* a modifier held, *When* a preset key is
  pressed, *Then* the gesture declines before looking anything up and the menu
  table sees the press. *Given* no knob under the pointer, *When* a preset key
  is pressed, *Then* `keyPressed` behaves exactly as it did before this plan.
  *Given* the sources, *When* they are searched, *Then* no `grabKeyboardFocus`
  call was added and no hovered component is cached.
- **Dependencies**: TASK-GUI-048, TASK-GUI-049
- **Assignee**: AI

### TASK-GUI-051: Test coverage for both halves
- **Tier**: M
- **Status**: Done
- **Description**: `KnobPresetValuesTests.cpp` in the headless target (keymap
  resolved against a fake US-QWERTY and a fake fr-FR AZERTY layout; value
  tables; degradation) and three added scenarios in `NumericEntryKnobTests.cpp`
  in the JUCE-linked target (lazy derivation, inert eleventh key, override).
  Non-ASCII characters are written as numeric `char32_t` constants so MSVC does
  not decode the source through the host ANSI codepage.
- **Requirement refs**: RQ-GUI-079, RQ-GUI-080, RQ-TST-001, RQ-BLD-025
- **ADR refs**: ADR-JUC-037, ADR-JUC-003
- **Acceptance Criteria**: *Given* a fake AZERTY layout, *When* the keymap is
  resolved, *Then* the ten physical keys select presets 0..9 under
  `& é " ' ( - è _ ç à`, the character `1` selects nothing, and `)` selects the
  eleventh. *Given* the full suite, *When* CTest runs, *Then* every scenario
  passes and no pre-existing test was modified.
- **Dependencies**: TASK-GUI-047, TASK-GUI-049
- **Assignee**: AI

---

## Verification record (2026-08-27)

| Check | Result |
|---|---|
| `cmake --build . --config Release` (all targets) | clean — no error, no warning |
| `ctest -C Release` | **179/179 passed**, 0 failed |
| New headless scenarios (`[RQ-GUI-079],[RQ-GUI-080]`) | 6 cases, 113 assertions, all passed |
| New JUCE-linked scenarios (`[RQ-GUI-079]`) | 3 cases, 16 assertions, all passed |
| Pre-existing tests modified | **none** |

## Open for owner verification
- The end-to-end gesture in the running application on a real keyboard: no test
  controls the CI runner's layout, so the resolved-character path is pinned
  against a fake and the live path is not.
- The Linux and macOS position tables are hand-derived and have never been
  compiled or executed in this session (the same standing caveat ADR-JUC-035
  carries for the piano positions beside them). CI is their first build.
