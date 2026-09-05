#pragma once

// Logic behind wiring the diagnostic logger from persisted configuration:
// where the log file goes, what severity it resolves to, and applying that
// configuration to the running Logger. JUCE-free and OS-free so it stays
// headless-testable (RQ-BLD-025) -- the same split KnobPresetValues/
// PianoKeyMapping already use for their own UI-adjacent logic.
// [RQ-FMW-070, RQ-SET-008, ADR-FMW-001 (DEC-FMW-003)]

#include "midiapp/service/Logger.hpp"

#include <string>

namespace xplorer::app
{
    /// The port's log filename -- the owner's own choice (ADR-FMW-001), not
    /// the .NET reference's `applog.txt`. [RQ-FMW-070]
    inline constexpr const char* LOG_FILE_NAME = "xplorer.log";

    /// The full path the log file should be written to: `logDirectoryOverride`
    /// verbatim (plus LOG_FILE_NAME) when non-empty; otherwise the directory
    /// `settingsFilePath` already resolved to, reusing ADR-SET-001's
    /// preferred/fallback fallback instead of repeating it. [RQ-SET-008,
    /// ADR-FMW-001 (DEC-FMW-003)]
    [[nodiscard]] std::string resolveLogFilePath(const std::string& logDirectoryOverride,
                                                  const std::string& settingsFilePath);

    /// Clamps a persisted, possibly out-of-range severity value (e.g. a
    /// hand-edited settings file) to a valid TraceLevel instead of producing
    /// an undefined enum value. [RQ-SET-008]
    [[nodiscard]] midiapp::service::TraceLevel resolveSeverityLevel(int rawSeverityLevel);

    /// Resolves `rawSeverityLevel` (via resolveSeverityLevel) and pushes it,
    /// plus the three domain flags, into the running Logger -- the single
    /// call both startup wiring and a live Settings-dialog Accept use, so the
    /// two can never disagree. [RQ-FMW-070, RQ-FMW-073, RQ-SET-008,
    /// RQ-GUI-046, ADR-FMW-001 (DEC-FMW-003)]
    void applyLoggingConfiguration(int rawSeverityLevel, bool midiDomainEnabled,
                                    bool controllerDomainEnabled, bool uiDomainEnabled);
}
