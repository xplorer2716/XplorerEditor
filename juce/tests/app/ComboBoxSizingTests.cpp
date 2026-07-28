#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/ComboBoxSizing.hpp"
#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"

#include <algorithm>
#include <set>
#include <string>

// Headless coverage of the shared combo-box font-size policy. The real width
// measurement needs juce::Font, so it is injected: these tests supply a
// deterministic proportional-width stub, which keeps the arithmetic exact and
// lets the always-run linux-headless CI job cover the policy.
// [RQ-GUI-047, RQ-DSN-011, ADR-JUC-021 (DEC-JUC-041..043), issue #12]

using namespace xplorer::app;
using Catch::Approx;

namespace
{
    // A character is CHAR_RATIO * fontSize wide — the simplest measurer that is
    // monotonic in both text length and font size, like a real one.
    constexpr float CHAR_RATIO = 0.5F;

    TextWidthMeasurer fakeMeasurer()
    {
        return [](std::string_view text, float fontSize)
        { return static_cast<float>(text.size()) * CHAR_RATIO * fontSize; };
    }

    constexpr float BASE_SIZE = 16.0F;
    constexpr float MIN_SIZE = 9.0F;
    constexpr int ARROW_ZONE = 30;
    constexpr int LABEL_MARGIN = 10;

    SharedComboBoxFontSize compute(const std::vector<ComboBoxSizingInput>& inputs)
    {
        return computeSharedComboBoxFontSize(inputs, BASE_SIZE, MIN_SIZE, ARROW_ZONE, LABEL_MARGIN,
                                             fakeMeasurer());
    }

    /// Width a box needs so that `chars` characters fit at `fontSize`.
    int widthFor(std::size_t chars, float fontSize)
    {
        return static_cast<int>(static_cast<float>(chars) * CHAR_RATIO * fontSize) + ARROW_ZONE
               + LABEL_MARGIN;
    }
}

SCENARIO("Every combo box shares one font size, set by the most constrained box",
         "[RQ-GUI-047][ADR-JUC-021]")
{
    GIVEN("boxes that all fit their widest label at the base size")
    {
        const std::vector<ComboBoxSizingInput> inputs{
            {"ROOMY_A", widthFor(10, BASE_SIZE), {"SHORT", "ALSO SHORT"}},
            {"ROOMY_B", widthFor(20, BASE_SIZE), {"STILL FITS"}},
        };

        WHEN("the shared size is computed")
        {
            const auto result = compute(inputs);

            THEN("it stays at the base size and nothing needs widening")
            {
                CHECK(result.sharedSize == Approx(BASE_SIZE));
                CHECK(result.idsBelowFloor.empty());
            }
        }
    }

    GIVEN("one narrow box among roomy ones, still legible at its own size")
    {
        // NARROW fits 10 chars at base size but must show 15 → candidate is
        // BASE_SIZE * 10/15 ≈ 10.67, comfortably above the floor, so this
        // scenario exercises the sharing rule without touching the fallback.
        const std::vector<ComboBoxSizingInput> inputs{
            {"ROOMY", widthFor(30, BASE_SIZE), {"SHORT"}},
            {"NARROW", widthFor(10, BASE_SIZE), {std::string(15, 'W')}},
            {"ROOMY_TOO", widthFor(40, BASE_SIZE), {"ALSO SHORT"}},
        };

        WHEN("the shared size is computed")
        {
            const auto result = compute(inputs);

            THEN("the narrow box alone determines the size for all of them")
            {
                CHECK(result.sharedSize == Approx(BASE_SIZE * 10.0F / 15.0F).margin(0.1F));
                CHECK(result.sharedSize > MIN_SIZE); // the floor is not what capped it
                CHECK(result.idsBelowFloor.empty());
            }

            THEN("a roomy box never raises the shared size back up")
            {
                auto without = inputs;
                without.erase(without.begin() + 1); // drop NARROW
                CHECK(compute(without).sharedSize == Approx(BASE_SIZE));
                CHECK(result.sharedSize < compute(without).sharedSize);
            }
        }
    }

    GIVEN("the order of the inputs is reversed")
    {
        std::vector<ComboBoxSizingInput> inputs{
            {"A", widthFor(30, BASE_SIZE), {"SHORT"}},
            {"B", widthFor(12, BASE_SIZE), {std::string(18, 'W')}},
        };
        const auto forward = compute(inputs);
        std::reverse(inputs.begin(), inputs.end());

        THEN("the result does not depend on iteration order")
        {
            CHECK(compute(inputs).sharedSize == Approx(forward.sharedSize));
        }
    }
}

