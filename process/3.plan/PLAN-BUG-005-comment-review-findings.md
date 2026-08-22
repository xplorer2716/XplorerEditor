# PLAN-BUG-005: Defects Found During the Comment Review

## Overview

Three defects surfaced while reading the code for the 2026-08-22 comment pass
(commit `42972f2`), each recorded there as a `TODO` marker. This plan removes
all three and the markers with them. A fourth finding from the same pass — the
logger being inert — is out of scope here and stays with GitHub issue #68,
because closing it needs a design decision this plan does not make.

None of the three is a behaviour a user could observe today. Two are latent
traps that would have produced a hard-to-diagnose defect later; the third is
visible text.

## References
- **Requirements**: RQ-BUG-004 (settings accessor), RQ-BUG-005 (mock port
  lifetime), RQ-BUG-006 (message wording). Related: RQ-SET-001, RQ-SET-005,
  RQ-MID-002, RQ-MID-003, RQ-MID-005, RQ-MID-041, RQ-CTL-004, RQ-TST-004
- **ADRs**: ADR-BUG-003 (DEC-BUG-010 … DEC-BUG-012)

## Baseline (before any change)
Branch `feature/BUG` at `42972f2`, this toolchain, 2026-08-22:

- `ISettingsService::allUsersSettings()` returns a non-const reference into the
  implementation's cache; `XpanderController::settings()` forwards it. Survey of
  every call site: **23 total (11 production, 12 test), all read-only.**
- `MockMidiBackend::State::deliver`/`deliverError` snapshot
  `std::vector<MockInputPort*>` under the mutex and dereference it after
  releasing it.
- `XpanderController::getSingleTonesFromSynth` throws
  `"Destionation folder … does not exists."`
- Test suite: 122/122 pass.

---

## Tasks

### TASK-BUG-007: Make the settings read accessor const
- **Tier**: L
- **Status**: Done (2026-08-22)
- **Description**: Change `ISettingsService::allUsersSettings()` and both
  implementations to return `const AllUsersSettings&`, and
  `XpanderController::settings()` with them. Document the copy → edit →
  `saveSettings()` protocol on the interface declaration.
- **Requirement refs**: RQ-BUG-004
- **ADR refs**: ADR-BUG-003 (DEC-BUG-010, DEC-BUG-011, DEC-BUG-012)
- **Acceptance Criteria** (Gherkin):
  - **Given** the settings interface, **When** it is read, **Then** the read
    accessor returns a const reference
  - **Given** the whole tree, **When** it is built, **Then** every existing
    call site compiles unchanged
  - **Given** a hypothetical caller assigning through the accessor, **When** it
    is compiled, **Then** it is rejected
- **Dependencies**: None
- **Assignee**: AI

### TASK-BUG-008: Give mock MIDI delivery shared ownership of its targets
- **Tier**: M
- **Status**: Done (2026-08-22)
- **Description**: Split `MockInputPort` into a caller-owned handle and a
  `shared_ptr`-held `InputPortBody`. The registry keeps `weak_ptr`s; delivery
  upgrades them and holds the resulting `shared_ptr`s for the duration of the
  callback. Output ports are left on raw pointers — they are only counted under
  the mutex, never dereferenced outside it.
- **Requirement refs**: RQ-BUG-005
- **ADR refs**: None — no architectural choice, the port/adapter shape is
  unchanged (ADR-JUC-004 still governs it)
- **Acceptance Criteria** (Gherkin):
  - **Given** the delivery path, **When** it is read, **Then** it holds shared
    ownership of every body it calls, and no lock while calling
  - **Given** a closed port, **When** a message is injected for its device,
    **Then** nothing is delivered to it
  - **Given** a hot-swapped device, **When** `openInputPortCount` is queried,
    **Then** it reports live handles only, not registry slots
  - **Given** the MIDI suites, **When** they run, **Then** they pass unchanged
- **Dependencies**: None
- **Assignee**: AI

### TASK-BUG-009: Correct the extract-single-patches error message
- **Tier**: S
- **Status**: Done (2026-08-22)
- **Description**: `"Destionation folder … does not exists."` →
  `"Destination folder … does not exist."` A systematic sweep of user-facing
  string literals for the same class of error found one further instance in
  `extractSinglePatchesFromAllDataDumpFileToDirectory` —
  `"File or directory does not exists."` → `"… does not exist."` — and nothing
  else. Both are corrected; the sweep is what makes "and nothing else" a
  statement rather than an assumption.
- **Requirement refs**: RQ-BUG-006
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** an invalid destination folder, **When** extraction is attempted,
    **Then** the message is spelled correctly
  - **Given** every user-facing string literal, **When** they are searched for
    this class of error, **Then** no match remains
- **Dependencies**: None
- **Assignee**: AI

---

## Verification
Run on this toolchain, 2026-08-22:

1. Every headless target and `XplorerApp` (`-DXPL_BUILD_APP=ON`) build with
   warnings-as-errors (`-Wall -Wextra -Wpedantic -Werror`) — no new diagnostic.
   For TASK-BUG-007 the build **is** the acceptance test: a const tightening
   either compiles everywhere or names its violators.
2. `ctest` reports **122/122 pass**, 1 skipped as always. No pre-existing test
   modified.
3. `grep -rn "TODO:" juce/` no longer matches any of the three findings; the
   two logger markers remain, deliberately.

## Note on testing
No test is added, and the reason differs per task.

**TASK-BUG-007**: the property is "this does not compile", which the compiler
checks on every build across all three CI platforms. A runtime test cannot
express it.

**TASK-BUG-008**: the defect is a data race between a delivery in flight and a
port released on another thread. A deterministic unit test cannot schedule that
window — a test that passed would prove nothing about the ordering it did not
hit. The guarantee here is **structural** instead: the snapshot holds
`shared_ptr`s, so the body it will call cannot be freed, whatever the
interleaving. The existing MIDI suites (68 assertions) cover the observable
contract — closed ports deliver nothing, hot-swap releases handles — and pass
unchanged.

**TASK-BUG-009**: a string literal, verified by reading it.

## Out of scope
- The inert logger (GitHub issue #68). It needs two decisions — where the log
  file lives and how the level is configured — and therefore its own
  requirement and ADR, not a line in this plan.
