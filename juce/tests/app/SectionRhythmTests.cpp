#include <catch2/catch_test_macros.hpp>

#include "DesignTokens.hpp"
#include "SectionLayout.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"

#include <algorithm>
#include <initializer_list>
#include <limits>
#include <string_view>

// The vertical rhythm of the section separators. What this suite protects is a
// RELATION, not a set of coordinates: a separator bar terminates the section
// above it (ADR-JUC-034, DEC-JUC-107), so it must sit closer to that section
// than to the one below. Before RQ-CLR-001 the layout said the opposite —
// 17-43 px above each bar against a uniform 9-11 px below — and it got there
// one uncontrolled edit at a time, because nothing failed when a coordinate
// drifted.
//
// So the assertions read the SAME constants the painter draws from
// (SectionLayout.hpp) and derive every section's extent from the control table
// rather than restating it. A test that repeated the numbers would only prove
// it could copy them.
// [RQ-CLR-001, RQ-CLR-002, RQ-CLR-003, RQ-CLR-004, RQ-CLR-010,
//  ADR-CLR-001 (DEC-CLR-001-D)]

using namespace xplorer::app;

namespace
{
    constexpr int NO_CAPTION = std::numeric_limits<int>::min();

    [[nodiscard]] const ControlSpec& control(std::string_view id)
    {
        const auto table = controlTable();
        const auto it = std::find_if(table.begin(), table.end(),
                                     [id](const ControlSpec& s) { return id == s.id; });
        REQUIRE(it != table.end());
        return *it;
    }

    /// Where a control's INK ends, which is not always where its bounds do.
    ///
    /// RQ-CLR-001 measures to the lowest *visible* element, and a stacked
    /// RadioButtonPanel paints less than it declares: BoundRadioGroup::resized
    /// gives each option a slot of `height / N` but draws it at
    /// `controlRowHeight`, so the last row's ink stops at
    /// `y + (N-1) * (height / N) + controlRowHeight`. LAG_TIMING (79x47, two
    /// options) therefore ends at y+40, not y+47 — seven px that are never
    /// painted. Measuring to the bounds put LAG's bar 13 px above its section
    /// instead of 16 and would have shifted the whole group to compensate for
    /// empty space. A side-by-side panel is its own row and needs no such
    /// correction. [RQ-CLR-001, RQ-GUI-040, ADR-JUC-016 (DEC-JUC-086, DEC-JUC-094)]
    [[nodiscard]] int paintedBottom(const ControlSpec& spec)
    {
        if (spec.kind != ControlKind::RadioButtonPanel)
            return spec.y + spec.height;

        const auto optionCount = static_cast<int>(radioPanelOptions(spec.id).size());
        if (optionCount <= 0 || spec.height < optionCount * tokens::semantic::controlRowHeight)
            return spec.y + spec.height; // side by side: the panel IS the row

        const int slotHeight = spec.height / optionCount;
        return spec.y + (optionCount - 1) * slotHeight + tokens::semantic::controlRowHeight;
    }

    /// Bottom of a section: the lowest ink of any control it owns, or a
    /// painter-drawn reference when that sits lower still (LAG ends on its RATE
    /// caption, TRACK on its PT captions, RAMP on its TRIGGER frame).
    [[nodiscard]] int sectionBottom(std::initializer_list<const char*> members, int captionBottom)
    {
        int bottom = captionBottom;
        for (const auto* id : members)
            bottom = std::max(bottom, paintedBottom(control(id)));
        return bottom;
    }

    /// Top of the first visible element of a section.
    [[nodiscard]] int sectionTop(std::initializer_list<const char*> members)
    {
        int top = std::numeric_limits<int>::max();
        for (const auto* id : members)
            top = std::min(top, control(id).y);
        return top;
    }

    // --- the two columns, top to bottom -----------------------------------
    // Each entry: the section's own bottom, then its separator's anchor, then
    // the top of the section that follows it.

