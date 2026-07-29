#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/ComboBoxSizing.hpp"
#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <string>

// Headless guards for the combo-box fit and width rules of RQ-GUI-047. The real
// width measurement needs juce::Font, so it is injected: these tests supply a
// deterministic proportional-width stub, which keeps the arithmetic exact and
// lets the always-run linux-headless CI job cover the predicate. The check
// against the *real* embedded-face metrics is the development-build assertion
// of RQ-GUI-048, which lives on the JUCE side.
// [RQ-GUI-047, RQ-GUI-048, ADR-JUC-022 (DEC-JUC-050), issue #12]

using namespace xplorer::app;

namespace
{
    // A character is CHAR_RATIO * fontSize wide — the simplest measurer that is
    // monotonic in both text length and font size, like a real one.
    constexpr float CHAR_RATIO = 0.5F;
    constexpr float SIZE = 12.0F;
    constexpr int ARROW_ZONE = 20;
    constexpr int LABEL_BORDER = 2;
    constexpr int OVERHEAD = ARROW_ZONE + 2 * LABEL_BORDER;

    TextWidthMeasurer fakeMeasurer()
    {
        return [](std::string_view text, float fontSize)
        { return static_cast<float>(text.size()) * CHAR_RATIO * fontSize; };
    }

    std::vector<std::string> overflowing(const std::vector<ComboBoxSizingInput>& inputs)
    {
        return comboBoxesOverflowingAt(inputs, SIZE, ARROW_ZONE, LABEL_BORDER, fakeMeasurer());
    }

    /// Width that exactly fits `chars` characters at SIZE.
    int widthFor(std::size_t chars)
    {
        return static_cast<int>(static_cast<float>(chars) * CHAR_RATIO * SIZE) + OVERHEAD;
    }
}

SCENARIO("Combo boxes whose widest label does not fit are reported", "[RQ-GUI-047][RQ-GUI-048]")
{
    GIVEN("boxes that all fit their widest label")
    {
        const std::vector<ComboBoxSizingInput> inputs{
            {"ROOMY", widthFor(20), {"SHORT", "ALSO SHORT"}},
            {"EXACT", widthFor(6), {"WWWWWW"}}, // fits exactly, must not be reported
        };

        THEN("nothing is reported")
        {
            CHECK(overflowing(inputs).empty());
        }
    }

    GIVEN("one box too narrow for its longest label")
    {
        const std::vector<ComboBoxSizingInput> inputs{
            {"ROOMY", widthFor(20), {"SHORT"}},
            {"TOO_TIGHT", widthFor(6), {"SHORT", std::string(12, 'W')}},
            {"ROOMY_TOO", widthFor(30), {"ALSO SHORT"}},
        };

        WHEN("the boxes are checked")
        {
            const auto result = overflowing(inputs);

            THEN("that box is reported, and only that box")
            {
                REQUIRE(result.size() == 1);
                CHECK(result.front() == "TOO_TIGHT");
            }

            THEN("it is the longest label that decides, not the selected one")
            {
                // Same box, same width, but without its long entry.
                std::vector<ComboBoxSizingInput> shortened = inputs;
                shortened[1].labels = {"SHORT"};
                CHECK(overflowing(shortened).empty());
            }
        }
    }

    GIVEN("a box with no usable text area at all")
    {
        const std::vector<ComboBoxSizingInput> inputs{{"NO_ROOM", OVERHEAD, {"ANY"}}};

        THEN("it is reported rather than silently accepted")
        {
            REQUIRE(overflowing(inputs).size() == 1);
            CHECK(overflowing(inputs).front() == "NO_ROOM");
        }
    }

    GIVEN("no inputs at all")
    {
        THEN("nothing is reported")
        {
            CHECK(overflowing({}).empty());
        }
    }
}

SCENARIO("The real control table yields a usable combo-box inventory",
         "[RQ-GUI-047][ADR-JUC-022]")
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

SCENARIO("Combo boxes that display the same values share one width",
         "[RQ-GUI-047][ADR-JUC-022]")
{
    // RQ-GUI-047's group rule, which the requirement otherwise states only in
    // prose: a group is the set of combo boxes showing the same value list, and
    // a group has a single width. Grouping by the value list rather than by
    // panel location is what makes the rule self-maintaining — a combo box
    // added later lands in the right group automatically.
    GIVEN("the combo boxes grouped by their value list")
    {
        std::map<std::vector<std::string>, std::map<int, std::vector<std::string>>> byLabels;
        for (const auto& input : collectComboBoxSizingInputs())
        {
            byLabels[input.labels][input.width].push_back(input.controlId);
        }

        THEN("every group has exactly one width")
        {
            REQUIRE_FALSE(byLabels.empty());
            for (const auto& [labels, widths] : byLabels)
            {
                INFO("group whose first label is: " << labels.front());
                for (const auto& [width, ids] : widths)
                {
                    INFO("  width " << width << " used by " << ids.size() << " control(s), e.g. "
                                    << ids.front());
                }
                CHECK(widths.size() == 1);
            }
        }

        THEN("the expected groups are present with their reference widths")
        {
            const auto widthOf = [&byLabels](const std::string& probeId) -> int
            {
                const auto labels = comboLabelsForControl(probeId);
                const auto it = byLabels.find(labels);
                return it == byLabels.end() ? -1 : it->second.begin()->first;
            };
            CHECK(widthOf("LAG_IN") == 67);                // + TRACK_X_IN, LFO_X_SAMPLE_INPUT
            CHECK(widthOf("ENV_X_TRIG_LFO_SOURCE") == 65); // + RAMP_X_TRIG_LFO_SOURCE
            CHECK(widthOf("MOD_SRC_1") == 103);            // x20
            CHECK(widthOf("MOD_DEST_1") == 103);           // x20
            CHECK(widthOf("VCF_MODE") == 127);
            CHECK(widthOf("LFO_X_WAVESHAPE") == 100);
            CHECK(widthOf("LFO_X_RETRIG_MODE") == 65);
        }
    }
}
