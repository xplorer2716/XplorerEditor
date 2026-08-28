# PLAN-QLT-002: SonarCloud CRITICAL Remediation (Tranche 1)

## Overview

Addresses the CRITICAL-severity SonarCloud issues that are safe,
behaviour-preserving mechanical fixes or legitimate accept-with-rationale
triage decisions. First of two planned tranches — the more invasive
refactors (cognitive complexity, rule-of-five, `std::function`-to-template)
are deferred to PLAN-QLT-003.

Origin: SonarCloud Cloud-hosted scan of `xplorer2716_XplorerEditor`,
2026-08-29 — 607 open issues (75 CRITICAL, 435 MAJOR, 94 MINOR, 3 INFO).

## References

- **Requirements**: RQ-QLT-011, RQ-QLT-012, RQ-QLT-013, RQ-QLT-014,
  RQ-QLT-015, RQ-QLT-016
- **ADRs**: None — no task in this plan introduces a new public API, a new
  dependency, or a cross-cutting architectural change
- **Preserved unchanged**: ADR-BUG-002 (controller lifecycle ownership,
  RQ-QLT-011 only re-qualifies an existing call)

## Baseline (before any change)

Established 2026-08-29, same toolchain as PLAN-QLT-001.

---

## Tasks

### TASK-QLT-007: Qualify the destructor's `closeMidiDevices()` call
- **Tier**: S
- **Status**: Done
- **Description**: In `~AbstractController()`, qualify the call as
  `AbstractController::closeMidiDevices()`.
- **Requirement refs**: RQ-QLT-011
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** `AbstractController.cpp:30`
  - **When** the destructor runs
  - **Then** it calls `AbstractController::closeMidiDevices()` explicitly
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-008: Nested-comment the 8 deliberately empty methods
- **Tier**: M
- **Status**: Done
- **Description**: Add a comment inside the braces of each of the 8 flagged
  empty methods across `ProgressWindow.hpp`, `AbstractControllerDevices.cpp`
  and `XpanderParameter.hpp`, stating why the method is empty.
- **Requirement refs**: RQ-QLT-012
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 8 flagged sites
  - **When** each is read
  - **Then** its body carries a nested rationale comment
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-009: Type the lock handle and icon-data parameter meaningfully
- **Tier**: M
- **Status**: Done
- **Description**: `SingleInstanceGuard::_handle` becomes `HANDLE` (under
  `#ifdef _WIN32`, `<windows.h>` included in the header); `addReferenceItem`'s
  `iconData` parameter becomes `const char*`.
- **Requirement refs**: RQ-QLT-013
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the two sites
  - **When** the sources are searched
  - **Then** no `void*` remains at either declaration
  - **And** the GUI and framework targets compile with no new diagnostic
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-010: RAII up to the JUCE ownership handoff in SettingsDialog
- **Tier**: M
- **Status**: Done
- **Description**: The 3 settings-page constructions in `SettingsContent`'s
  constructor and the `DigitsOnlyLabel` in
  `XplorerLookAndFeel::createSliderTextBox` go through `std::make_unique`,
  released exactly at the JUCE ownership-taking call (`addTab`, `return`).
- **Requirement refs**: RQ-QLT-014
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 4 flagged sites
  - **When** the sources are searched
  - **Then** each construction is held in a smart pointer until release
  - **And** the receiving JUCE call is unchanged
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-011: Explicit lambda captures in the 6 flagged test sites
- **Tier**: M
- **Status**: Not Started
- **Description**: Replace `[&]` with a named capture list at the 6 flagged
  sites in `ParameterBindingRegistryTests.cpp` and
  `XpanderControllerTests.cpp`.
- **Requirement refs**: RQ-QLT-015
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 6 flagged lambdas
  - **When** the test suite is built and run
  - **Then** it passes unchanged with no default-reference capture remaining
    at those sites
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-012: Accept the 34 non-mechanical CRITICAL issues in SonarCloud
- **Tier**: S
- **Status**: Done
- **Description**: Mark 29 `cpp:S134` (test files), 2 `cpp:S5025`
  (`SettingsDialog.cpp:151,157`) and 3 `cpp:S5421` (`Logger.cpp:37-39`)
  issues as `accept` via the SonarCloud MCP server. No code change. Rationale
  recorded in RQ-QLT-code-quality.md (RQ-QLT-016) since the status-change
  call carries no comment field.
- **Requirement refs**: RQ-QLT-016
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 34 issue keys
  - **When** SonarCloud is queried
  - **Then** each shows status `accept`
  - **And** its rationale is recorded in RQ-QLT-code-quality.md
- **Dependencies**: None
- **Assignee**: AI

---

## Verification

Every code-touching task (007-011) is verified by the same two gates:

1. `xpl_app_core`, `xpl_framework`, `xpl_tests_app` (and any other touched
   target) build with no new diagnostic, warnings-as-errors on.
2. The full test suite passes, no pre-existing test modified except at the
   TASK-QLT-011 capture-list sites (syntax only, no assertion changed).

TASK-QLT-012 is verified directly against the SonarCloud API (no build step).
