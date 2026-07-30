// 16-segment glyph topology. [RQ-GUI-033, RQ-GUI-049, ADR-JUC-023 (DEC-JUC-051)]
//
// The point of this suite is the bit -> position mapping. The vendored table
// gives 95 opaque 16-bit masks; nothing in the data itself says which bit is
// the top-left horizontal. Get that wrong and every glyph still renders — as a
// plausible, silently incorrect shape. So the mapping is pinned here by
// decoding characters whose segment set is unambiguous from their shape.

#include "xplorer/app/SegmentFont.hpp"

#include <catch2/catch_test_macros.hpp>

#include <initializer_list>
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
            THEN("it lights only the two right verticals and one diagonal serif")
            {
                REQUIRE(litSegments('1')
                        == setOf({Segment::C, Segment::D, Segment::N}));
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
    GIVEN("'0' and '8', which share the whole outer ring")
    {
        const auto ring = setOf({Segment::A, Segment::B, Segment::C, Segment::D,
                                 Segment::E, Segment::F, Segment::G, Segment::H});

        WHEN("their common segments are taken")
        {
            std::set<Segment> common;
            for (const auto segment : litSegments('0'))
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
            THEN("'0' adds the slash diagonals and '8' adds the middle bar")
            {
                auto zero = litSegments('0');
                auto eight = litSegments('8');
                for (const auto segment : ring)
                {
                    zero.erase(segment);
                    eight.erase(segment);
                }
                REQUIRE(zero == setOf({Segment::N, Segment::T}));
                REQUIRE(eight == setOf({Segment::P, Segment::U}));
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
                // The sheet had no lowercase artwork at all, so what
                // RQ-GUI-049 asks for is legibility: a lowercase letter must
                // light something. Distinctness from uppercase is NOT required
                // and is not always achievable — 'x' is the four diagonals in
                // both cases, and a 16-segment display has no smaller form to
                // fall back on. It is the single such collision in the table.
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
