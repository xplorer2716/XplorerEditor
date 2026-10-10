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

---

## SonarCloud CRITICAL Remediation (2026-08-29)

Origin: SonarCloud Cloud-hosted automated analysis of project
`xplorer2716_XplorerEditor` (607 open issues at scan time: 75 CRITICAL, 435
MAJOR, 94 MINOR, 3 INFO). This tranche covers the CRITICAL-severity issues
that are safe, behaviour-preserving mechanical fixes. Excluded from this
tranche and triaged separately:

- **Accepted (SonarCloud status `accept`), no code change** — recorded here
  for traceability since the MCP status-change call carries no comment field:
  - `cpp:S134` (nesting depth) in the 29 test files under `juce/tests/` —
    these suites are machine-generated ports of the reference test corpus,
    not intended for human-maintained readability; the nesting mirrors the
    reference's own control flow.
  - `cpp:S5025` at `SettingsDialog.cpp:151,157` — `TableListBoxModel::
    refreshComponentForCell`'s cell-recycling contract is JUCE's own raw
    owning-pointer API; there is no RAII-compatible override signature.
  - `cpp:S5421` at `Logger.cpp:37-39` (`g_mutex`, `g_sink`, `g_level`) —
    deliberately mutable, mutex-guarded process-wide singleton state (see the
    file's own header comment); `const` would break the logger.
- **Deferred to a follow-up plan** (PLAN-QLT-003, out of this session's
  scope): the remaining 20 CRITICAL issues requiring more invasive
  refactoring — `cpp:S134` outside test files (5), `cpp:S3776` in C++ and
  Python (8), `cpp:S3624` rule-of-five (3), `cpp:S3656` (1), `cpp:S5213`
  `std::function`-to-template (3).

### RQ-QLT-011: Destructor MIDI-device teardown does not rely on virtual dispatch

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: `AbstractController`'s destructor SHALL invoke
  `closeMidiDevices()` through an explicitly qualified, non-virtual call.
- **Rationale**: SonarCloud `cpp:S1699` — a virtual call made from a
  destructor never reaches a derived override (the vtable already points to
  the base at that point), so the implicit call silently pins itself to
  `AbstractController::closeMidiDevices` regardless of what the declaration
  suggests. Qualifying the call states that pinning explicitly instead of
  leaving it as an accident of destructor semantics.
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** `~AbstractController()`
  - **When** it calls `closeMidiDevices()`
  - **Then** the call is qualified as `AbstractController::closeMidiDevices()`
  - **And** teardown behaviour is unchanged
- **Dependencies**: RQ-MID-006, ADR-BUG-002

### RQ-QLT-012: Deliberately empty methods document their own emptiness

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: Every method with a deliberately empty body SHALL carry a
  comment nested inside its braces explaining why it is empty.
- **Rationale**: SonarCloud `cpp:S1186` flags 8 sites (`ProgressWindow::
  closeButtonPressed`, six `AbstractController` synth/automation no-op
  handlers, `XpanderFullToneParameter::updateMessageFromValue`) whose
  rationale today sits in a trailing same-line or preceding comment rather
  than inside the body the rule inspects.
- **Priority**: Should
- **Acceptance Criteria**:
  - **Given** the 8 flagged sites
  - **When** each method is read
  - **Then** its body contains a nested comment stating why it is empty
  - **And** no method's behaviour changes
- **Dependencies**: None

### RQ-QLT-013: Handle and image-data parameters carry meaningful types

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: `SingleInstanceGuard`'s platform lock handle and
  `addReferenceItem`'s icon-data parameter SHALL be typed as the concrete
  type they hold rather than `void*`.
- **Rationale**: SonarCloud `cpp:S5008` flags both. `SingleInstanceGuard::
  _handle` stores a Win32 `HANDLE` (already compared against
  `INVALID_HANDLE_VALUE` and passed to `CloseHandle` in the `.cpp`, which
  already includes `<windows.h>`; both current includers of the header
  already include it too, so widening the header's include has no new
  blast radius). `addReferenceItem`'s `iconData` is always either absent or
  one of the `BinaryData::*_png` byte arrays (`const char*`), which is what
  it should be declared as; `juce::Drawable::createFromImageData` still
  accepts it unchanged since `const char*` converts to `const void*`.
- **Priority**: Should
- **Acceptance Criteria**:
  - **Given** the two sites
  - **When** their declarations are read
  - **Then** `_handle` is typed `HANDLE` under `#ifdef _WIN32`
  - **And** `iconData` is typed `const char*`
  - **And** all call sites compile unchanged
- **Dependencies**: None

### RQ-QLT-014: Settings-dialog page ownership is expressed as RAII up to the JUCE handoff

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: Where a `SettingsDialog.cpp` component is constructed only
  to be immediately handed to a JUCE owning API (`TabbedComponent::addTab`,
  `LookAndFeel::createSliderTextBox`'s return), construction SHALL go through
  a smart pointer that is released only at the handoff call.
- **Rationale**: SonarCloud `cpp:S5025` flags 4 such sites (the three
  settings-page tabs and `XplorerLookAndFeel::createSliderTextBox`'s label).
  The bare `new` and the ownership transfer are textually separated today;
  routing construction through `std::make_unique` and calling `.release()`
  exactly at the transfer point makes the ownership handoff the only place a
  raw pointer exists, without changing which object ends up owning what.
- **Priority**: Should
- **Acceptance Criteria**:
  - **Given** the 4 flagged sites
  - **When** each component is constructed
  - **Then** it is held in a smart pointer until the JUCE ownership-taking
    call
  - **And** that call still receives the same raw pointer type it did before
- **Dependencies**: None

### RQ-QLT-015: Test lambdas capture only what they use, explicitly

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The 6 flagged test lambdas in `ParameterBindingRegistryTests.
  cpp` and `XpanderControllerTests.cpp` SHALL capture their referenced
  variables by explicit name rather than by default `[&]`.
- **Rationale**: SonarCloud `cpp:S3608` — default reference capture pulls in
  the whole enclosing scope, so a later edit to the test fixture can silently
  change what a lambda captures. Naming the capture makes the dependency
  visible at the call site and is a mechanical, non-behavioural edit.
- **Priority**: Should
- **Acceptance Criteria**:
  - **Given** the 6 flagged lambdas
  - **When** their capture lists are read
  - **Then** each names its captured variables explicitly
  - **And** the test suite still passes unchanged
- **Dependencies**: RQ-TST-*

### RQ-QLT-016: SonarCloud triage decisions are recorded, not silent

- **Category**: Non-Functional
- **NFR Type**: Maintainability
- **EARS Type**: Ubiquitous
- **Statement**: WHEN a CRITICAL SonarCloud issue is accepted rather than
  fixed, the rationale SHALL be recorded in this file, since the SonarCloud
  MCP status-change tool carries no comment field.
- **Metric**: every issue key accepted in this session is covered by a bullet
  in the "SonarCloud CRITICAL Remediation" section above
- **Measurement Method**: manual cross-check between the 34 accepted issue
  keys and the bullets above at session close
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** an issue marked `accept` in SonarCloud
  - **When** this file is read
  - **Then** its rationale is findable by rule id and location
- **Dependencies**: None

## SonarCloud `dev` Backlog Remediation (PLAN-QLT-003)

Origin: SonarCloud scan of branch `dev`, analysis 2026-10-09 — 324 OPEN issues
(0 BLOCKER, 33 HIGH, 177 MEDIUM, 114 LOW; MQR impact severities).

### RQ-QLT-017: HIGH-impact SonarCloud issues are resolved or triaged

- **Category**: Non-Functional
- **NFR Type**: Maintainability
- **EARS Type**: Ubiquitous
- **Statement**: The codebase SHALL have zero OPEN HIGH-impact SonarCloud
  issues on the delivered branch, each being either fixed or accepted with a
  rationale recorded per RQ-QLT-016.
- **Metric**: OPEN issues with impact severity HIGH = 0 (33 at baseline)
- **Measurement Method**: `sonar list issues --branch <branch> --severities HIGH`
  after the next SonarCloud analysis
- **Priority**: Must
- **Acceptance Criteria**:
  - **Given** the 33 baseline HIGH issues
  - **When** SonarCloud re-analyses the branch
  - **Then** none remains OPEN
  - **And** the test suite passes with no test modified to force a pass
- **Dependencies**: RQ-QLT-016, RQ-TST-*

### RQ-QLT-018: MEDIUM-impact SonarCloud issues are resolved or triaged

- **Category**: Non-Functional
- **NFR Type**: Maintainability
- **EARS Type**: Ubiquitous
- **Statement**: The codebase SHALL have zero OPEN MEDIUM-impact SonarCloud
  issues on the delivered branch, each being either fixed or accepted with a
  rationale recorded per RQ-QLT-016.
- **Metric**: OPEN issues with impact severity MEDIUM = 0 (177 at baseline)
- **Measurement Method**: `sonar list issues --branch <branch> --severities MEDIUM`
- **Priority**: Should
- **Acceptance Criteria**:
  - **Given** the 177 baseline MEDIUM issues
  - **When** SonarCloud re-analyses the branch
  - **Then** none remains OPEN
  - **And** the test suite passes with no test modified to force a pass
- **Dependencies**: RQ-QLT-016, RQ-QLT-017, RQ-TST-*

### RQ-QLT-019: LOW-impact SonarCloud issues are resolved or triaged

- **Category**: Non-Functional
- **NFR Type**: Maintainability
- **EARS Type**: Ubiquitous
- **Statement**: The codebase SHALL have zero OPEN LOW-impact SonarCloud
  issues on the delivered branch, each being either fixed or accepted with a
  rationale recorded per RQ-QLT-016.
- **Metric**: OPEN issues with impact severity LOW = 0 (114 at baseline)
- **Measurement Method**: `sonar list issues --branch <branch> --severities LOW`
- **Priority**: Could
- **Acceptance Criteria**:
  - **Given** the 114 baseline LOW issues
  - **When** SonarCloud re-analyses the branch
  - **Then** none remains OPEN
  - **And** the test suite passes with no test modified to force a pass
- **Dependencies**: RQ-QLT-016, RQ-QLT-018, RQ-TST-*

### Triage record for PLAN-QLT-003 (RQ-QLT-016)

Policy: an issue located in a **test file** is accepted, not fixed — the tests
are AI-generated and not maintained by a human. An issue in a **Python or
shell file** is accepted as-is too: those files are copied from another
repository and reviewed there. Other production-code issues are fixed, unless
the owner decides otherwise case by case (recorded below). Each SonarCloud
comment carries the task id.

- **TASK-QLT-013** — `cpp:S134` x5, `juce/tests/app/KnobPresetValuesTests.cpp`
  lines 97, 132, 236, 258, 290 (nesting deeper than 3 in test code).
- **TASK-QLT-014** — `cpp:S5213` x1, `juce/controller/src/XpanderController.cpp`
  line 299 (`restoreAllDataDumpToSynth`, `std::function` parameter). Production
  code, accepted on the owner's explicit decision: the API is blocking and
  paced by sleeps, so the type-erasure cost is nil, and a template would move
  the body into the header and add private members to a class already over the
  S1448 limit.
- **TASK-QLT-015** — `python:S3776` x5 (`extract_control_table.py:46`,
  `fit_vfd_tokens.py:253,445`, `generate_design_tokens.py:74`,
  `prototypes/triangle_lattice.py:120`) and `shelldre:S7688` x2
  (`validate-ids.sh:15`, `check_no_version_literals.sh:71`): Python and shell
  code copied from another repository, reviewed upstream.
- **TASK-QLT-016** — 17 issues in test files (`cpp:S5827` x7, `S6045` x4,
  `S5817` x2, `S7034`, `S6033`, `S5950`, `S6010` x1 each). Production code,
  accepted on the owner's decision: `cpp:S7034` x2 (`MainComponent.cpp:111`,
  `FileUtils.cpp:24`: `std::string::contains` is C++23, the project is C++20);
  `cpp:S1238` x1 (`EventDispatcher.hpp:20`: `post()` by value is the base
  class's sink signature); `cpp:S6010` x1 (`XpanderController.cpp:201`: every
  path API of the project is `std::string`); `cpp:S5950` x4
  (`XpanderParameter.cpp:48,82,107,127`: `clone()` needs the copy constructors,
  non-public by design, which `std::make_unique` cannot reach).
- **TASK-QLT-017** — 1 issue in a test file (`cpp:S1117`). Production code,
  accepted on the owner's decision: `cpp:S7040` x2 (`Dialogs.cpp:234`,
  `FileUtils.cpp:12`), `S7035` (`EnumUtils.hpp:34`) and `S6495`
  (`Logger.cpp:108`): C++23 features, or `std::format` / `std::put_time`, that
  the C++20 project and the pinned GCC 11 CI cannot use; `cpp:S6185` x3
  (`MidiMessage.cpp:33,52,139`): the MIDI layer cannot reach `formatStr`;
  `cpp:S1181` x4 (`Dialogs.cpp:549,587`, `MainComponent.cpp` "New patch" and
  "Load patch"): UI-boundary handlers that show `e.what()`; `cpp:S3230`
  (`ProgressWindow.cpp:38`): the RAII-until-handoff pattern of RQ-QLT-014;
  `cpp:S1117` (`PageFamilyBlock.cpp:105`): analyzer false positive.
- **TASK-QLT-018** — no test-file issue. Production code, accepted on the
  owner's decision: `cpp:S107` x8 (`XplorerLookAndFeel.cpp` JUCE overrides
  `drawRotarySlider`, `drawTickBox`, `drawComboBox`; `XpanderParameter.cpp`
  constructors x3 and `XpanderTone.cpp` `addUnsigned`/`addSigned` lambdas,
  which mirror the synth's parameter record); `cpp:S1448` x3
  (`XpanderController`, `AbstractController`, `XpanderTone`: splitting is an
  architecture change); `cpp:S1479` (`ModulationHighlight.cpp:174`: the
  exhaustive switch is a compile-time guarantee); `cpp:S5414`
  (`ModulationMatrixEntry.hpp`: about 130 uses of its public fields).
- **TASK-QLT-019** — 20 Python and shell issues (`shelldre:S7679` x8,
  `S7682` x7, `python:S1542` x2, `S3457` x2, `S1656`): code copied from another
  repository, reviewed upstream.
