# PLAN-BLD-004: macOS Debug Canary Launch Screenshot

## Overview
Add a launch-and-screenshot smoke test to the `macos-arm64-debug-canary`
workflow only: build the app as today, then launch it on the runner and
capture a screenshot to prove it actually opens a window, failing the job if
it crashed first. No other workflow (other macOS streams, Windows, Linux) is
touched.

## References
- **Requirements**: RQ-BLD-032 (new), RQ-BLD-033 (new — portability defect found while executing TASK-BLD-019)
- **ADRs**: ADR-BLD-006 (DEC-BLD-029, DEC-BLD-030). None for RQ-BLD-033 — a
  two-line, directly-root-caused encoding fix, Tier S.

This plan implements the tasks in the format specified below.
---

## Tasks

### TASK-BLD-018: `screenshot-macos-app` composite action
- **Tier**: M
- **Status**: Done (2026-09-12) — YAML validated; runtime behaviour provable only on a real macOS runner (see Note on testing below).
- **Description**: New composite action `.github/actions/screenshot-macos-app/action.yml`, mirroring the owner-supplied working reference (XplorerChat's `macos-ui-check` job). Given `build-app`'s `artefact-dir` output, derives the `.app` bundle path (two levels up from `Contents/MacOS`), `open`s it, sleeps 5s, verifies with `pgrep -x Xplorer` that the process is running, runs `screencapture -x` to `$RUNNER_TEMP`, always `pkill -x Xplorer`s to clean up, and outputs the screenshot's path.
- **Requirement refs**: RQ-BLD-032
- **ADR refs**: ADR-BLD-006 (DEC-BLD-029, DEC-BLD-030)
- **Acceptance Criteria** (Gherkin):
  - **Given** a valid `artefact-dir` containing a working `Xplorer` app, **When** the action runs, **Then** it outputs `screenshot-path` pointing at a PNG file that exists.
  - **Given** an `Xplorer` app that crashes immediately after `open`, **When** the action runs, **Then** the step fails at the `pgrep -x Xplorer` check instead of producing a blank or missing screenshot.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-BLD-019: Generate the step for macOS/Debug/canary only, with a unit test
- **Tier**: M
- **Status**: Not Started
- **Description**: Extend `juce/tools/generate_workflows.py`'s `workflow()` function so the generated body appends a call to `screenshot-macos-app` plus an `upload-artifact` step for its output, gated by `os_name == "macos" and config == "debug" and stage == "canary"` — the same conditional-augmentation style already used there for permissions. Add `juce/tools/test_generate_workflows.py` (stdlib `unittest`, no new dependency) asserting the step is present for that one combination and absent from every other platform/config/stream combination the matrix produces. Regenerate `.github/workflows/` and commit the result.
- **Requirement refs**: RQ-BLD-032
- **ADR refs**: ADR-BLD-006 (DEC-BLD-029, DEC-BLD-030)
- **Acceptance Criteria** (Gherkin):
  - **Given** `generate_workflows.py`'s output, **When** `macos-arm64-debug-canary.yml` is read, **Then** it calls `screenshot-macos-app` and uploads its output as `Xplorer-<version>-macos-arm64-debug-screenshot`.
  - **Given** the same generator output, **When** any other generated workflow file is read, **Then** it contains no reference to `screenshot-macos-app`.
  - **Given** `test_generate_workflows.py`, **When** run via `python3 -m unittest`, **Then** it passes and fails if the scope guard regresses (verified by temporarily broadening the guard and observing the test catch it).
  - **Given** `python3 juce/tools/generate_workflows.py --check`, **When** run after committing the regenerated files, **Then** it reports all workflows up to date.
- **Dependencies**: TASK-BLD-018
- **Assignee**: AI

---

### TASK-BLD-020: Fix non-portable text encoding in the workflow generator
- **Tier**: S
- **Status**: Done (2026-09-12) — verified: `generate_workflows.py` then `--check` on `session.platform = windows` reports all 15 up to date; confirmed the fix touches only encoding/newline handling (`git diff --stat` showed zero content drift on the 14 files outside this task's own scope).
- **Description**: `generate_workflows.py`'s `main()` reads and writes the generated `.yml` files with no explicit encoding, so it uses the platform's default text encoding. On `session.platform = windows` this is `cp1252`, not UTF-8, which mis-decodes/mis-encodes the generated header's em dash characters — found while regenerating for TASK-BLD-019, when `--check` reported all 15 files stale with no matrix change. Fix: pass `encoding="utf-8"` explicitly to both `target.read_text()` and `target.write_text(body)`.
- **Requirement refs**: RQ-BLD-033
- **ADR refs**: None (Tier S — directly root-caused, two-line fix)
- **Acceptance Criteria** (Gherkin):
  - **Given** `session.platform = windows`, **When** `generate_workflows.py` is run and then `generate_workflows.py --check` immediately after, **Then** `--check` reports all workflows up to date.
- **Dependencies**: None
- **Assignee**: AI

---

## Note on testing
Both tasks are CI/build tooling (Python + composite actions), not one of the
C++ layers RQ-TST scopes to CTest — `test_generate_workflows.py` covers the
generator logic (a pure string-templating function, fully exercisable
locally). The composite action's actual runtime behaviour (does `Xplorer`
really paint a window on a live macOS runner) can only be proven by the next
real `macos-arm64-debug-canary` run — same limitation PLAN-BLD-002 recorded
for its own runner-only defect.
