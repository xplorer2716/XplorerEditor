#include <catch2/catch_test_macros.hpp>

#include "BinaryData.h"
#include "DesignTokens.hpp"

#include "xplorer/app/ComboBoxSizing.hpp"

#include <juce_graphics/juce_graphics.h>

#include <string>

// The fit guarantee of RQ-GUI-047, checked against the REAL metrics of the
// embedded typeface on whatever platform this runs on. The headless
// ComboBoxSizingTests cover the predicate's arithmetic with a fake measurer;
// this one answers the different question the fake cannot: do the shipped
// labels actually fit at the shipped size, with the shipped font, here?
//
// It exists because embedding makes the widths deterministic *given a
// rasteriser*, not identical across every text stack: platforms may round or
// hint advances differently. Running this on each target platform's CI turns
// that residual risk into a build failure instead of a truncated label a user
// notices first. It is the deterministic counterpart of the development-build
// assertion (RQ-GUI-048), which only logs when no debugger is attached and so
// cannot fail a CI run on its own.
// [RQ-GUI-047, RQ-GUI-048, RQ-DSN-096, ADR-JUC-022 (DEC-JUC-050)]

using namespace xplorer::app;

SCENARIO("Every combo-box label fits at the shipped size in the embedded face",
         "[RQ-GUI-047][RQ-GUI-048][RQ-DSN-096]")
{
    GIVEN("the embedded Roboto Condensed face at the combo-box token size")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        auto typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::RobotoCondensedRegular_ttf, BinaryData::RobotoCondensedRegular_ttfSize);
        REQUIRE(typeface != nullptr);

        const juce::Font font{
            juce::FontOptions{typeface}.withHeight(tokens::semantic::comboTextSize)};

        // Deliberately the same (deprecated) measurement the production code
        // uses, so this test validates what actually ships rather than a second
        // opinion. Migrating both to GlyphArrangement is a separate change.
        JUCE_BEGIN_IGNORE_DEPRECATION_WARNINGS
        const auto measure = [&font](std::string_view text, float)
        { return font.getStringWidthFloat(juce::String(std::string(text))); };
        JUCE_END_IGNORE_DEPRECATION_WARNINGS

        WHEN("every combo box of the control table is checked")
        {
            const auto inputs = collectComboBoxSizingInputs();
            REQUIRE_FALSE(inputs.empty());

            const auto overflowing =
                comboBoxesOverflowingAt(inputs, tokens::semantic::comboTextSize,
                                        tokens::semantic::comboArrowZone,
                                        tokens::semantic::comboLabelBorder, measure);

            THEN("none of them truncates its widest label")
            {
                for (const auto& id : overflowing)
                {
                    INFO("combo box that no longer fits: " << id);
                }
                CHECK(overflowing.empty());
            }

            THEN("each one keeps a usable margin, so small metric differences are absorbed")
            {
                const auto overhead = tokens::semantic::comboArrowZone
                                      + 2 * tokens::semantic::comboLabelBorder;
                for (const auto& input : inputs)
                {
                    float widest = 0.0F;
                    std::string widestLabel;
                    for (const auto& label : input.labels)
                    {
                        const float w = measure(label, tokens::semantic::comboTextSize);
                        if (w > widest)
                        {
                            widest = w;
                            widestLabel = label;
                        }
                    }
                    const auto available = static_cast<float>(input.width - overhead);
                    INFO(input.controlId << " widest=\"" << widestLabel << "\" needs " << widest
                                         << " of " << available << " px");
                    CHECK(widest <= available);
                }
            }
        }
    }
}
