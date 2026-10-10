# PLAN-QLT-003: SonarCloud `dev` Backlog Remediation by Severity

## Overview

Code-review session. Remediates the 324 OPEN SonarCloud issues of branch `dev`
(analysis 2026-10-09), phase by phase in decreasing severity. Fulfils the
"deferred to PLAN-QLT-003" note of PLAN-QLT-002. Non-fixable or
disproportionate items are accepted in SonarCloud with the rationale recorded
in RQ-QLT-code-quality.md (RQ-QLT-016).

## References

- **Requirements**: RQ-QLT-016, RQ-QLT-017, RQ-QLT-018, RQ-QLT-019
- **ADRs**: None — behaviour-preserving refactors; no new public API or
  dependency. A task that turns out to need one is re-tiered L and gets an ADR.

## Baseline (SonarCloud `dev`, 2026-10-09)

| Impact | Count | Dominant rules |
|---|---:|---|
| BLOCKER | 0 | — |
| HIGH | 33 | cpp:S134 (10), python:S3776 (5), cpp:S3776 (4), cpp:S5421/S5213/S3624 (3 each), cpp:S7172, shelldre:S7688 (2 each), cpp:S3656 (1) |
| MEDIUM | 177 | cpp:S6045 (22), S6022 (19), S3230 (14), S5827 (14), S6003 (10), S6185/S107/S6033 (9 each), shelldre S7679/S7682, … |
| LOW | 114 | cpp:S6177 (21), S1659 (17), S6009 (16), S6197 (11), python:S9409 (9), S6012 (8), … |

Issue list: `sonar list issues -p xplorer2716_XplorerEditor --branch dev`
(output redirected to a file and summarised — the payload is large).

---

## Phase 1 — HIGH (RQ-QLT-017)

### TASK-QLT-013: Reduce cognitive complexity and nesting in C++
- **Tier**: M
- **Status**: Not Started
- **Description**: Fix `cpp:S3776` (MainComponent.cpp:327, XpanderControllerMidiEvents.cpp:341, XpanderTone.cpp:291,455) and `cpp:S134` (VfdSegmentRenderer.cpp:165,183, XpanderController.cpp:891, XpanderControllerMidiEvents.cpp:411, AbstractControllerDevices.cpp:119) by extracting named helpers. The 5 test-file S134 (KnobPresetValuesTests.cpp) are fixed if cheap, else accepted per RQ-QLT-016.
- **Requirement refs**: RQ-QLT-017, RQ-QLT-016
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 9 production S3776/S134 sites
  - **When** SonarCloud re-analyses
  - **Then** none is OPEN
  - **And** the full test suite passes unchanged
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-014: Class-design and global-state HIGH issues
- **Tier**: M
- **Status**: Not Started
- **Description**: `cpp:S3624` (rule of five: MockMidiBackend.cpp:131,166, PianoKeyboardLayoutQuery_linux.cpp:62), `cpp:S3656` (BoundControls.hpp:25 protected member), `cpp:S5421` (Logger.cpp:35,36,43 mutable globals), `cpp:S5213` (std::function to template: ComboBoxSizing.cpp:32, PianoKeyLabelPlan.cpp:7, XpanderController.cpp:299), `cpp:S7172` (KnobPresetValues.cpp:92, PianoKeyMapping.cpp:25).
- **Requirement refs**: RQ-QLT-017, RQ-QLT-016
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 14 flagged sites
  - **When** SonarCloud re-analyses
  - **Then** each is fixed or accepted with rationale
  - **And** the full test suite passes unchanged
- **Dependencies**: None
- **Assignee**: AI

### TASK-QLT-015: Python and shell HIGH issues
- **Tier**: M
- **Status**: Not Started
- **Description**: `python:S3776` x5 (extract_control_table.py, fit_vfd_tokens.py x2, generate_design_tokens.py, prototypes/triangle_lattice.py) and `shelldre:S7688` x2 (`[` to `[[` in validate-ids.sh, check_no_version_literals.sh).
- **Requirement refs**: RQ-QLT-017
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 7 flagged sites
  - **When** the scripts are run on their existing inputs
  - **Then** outputs are identical and SonarCloud shows none OPEN