    const auto VCO_BOTTOM = [] {
        return sectionBottom({"VCO2_MOD_KEYB", "VCO2_MOD_LAG", "VCO2_MOD_LEV1",
                              "VCO2_MOD_VIB", "VCO2_WAVE_SYNC"}, NO_CAPTION);
    };
    const auto LAG_BOTTOM = [] {
        return sectionBottom({"LAG_IN", "LAG_TIMING_LINEAR_EXPO", "LAG_MODE_LEGATO",
                              "LAG_LINEAR_EQUAL_TIME", "FMLAG_RATE"},
                             layout::LAG_RATE_CAPTION_BASELINE_CANVAS_Y);
    };
    const auto TRACK_BOTTOM = [] {
        return sectionBottom({"TRACK_X_IN", "TRACK_X_PT1", "TRACK_X_PT5"},
                             layout::TRACK_PT_CAPTION_BASELINE_CANVAS_Y);
    };
    const auto VCF_BOTTOM = [] {
        return sectionBottom({"VCF_MOD_KEYB", "VCF_MOD_LAG", "VCF_MOD_LEV1", "VCF_MOD_VIB"},
                             NO_CAPTION);
    };
    const auto ENV_BOTTOM = [] {
        return sectionBottom({"ENV_X_MODE_RESET", "ENV_X_MODE_FREERUN", "ENV_X_MODE_DADR"},
                             NO_CAPTION);
    };
    const auto LFO_BOTTOM = [] {
        return sectionBottom({"LFO_X_LAG", "LFO_X_SAMPLE_INPUT", "LFO_X_RETRIG_MODE"},
                             NO_CAPTION);
    };
    const auto RAMP_BOTTOM = [] {
        return sectionBottom({"RAMP_X_TRIG_EXTRIG", "RAMP_X_TRIG_GATED",
                              "RAMP_X_TRIG_SINGLE_MULTI"},
                             layout::RAMP_TRIGGER_FRAME_BOTTOM_CANVAS_Y);
    };

    const auto TRACK_TOP = [] { return sectionTop({"TRACK_1", "TRACK_2", "TRACK_3"}); };
    const auto ENV_TOP = [] { return sectionTop({"ENV_1", "ENV_2", "ENV_3", "ENV_4", "ENV_5"}); };
    const auto LFO_TOP = [] { return sectionTop({"LFO_1", "LFO_2", "LFO_3", "LFO_4", "LFO_5"}); };
    const auto RAMP_TOP = [] { return sectionTop({"RAMP_1", "RAMP_2", "RAMP_3", "RAMP_4"}); };
}

SCENARIO("A separator bar sits one fixed gap below the section it terminates",
         "[RQ-CLR-001][TASK-CLR-001]")
{
    GIVEN("the left column's three sections")
    {
        THEN("VCO1/VCO2/FM clears the VCO2 MOD row by sectionGapAbove")
        {
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_VCO_Y) - VCO_BOTTOM()
                    == tokens::component::sectionGapAbove);
        }

        THEN("LAG clears its RATE caption by sectionGapAbove")
        {
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_LAG_Y) - LAG_BOTTOM()
                    == tokens::component::sectionGapAbove);
        }

        THEN("TRACK X clears its PT captions by sectionGapAbove")
        {
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_TRACK_Y) - TRACK_BOTTOM()
                    == tokens::component::sectionGapAbove);
        }
    }
}

SCENARIO("The next section starts farther from the bar than the last one ended",
         "[RQ-CLR-002][TASK-CLR-001]")
{
    GIVEN("the rhythm's two tokens")
    {
        // The relation, not the values, is what the design bought. A later
        // retune may move both; it may not invert them, or the bar goes back to
        // reading as a heading for the section below it.
        THEN("the gap below a bar is required to exceed the gap above it")
        {
            REQUIRE(tokens::component::sectionGapBelowMin > tokens::component::sectionGapAbove);
        }
    }

    GIVEN("the left column's two internal section boundaries")
    {
        THEN("the LAG block starts at least sectionGapBelowMin below the VCO bar")
        {
            REQUIRE(layout::LAG_FRAME_TOP_CANVAS_Y
                        - layout::sectionBaselineCanvasY(layout::SECTION_VCO_Y)
                    >= tokens::component::sectionGapBelowMin);
        }

        THEN("the TRACK buttons start at least sectionGapBelowMin below the LAG bar")
        {
            REQUIRE(TRACK_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_LAG_Y)
                    >= tokens::component::sectionGapBelowMin);
        }
    }
}

