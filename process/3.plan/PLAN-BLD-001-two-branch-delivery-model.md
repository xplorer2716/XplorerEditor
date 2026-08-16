# PLAN-BLD-001: Two-Branch Delivery Model

## Overview
Move the project from one long-lived branch publishing alpha pre-releases to `main` (production)
+ `dev` (pre-production, default) + `feature/*` (canary, no deployment), with a version derived
from the commit and deployments that actually contain what a user needs to run the application.

## References
- **Requirements**: RQ-BLD-019 … RQ-BLD-028; reworks RQ-BLD-015/016; supersedes RQ-BLD-009/010/013/017/018
- **ADRs**: ADR-BLD-003 (branching, versioning, streams — DEC-BLD-013..017), ADR-BLD-004 (packaging — DEC-BLD-018..023)
- **Referenced, not amended**: ADR-BLD-001, ADR-BLD-002, ADR-ABT-001, ADR-GOV-001, ADR-GUI-001

**Nothing under `.github/workflows/` is touched before the owner approves this plan.**

---

## Tasks

### TASK-BLD-001: Create `dev`, make it the default, protect `main`, teach the process
- **Tier**: M
- **Status**: Not Started
- **Description**: Create `dev` from `main`; set it as the repository default; add an explicit
  protection rule on `main`; update the `agnos-git-workflow` skill and `ADR-GOV-001`'s RQ-GIT rules
  so session branches are cut from and merged into `dev`.
- **Requirement refs**: RQ-BLD-019
- **ADR refs**: ADR-BLD-003 (DEC-BLD-013); ADR-GOV-001 (amended)
- **Acceptance Criteria** (Gherkin):
  - **Given** the repository settings, **When** the default branch is read, **Then** it is `dev`
  - **Given** the protection rules, **When** they are read, **Then** one names `main` explicitly
  - **Given** the `agnos-git-workflow` skill, **When** `start-session` is read, **Then** it cuts
    `feature/<TRI>` from `dev`, and its "never commit to the default branch" rule now protects both
- **Dependencies**: None
- **Assignee**: Human (repository settings) + AI (skill and process files)

### TASK-BLD-002: `resolve-version` composite action
- **Tier**: M
- **Status**: Not Started
- **Description**: `.github/actions/resolve-version` — derives the three version forms from HEAD's
  UTC committer date and the stage from `github.ref`, and emits them as step outputs. The single
  place a version is computed.
- **Requirement refs**: RQ-BLD-020, RQ-BLD-015
- **ADR refs**: ADR-BLD-003 (DEC-BLD-014, DEC-BLD-015)
- **Acceptance Criteria** (Gherkin):
  - **Given** a commit whose UTC committer time is 2026-08-19 17:40, **When** the action runs on
    `dev`, **Then** it outputs `2026.8.19.1740`, `2026.08.19-1740` and `2026.08.19-1740-preprod`
  - **Given** the same commit on `main` and on `feature/x`, **When** the action runs, **Then** the
    suffix is absent and `-canary` respectively
  - **Given** the action run twice on one commit, **When** the outputs are compared, **Then** they
    are identical
  - **Given** the action's source, **When** searched for `run_number`, a repository variable or a
    wall-clock date, **Then** there is no match
- **Verification**: a shell test fixture builds a throwaway git repository with fixed committer
  dates in several timezones and asserts every output — this logic is what fifteen workflows agree
  through, so it is tested rather than observed.
- **Dependencies**: None
- **Assignee**: AI

### TASK-BLD-003: Single version derivation, injected everywhere
- **Tier**: M
- **Status**: Not Started
- **Description**: CMake takes the numeric form on the configure line and feeds the JUCE target,
  the Windows version resources, the macOS `Info.plist` and the About dialog; the full form reaches
  the About text and the `ProductVersion` string. Removes the four `0.1.0` literals
  (`juce/CMakeLists.txt`, `juce/app/CMakeLists.txt`, `Main.cpp:37`, `MainComponent.cpp:963`).
- **Requirement refs**: RQ-BLD-015, RQ-BLD-016, RQ-BLD-020
- **ADR refs**: ADR-BLD-003 (DEC-BLD-015)
- **Acceptance Criteria** (Gherkin):
  - **Given** a build configured with a version, **When** the About dialog is opened, **Then** it
    reads the product name followed by that build's full version
  - **Given** the repository, **When** it is searched for a version-shaped literal outside generated
    files, **Then** there is no match
  - **Given** the Windows executable, **When** its properties are read, **Then** the numeric fields
    carry the numeric form and the product-version string carries the full form
  - **Given** a default local build with no version supplied, **When** it runs, **Then** it still
    builds and reports a clearly non-deployment version
