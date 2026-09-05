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

---

## Completion against the .NET reference's own call sites (ADR-FMW-001 DEC-FMW-004)

TASK-FMW-001..GUI-066 above designed and wired the mechanism but populated only 5 call sites. The
tasks below complete it against a verified inventory of all 57 `Logger` call sites in the .NET
reference (`xplorer2716/XplorerEditor-dotnet-archive` + its `MidiApp` submodule) — see DEC-FMW-004
for the full inventory, exclusions and domain-assignment rationale. 10 reference sites are
excluded (dead code, eliminated failure modes, or no JUCE equivalent) and are not tasked below.

### TASK-FMW-003: Remaining MIDI I/O call sites in the controller's inbound event handler
- **Tier**: M
- **Status**: Done
- **Description**: In `XpanderControllerMidiEvents.cpp`, add the 5 remaining reference call sites
  (of 8; 3 already ported) at their confirmed insertion points: `isProgramChangeDownSysex`/
  `isProgramChangeUpSysex` handlers (`Info`), `isPageSubPageSelectSysex` handler (`Info`),
  `isPageEditFollowsSysex` handler (`Info`), `handleModulationEditFromSynth` (`Verbose`).
- **Requirement refs**: RQ-FMW-074, RQ-FMW-073
- **ADR refs**: ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)
- **Acceptance Criteria** (Gherkin):
  - *Given* the MIDI domain is enabled and the threshold passes, *When* the synth sends a
    program-down SysEx, *Then* a line is written containing `[MIDI]`, `Info`, and
    `ProgramChangeDOWN`.
  - *Given* the codebase, *When* `XpanderControllerMidiEvents.cpp` is searched, *Then* all 8 of the
    file's reference-inventoried call sites are present under `XPL_LOG(LogDomain::Midi, ...)`.
- **Dependencies**: None
- **Assignee**: AI

### TASK-FMW-004: Remaining Controller-calls sites in the model/controller layer
- **Tier**: M
- **Status**: Done
- **Description**: Add the remaining reference sites classified Controller calls that are live
  diagnostics: 4 in `PageSubPageHelper.cpp` (`isAuthorizedRotary`/`isLfoRetrig`/
  `isPageEnvLfoRampTrack`/`isPageLfo`, `Verbose`). **Not ported** (corrected mid-task, DEC-FMW-004):
  the reference's other 6 model-layer sites — `XpanderToneModulationMatrix.cs`'s 4-site
  `DumpModulationMatrixDestination`/`DumpModulationMatrix` and `XpanderTone.cs`'s 2-site
  `DumpModulationParameters` — are explicitly `for debug purpose` methods with **zero callers**
  anywhere in the reference, confirmed by an exhaustive search; porting them would resurrect dead
  code, not a live diagnostic.
- **Requirement refs**: RQ-FMW-075, RQ-FMW-073
- **ADR refs**: ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)
- **Acceptance Criteria** (Gherkin):
  - *Given* the Controller-calls domain is enabled and the threshold passes, *When*
    `PageSubPageHelper::isAuthorizedRotary` is called, *Then* a `Verbose` line is written tagged
    `[CONTROLLER_CALLS]`.
  - *Given* the codebase, *When* `PageSubPageHelper.cpp` is searched, *Then* all 4 of its
    reference-inventoried sites are present under `XPL_LOG(LogDomain::ControllerCalls, ...)`.
- **Dependencies**: TASK-FMW-003
- **Assignee**: AI