SCENARIO("A box that cannot fit its widest label legibly is reported for widening",
         "[RQ-GUI-047][ADR-JUC-021]")
{
    GIVEN("a box whose widest label overflows even at the legibility floor")
    {
        // Fits 6 chars at base size, must show 40 → candidate ≈ 2.4, under the floor.
        const std::vector<ComboBoxSizingInput> inputs{
            {"ROOMY", widthFor(30, BASE_SIZE), {"SHORT"}},
            {"TOO_TIGHT", widthFor(6, BASE_SIZE), {std::string(40, 'W')}},
        };

        WHEN("the shared size is computed")
        {
            const auto result = compute(inputs);

            THEN("that box is reported, and only that box")
            {
                REQUIRE(result.idsBelowFloor.size() == 1);
                CHECK(result.idsBelowFloor.front() == "TOO_TIGHT");
            }

            THEN("the shared size is clamped at the floor, never below it")
            {
                CHECK(result.sharedSize == Approx(MIN_SIZE));
            }
        }
    }

    GIVEN("a box with no usable text area at all")
    {
        const std::vector<ComboBoxSizingInput> inputs{
            {"ROOMY", widthFor(30, BASE_SIZE), {"SHORT"}},
            {"NO_ROOM", ARROW_ZONE + LABEL_MARGIN, {"ANY"}},
        };

        WHEN("the shared size is computed")
        {
            const auto result = compute(inputs);

            THEN("it is reported for widening without collapsing the shared size")
            {
                REQUIRE(result.idsBelowFloor.size() == 1);
                CHECK(result.idsBelowFloor.front() == "NO_ROOM");
                CHECK(result.sharedSize == Approx(BASE_SIZE)); // ROOMY still fits
            }
        }
    }

    GIVEN("no inputs at all")
    {
        THEN("the base size is returned unchanged")
        {
            const auto result = compute({});
            CHECK(result.sharedSize == Approx(BASE_SIZE));
            CHECK(result.idsBelowFloor.empty());
        }
    }
}

SCENARIO("The real control table yields a usable combo-box sizing inventory",
         "[RQ-GUI-047][ADR-JUC-021]")
{
    GIVEN("the generated control table")
    {
        const auto inputs = collectComboBoxSizingInputs();

        THEN("every collected entry is a real combo box with labels and a width")
        {
            REQUIRE_FALSE(inputs.empty());
            std::set<std::string> ids;
            for (const auto& input : inputs)
            {
                INFO("combo id: " << input.controlId);
                CHECK(ids.insert(input.controlId).second); // no duplicates
                CHECK(input.width > 0);
                CHECK_FALSE(input.labels.empty());
                CHECK_FALSE(comboLabelsForControl(input.controlId).empty());
            }
        }

        THEN("it covers every ComboBoxValuedControl that has a label set")
        {
            std::size_t expected = 0;
            for (const auto& spec : controlTable())
            {
                if (spec.kind == ControlKind::ComboBoxValuedControl
                    && !comboLabelsForControl(spec.id).empty())
                {
                    ++expected;
                }
            }
            CHECK(inputs.size() == expected);
        }

        THEN("well-known combo boxes are present with their reference widths")
        {
            const auto find = [&inputs](const std::string& id)
            {
                return std::find_if(inputs.begin(), inputs.end(),
                                    [&id](const auto& i) { return i.controlId == id; });
            };
            const auto vcfMode = find("VCF_MODE");
            REQUIRE(vcfMode != inputs.end());
            CHECK(vcfMode->width == 127);
            CHECK(vcfMode->labels.front() == "1 POLE LOW");

            const auto modSrc = find("MOD_SRC_1");
            REQUIRE(modSrc != inputs.end());
            CHECK(modSrc->width == 103);
            CHECK(modSrc->labels.back() == "NONE");
        }
    }
}
