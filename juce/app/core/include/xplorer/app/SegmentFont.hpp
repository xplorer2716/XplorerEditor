#pragma once

// 16-segment glyph topology for the VFD: which segments each printable ASCII
// character lights. Pure data and pure logic — no JUCE, no geometry in pixels —
// so it is headless-testable like the rest of xpl_app_core.
//
// The masks come from the vendored upstream table (see GeneratedSegmentFont.inc
// and juce/tools/vendor/), NOT from hand-authored artwork: it is the same data,
// it has to be verified either way, and hand artwork could not be regression-
// tested against a published reference. [ADR-JUC-023 (DEC-JUC-051)]
//
// Rendering this topology into shapes is the renderer's job; the geometry lives
// in design tokens (RQ-DSN-097), never here.
// [RQ-GUI-033, RQ-GUI-049, ADR-JUC-023]

#include "xpl/util/EnumUtils.hpp"

#include <cstdint>

namespace xplorer::app
{
    /// The 16 segments, in the vendored table's bit order (bit 0 = A .. bit 15 = U).
    ///
    /// Upstream names them with letters, skipping I/J/L/O/Q to avoid confusion
    /// with digits and each other. The letters carry no positional meaning on
    /// their own — the layout below was recovered by decoding known glyphs and
    /// is pinned by SegmentFontTests.cpp, because a silently wrong mapping
    /// would still render plausible-looking (but incorrect) characters:
    ///
    ///        A       B          outer ring, clockwise from top-left:
    ///      ┌─────┬─────┐        A,B  top horizontals (left, right)
    ///      │╲    │    ╱│        C,D  right verticals (upper, lower)
    ///     H│ ╲K  │M  ╱ │C       E,F  bottom horizontals (right, left)
    ///      │  ╲  │  ╱N │        G,H  left verticals (lower, upper)
    ///      ├──U──┼──P──┤
    ///      │  ╱  │  ╲  │        inner:
    ///     G│ ╱T  │S  ╲R│        K,N  upper diagonals (left, right)
    ///      │╱    │    ╲│        M,S  centre verticals (upper, lower)
    ///      └─────┴─────┘        U,P  middle horizontals (left, right)
    ///        F       E          T,R  lower diagonals (left, right)
    enum class Segment : int
    {
        A = 0, B, C, D, E, F, G, H,
        K, M, N, P, R, S, T, U,
    };

    inline constexpr int SEGMENT_COUNT = 16;

    /// Printable ASCII range the font covers. Everything outside renders as a
    /// space, matching the behaviour the display has always had (RQ-GUI-033).
    inline constexpr int FIRST_GLYPH = 32;   // ' '
    inline constexpr int LAST_GLYPH = 126;   // '~'
    inline constexpr int GLYPH_COUNT = LAST_GLYPH - FIRST_GLYPH + 1;

    /// Lit-segment mask for a code point; 0 (all segments dark) for the space
    /// and for anything outside FIRST_GLYPH..LAST_GLYPH.
    [[nodiscard]] std::uint16_t segmentMaskFor(int codePoint) noexcept;

    /// Whether `segment` is lit in `mask`.
    [[nodiscard]] constexpr bool isLit(std::uint16_t mask, Segment segment) noexcept
    {
        return ((mask >> xpl::util::toUnderlying(segment)) & 1U) != 0U;
    }
}
