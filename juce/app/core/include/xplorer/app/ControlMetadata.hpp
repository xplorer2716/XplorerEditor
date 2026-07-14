#pragma once

// UI-framework-free control metadata derived from the reference: ComboBox
// enum labels, control-id → enum-type mapping, and RadioButtonPanel options.
// Consumed by the JUCE control wrappers; kept here so it is headless-testable.
// [RQ-GUI-032, ADR-JUC-006]

#include <string>
#include <utility>
#include <vector>

namespace xplorer::app
{
    /// Ordered display labels for an enum-backed combo; the index is the
    /// parameter value (contiguous enums). Empty when the id is not an
    /// enum combo.
    [[nodiscard]] std::vector<std::string> comboLabelsForControl(const std::string& controlId);

    /// Options (label, value) for a RadioButtonPanel control id; empty when
    /// the id is not a known radio panel.
    [[nodiscard]] std::vector<std::pair<std::string, int>> radioPanelOptions(const std::string& controlId);

    /// End-user display name for a parameter tag (reference VfdDisplayHelper
    /// resource lookup); returns the tag unchanged when no friendly name
    /// exists. [RQ-GUI-020]
    [[nodiscard]] std::string parameterDisplayName(const std::string& parameterName);
}
