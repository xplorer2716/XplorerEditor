# PLAN-QLT-001: Named Constants and Single-Source Menu Identity

## Overview

Removes the duplicated and unnamed literals found by the 2026-08-21 code review
of `juce/`, concentrated in `MainComponent.cpp`. Behaviour-preserving
throughout: every value keeps the number or string it has today; only its
declaration and reuse change.

The menu identity is extracted to `xpl_app_core` so its invariants become
verifiable in the headless test target — `MainComponent.cpp` is compiled only in
the GUI target (`XPL_BUILD_APP`, default `OFF`) and is covered by no test today.

## References

- **Requirements**: RQ-QLT-001, RQ-QLT-002, RQ-QLT-003, RQ-QLT-004, RQ-QLT-005,
  RQ-QLT-006, RQ-QLT-007, RQ-QLT-010
- **ADRs**: ADR-QLT-001 (DEC-QLT-001 … DEC-QLT-005)
- **Preserved unchanged**: RQ-GUI-008 and ADR-JUC-032 (reference menu parity),
  ADR-JUC-007 (VFD glass height — explicitly out of scope, see RQ-QLT-007)

## Baseline (before any change)

Established on this toolchain, 2026-08-21:

- `xpl_app_core`, `xpl_tests_app`, `XplorerApp` — build clean.
- Test suite — **115/115 pass** (1 skipped: virtual MIDI cable absent).
- `xpl_tests_app_juce` — **fails to build, pre-existing**, unrelated to this
  work: GCC 13 `-Wdangling-reference` false positive at
  `tests/app/BoundRadioGroupTests.cpp:154`, where `specById()` returns a
  reference into the static control table, not into the temporary `std::string`
  materialised for its parameter. Not touched by this plan — modifying a test
  to make a build green is forbidden by the DoD, and the diagnostic is an
  artifact of this container's compiler version.

---

## Tasks

### TASK-QLT-001: Menu identity header and its invariant tests
- **Tier**: L
- **Status**: Done
- **Description**: Add `app/core/include/xplorer/app/MenuIds.hpp` declaring the
  `MenuItem` and `TopLevelMenu` enumerations plus the View id constants, and a
  headless test suite asserting the invariants that no reader can check by
  inspection.
- **Requirement refs**: RQ-QLT-001, RQ-QLT-002, RQ-QLT-003, RQ-QLT-010
- **ADR refs**: ADR-QLT-001 (DEC-QLT-001, DEC-QLT-002, DEC-QLT-004)
- **Acceptance Criteria**:
  - **Given** the declared menu item ids
  - **When** the set is evaluated
  - **Then** all ids are pairwise distinct
  - **And** no fixed id falls inside the View scale preset id range
  - **And** the top-level menu count matches the number of menu bar names
  - **And** the suite builds and runs with `-DXPL_BUILD_APP=OFF`
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-002: Rewire the three menu sites to the shared identity
- **Tier**: M
- **Status**: Done
- **Description**: Replace the raw ids at all three sites — `MENU_SHORTCUTS`,
  `getMenuForIndex`, `menuItemSelected` — with `MenuItem` enumerators, and build
  `getMenuBarNames()` and the per-index dispatch from `TopLevelMenu`.
- **Requirement refs**: RQ-QLT-001, RQ-QLT-003
- **ADR refs**: ADR-QLT-001 (DEC-QLT-003, DEC-QLT-004, DEC-QLT-005)
- **Acceptance Criteria**:
  - **Given** the rewired sites
  - **When** the GUI target is built
  - **Then** it compiles with no new diagnostic
  - **And** no numeric literal remains as a menu item id or top-level index
  - **And** every id keeps the value it had before the change
- **Dependencies**: TASK-QLT-001
- **Assignee**: AI

### TASK-QLT-003: Shortcut-button ids as named constants
- **Tier**: M
- **Status**: Done
- **Description**: Declare the eight shortcut-button control ids once and refer
  to them from the action registration, the icon table and the menu handlers
  that invoke a button's action.
- **Requirement refs**: RQ-QLT-004
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the shortcut-button ids
  - **When** the sources are searched
  - **Then** each id appears as a string literal exactly once
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-004: One implementation per shared action
- **Tier**: M
- **Status**: Done
- **Description**: Extract the duplicated "Go to patch" and "Store" bodies into
  private methods, following the existing
  `showRenameDialogForCurrentTone()` idiom, and call them from both the shortcut
  button and the menu item.
- **Requirement refs**: RQ-QLT-005
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the shortcut button and the menu item for an action
  - **When** either is triggered
  - **Then** both reach the same single implementation
  - **And** the dialog title literal appears once per action
- **Dependencies**: TASK-QLT-003
- **Assignee**: AI

### TASK-QLT-005: Sysex dialog filters derive from the extension constant
- **Tier**: S
- **Status**: Done
- **Description**: Replace the four `"*.syx"` literals with a filter derived
  from `SYSEX_FILE_EXTENSION_WITH_DOT`.
- **Requirement refs**: RQ-QLT-006
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** a file dialog construction site
  - **When** its filter is produced
  - **Then** the filter derives from `SYSEX_FILE_EXTENSION_WITH_DOT`
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-006: Residual literals named at point of use
- **Tier**: M
- **Status**: Done
- **Description**: Name the settings-directory component, the modal first-button
  result, the device→LED index mapping and its colour ordering, the radio-group
  base id, the VFD control-table id and the selector-suffix digit bounds; derive
  the two `std::array` sizes from their initialisers. Excludes the VFD glass
  height (see RQ-QLT-007).
- **Requirement refs**: RQ-QLT-007
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** each listed literal
  - **When** it is used
  - **Then** it is referenced through a named constant declared once
  - **And** no array restates a size its initialiser already fixes
- **Dependencies**: None
- **Assignee**: AI

---

## Verification

Every task is verified by the same two gates, run on the whole plan:

1. `xpl_app_core`, `xpl_tests_app` and `XplorerApp` build with no new
   diagnostic, warnings-as-errors on (`-Wall -Wextra -Wpedantic -Werror`).
2. The test suite still reports **115/115 pass**, plus the new suite from
   TASK-QLT-001 — no pre-existing test modified.

Behaviour preservation is argued structurally rather than by GUI test: the ids
and strings keep their values (DEC-QLT-002), so the compiler proves the rewiring
where the values are enumerators, and the residual literals are single-value
substitutions.
