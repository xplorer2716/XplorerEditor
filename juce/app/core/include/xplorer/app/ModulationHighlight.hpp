#pragma once

// Modulation cross-reference resolver (UI-framework-free, headless-tested):
// maps a knob's parameter to the modulation destination it represents, and a
// page-family selector to the modulation source it represents — the reference
// _knobTagToModDestMap / _knobTagToPagedModDestMap / _radioButtonToModSourceMap.
// Used by the matrix hover highlight (RQ-GUI-018) and the VFD active-
// destination "." marker (RQ-GUI-020). [ADR-JUC-010]

#include "xplorer/app/BlockIdentity.hpp"
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

    /// The functional block a modulation source belongs to, or nullopt when it
    /// belongs to none — KBD, VEL, RVEL, PRES, PED1/2, VIB, LEV1/2 and NONE are
    /// performance/global sources with no area on the panel. nullopt is a real
    /// state (the control keeps its default appearance), not a missing entry.
    /// [RQ-GUI-052, ADR-JUC-028 (DEC-JUC-079)]
    [[nodiscard]] std::optional<BlockId>
    modulationSourceBlock(model::EnumModulationSourcesModMatrix source);

    /// The functional block a modulation destination belongs to. Every
    /// destination maps to one — the model has no TRACK, RAMP or MATRIX
    /// destination — but the optional is kept for symmetry with the source
    /// side and so a future destination cannot silently default to a block.
    /// [RQ-GUI-052, ADR-JUC-028 (DEC-JUC-079)]
    [[nodiscard]] std::optional<BlockId>
    modulationDestinationBlock(model::EnumModulationDestinations destination);
}
