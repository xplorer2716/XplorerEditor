// 16-segment glyph topology. [RQ-GUI-033, RQ-GUI-049, ADR-JUC-023 (DEC-JUC-051)]
//
// The point of this suite is the bit -> position mapping. The vendored table
// gives 95 opaque 16-bit masks; nothing in the data itself says which bit is
// the top-left horizontal. Get that wrong and every glyph still renders — as a
// plausible, silently incorrect shape. So the mapping is pinned here by
// decoding characters whose segment set is unambiguous from their shape.

#include "xplorer/app/SegmentFont.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <initializer_list>
#include <map>
#include <set>
#include <string>

using namespace xplorer::app;

namespace
{
    /// The exact set of lit segments of `character`, as a sorted set.
    std::set<Segment> litSegments(char character)
    {
        std::set<Segment> lit;
        const auto mask = segmentMaskFor(static_cast<int>(character));
        for (int bit = 0; bit < SEGMENT_COUNT; ++bit)
        {
            const auto segment = static_cast<Segment>(bit);
            if (isLit(mask, segment))
            {
                lit.insert(segment);
            }
        }
        return lit;
    }

    std::set<Segment> setOf(std::initializer_list<Segment> segments)
    {
        return std::set<Segment>(segments);
    }
}

// --- the mapping itself -----------------------------------------------------

SCENARIO("the segment bit order matches the documented layout", "[RQ-GUI-033]")
{
    GIVEN("characters whose 16-segment shape is unambiguous")
    {
        WHEN("'H' is decoded")
        {
            THEN("it lights both side verticals and the middle bar, nothing else")
            {
                // Two full verticals + the crossbar: pins C,D as the right
                // verticals, G,H as the left ones, U,P as the middle halves.
                REQUIRE(litSegments('H')
                        == setOf({Segment::C, Segment::D, Segment::G, Segment::H,
                                  Segment::P, Segment::U}));
            }
        }

        WHEN("'1' is decoded")
        {
            THEN("it lights both full verticals, left and right, and nothing else")
            {
                // Xpander hardware draws '1' as two parallel vertical strokes,
                // not a single stroke with a diagonal serif — corrected from
                // the vendored table's {C,D,N} against the physical display.
                // [PLAN-GUI-015]
                REQUIRE(litSegments('1')
                        == setOf({Segment::C, Segment::D, Segment::G, Segment::H}));
            }
        }

        WHEN("'T' is decoded")
        {
            THEN("it lights the top horizontals and the two centre verticals")
            {
                // Pins A,B as the top pair and M,S as the centre verticals.
                REQUIRE(litSegments('T')
                        == setOf({Segment::A, Segment::B, Segment::M, Segment::S}));
            }
        }

        WHEN("'+' is decoded")
        {
            THEN("it lights the centre verticals and the middle horizontals only")
            {
                REQUIRE(litSegments('+')
                        == setOf({Segment::M, Segment::P, Segment::S, Segment::U}));
            }
        }

        WHEN("'-' is decoded")
        {
            THEN("it lights the two middle horizontals only")
            {
                REQUIRE(litSegments('-') == setOf({Segment::P, Segment::U}));
            }
        }

        WHEN("'X' is decoded")
        {
            THEN("it lights the four diagonals and nothing else")
            {
                // Pins K,N,R,T as the diagonals — the only four-way crossing.
                REQUIRE(litSegments('X')
                        == setOf({Segment::K, Segment::N, Segment::R, Segment::T}));
            }
        }
    }
}

