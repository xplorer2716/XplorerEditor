#pragma once

// Modulation cross-reference resolver (UI-framework-free, headless-tested):
// maps a knob's parameter to the modulation destination it represents, and a
// page-family selector to the modulation source it represents — the reference
// _knobTagToModDestMap / _knobTagToPagedModDestMap / _radioButtonToModSourceMap.
// Used by the matrix hover highlight (RQ-GUI-018) and the VFD active-
// destination "." marker (RQ-GUI-020). [ADR-010]

#include "xplorer/model/XpanderConstants.hpp"

#include <optional>
#include <string>

namespace xplorer::app
{
    /// The modulation destination a knob edits, or nullopt when the knob is
    /// not a modulation destination. Accepts the concrete parameter name;
    /// paged ENV/LFO knobs (e.g. "ENV_3_ATTACK") resolve to the instance's
    /// destination via the family digit.
    [[nodiscard]] std::optional<model::EnumModulationDestinations>
    modulationDestinationForParameter(const std::string& parameterName);

    /// The modulation source a page-family selector represents (selector id
    /// e.g. "ENV_1"), or nullopt when it is not a source.
    [[nodiscard]] std::optional<model::EnumModulationSourcesModMatrix>
    modulationSourceForSelector(const std::string& selectorId);
}
