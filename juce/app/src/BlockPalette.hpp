#pragma once

// Runtime block-identity palette: the design tokens stay the defaults, a user
// override (settings) wins, and every consumer resolves through the single
// LookAndFeel accessor — no cached copies. The painter keeps named fields
// (palette.env, ...) so BackgroundRenderer stays the 1:1 transcription of the
// validated mockup; generic consumers (settings UI, XML persistence) iterate
// through the descriptor table instead. [RQ-DSN-095, ADR-JUC-020
// (DEC-JUC-034, DEC-JUC-035)]

#include "xplorer/app/BlockIdentity.hpp"
#include "xplorer/settings/AllUsersSettings.hpp"

#include <juce_graphics/juce_graphics.h>

#include <array>
#include <cstddef>

namespace xplorer::app
{
    /// Value type with eight named fields — never a keyed container, so the
    /// painter names its colours explicitly. [DEC-JUC-034]
    struct BlockPalette
    {
        juce::Colour vco, lag, track, vcf, env, lfo, ramp, matrix;
    };

    // BlockId is declared in xplorer/app/BlockIdentity.hpp (headless core), so
    // the JUCE-free modulation cross-reference can map a source/destination to
    // its block. [ADR-JUC-028 (DEC-JUC-079)]

    inline constexpr std::size_t BLOCK_COLOUR_COUNT =
        settings::AllUsersSettings::UiConfiguration::BLOCK_COLOUR_COUNT;

    /// One row per block for settings/persistence iteration: id, the label the
    /// settings page shows (owner-validated mockup wording), and the palette
    /// member it edits. [DEC-JUC-035, RQ-GUI-046]
    struct BlockColourDescriptor
    {
        BlockId id;
        const char* displayName;
        juce::Colour BlockPalette::*member;
    };

    [[nodiscard]] const std::array<BlockColourDescriptor, BLOCK_COLOUR_COUNT>& blockColourDescriptors();

    /// The design-system tokens as a palette — what a user without overrides
    /// gets, and what "Reset to defaults" restores. [DEC-JUC-034, RQ-DSN-092]
    [[nodiscard]] BlockPalette defaultBlockPalette();

    /// Colour of one block in a palette, resolved through the descriptor table.
    [[nodiscard]] juce::Colour blockColourOf(const BlockPalette& palette, BlockId id);

    /// Defaults with the persisted per-block overrides applied (override ??
    /// default) — the palette the LookAndFeel is given at startup and on
    /// settings accept. [RQ-SET-007, ADR-JUC-020 (DEC-JUC-039)]
    [[nodiscard]] BlockPalette resolveBlockPalette(
        const settings::AllUsersSettings::UiConfiguration& uiConfig);
}
