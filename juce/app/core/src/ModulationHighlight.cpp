#include "xplorer/app/ModulationHighlight.hpp"

#include "xpl/util/EnumUtils.hpp"
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
                    const int value = xpl::util::toUnderlying(entry.base) + (family->instance - 1) * entry.stride;
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

    std::optional<BlockId> modulationSourceBlock(EnumModulationSourcesModMatrix source)
    {
        using enum EnumModulationSourcesModMatrix;
        switch (source)
        {
            case LAG:
                return BlockId::Lag;

            case TRK1:
            case TRK2:
            case TRK3:
                return BlockId::Track;

            case RMP1:
            case RMP2:
            case RMP3:
            case RMP4:
                return BlockId::Ramp;

            case ENV1:
            case ENV2:
            case ENV3:
            case ENV4:
            case ENV5:
                return BlockId::Env;

            case LFO1:
            case LFO2:
            case LFO3:
            case LFO4:
            case LFO5:
                return BlockId::Lfo;

            // Performance and global sources: the keyboard, the pedals, the
            // touch responses and the vibrato/level modifiers have no area on
            // the panel to borrow a colour from. Listed explicitly rather than
            // left to a default, so a source added later is a compile error
            // here instead of silently rendering neutral.
            case KBD:
            case VEL:
            case RVEL:
            case PRES:
            case PED1:
            case PED2:
            case VIB:
            case LEV1:
            case LEV2:
            case NONE:
                return std::nullopt;
        }
        return std::nullopt;
    }

    std::optional<BlockId> modulationDestinationBlock(EnumModulationDestinations destination)
    {
        using enum EnumModulationDestinations;
        switch (destination)
        {
            // FM_AMP is the FM VCA, which BackgroundRenderer already draws in
            // the VCO group's hue — the matrix follows the panel rather than
            // inventing a separate grouping.
            case VCO1_FRQ:
            case VCO1_PW:
            case VCO1_VOL:
            case VCO2_FRQ:
            case VCO2_PW:
            case VCO2_VOL:
            case FM_AMP:
                return BlockId::Vco;

            case VCF_FRQ:
            case VCF_RES:
            case VCA1_VOL:
            case VCA2_VOL:
                return BlockId::Vcf;

            case LFO1_SPD:
            case LFO1_AMP:
            case LFO2_SPD:
            case LFO2_AMP:
            case LFO3_SPD:
            case LFO3_AMP:
            case LFO4_SPD:
            case LFO4_AMP:
            case LFO5_SPD:
            case LFO5_AMP:
                return BlockId::Lfo;

            case ENV1_DLY:
            case ENV1_ATK:
            case ENV1_DCY:
            case ENV1_REL:
            case ENV1_AMP:
            case ENV2_DLY:
            case ENV2_ATK:
            case ENV2_DCY:
            case ENV2_REL:
            case ENV2_AMP:
            case ENV3_DLY:
            case ENV3_ATK:
            case ENV3_DCY:
            case ENV3_REL:
            case ENV3_AMP:
            case ENV4_DLY:
            case ENV4_ATK:
            case ENV4_DCY:
            case ENV4_REL:
            case ENV4_AMP:
            case ENV5_DLY:
            case ENV5_ATK:
            case ENV5_DCY:
            case ENV5_REL:
            case ENV5_AMP:
                return BlockId::Env;

            case LAG_RATE:
                return BlockId::Lag;
        }
        return std::nullopt;
    }
}
