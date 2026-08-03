#pragma once

// Identity of a functional block of the signal path — the enum only, with no
// colour and no UI-framework dependency, so headless logic can reason about
// blocks. The colours that go with these identities live one layer up, in
// BlockPalette.hpp (app layer, juce::Colour).
//
// Moved here from BlockPalette.hpp so ModulationHighlight — which is JUCE-free
// by the ADR-JUC-006 core boundary — can map a modulation source/destination to
// the block it belongs to. Duplicating the enum instead was rejected: its
// declaration order IS the settings persistence contract (below), and two
// copies of a contract diverge silently into mis-mapped user colours.
// [RQ-GUI-052, RQ-DSN-100, ADR-JUC-006, ADR-JUC-028 (DEC-JUC-079)]

#include <cstddef>

namespace xplorer::app
{
    /// Identity of a functional block for the generic consumers. Order is the
    /// persistence contract: UiConfiguration::blockColours is indexed by this
    /// enum's value — never reorder. [DEC-JUC-035, RQ-SET-007]
    enum class BlockId : std::size_t
    {
        Vco,
        Lag,
        Track,
        Vcf,
        Env,
        Lfo,
        Ramp,
        Matrix
    };
}
