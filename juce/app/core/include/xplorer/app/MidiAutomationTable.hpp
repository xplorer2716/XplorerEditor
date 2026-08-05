#pragma once

// MIDI CC automation table helpers (UI-framework-free, headless-tested): the
// reference Continuous-Controller name list and the "NAME;CC" persisted-entry
// parser shared by the settings editor and the startup load path. [RQ-GUI-036,
// RQ-SET-002, ADR-JUC-012]

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace xplorer::app
{
    /// Builds a compact, light, print-friendly HTML document listing the CC
    /// automation mapping (rows = {parameter display name, CC name}). Port of
    /// the reference MidiPage HTML export; `generatedOn` is injected so the
    /// output is deterministic and headless-testable. [RQ-GUI-036]
    [[nodiscard]] std::string
    buildMidiMappingHtml(const std::vector<std::pair<std::string, std::string>>& rows,
                         const std::string& generatedOn);

    /// Number of CC names (reference ControlChangesNames.Names length; the last
    /// index is "None"/unassigned).
    [[nodiscard]] int controlChangeNameCount();

    /// CC name for a number; out-of-range yields the "None" entry.
    [[nodiscard]] const std::string& controlChangeName(int ccNumber);

    /// CC number for the "None"/unassigned entry (last index).
    [[nodiscard]] int unassignedControlChange();

    /// Owner-curated short label for a CC (distinct from the reference name):
    /// CC 32-63 resolve to their MSB counterpart (CC-32) plus " (LSB)"; a
    /// curated base name that is the literal "Undefined" gets its own CC
    /// number appended. Out-of-range or unassigned (128) yields "None".
    /// [RQ-GUI-059, ADR-JUC-012 DEC-JUC-103]
    [[nodiscard]] std::string controlChangeShortName(int ccNumber);

    /// Automation-table "MIDI CC" column display label: `"<CC, zero-padded 3
    /// digits> - <short name>"`; the unassigned entry (128) or any
    /// out-of-range value is the bare word "None", with no numeric prefix.
    /// [RQ-GUI-059, ADR-JUC-012 DEC-JUC-102]
    [[nodiscard]] std::string controlChangeDisplayLabel(int ccNumber);

    /// Parses a persisted "NAME;CC" entry (split on the last ';'), clamping the
    /// CC to 1..128 as the reference does. nullopt when malformed or CC is not
    /// an integer. [reference SettingsManager.LoadSettings]
    [[nodiscard]] std::optional<std::pair<std::string, int>>
    parseAutomationEntry(const std::string& entry);
}
