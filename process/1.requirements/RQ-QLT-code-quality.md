# RQ-QLT — Code Quality: Named Constants and Single-Source Identity

Scope: the elimination of duplicated and unnamed literals in the application
layer, starting with the main menu bar identity (`MainComponent.cpp`), where the
same raw integers and id strings are repeated across independent sites with
nothing but a comment tying them together.

This file specifies **quality properties of the source**, not behaviour. Every
requirement below is behaviour-preserving by construction: the values stay
identical, only their declaration and reuse change.

Origin: code review of 2026-08-21 over `juce/` (18 474 lines, production
sources). The review found the codebase generally disciplined —
`Dialogs.cpp`, `SettingsDialog.cpp`, `PageFamilyBlock.cpp` and
`XpanderConstants.hpp` already declare their constants — with the dette
concentrated in `MainComponent.cpp`.

### Measured state before the change

| Site | Raw menu-id occurrences |
|---|---|
| `MENU_SHORTCUTS` table (keyboard dispatch) | 13 |
| Menu construction (`addReferenceItem` / `addItem`) | 22 |
| `menuItemSelected` dispatch (`case N:`) | 22 |

Shortcut-button id strings (`"btPatchLoad"`, …): 18 occurrences over 8 distinct
ids, across 3 sites.

---

## Functional Requirements

### RQ-QLT-001: Menu item identity is declared once

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: Every main-menu item id SHALL be declared exactly once as a
  named constant, and every site that constructs a menu item, binds a keyboard
  shortcut to it, or dispatches its selection SHALL refer to that constant
  rather than to a numeric literal.
- **Rationale**: the id is currently repeated across three independent sites; a
  change at one site alone silently breaks the shortcut or the action, with no
  compile-time error. The same file already names its View-menu ids
  (`VIEW_SCALE_FIRST_ID`, `VIEW_FULL_SCREEN_ID`) and `PageFamilyBlock.cpp`
  already names its context-menu ids — the idiom exists and is simply not
  applied here.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** the production sources
  - **When** menu item ids are searched for
  - **Then** no numeric literal is used as a menu item id at any construction,
    shortcut-binding or dispatch site
- **Dependencies**: RQ-GUI-008, ADR-JUC-032

### RQ-QLT-002: Menu item ids are unique and non-overlapping

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The set of declared menu item ids SHALL contain no duplicate
  value, and no fixed item id SHALL fall inside the contiguous id range
  reserved for the View menu's scale presets.
- **Rationale**: dispatch is by id, so a collision routes one item's selection
  to another's action. The View scale presets occupy a computed range
  (`VIEW_SCALE_FIRST_ID + index`) whose width follows
  `WINDOW_SCALE_PRESETS.size()`, so adding a preset can silently grow that range
  over a fixed id. Nothing checks this today.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** the declared menu item ids and the View scale id range
  - **When** the set is evaluated
  - **Then** all ids are pairwise distinct
  - **And** no fixed id lies within `[VIEW_SCALE_FIRST_ID,
    VIEW_SCALE_FIRST_ID + WINDOW_SCALE_PRESETS.size())`
- **Dependencies**: RQ-QLT-001, RQ-SCL-002

### RQ-QLT-003: Top-level menu order is declared once

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The top-level menu order SHALL be declared once, and the menu
  name list and the per-index menu builder SHALL both derive from that single
  declaration.
- **Rationale**: `getMenuBarNames()` returns the names positionally and
  `getMenuForIndex()` switches on bare indices `0..4`; the two are coupled only
  by position, so reordering the names rewires every menu silently.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** the top-level menu declaration
  - **When** the menu bar names are produced
  - **Then** the number of names equals the number of declared top-level menus
  - **And** each menu is built from a named top-level identifier, not a numeric
    literal
- **Dependencies**: RQ-GUI-008

### RQ-QLT-004: Shortcut-button identity is declared once

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: Each shortcut-button control id SHALL be declared exactly once
  as a named constant, and every site that registers its action, assigns its
  icon or invokes its action SHALL refer to that constant.
- **Rationale**: the ids are string keys into a `std::map`; a typo at one of the
  three sites yields a default-constructed empty action, so the button silently
  does nothing — no compile error, no runtime error.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** the production sources
  - **When** shortcut-button ids are searched for
  - **Then** each id appears as a string literal exactly once
