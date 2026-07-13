#include "xplorer/app/ModulationHighlight.hpp"

#include "xplorer/app/PageFamilyModel.hpp"

#include <array>
#include <string_view>

namespace xplorer::app
{
    namespace
    {
        using model::EnumModulationDestinations;
        using model::EnumModulationSourcesModMatrix;

        // Fixed (non-paged) knob tag → destination. Port of the reference
        // _knobTagToModDestMap (control tags identical to the WinForms names).
        struct FixedDest
        {
            std::string_view tag;
            EnumModulationDestinations destination;
        };
        constexpr std::array FIXED_DESTINATIONS = {
            FixedDest{"VCO1_FREQ", EnumModulationDestinations::VCO1_FRQ},
            FixedDest{"VCO1_PW", EnumModulationDestinations::VCO1_PW},
            FixedDest{"VCO1_VOLUME", EnumModulationDestinations::VCO1_VOL},
            FixedDest{"VCO2_FREQ", EnumModulationDestinations::VCO2_FRQ},
            FixedDest{"VCO2_PW", EnumModulationDestinations::VCO2_PW},
            FixedDest{"VCO2_VOLUME", EnumModulationDestinations::VCO2_VOL},
            FixedDest{"VCF_FREQ", EnumModulationDestinations::VCF_FRQ},
            FixedDest{"VCF_RES", EnumModulationDestinations::VCF_RES},
            FixedDest{"VCF_VCA1_VOLUME", EnumModulationDestinations::VCA1_VOL},
            FixedDest{"VCF_VCA2_VOLUME", EnumModulationDestinations::VCA2_VOL},
            FixedDest{"FM_AMP", EnumModulationDestinations::FM_AMP},
            FixedDest{"FMLAG_RATE", EnumModulationDestinations::LAG_RATE},
        };

        // Paged family control tag → (page-1 destination, per-instance stride).
        // Port of _knobTagToPagedModDestMap (5 destinations per ENV, 2 per LFO).
        struct PagedDest
        {
            std::string_view controlTag; ///< the "_X_" family tag
            EnumModulationDestinations base;
            int stride;
        };
        constexpr std::array PAGED_DESTINATIONS = {
            PagedDest{"ENV_X_DELAY", EnumModulationDestinations::ENV1_DLY, 5},
            PagedDest{"ENV_X_ATTACK", EnumModulationDestinations::ENV1_ATK, 5},
            PagedDest{"ENV_X_DECAY", EnumModulationDestinations::ENV1_DCY, 5},
            PagedDest{"ENV_X_RELEASE", EnumModulationDestinations::ENV1_REL, 5},
            PagedDest{"ENV_X_VOLUME", EnumModulationDestinations::ENV1_AMP, 5},
            PagedDest{"LFO_X_SPEED", EnumModulationDestinations::LFO1_SPD, 2},
            PagedDest{"LFO_X_AMP", EnumModulationDestinations::LFO1_AMP, 2},
        };

        struct SelectorSource
        {
            std::string_view selectorId;
            EnumModulationSourcesModMatrix source;
        };
        constexpr std::array SELECTOR_SOURCES = {
            SelectorSource{"ENV_1", EnumModulationSourcesModMatrix::ENV1},
            SelectorSource{"ENV_2", EnumModulationSourcesModMatrix::ENV2},
            SelectorSource{"ENV_3", EnumModulationSourcesModMatrix::ENV3},
            SelectorSource{"ENV_4", EnumModulationSourcesModMatrix::ENV4},
            SelectorSource{"ENV_5", EnumModulationSourcesModMatrix::ENV5},
            SelectorSource{"LFO_1", EnumModulationSourcesModMatrix::LFO1},
            SelectorSource{"LFO_2", EnumModulationSourcesModMatrix::LFO2},
            SelectorSource{"LFO_3", EnumModulationSourcesModMatrix::LFO3},
            SelectorSource{"LFO_4", EnumModulationSourcesModMatrix::LFO4},
            SelectorSource{"LFO_5", EnumModulationSourcesModMatrix::LFO5},
            SelectorSource{"RAMP_1", EnumModulationSourcesModMatrix::RMP1},
            SelectorSource{"RAMP_2", EnumModulationSourcesModMatrix::RMP2},
            SelectorSource{"RAMP_3", EnumModulationSourcesModMatrix::RMP3},
            SelectorSource{"RAMP_4", EnumModulationSourcesModMatrix::RMP4},
            SelectorSource{"TRACK_1", EnumModulationSourcesModMatrix::TRK1},
            SelectorSource{"TRACK_2", EnumModulationSourcesModMatrix::TRK2},
            SelectorSource{"TRACK_3", EnumModulationSourcesModMatrix::TRK3},
        };
    }

    std::optional<model::EnumModulationDestinations>
    modulationDestinationForParameter(const std::string& parameterName)
    {
        for (const auto& entry : FIXED_DESTINATIONS)
        {
            if (entry.tag == parameterName)
            {
                return entry.destination;
            }
        }
        // Paged knob: normalize the concrete name to its family tag + instance.
        if (const auto family = familyParameterFor(parameterName))
        {
            for (const auto& entry : PAGED_DESTINATIONS)
            {
                if (entry.controlTag == family->controlTag)
                {
                    const int value = static_cast<int>(entry.base) + (family->instance - 1) * entry.stride;
                    return static_cast<model::EnumModulationDestinations>(value);
                }
            }
        }
        return std::nullopt;
    }

    std::optional<model::EnumModulationSourcesModMatrix>
    modulationSourceForSelector(const std::string& selectorId)
    {
        for (const auto& entry : SELECTOR_SOURCES)
        {
            if (entry.selectorId == selectorId)
            {
                return entry.source;
            }
        }
        return std::nullopt;
    }
}