- **Dependencies**: None
- **Assignee**: AI

## Phase 2 — MEDIUM (RQ-QLT-018)

### TASK-QLT-016: Standard-library idiom modernisation (C++)
- **Tier**: M
- **Status**: Not Started
- **Description**: `cpp:S6045` transparent comparators, `S6033`/`S6030`/`S6003` emplace family, `S7034` contains, `S5950` make_unique, `S5827` auto, `S6010` std::filesystem::path, `S5817` const methods, `S1238` pass-by-const-ref.
- **Requirement refs**: RQ-QLT-018
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the flagged sites
  - **When** built warnings-as-errors and tested
  - **Then** the suite passes unchanged and the rules report none OPEN
- **Dependencies**: TASK-QLT-013, TASK-QLT-014
- **Assignee**: AI

### TASK-QLT-017: Type-safety and formatting (C++)
- **Tier**: M
- **Status**: Not Started
- **Description**: `cpp:S6022` std::byte (19), `S3230` in-class initialisers (14), `S6185` std::format (9), `S7040` bounded escapes, `S6495`, `S7035` to_underlying, `S1117` shadowing, `S1181`/`S112` exception specificity, `S3806` include case.
- **Requirement refs**: RQ-QLT-018
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the flagged sites
  - **When** built and tested
  - **Then** behaviour is unchanged and the rules report none OPEN
- **Dependencies**: TASK-QLT-016
- **Assignee**: AI

### TASK-QLT-018: Size and shape smells — fix or accept
- **Tier**: M
- **Status**: Not Started
- **Description**: `cpp:S107` (10 params), `S1448` (83 methods), `S1479` (47 cases), `S1820` (21 fields), `S5414`, `S1188`, `S3358`, `S924`, `S1066`, `S4144`. Fix where proportionate; otherwise accept with rationale per RQ-QLT-016.
- **Requirement refs**: RQ-QLT-018, RQ-QLT-016
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** each flagged site
  - **When** reviewed
  - **Then** it is fixed or accepted with a recorded rationale
- **Dependencies**: TASK-QLT-017
- **Assignee**: AI

### TASK-QLT-019: Python and shell MEDIUM issues
- **Tier**: S
- **Status**: Not Started
- **Description**: `shelldre:S7679` x8, `S7682` x7, `python:S1542` x2, `S3457` x2, `S1656`.
- **Requirement refs**: RQ-QLT-018
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the 20 flagged sites
  - **When** the scripts are run on existing inputs
  - **Then** outputs are identical and none is OPEN
- **Dependencies**: TASK-QLT-015
- **Assignee**: AI

## Phase 3 — LOW (RQ-QLT-019)

### TASK-QLT-020: C++ LOW issues
- **Tier**: M
- **Status**: Not Started
- **Description**: All `cpp:` LOW issues (S6177 using enum, S1659, S6009 string_view, S6197, S6012, S6004, S5408, S2486, S5951, S5350, S5566, S6171, S3574, S3628, S6179).
- **Requirement refs**: RQ-QLT-019
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the flagged sites
  - **When** built and tested
  - **Then** the suite passes unchanged and none is OPEN
- **Dependencies**: TASK-QLT-018
- **Assignee**: AI

### TASK-QLT-021: Python and shell LOW issues
- **Tier**: S
- **Status**: Not Started
- **Description**: `python:S9409` x9, `S6353` x4, `S7498` x3, `S1481`, `S7494`; `shelldre:S1192`.
- **Requirement refs**: RQ-QLT-019
- **ADR refs**: None
- **Acceptance Criteria**:
  - **Given** the flagged sites
  - **When** the scripts are run on existing inputs
  - **Then** outputs are identical and none is OPEN
- **Dependencies**: TASK-QLT-019
- **Assignee**: AI

---

## Verification

1. Touched C++ targets build with no new diagnostic, warnings-as-errors on.
2. Full test suite passes; no test modified except syntax-only (e.g. S134 helper extraction in test files), no assertion changed.
3. After merge to `dev`, a new SonarCloud analysis confirms the phase's severity count; each task is committed separately (`commit-task`).
