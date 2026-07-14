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

    /// Parses a persisted "NAME;CC" entry (split on the last ';'), clamping the
    /// CC to 1..128 as the reference does. nullopt when malformed or CC is not
    /// an integer. [reference SettingsManager.LoadSettings]
    [[nodiscard]] std::optional<std::pair<std::string, int>>
    parseAutomationEntry(const std::string& entry);
}
