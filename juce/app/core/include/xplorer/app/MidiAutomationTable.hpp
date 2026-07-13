#pragma once

// MIDI CC automation table helpers (UI-framework-free, headless-tested): the
// reference Continuous-Controller name list and the "NAME;CC" persisted-entry
// parser shared by the settings editor and the startup load path. [RQ-GUI-036,
// RQ-SET-002, ADR-012]

#include <optional>
#include <string>
#include <utility>

namespace xplorer::app
{
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