- **Dependencies**: RQ-GUI-021, RQ-GUI-063

### RQ-QLT-005: A shared user action has one implementation

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: WHEN a user action is reachable from more than one trigger, the
  action SHALL have exactly one implementation, and every trigger SHALL invoke
  it.
- **Rationale**: "Go to patch" and "Store" are each implemented twice — once for
  the shortcut button and once for the menu item — duplicating both the dialog
  title literal and the lambda body, so the two triggers can drift apart. The
  same file already applies the correct idiom for rename
  (`showRenameDialogForCurrentTone()`, shared by the menu item and the VFD
  double-click).
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** an action reachable from a shortcut button and a menu item
  - **When** either trigger fires
  - **Then** both reach the same single implementation
- **Dependencies**: RQ-GUI-008, RQ-GUI-021

### RQ-QLT-006: File-dialog filters derive from the extension constant

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: Every sysex file-dialog filter SHALL derive from the existing
  `SYSEX_FILE_EXTENSION_WITH_DOT` constant rather than from a repeated wildcard
  literal.
- **Rationale**: `"*.syx"` is written out four times in the same file that
  already uses `SYSEX_FILE_EXTENSION_WITH_DOT` three times — the constant exists
  and is simply bypassed at the dialog sites.
- **Priority**: Should
- **Acceptance Criteria**:
  - **Given** the file-dialog construction sites
  - **When** the filter string is produced
  - **Then** it derives from `SYSEX_FILE_EXTENSION_WITH_DOT`
- **Dependencies**: RQ-CTL-001

### RQ-QLT-007: Residual literals are named at their point of use

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: Every remaining numeric or string literal that carries meaning
  beyond its own value SHALL be declared as a named constant at the narrowest
  scope that covers its uses.
- **Rationale**: the review found seven such literals in `MainComponent.cpp`:
  the settings-directory name repeated four times, the modal "first button"
  result code, the device→LED index mapping and its parallel colour ordering,
  the radio-group base id, the VFD control-table id, the selector-suffix digit
  range, and two hand-written `std::array` sizes.
- **Note on exclusion**: the VFD glass height `5 * GLYPH_HEIGHT + 2` is **out of
  scope**. Its rationale is recorded in ADR-JUC-007 ("Open point — resolved":
  owner decision of 2026-07-13, option (b), growth upward with the bottom edge
  fixed) and restated in the code comment at the site. `GLYPH_HEIGHT` is already
  sourced from the authority `VfdSegmentRenderer::CELL_HEIGHT`. Routing the
  remaining terms through the design system would move them away from the
  arbitrated decision that justifies them, which is a regression in
  traceability, not an improvement.
- **Priority**: Should
- **Acceptance Criteria**:
  - **Given** the listed literals
  - **When** each is used
  - **Then** it is referenced through a named constant declared once
  - **And** an array whose size follows its initialiser derives that size rather
    than restating it
- **Dependencies**: RQ-QLT-001

---

## Non-Functional Requirements

### RQ-QLT-010: Menu identity is verifiable without a GUI build

- **Category**: Non-Functional
- **NFR Type**: Maintainability
- **EARS Type**: Ubiquitous
- **Statement**: The menu identity declaration SHALL be free of any UI-framework
  dependency, so that its invariants are verifiable in the headless test target.
- **Metric**: the declaring header compiles and its tests run in a build
  configured with `-DXPL_BUILD_APP=OFF`
- **Measurement Method**: the test suite covering RQ-QLT-002 and RQ-QLT-003 is
  part of `xpl_tests_app`, which links `xpl_app_core` only
- **Rationale**: `MainComponent.cpp` belongs to the GUI target, which is off by
  default (`XPL_BUILD_APP=OFF`) and needs the full JUCE stack; nothing in it is
  covered by any test today. Moving the identity declaration into
  `xpl_app_core` is what makes RQ-QLT-002 and RQ-QLT-003 checkable at all.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** a build configured with `-DXPL_BUILD_APP=OFF`
  - **When** the test suites are built and run
  - **Then** the menu identity tests are among them and pass
- **Dependencies**: RQ-QLT-001, RQ-BLD-025, ADR-JUC-006
