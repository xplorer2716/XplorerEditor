#include <catch2/catch_test_macros.hpp>

#include "BlockPalette.hpp"
#include "XplorerLookAndFeel.hpp"

#include <juce_gui_basics/juce_gui_basics.h>

// The knob LED colour is retuned IN PLACE rather than by rebuilding the whole
// LookAndFeel, so the settings dialog can preview a pick live (RQ-GUI-073).
// That trades one guarantee for another: a rebuild could not leave a stale
// derived value behind because nothing survived it, whereas an in-place
// mutation can — the PopupMenu highlight of RQ-GUI-068 is baked from the accent
// at construction rather than read at paint time, and it is the one value that
// would silently drift. So it is pinned here, next to the accessor itself and
// the palette a LED change must not disturb.
// [RQ-GUI-073, RQ-GUI-068, ADR-JUC-011, ADR-JUC-020 (DEC-JUC-113)]

using namespace xplorer::app;

SCENARIO("The knob LED colour is retuned in place", "[RQ-GUI-073][RQ-GUI-068]")
{
    GIVEN("a LookAndFeel built with one LED colour")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        const auto initial = juce::Colours::orange;
        const auto retuned = juce::Colours::cyan;
        REQUIRE(initial != retuned);

        XplorerLookAndFeel lookAndFeel{initial};

        THEN("it reports that colour, and seeds the popup highlight from it")
        {
            REQUIRE(lookAndFeel.ledColour() == initial);
            REQUIRE(lookAndFeel.findColour(juce::PopupMenu::highlightedBackgroundColourId) == initial);
        }

        WHEN("the LED colour is retuned")
        {
            lookAndFeel.setLedColour(retuned);

            THEN("the single runtime source of truth reports the new colour")
            {
                REQUIRE(lookAndFeel.ledColour() == retuned);
            }

            THEN("the popup highlight follows it, with no stale cached copy")
            {
                // The live-preview equivalent of what the rebuild used to give
                // for free. [RQ-GUI-068, DEC-JUC-113]
                REQUIRE(lookAndFeel.findColour(juce::PopupMenu::highlightedBackgroundColourId)
                        == retuned);
            }
        }
    }
}

SCENARIO("Retuning the LED colour leaves the block palette alone", "[RQ-GUI-073][RQ-DSN-095]")
{
    GIVEN("a LookAndFeel carrying a customised block palette")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        XplorerLookAndFeel lookAndFeel{juce::Colours::orange};

        auto customised = defaultBlockPalette();
        customised.env = juce::Colours::magenta;
        lookAndFeel.setBlockPalette(customised);

        WHEN("only the LED colour is retuned")
        {
            lookAndFeel.setLedColour(juce::Colours::cyan);

            THEN("the customised palette survives untouched")
            {
                // Before DEC-JUC-113 this was a rebuild, and the palette only
                // survived because the caller saved and restored it by hand.
                // In-place mutation removes that dance — and this is what says
                // so. [RQ-DSN-095, ADR-JUC-020]
                REQUIRE(lookAndFeel.blockPalette().env == juce::Colours::magenta);
                REQUIRE(lookAndFeel.blockPalette().vco == defaultBlockPalette().vco);
            }
        }
    }
}