SCENARIO("A column's leftover space is shared evenly between its section boundaries",
         "[RQ-CLR-004][TASK-CLR-001]")
{
    GIVEN("the left column's two below-separator gaps")
    {
        const int afterVco = layout::LAG_FRAME_TOP_CANVAS_Y
                             - layout::sectionBaselineCanvasY(layout::SECTION_VCO_Y);
        const int afterLag = TRACK_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_LAG_Y);

        // One px, not zero: the slack rarely divides exactly by the number of
        // gaps, and rounding has to land somewhere. [RQ-CLR-004]
        THEN("they differ by at most one pixel")
        {
            REQUIRE(std::abs(afterVco - afterLag) <= 1);
        }
    }
}

SCENARIO("The canvas floor is fixed", "[RQ-CLR-003][TASK-CLR-001]")
{
    GIVEN("the three bottom separators")
    {
        THEN("TRACK X, RAMP X and MOD MATRIX share one label baseline")
        {
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_RAMP_Y)
                    == layout::sectionBaselineCanvasY(layout::SECTION_TRACK_Y));
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_MATRIX_Y)
                    == layout::sectionBaselineCanvasY(layout::SECTION_TRACK_Y));
        }

        THEN("that baseline is the one the rhythm was computed against")
        {
            REQUIRE(layout::BOTTOM_SECTION_BASELINE_CANVAS_Y == 776);
        }

        THEN("the canvas has not grown to make the rhythm fit")
        {
            REQUIRE(LOGICAL_CANVAS_HEIGHT == 786);
        }
    }
}

// The centre column is the tight one: 633 px between the VCF group's bottom and
// the RAMP X baseline, of which the ENV, LFO and RAMP groups take 508. That is
// what fixed sectionGapAbove at 16 — see ADR-CLR-001 (DEC-CLR-001-B) — so these
// four sections are where a later retune breaks first.

SCENARIO("The centre column follows the same rhythm",
         "[RQ-CLR-001][RQ-CLR-002][TASK-CLR-002]")
{
    GIVEN("its four sections")
    {
        THEN("each bar clears its own section by sectionGapAbove")
        {
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_VCF_Y) - VCF_BOTTOM()
                    == tokens::component::sectionGapAbove);
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_ENV_Y) - ENV_BOTTOM()
                    == tokens::component::sectionGapAbove);
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_LFO_Y) - LFO_BOTTOM()
                    == tokens::component::sectionGapAbove);
            REQUIRE(layout::sectionBaselineCanvasY(layout::SECTION_RAMP_Y) - RAMP_BOTTOM()
                    == tokens::component::sectionGapAbove);
        }

        THEN("and each following section starts at least sectionGapBelowMin away")
        {
            REQUIRE(ENV_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_VCF_Y)
                    >= tokens::component::sectionGapBelowMin);
            REQUIRE(LFO_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_ENV_Y)
                    >= tokens::component::sectionGapBelowMin);
            REQUIRE(RAMP_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_LFO_Y)
                    >= tokens::component::sectionGapBelowMin);
        }

        THEN("and its three gaps are shared evenly")
        {
            const int a = ENV_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_VCF_Y);
            const int b = LFO_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_ENV_Y);
            const int c = RAMP_TOP() - layout::sectionBaselineCanvasY(layout::SECTION_LFO_Y);
            REQUIRE(std::max({a, b, c}) - std::min({a, b, c}) <= 1);
        }
    }
}
