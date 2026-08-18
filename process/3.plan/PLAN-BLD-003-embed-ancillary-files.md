# PLAN-BLD-003: Embed the Default Patch and the SBOM as Binary Data

## Overview
Ends the "three loose siblings" deployment shape: `oberheim.syx` and the generated
`xplorer.sbom.spdx.json` move from POST_BUILD-copied files beside the executable to
`BinaryData` compiled into it, so each platform's deployment is one self-contained,
tamper-resistant binary. Owner request, session BLD.

## References
- **Requirements**: RQ-BLD-014 (amended), RQ-BLD-021 (amended), RQ-BLD-022 (amended),
  RQ-GUI-008, RQ-GUI-057
- **ADRs**: ADR-BLD-005 (new; supersedes DEC-JUC-100, DEC-ABT-002, DEC-ABT-007;
  narrows DEC-BLD-018, DEC-BLD-021)

---

## Tasks

### TASK-BLD-015: Embed `oberheim.syx`; resolve it via a fresh temp-file bounce
- **Tier**: M
- **Status**: Done (2026-08-18)
- **Description**: Add `juce/oberheim.syx` to `XplorerAssets`' binary-data sources;
  remove its `POST_BUILD` copy step; rewrite `MainComponent.cpp`'s `defaultToneFile()`
  to write `BinaryData::oberheim_syx` to a freshly-overwritten OS-temp-dir file on
  every call and return that path, keeping the File > New call site and its error
  handling untouched.
- **Requirement refs**: RQ-BLD-014, RQ-BLD-021, RQ-GUI-008
- **ADR refs**: ADR-BLD-005 (DEC-BLD-027, DEC-BLD-028)
- **Acceptance Criteria** (Gherkin):
  - **Given** `juce/app/CMakeLists.txt`, **When** it is read, **Then** `oberheim.syx`
    is a `juce_add_binary_data` source and no `POST_BUILD` step copies it anywhere
  - **Given** a built application with no `oberheim.syx` beside the executable,
    **When** File > New is invoked, **Then** the bundled default patch loads
    successfully
  - **Given** File > New invoked twice in the same run, **When** the temp copy is
    inspected between the two, **Then** it is rewritten from the embedded bytes each
    time, not reused from a cached path
- **Dependencies**: None
- **Assignee**: AI

### TASK-BLD-016: Embed the SBOM; read it from memory, no file involved
- **Tier**: M
- **Status**: Done (2026-08-18)
- **Description**: Move the SBOM's `configure_file` step before `juce_add_binary_data`
  and add its generated path as a source; remove its `POST_BUILD` copy step; refactor
  `SbomReader::readSbom` to take a `juce::String` instead of a `juce::File`, add
  `readEmbeddedSbom()`, remove `defaultSbomFile()` and the now-unreachable
  `FileNotFound`/`Unreadable` statuses; update `Dialogs.cpp`'s call site and
  `explanationFor` accordingly; simplify `SbomReaderTests.cpp` off the
  temp-file-writing fixture helper and add a scenario exercising the real embedded
  document end-to-end.
- **Requirement refs**: RQ-BLD-014, RQ-BLD-021, RQ-BLD-022, RQ-GUI-057
- **ADR refs**: ADR-BLD-005 (DEC-BLD-027, DEC-BLD-028)
- **Acceptance Criteria** (Gherkin):
  - **Given** `juce/app/CMakeLists.txt`, **When** it is read, **Then** the SBOM's
    `configure_file` call precedes `juce_add_binary_data`, the generated file is one
    of its sources, and no `POST_BUILD` step copies it anywhere
  - **Given** a built application with no SBOM beside the executable, **When**
    About > Dependencies is opened, **Then** the real curated component list is shown
  - **Given** `SbomReaderTests.cpp`, **When** it is read, **Then** every scenario
    calls `readSbom` with an in-memory string and at least one scenario asserts
    `readEmbeddedSbom()` returns `Loaded`
- **Dependencies**: None
- **Assignee**: AI

### TASK-BLD-017: Drop the sibling-file checks from CI build and packaging
- **Tier**: M
- **Status**: Done (2026-08-18)
- **Description**: Remove the `locate` step's `oberheim.syx`/`xplorer.sbom.spdx.json`
  existence check in `build-app/action.yml`, and every `require`/`cp` line for either
  file across the Windows, macOS and Linux branches of `package-deployment/action.yml`
  (including the Linux AppDir staging and the macOS bundle checks); update both files'
  header comments to state each archive/bundle/AppImage now carries the executable
  alone.
- **Requirement refs**: RQ-BLD-021
- **ADR refs**: ADR-BLD-005 (DEC-BLD-027)
- **Acceptance Criteria** (Gherkin):
  - **Given** `build-app/action.yml`'s `locate` step, **When** it is read, **Then** it
    checks only for the executable/bundle, not for either ancillary file
  - **Given** `package-deployment/action.yml`, **When** it is read for all three
    OS branches, **Then** no `require` or `cp`/staging line names `oberheim.syx` or
    `xplorer.sbom.spdx.json`
- **Dependencies**: TASK-BLD-015, TASK-BLD-016
- **Assignee**: AI

---

## Note on testing
TASK-BLD-015's runtime change lives in `MainComponent.cpp`'s anonymous namespace —
private, unexported, and (like the rest of this file's menu-wiring) not covered by
existing unit tests; it was verified manually rather than by a new automated test,
consistent with how the rest of `MainComponent.cpp` is already tested. TASK-BLD-016's
change lands entirely in `SbomReader`'s public, already-extensively-unit-tested
surface, so it gets full automated coverage plus the same manual pass. TASK-BLD-017 is
CI/YAML with no unit-testable surface; verified by reading the edited scripts, proven
for real by the next CI run producing single-item archives (same standard PLAN-BLD-002
used for the parallel build-command fix).

**Manual pass actually performed** (not merely planned): built Debug with
`XPL_BUILD_APP=ON`, confirmed the artefact directory holds only `Xplorer` (no
`oberheim.syx`/`xplorer.sbom.spdx.json` siblings), launched it under Xvfb, and drove it
with real clicks. File > New loaded the embedded default patch (VFD read `OBERHEIM`,
matching panel state changed from the prior idle state, no error dialog) — TASK-BLD-015
confirmed. Help > About > Dependencies opened the real curated list (Catch2 3.9.1,
JUCE 8.0.9, Roboto Condensed 3.008, each with licence and URL) — TASK-BLD-016
confirmed. `ctest`: 161/161 passed (1 pre-existing, unrelated MIDI-loopback skip); an
added `-Wshadow` probe on all four touched files found one pre-existing hit unrelated
to this change (`MainComponent.cpp`'s `ShortcutButton::paintButton`, untouched here)
and nothing in the new code.

## Out of scope
Refactoring `IToneReader`/`IToneWriter` to accept in-memory data — considered and
rejected in ADR-BLD-005's Alternatives (cross-cutting change to a stable, heavily-used
port, for a cost the temp-file bounce already pays in three lines). Changing whether
deployments are archived at all now that each holds one item — not asked, and a
separate scope (asset-naming consistency across fifteen workflows).
