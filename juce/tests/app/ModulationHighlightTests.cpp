#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/ModulationHighlight.hpp"

#include <set>

// Block identity of modulation sources and destinations: the lookup that lets
// the modulation matrix carry the panel's colour coding (RQ-GUI-052). Covered
// EXHAUSTIVELY rather than by sampling, because the interesting failure is a
// value nobody thought about — a source that should be neutral quietly getting
// a hue, or a destination silently falling through to none.
//
// JUCE-free by construction: this is enum-to-enum data, and it lives in
// xpl_app_core precisely so it can be checked without a graphics stack.
// [RQ-GUI-052, RQ-DSN-100, ADR-JUC-006, ADR-JUC-028 (DEC-JUC-079)]

using namespace xplorer::app;
using xplorer::model::EnumModulationDestinations;
using xplorer::model::EnumModulationSourcesModMatrix;

namespace
{
    // The enums are contiguous from 0; NONE closes the source range at 0x1B and
    // LAG_RATE closes the destination range.
    constexpr int LAST_SOURCE = static_cast<int>(EnumModulationSourcesModMatrix::NONE);
    constexpr int LAST_DESTINATION = static_cast<int>(EnumModulationDestinations::LAG_RATE);
}

SCENARIO("Every modulation source resolves to its functional block, or to none",
         "[RQ-GUI-052][RQ-DSN-100]")
{
    GIVEN("the modulation sources that belong to a block")
    {
        THEN("the paged families map to the block their instances live in")
        {
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::ENV1) == BlockId::Env);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::ENV5) == BlockId::Env);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::LFO1) == BlockId::Lfo);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::LFO5) == BlockId::Lfo);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::RMP1) == BlockId::Ramp);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::RMP4) == BlockId::Ramp);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::TRK1) == BlockId::Track);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::TRK3) == BlockId::Track);
        }

        THEN("the lag processor maps to the LAG block")
        {
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::LAG) == BlockId::Lag);
        }
    }

    GIVEN("the performance and global sources")
    {
        // These are the "entries such as VEL, RVEL, PRESS, VIB, LEV1, LEV2 and
        // NONE" of RQ-GUI-052 — plus KBD and the two pedals, which have no
        // panel area either and would otherwise be tinted by accident.
        THEN("they belong to no block and keep the default appearance")
        {
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::KBD) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::VEL) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::RVEL) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::PRES) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::PED1) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::PED2) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::VIB) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::LEV1) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::LEV2) == std::nullopt);
            REQUIRE(modulationSourceBlock(EnumModulationSourcesModMatrix::NONE) == std::nullopt);
        }
    }

    GIVEN("the whole source range")
    {
        WHEN("every value is mapped")
        {
            std::set<BlockId> blocksSeen;
            int neutralCount = 0;
            for (int raw = 0; raw <= LAST_SOURCE; ++raw)
            {
                const auto block =
                    modulationSourceBlock(static_cast<EnumModulationSourcesModMatrix>(raw));
                if (block.has_value())
                {
                    blocksSeen.insert(*block);
                }
                else
                {
                    ++neutralCount;
                }
            }

            THEN("exactly the ten performance/global sources are neutral")
            {
                // Pins the split: 28 sources, 18 blocked (LAG + 3 TRK + 4 RMP +
                // 5 ENV + 5 LFO), 10 neutral. A source moving from one side to
                // the other is a visible UI change and must be a deliberate edit.
                REQUIRE(neutralCount == 10);
            }

            THEN("the sources reach exactly the five blocks that own them")
            {
                // No VCO, VCF or MATRIX source exists — nothing in those blocks
                // produces a modulation signal.
                REQUIRE(blocksSeen
                        == std::set<BlockId>{BlockId::Lag, BlockId::Track, BlockId::Ramp,
                                             BlockId::Env, BlockId::Lfo});
            }
        }
    }
}

SCENARIO("Every modulation destination resolves to a functional block",
         "[RQ-GUI-052][RQ-DSN-100]")
{
    GIVEN("destinations drawn inside a specific block")
    {
        THEN("they map to the block that draws their knob")
        {
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::VCO1_FRQ) == BlockId::Vco);
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::VCO2_VOL) == BlockId::Vco);
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::VCF_FRQ) == BlockId::Vcf);
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::VCA2_VOL) == BlockId::Vcf);
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::LFO3_SPD) == BlockId::Lfo);
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::ENV4_DCY) == BlockId::Env);
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::LAG_RATE) == BlockId::Lag);
        }

        THEN("the FM amplitude follows the panel, which draws the FM VCA in the VCO hue")
        {
            REQUIRE(modulationDestinationBlock(EnumModulationDestinations::FM_AMP) == BlockId::Vco);
        }
    }

    GIVEN("the whole destination range")
    {
        WHEN("every value is mapped")
        {
            std::set<BlockId> blocksSeen;
            bool anyNeutral = false;
            for (int raw = 0; raw <= LAST_DESTINATION; ++raw)
            {
                const auto block =
                    modulationDestinationBlock(static_cast<EnumModulationDestinations>(raw));
                if (block.has_value())
                {
                    blocksSeen.insert(*block);
                }
                else
                {
                    anyNeutral = true;
                }
            }

            THEN("none is neutral — every destination has a block")
            {
                // This is what lets RQ-GUI-052 say the neutral case arises on
                // the source side only. If a destination ever returns nullopt,
                // that statement in the requirement is wrong, not just the code.
                REQUIRE_FALSE(anyNeutral);
            }

            THEN("the destinations reach exactly the five blocks that own them")
            {
                // No TRACK, RAMP or MATRIX destination exists — those blocks
                // only ever produce modulation, never receive it.
                REQUIRE(blocksSeen
                        == std::set<BlockId>{BlockId::Vco, BlockId::Vcf, BlockId::Lfo,
                                             BlockId::Env, BlockId::Lag});
            }
        }
    }
}
