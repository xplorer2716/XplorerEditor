#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/PianoKeyLabelPlan.hpp"

// The fake measurer stands in for real font metrics (GlyphArrangement, in
// PianoLabelledKeyboardComponent) — this file proves the DECISION logic only:
// when a label is shown, not whether JUCE draws it correctly. [RQ-GUI-075]

using namespace xplorer::app;

namespace
{
    constexpr int BASE_MIDI_NOTE = 72; // 12 * JUCE's default keyMappingOctave (6)

    std::vector<ResolvedPianoKey> sampleMapping()
    {
        return {
            {U'a', 0}, // C, midi 72 — white
            {U'w', 1}, // C#, midi 73 — black
            {U'k', 12}, // C (next octave), midi 84 — white, second mapped C
        };
    }
}

SCENARIO("A white key with a resolved binding always shows it", "[RQ-GUI-075]")
{
    GIVEN("a mapping binding midi 72 to 'a', and a measurer that would reject anything")
    {
        const auto mapping = sampleMapping();
        const PianoLabelWidthMeasurer alwaysTooWide = [](char32_t, float) { return 1000.0f; };

        WHEN("the label is planned for midi 72 as a white key")
        {
            const auto label =
                pianoKeyLabelFor(72, mapping, BASE_MIDI_NOTE, false, 10.0f, 9.0f, alwaysTooWide);

            THEN("the character is shown regardless of measured width")
            {
                REQUIRE(label.has_value());
                CHECK(*label == U'a');
            }
        }
    }
}

SCENARIO("A black key drops its label when it does not fit", "[RQ-GUI-075][DEC-JUC-121]")
{
    GIVEN("a mapping binding midi 73 to 'w', a black key")
    {
        const auto mapping = sampleMapping();

        WHEN("the measured width fits within the key")
        {
            const PianoLabelWidthMeasurer fits = [](char32_t, float) { return 5.0f; };
            const auto label = pianoKeyLabelFor(73, mapping, BASE_MIDI_NOTE, true, 10.0f, 9.0f, fits);

            THEN("the character is shown")
            {
                REQUIRE(label.has_value());
                CHECK(*label == U'w');
            }
        }

        WHEN("the measured width exceeds the key")
        {
            const PianoLabelWidthMeasurer tooWide = [](char32_t, float) { return 15.0f; };
            const auto label = pianoKeyLabelFor(73, mapping, BASE_MIDI_NOTE, true, 10.0f, 9.0f, tooWide);

            THEN("no label is shown")
            {
                CHECK_FALSE(label.has_value());
            }
        }
    }
}

SCENARIO("A note carrying no binding shows no label, on either key colour", "[RQ-GUI-075]")
{
    GIVEN("a mapping that does not bind midi 74")
    {
        const auto mapping = sampleMapping();
        const PianoLabelWidthMeasurer fits = [](char32_t, float) { return 1.0f; };

        WHEN("the label is planned for midi 74, white and black")
        {
            const auto whiteLabel = pianoKeyLabelFor(74, mapping, BASE_MIDI_NOTE, false, 10.0f, 9.0f, fits);
            const auto blackLabel = pianoKeyLabelFor(74, mapping, BASE_MIDI_NOTE, true, 10.0f, 9.0f, fits);

            THEN("neither shows a label")
            {
                CHECK_FALSE(whiteLabel.has_value());
                CHECK_FALSE(blackLabel.has_value());
            }
        }
    }
}

SCENARIO("Both mapped Cs resolve independently", "[RQ-GUI-075][DEC-JUC-120]")
{
    GIVEN("a mapping with two Cs, one octave apart (midi 72 and midi 84)")
    {
        const auto mapping = sampleMapping();
        const PianoLabelWidthMeasurer fits = [](char32_t, float) { return 1.0f; };

        WHEN("both are planned")
        {
            const auto low = pianoKeyLabelFor(72, mapping, BASE_MIDI_NOTE, false, 10.0f, 9.0f, fits);
            const auto high = pianoKeyLabelFor(84, mapping, BASE_MIDI_NOTE, false, 10.0f, 9.0f, fits);

            THEN("each carries its own binding character")
            {
                REQUIRE(low.has_value());
                REQUIRE(high.has_value());
                CHECK(*low == U'a');
                CHECK(*high == U'k');
            }
        }
    }
}