SCENARIO("the outer ring and the diagonals are distinguishable", "[RQ-GUI-033]")
{
    GIVEN("'O' and '8', which share the whole outer ring")
    {
        const auto ring = setOf({Segment::A, Segment::B, Segment::C, Segment::D,
                                 Segment::E, Segment::F, Segment::G, Segment::H});

        WHEN("their common segments are taken")
        {
            std::set<Segment> common;
            for (const auto segment : litSegments('O'))
            {
                if (litSegments('8').count(segment) != 0)
                {
                    common.insert(segment);
                }
            }

            THEN("it is exactly the eight-segment outer ring")
            {
                REQUIRE(common == ring);
            }
        }

        WHEN("their differences are taken")
        {
            THEN("'O' adds nothing and '8' adds the middle bar")
            {
                // 'O' no longer differs from the plain ring: digit '0' was
                // moved onto this same mask (Xpander hardware draws no
                // slash — no visual difference between 0 and O), so the ring
                // alone is 'O''s whole shape now. [PLAN-GUI-015]
                auto letterO = litSegments('O');
                auto eight = litSegments('8');
                for (const auto segment : ring)
                {
                    letterO.erase(segment);
                    eight.erase(segment);
                }
                REQUIRE(letterO.empty());
                REQUIRE(eight == setOf({Segment::P, Segment::U}));
            }
        }
    }

    GIVEN("'7', whose only non-horizontal segments are its diagonal stroke")
    {
        WHEN("it is decoded")
        {
            THEN("it is exactly the top horizontals plus N and T")
            {
                // Corrected '7' is oblique (Xpander hardware), built from the
                // top bar plus a single top-right-to-bottom-left diagonal —
                // the same N,T pair the old sprite-derived '0' used to pin
                // before '0' moved onto the plain-ring mask above. Combined
                // with 'M'/'W' below ({K,N} vs {R,T}), this still isolates
                // all four diagonal bit positions individually:
                // N = {K,N}∩{N,T}, T = {R,T}∩{N,T}, K = {K,N}\{N}, R = {R,T}\{T}.
                // [PLAN-GUI-015]
                REQUIRE(litSegments('7')
                        == setOf({Segment::A, Segment::B, Segment::N, Segment::T}));
            }
        }
    }
}

SCENARIO("'M' and 'W' use opposite diagonal pairs", "[RQ-GUI-033]")
{
    GIVEN("the two characters that differ only by which diagonals they use")
    {
        WHEN("both are decoded")
        {
            THEN("'M' takes the upper diagonals and 'W' the lower ones")
            {
                const auto verticals = setOf({Segment::C, Segment::D,
                                              Segment::G, Segment::H});
                auto emm = litSegments('M');
                auto doubleU = litSegments('W');
                for (const auto segment : verticals)
                {
                    REQUIRE(emm.count(segment) == 1);
                    REQUIRE(doubleU.count(segment) == 1);
                    emm.erase(segment);
                    doubleU.erase(segment);
                }
                REQUIRE(emm == setOf({Segment::K, Segment::N}));
                REQUIRE(doubleU == setOf({Segment::R, Segment::T}));
            }
        }
    }
}

SCENARIO("'A' and 'E' pin the top and bottom horizontals apart", "[RQ-GUI-033]")
{
    GIVEN("'A', which has a closed top and open bottom")
    {
        WHEN("it is decoded")
        {
            THEN("the top horizontals are lit and the bottom ones are not")
            {
                const auto lit = litSegments('A');
                REQUIRE(lit.count(Segment::A) == 1);
                REQUIRE(lit.count(Segment::B) == 1);
                REQUIRE(lit.count(Segment::E) == 0);
                REQUIRE(lit.count(Segment::F) == 0);
            }
        }
    }

    GIVEN("'E', which is closed top and bottom on the left side")
    {
        WHEN("it is decoded")
        {
            THEN("both horizontal pairs and the left verticals are lit")
            {
                REQUIRE(litSegments('E')
                        == setOf({Segment::A, Segment::B, Segment::E, Segment::F,
                                  Segment::G, Segment::H, Segment::U}));
            }
        }
    }
}

// --- coverage ---------------------------------------------------------------

SCENARIO("the font covers the whole printable ASCII range", "[RQ-GUI-049]")
{
    GIVEN("the printable range 32..126")
    {
        WHEN("every code point is looked up")
        {
            THEN("only the space is dark")
            {
                // This is the table-level half of RQ-GUI-049: the inherited
                // sprite sheet left 44 of these 95 cells blank — every
                // lowercase letter among them — so a lowercase patch name
                // silently vanished. Nothing may be dark here but the space.
                for (int codePoint = FIRST_GLYPH; codePoint <= LAST_GLYPH; ++codePoint)
                {
                    const auto mask = segmentMaskFor(codePoint);
                    if (codePoint == ' ')
                    {
                        REQUIRE(mask == 0);
                    }
                    else
                    {
                        INFO("code point " << codePoint << " ('"
                                           << static_cast<char>(codePoint) << "')");
                        REQUIRE(mask != 0);
                    }
                }
            }
        }

        WHEN("the lowercase letters are looked up")
        {
            THEN("every one is a real glyph, with 'x' the only uppercase alias")
            {
                // The sheet had no lowercase artwork at all, so at table level
                // what matters is that a lowercase letter lights something.
                // 'x' is the four diagonals in BOTH cases: a 16-segment cell
                // draws lowercase in its lower half, but a crossing needs one
                // '\' and one '/' stroke and both diagonal pairs start at the
                // top corners, so no lower-half crossing exists. Upstream is
                // right for a pure 16-segment device; RQ-GUI-049 nonetheless
                // requires the two to render differently, which the renderer
                // achieves with an off-model primitive (DEC-JUC-052) rather
                // than by editing the vendored table. This suite therefore
                // asserts the table as vendored — see the collision scenario
                // below for what the renderer must resolve.
                for (char letter = 'a'; letter <= 'z'; ++letter)
                {
                    INFO("letter '" << letter << "'");
                    const auto lower = segmentMaskFor(static_cast<int>(letter));
                    const auto upper = segmentMaskFor(static_cast<int>(letter) - 32);
                    REQUIRE(lower != 0);
                    if (letter == 'x')
                    {
                        REQUIRE(lower == upper);
                    }
                    else
                    {
                        REQUIRE(lower != upper);
                    }
                }
            }
        }
    }
}

