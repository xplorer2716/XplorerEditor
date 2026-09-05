# PLAN-FMW-001: Domain-Based Diagnostic Logging

## Overview
Wires the existing, currently-inert `midiapp::service::Logger` into the running application:
severity level configurable at runtime, three independently-switchable log domains (MIDI I/O,
Controller calls, UI events), a `juce::FileLogger`-backed sink encapsulated behind a JUCE-free
interface, and a Settings-dialog tab to configure it. Implements ADR-FMW-001 in full.

## References
- **Requirements**: RQ-FMW-070, RQ-FMW-073, RQ-FMW-074, RQ-FMW-075, RQ-FMW-076, RQ-SET-008,
  RQ-GUI-083, RQ-NFR-008
- **ADRs**: ADR-FMW-001 (DEC-FMW-001, DEC-FMW-002, DEC-FMW-003), ADR-SET-001 (directory
  resolution, reused not modified)

This plan implements the tasks in the format specified below.
---

## Tasks

### TASK-FMW-001: Logger domain model and injected-sink abstraction
- **Tier**: L
- **Status**: Done
- **Description**: In `juce/framework`, add `LogDomain` (Midi/ControllerCalls/UiEvents), the three
  independent domain-enable flags, the JUCE-free `ILogSink` interface, and the new
  `writeLine(domain, level, file, line, message)` overload reached via the `XPL_LOG(domain, level,
  message)` macro. Remove the old `writeLine(source, level, message)` overload outright (DEC-FMW-002)
  and migrate its five existing callers — three in `XpanderControllerMidiEvents.cpp` (all
  `LogDomain::Midi` — the file is the controller's inbound MIDI-event handler) and two in
  `XpanderToneModulationMatrix.cpp` (`LogDomain::ControllerCalls` — internal tone-model
  diagnostics, found only once the build broke on them, not by the original grep) — plus the
  `ServicesTests.cpp` scenario that exercises it.
- **Requirement refs**: RQ-FMW-070, RQ-FMW-073
- **ADR refs**: ADR-FMW-001 (DEC-FMW-001, DEC-FMW-002)
- **Acceptance Criteria** (Gherkin):
  - *Given* the MIDI domain is disabled, *When* a line is logged under it at a severity that would
    otherwise pass, *Then* nothing is written.
  - *Given* the MIDI domain is enabled and the global threshold is Warning, *When* an Info-level
    MIDI line is logged, *Then* nothing is written (Info is less severe than Warning).
  - *Given* the MIDI domain is enabled and the threshold passes, *When* `XPL_LOG(LogDomain::Midi,
    TraceLevel::Error, "msg")` is called, *Then* the written line contains the timestamp, `[ERROR]`,
    `[MIDI]`, the calling file and line, and `msg`.
  - *Given* the codebase, *When* it is searched, *Then* no caller of the removed
    `writeLine(source, level, message)` overload remains.
- **Dependencies**: None
- **Assignee**: AI

### TASK-SET-002: Persist logging configuration in settings
- **Tier**: M
- **Status**: Done
- **Description**: Add `AllUsersSettings::LoggingConfiguration` (plain `int severityLevel`, three
  `bool` domain-enable flags, `std::string logDirectoryOverride`) with defaults matching RQ-SET-008
  (severity Off, all domains enabled, no override). Persist it as a new `<LoggingConfig>` XML
  section in `XmlSettingsService`, tolerant of absence (RQ-SET-006/007 convention: a settings file
  without the section loads the defaults, not an error). Stored as primitives, not framework types
  — `xpl_settings` does not depend on `xpl_framework` (ADR-FMW-001 DEC-FMW-003) and this task does
  not introduce that dependency.
- **Requirement refs**: RQ-SET-008
- **ADR refs**: ADR-FMW-001 (DEC-FMW-003), ADR-SET-001
- **Acceptance Criteria** (Gherkin):
  - *Given* a settings file with no `LoggingConfig` section, *When* it is loaded, *Then*
    `severityLevel` is 0 (Off), all three domain flags are `true`, and `logDirectoryOverride` is
    empty.
  - *Given* a `LoggingConfiguration` with non-default values, *When* it is saved then reloaded,
    *Then* every field round-trips exactly.
  - *Given* reset-to-defaults runs, *When* the settings are reloaded, *Then* the logging section
    reads back as the defaults above.
- **Dependencies**: None
- **Assignee**: AI

### TASK-FMW-002: Application-layer sink and startup wiring
- **Tier**: M
- **Status**: Done
- **Description**: In `juce/app`, add `JuceFileLoggerSink` (implements `ILogSink`, wraps
  `juce::FileLogger`) and wire diagnostic logging in `MainComponent`'s constructor, immediately
  after `_settingsService` is constructed and before `_controller` is constructed: resolve the log
  file path (`logDirectoryOverride` if set, else `settingsFilePath()`'s parent directory, filename
  `xplorer.log`), build the sink, and call `Logger::configure(sink)`, `setLevel(...)`,
  `setDomainEnabled(...)` (×3) from the loaded `LoggingConfiguration`.
- **Requirement refs**: RQ-FMW-070, RQ-FMW-073, RQ-SET-008, RQ-NFR-008
- **ADR refs**: ADR-FMW-001 (DEC-FMW-001, DEC-FMW-003)
- **Acceptance Criteria** (Gherkin):
  - *Given* a settings file with `severityLevel` non-zero and no directory override, *When* the
    application starts, *Then* `xplorer.log` is created next to the resolved settings file.
  - *Given* a settings file with a `logDirectoryOverride`, *When* the application starts, *Then*
    `xplorer.log` is created in that directory instead.
  - *Given* `severityLevel` is 0 (Off) — the default — *When* the application starts and any
    logging call is made, *Then* no line is written.
- **Dependencies**: TASK-FMW-001, TASK-SET-002
- **Assignee**: AI

### TASK-GUI-066: Settings dialog "Logging" tab
- **Tier**: M
- **Status**: Done
- **Description**: Add a fourth tab, "Logging", to the Settings dialog (`juce/app/src/
  SettingsDialog.{hpp,cpp}`), alongside MIDI / User interface / Randomizer: a severity selector
  (Aucun/Erreur/Avertissement/Info/Détaillé), three domain checkboxes, and a directory chooser
  (`juce::FileChooser`, directory mode, following the existing pattern in `Dialogs.cpp`) to edit
  `LoggingConfiguration`. Apply-on-accept / discard-on-cancel, matching the dialog's other tabs.
  Every visual value on the tab resolves to design-system tokens (RQ-DSN-021, RQ-DSN-061).
- **Requirement refs**: RQ-GUI-083
- **ADR refs**: ADR-FMW-001 (DEC-FMW-003), ADR-JUC-014
- **Acceptance Criteria** (Gherkin):
  - *Given* the Logging tab is open, *When* the user selects "Aucun" and accepts, *Then* the
    settings file is written with `severityLevel` 0.
  - *Given* the user picks a custom directory and accepts, *When* the application restarts, *Then*
    the log file is created in that directory.
  - *Given* the user edits controls and presses Cancel, *When* the dialog is reopened, *Then* it
    shows the previously persisted values, not the discarded edits.
- **Dependencies**: TASK-SET-002
- **Assignee**: AI
