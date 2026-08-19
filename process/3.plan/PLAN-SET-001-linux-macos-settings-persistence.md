# PLAN-SET-001: Linux/macOS Settings Persistence Fix

## Overview
Fixes the reported defect where settings do not persist on Linux (and structurally, likely
macOS): the per-machine settings directory (`/opt` on Linux, `/Library` on macOS) is root-owned
and unwritable by a standard user, so every write silently failed and a second, independent bug
(dereferencing an empty `std::optional`) made the resulting state read back as near-zeroed
instead of falling back to the documented defaults.

## References
- **Requirements**: RQ-SET-001 (amended), RQ-SET-004
- **ADRs**: ADR-SET-001 (DEC-SET-001)

This plan implements the tasks in the format specified below.
---

## Tasks

### TASK-SET-001: Writable-directory fallback and in-memory safety net for settings persistence
- **Tier**: L
- **Status**: Not Started
- **Description**: `XmlSettingsService` gains an optional fallback directory, used when the
  preferred (per-machine) directory cannot be created; `MainComponent` supplies the per-user
  directory as that fallback. `allUsersSettings()` no longer dereferences an empty cache when
  persistence fails on both directories — it falls back to in-memory defaults instead, per
  RQ-SET-004.
- **Requirement refs**: RQ-SET-001, RQ-SET-004
- **ADR refs**: ADR-SET-001 (DEC-SET-001)
- **Acceptance Criteria** (Gherkin):
  - Given a preferred settings directory that cannot be created (e.g. root-owned on Linux),
    and a writable fallback directory,
    When the settings service is constructed and settings are accessed,
    Then the settings file is created under the fallback directory and the documented defaults
    (sysexTransmitDelay=30, knobLedBorderColor=0xFF66B5E3, knobMovementIsLinear=true, all
    randomizer flags set) are returned and persisted there.
  - Given both the preferred and the fallback directories cannot be created,
    When settings are accessed,
    Then the documented defaults are returned from memory without crashing or reading
    uninitialized state.
  - Given a single-directory construction (existing callers: tests, no fallback supplied),
    When settings are accessed,
    Then behaviour is unchanged from before this task.
- **Dependencies**: None
- **Assignee**: AI