- **Verification**: a unit test asserts the About string equals the compile-time definition, so the
  RQ-GUI-025 literal cannot come back.
- **Dependencies**: TASK-BLD-002
- **Assignee**: AI

### TASK-BLD-004: Application icon on all three platforms
- **Tier**: S
- **Status**: Not Started
- **Description**: `ICON_BIG`/`ICON_SMALL` on the JUCE target from `xdata/IconeXplorer/Hazard_256.png`
  and `Hazard_32.png`; the same PNG feeds the AppImage desktop entry in TASK-BLD-006.
- **Requirement refs**: RQ-BLD-026
- **ADR refs**: ADR-BLD-004 (DEC-BLD-023); ADR-GUI-001 (referenced)
- **Acceptance Criteria** (Gherkin):
  - **Given** the built Windows executable, **When** it is viewed in a file manager, **Then** it
    shows the Xplorer icon, not the generic one
  - **Given** the macOS bundle, **When** it is viewed in Finder, **Then** it shows the same icon
  - **Given** the repository, **When** searched for a duplicated copy of the artwork, **Then** none
- **Dependencies**: None
- **Assignee**: AI

### TASK-BLD-005: Static C runtime on Windows
- **Tier**: S
- **Status**: Not Started
- **Description**: `set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")` before
  JUCE is made available, so every target agrees.
- **Requirement refs**: RQ-BLD-024
- **ADR refs**: ADR-BLD-004 (DEC-BLD-020)
- **Acceptance Criteria** (Gherkin):
  - **Given** a Windows Debug build, **When** the executable's imports are listed, **Then** no
    debug C-runtime DLL appears
  - **Given** the same binary on a machine without Visual Studio, **When** it is launched, **Then**
    it starts
  - **Given** the CMake tree, **When** the policy is read, **Then** it is set once, ahead of JUCE
- **Dependencies**: None
- **Assignee**: AI

### TASK-BLD-006: Linux x64 GUI build and AppImage packaging
- **Tier**: L
- **Status**: Not Started
- **Description**: New GUI build for `linux-x64` on a pinned Ubuntu image with the X11/FreeType/
  Fontconfig/ALSA/GL development packages, packaged as an AppImage with a `.desktop` entry and the
  TASK-BLD-004 icon.
- **Requirement refs**: RQ-BLD-025, RQ-BLD-026
- **ADR refs**: ADR-BLD-004 (DEC-BLD-021)
- **Acceptance Criteria** (Gherkin):
  - **Given** the Linux workflow, **When** it completes, **Then** an AppImage is produced
  - **Given** that AppImage on a supported distribution, **When** it is made executable and run,
    **Then** the application window opens
  - **Given** a distribution without FUSE 2, **When** the documented fallback is used, **Then** it
    still runs
  - **Given** the Linux workflows, **When** the runner image is read, **Then** it is pinned
- **Dependencies**: TASK-BLD-004
- **Assignee**: AI

### TASK-BLD-007: Generate the SBOM at build time
- **Tier**: M
- **Status**: Not Started
- **Description**: Replace the committed `juce/app/sbom/xplorer.sbom.spdx.json` with a build step
  that emits it, carrying the full version and the pinned dependency versions CMake already knows.
- **Requirement refs**: RQ-BLD-022, RQ-BLD-014
- **ADR refs**: ADR-BLD-004 (DEC-BLD-019); ADR-ABT-001 (refined)
- **Acceptance Criteria** (Gherkin):
  - **Given** a deployment build, **When** the SBOM is opened, **Then** its describing package
    version equals the built binary's full version
  - **Given** the generated file, **When** its package list is read, **Then** JUCE appears with the
    pinned tag
  - **Given** the application built from it, **When** its dependency window is opened, **Then** it
    reads the file with no code change
  - **Given** the repository, **When** searched for a committed SBOM used as a build input,
    **Then** there is none
- **Dependencies**: TASK-BLD-003
- **Assignee**: AI

### TASK-BLD-008: `package-deployment` composite action
- **Tier**: M
- **Status**: Not Started
- **Description**: Assembles one archive per platform/configuration — executable or bundle,
  `oberheim.syx`, generated SBOM — named for version, os, architecture and configuration. macOS
  keeps `ditto`.
- **Requirement refs**: RQ-BLD-021
- **ADR refs**: ADR-BLD-004 (DEC-BLD-018); ADR-BLD-002 (referenced)
- **Acceptance Criteria** (Gherkin):
  - **Given** any produced archive, **When** it is expanded, **Then** it holds the executable,
    `oberheim.syx` and the SBOM
  - **Given** the six archives of a `dev` deployment, **When** their names are compared, **Then**
    no two collide and each states its version, os, architecture and configuration
