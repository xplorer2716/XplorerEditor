#include "BlockPalette.hpp"

#include "DesignTokens.hpp"

namespace xplorer::app
{
    const std::array<BlockColourDescriptor, BLOCK_COLOUR_COUNT>& blockColourDescriptors()
    {
        // Index order == BlockId value == UiConfiguration::blockColours index.
        // Display names are the owner-validated settings-mockup labels.
        // [DEC-JUC-035, RQ-GUI-046, RQ-SET-007]
        static const std::array<BlockColourDescriptor, BLOCK_COLOUR_COUNT> table{{
            {BlockId::Vco, "VCO/FM", &BlockPalette::vco},
            {BlockId::Lag, "LAG", &BlockPalette::lag},
            {BlockId::Track, "TRACK", &BlockPalette::track},
            {BlockId::Vcf, "VCF/VCA", &BlockPalette::vcf},
            {BlockId::Env, "ENV", &BlockPalette::env},
            {BlockId::Lfo, "LFO", &BlockPalette::lfo},
            {BlockId::Ramp, "RAMP", &BlockPalette::ramp},
            {BlockId::Matrix, "MOD MATRIX", &BlockPalette::matrix},
        }};
        return table;
    }

    BlockPalette defaultBlockPalette()
    {
        // The design tokens are the defaults (palette v2, RQ-DSN-092); a
        // future palette revision reaches every non-customised user through
        // this single point. [DEC-JUC-034]
        return {tokens::semantic::blockVco,  tokens::semantic::blockLag,
                tokens::semantic::blockTrack, tokens::semantic::blockVcf,
                tokens::semantic::blockEnv,  tokens::semantic::blockLfo,
                tokens::semantic::blockRamp, tokens::semantic::blockMatrix};
    }

    juce::Colour blockColourOf(const BlockPalette& palette, BlockId id)
    {
        const auto& descriptor = blockColourDescriptors()[static_cast<std::size_t>(id)];
        jassert(descriptor.id == id); // table order is the BlockId contract
        return palette.*(descriptor.member);
    }
}