SCENARIO("the vendored table has exactly two known mask collisions", "[RQ-GUI-049]")
{
    GIVEN("all 95 printable code points")
    {
        WHEN("their masks are grouped")
        {
            std::map<std::uint16_t, std::string> byMask;
            std::map<std::uint16_t, std::string> collisions;
            for (int codePoint = FIRST_GLYPH; codePoint <= LAST_GLYPH; ++codePoint)
            {
                const auto mask = segmentMaskFor(codePoint);
                const auto character = static_cast<char>(codePoint);
                if (auto existing = byMask.find(mask); existing != byMask.end())
                {
                    collisions[mask] = existing->second + character;
                }
                else
                {
                    byMask[mask] = std::string(1, character);
                }
            }

            THEN("they are the two off-model pairs plus three Xpander-accurate ones")
            {
                // ':'/'|' and 'x'/'X' are the vendored data's own collisions;
                // RQ-GUI-049 requires all 95 glyphs to render distinctly, and
                // the renderer buys that with exactly two off-model overrides
                // (DEC-JUC-052).
                //
                // '0'/'O', '('/'<' and ')'/'>' are NEW, deliberate collisions
                // from the Xpander hardware correction (PLAN-GUI-015): the
                // physical display draws each pair identically (no slashed
                // zero, and angle brackets reuse the same rounded-brace shape
                // as parentheses), so unlike the two above, these are NOT
                // given a distinguishing override — colliding IS the
                // hardware-accurate behaviour here.
                //
                // If a table update ever introduced an unaccounted-for
                // collision, that guarantee would break silently — so it must
                // break here instead.
                REQUIRE(collisions.size() == 5);
                REQUIRE(collisions[0x1400] == "(<");
                REQUIRE(collisions[0x2200] == ":|");
                REQUIRE(collisions[0x4100] == ")>");
                REQUIRE(collisions[0x5500] == "Xx");
                REQUIRE(collisions[0x00FF] == "0O");
            }
        }
    }
}

SCENARIO("code points outside the printable range render as a space", "[RQ-GUI-033]")
{
    GIVEN("values below, above and far outside the range")
    {
        WHEN("they are looked up")
        {
            THEN("every one yields the dark mask")
            {
                REQUIRE(segmentMaskFor(FIRST_GLYPH - 1) == 0);   // 31, control
                REQUIRE(segmentMaskFor(LAST_GLYPH + 1) == 0);    // 127, DEL
                REQUIRE(segmentMaskFor(0) == 0);
                REQUIRE(segmentMaskFor(-1) == 0);
                REQUIRE(segmentMaskFor(0x20AC) == 0);            // euro sign
            }
        }
    }
}

SCENARIO("isLit reads the documented bit positions", "[RQ-GUI-033]")
{
    GIVEN("a mask with a single known bit set")
    {
        WHEN("each segment is probed")
        {
            THEN("only that segment reads as lit")
            {
                for (int bit = 0; bit < SEGMENT_COUNT; ++bit)
                {
                    const auto mask = static_cast<std::uint16_t>(1U << bit);
                    for (int probe = 0; probe < SEGMENT_COUNT; ++probe)
                    {
                        INFO("bit " << bit << ", probe " << probe);
                        REQUIRE(isLit(mask, static_cast<Segment>(probe)) == (bit == probe));
                    }
                }
            }
        }
    }
}