- **Dependencies**: TASK-BLD-006, TASK-BLD-007
- **Assignee**: AI

### TASK-BLD-009: `publish-deployment` composite action, with the unsigned-binaries notice
- **Tier**: M
- **Status**: Not Started
- **Description**: Creates or reuses the stream's release — pre-release for `dev`, production plus
  Latest for `main` — attaches the archive idempotently, and generates the notes: change summary,
  the "not a production deployment" statement for `dev`, the promoted pre-release for `main`, and
  the three-platform first-launch notice from one shared source. Replaces
  `.github/actions/alpha-prerelease`.
- **Requirement refs**: RQ-BLD-019, RQ-BLD-027
- **ADR refs**: ADR-BLD-003 (DEC-BLD-013, DEC-BLD-017), ADR-BLD-004 (DEC-BLD-022)
- **Acceptance Criteria** (Gherkin):
  - **Given** two workflows publishing at the same moment, **When** both run, **Then** one creates
    the release, the other adds its asset, and neither fails
  - **Given** a `dev` deployment, **When** its page is read, **Then** it is a pre-release, says so
    in its notes, and does not hold the Latest badge
  - **Given** any published deployment, **When** its notes are read, **Then** the first-launch
    notice covers Windows, macOS and Linux
  - **Given** the macOS instruction, **When** it is read, **Then** it names the System Settings
    route and not the Control-click bypass macOS 15 removed
- **Dependencies**: TASK-BLD-002, TASK-BLD-008
- **Assignee**: AI

### TASK-BLD-010: The fifteen workflows, the cut action, and retiring the old four
- **Tier**: L
- **Status**: Not Started
- **Description**: `<os>-<arch>-<config>-<stage>` × 15, one job each, delegating to the composite
  actions; `cut-deployment` (`workflow_dispatch` on `main`) derives the version and pushes the tag;
  the four `*-app-*` workflows are removed and `linux-headless-release` kept as-is with a note
  saying why it is outside the scheme.
- **Requirement refs**: RQ-BLD-023, RQ-BLD-028, RQ-BLD-019
- **ADR refs**: ADR-BLD-003 (DEC-BLD-016, DEC-BLD-017)
- **Acceptance Criteria** (Gherkin):
  - **Given** any deployment workflow, **When** it is read, **Then** its file stem, `name:` and job
    key are the same `<os>-<arch>-<config>-<stage>` string
  - **Given** a push to `main` with no tag, **When** its workflows complete, **Then** no deployment
    was published
  - **Given** `cut-deployment` run on `main`, **When** it completes, **Then** a tag equal to the
    display version exists and the three production workflows have run
  - **Given** a push to a `feature/*` branch, **When** its workflows complete, **Then** no release
    and no tag exist and the binaries are downloadable only from the run
  - **Given** the fifteen files, **When** their build steps are compared, **Then** the shared logic
    appears once, in composite actions
- **Dependencies**: TASK-BLD-001, TASK-BLD-009
- **Assignee**: AI

---

## Sequencing

```
TASK-BLD-001 ─────────────────────────────────────────────┐
TASK-BLD-002 ──┬── TASK-BLD-003 ── TASK-BLD-007 ──┐        │
               │                                   ├─ 008 ─┼─ 009 ─ TASK-BLD-010
TASK-BLD-004 ──┴── TASK-BLD-006 ───────────────────┘        │
TASK-BLD-005 ──────────────────────────────────────────────┘
```

TASK-BLD-001 is the only one needing a human: repository settings cannot be changed from CI.
TASK-BLD-004 and TASK-BLD-005 are independent and can land first as small, self-contained fixes —
TASK-BLD-005 in particular repairs a defect that is live today.

## Out of scope
Code signing and notarisation (DEC-BLD-022 — owner declined the cost). `linux-arm64`,
`windows-arm64` and `macos-x64`. Automatic SBOM discovery (DEC-BLD-019 records why it is not
achievable for a `FetchContent` C++ project today). Any change to how the application is compiled
on Windows or macOS beyond the runtime-library policy.

## Verification note
The version derivation is the only genuinely testable unit here and it carries the whole scheme, so
it gets a shell fixture with pinned committer dates (TASK-BLD-002) and the About-box injection gets
a unit test (TASK-BLD-003). Everything else is CI behaviour and is verified by running it: a
deployment is only proven by downloading its archive and starting what is inside — which is exactly
the check that was skipped for RQ-GUI-070 and cost a defect the owner had to report.