### TASK-FMW-005: Controller-calls sites in settings, startup and UI↔controller binding
- **Tier**: M
- **Status**: Done
- **Description**: Add the reference sites classified Controller calls that sit outside the model
  layer: `XmlSettingsService.cpp` — a `Warning` when `allUsersSettings()` falls back to defaults
  because `load()` returned `nullopt`, and a `Warning` when `Impl::save()`'s
  `settingsToXml(...)->writeTo(file)` returns `false`; `MainComponent.cpp::configureDiagnosticLogging()`
  — one `Info` line reporting the resolved severity level right after `Logger::setLevel(...)`
  (the reference's `"Logger.TraceLevel is: ..."` self-report; its sibling name/version banner line
  is already produced by `JuceFileLoggerSink`'s welcome message and is not duplicated);
  `ParameterBindingRegistry.cpp` — `Verbose` on a successful `bind()`, `Warning` when `bind()`
  fails (parameter name not found on the controller — the reference's `deviceNameError`-adjacent
  "could not find parameter" case), `Warning` around `_controller.setParameter(...)` in
  `onControlEdited()` if it throws, and `Warning` when `onParameterChanged()` finds no bound
  control for an incoming controller notification.
- **Requirement refs**: RQ-FMW-075, RQ-FMW-073, RQ-SET-004
- **ADR refs**: ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)
- **Acceptance Criteria** (Gherkin):
  - *Given* a settings file that fails to load, *When* `allUsersSettings()` falls back to defaults,
    *Then* a `Warning` line is written before the defaults are returned.
  - *Given* the Controller-calls domain is enabled, *When* `ParameterBindingRegistry::bind()` is
    called with a parameter name the controller does not recognise, *Then* a `Warning` line is
    written and `bind()` still returns `false` unchanged.
- **Dependencies**: TASK-FMW-004
- **Assignee**: AI

### TASK-GUI-067: UI-events crash handler and MIDI settings tab diagnostics
- **Tier**: M
- **Status**: Done
- **Description**: `Main.cpp::unhandledException()` — one `Error` line (RQ-FMW-076 already
  names this disposition explicitly) built from the same `e`/`sourceFile`/`lineNumber` already
  passed to the alert; no separate flush call is added (`JuceFileLoggerSink`/`FileStreamSink`
  already flush per write, DEC-FMW-004). `SettingsDialog.cpp::MidiSettingsPage` — one `Verbose`
  line summarising the enumerated synth-output/synth-input/automation-input device name lists (the
  reference's per-device capability dump has no JUCE equivalent to read from; the enumerated name
  lists are what `MidiBackend` actually exposes). `SettingsDialog.cpp::exportMappingAsHtml()` — a
  `Warning` alongside the existing `AlertWindow` when `file.replaceWithText(html)` fails. **Not
  ported** (corrected mid-task, DEC-FMW-004): the reference's `MainForm.Overrides.cs::
  CheckScreenSize`/`DumpMidiInfoToLogFile` (4 sites) — both WinForms-specific diagnostics the
  port's own window-scale system (RQ-SCL-001..005) supersedes by construction, and the latter
  depends on `BugReportFactory` (RQ-FMW-071), confirmed never ported at all; porting either would
  be separate, larger scope, not this task's.
- **Requirement refs**: RQ-FMW-076, RQ-FMW-073, RQ-GUI-035
- **ADR refs**: ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)
- **Acceptance Criteria** (Gherkin):
  - *Given* the UI-events domain is enabled, *When* `unhandledException()` fires, *Then* an `Error`
    line is written before the alert is shown.
  - *Given* the MIDI domain is enabled, *When* the Settings dialog's MIDI tab is opened, *Then* a
    `Verbose` line reports the number of synth-output, synth-input and automation-input devices
    found.
- **Dependencies**: TASK-FMW-005
- **Assignee**: AI

### TASK-CTL-021: Log and surface a device-open failure in `applyMidiSettings`
- **Tier**: M
- **Status**: Done
- **Description**: `Dialogs.cpp::applyMidiSettings()` currently discards the `bool` returned by
  `setSynthOutputDevice`/`setSynthInputDevice`/`setAutomationInputDevice` — the exact structural
  equivalent of the reference's `deviceNameError` (`SettingsManager.cs`), silent in the port today.
  Check each return value and log a `Warning` naming which device (output/input/automation) and
  configured name failed to open — three distinct messages, more diagnosable than the reference's
  single undifferentiated flag (owner decision, session LOG, DEC-FMW-004).
- **Requirement refs**: RQ-FMW-075, RQ-FMW-073, RQ-NFR-008
- **ADR refs**: ADR-FMW-001 (DEC-FMW-002, DEC-FMW-004)
- **Acceptance Criteria** (Gherkin):
  - *Given* a settings file naming a synth output device that is not currently connected, *When*
    `applyMidiSettings()` runs, *Then* a `Warning` line is written naming the output device and its
    configured name, and the application continues to start normally.
  - *Given* all three configured device names resolve successfully, *When* `applyMidiSettings()`
    runs, *Then* no device-related warning is written.
- **Dependencies**: None
- **Assignee**: AI
