#include <catch2/catch_test_macros.hpp>

#include "BackgroundRenderer.hpp"
#include "BlockPalette.hpp"
#include "DesignTokens.hpp"
#include "SectionLayout.hpp"

#include "xplorer/app/ControlTable.hpp"

#include <juce_graphics/juce_graphics.h>

#include <string_view>

// The two things RQ-GUI-051 decided that can silently regress: the stroke-role
// ORDERING (a block frame must stay heavier than a widget frame, lighter than
// the widget stroke role it was split from), and the PAINT ORDER (a signal line
// must never land on the block it runs into).
//
// Neither is checkable by reading a value: the ordering is a relation between
// three tokens any one of which a later edit could move, and the paint order is
// an emergent property of ~250 draw statements. So the first is asserted as a
// relation, and the second by rendering the painter and looking at the pixel
// that used to be wrong.
// [RQ-GUI-051, RQ-DSN-099, ADR-JUC-027 (DEC-JUC-074, DEC-JUC-075)]

using namespace xplorer::app;

SCENARIO("The diagram stroke sits between the widget frame and the widget stroke",
         "[RQ-DSN-099][RQ-GUI-051]")
{
    GIVEN("the three stroke roles of the design system")
    {
        // What the design system actually decided is the ORDERING, not the three
        // numbers — RQ-DSN-099 says so explicitly. A future retune may move any
        // of them; it may not invert them, because "a block reads as stronger
        // than a widget drawn on top of it" is the property being bought.
        THEN("a block frame is heavier than a button/tick-box frame")
        {
            REQUIRE(tokens::semantic::strokeBorder < tokens::semantic::strokeDiagram);
        }

        THEN("and lighter than the control-widget stroke it was split from")
        {
            REQUIRE(tokens::semantic::strokeDiagram < tokens::semantic::strokeLine);
        }

        THEN("the split left the control-widget stroke untouched")
        {
            // DEC-JUC-074: the whole point of adding a role rather than retuning
            // the shared one is that no combo arrow or selector outline moves.
            // If this value ever changes, that promise is broken and the
            // widgets need re-reviewing.
            REQUIRE(tokens::semantic::strokeLine == 2.0F);
        }
    }
}

SCENARIO("Blocks are painted over the signal lines that run into them",
         "[RQ-GUI-051][RQ-GUI-037]")
{
    GIVEN("the vector background rendered at a magnified scale")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        // Magnified so the region under test — the block border, 1.5 logical px
        // wide — spans enough device pixels to sample away from its antialiased
        // edges. The painter is resolution-independent (ADR-JUC-013), so this
        // changes nothing but the sampling headroom.
        constexpr int SCALE = 8;

        juce::Image canvas(juce::Image::ARGB,
                           LOGICAL_CANVAS_WIDTH * SCALE,
                           LOGICAL_CANVAS_HEIGHT * SCALE, true);
        {
            juce::Graphics g{canvas};
            g.addTransform(juce::AffineTransform::scale(static_cast<float>(SCALE)));
            paintVectorBackground(g, defaultBlockPalette());
        }

        WHEN("a block edge that a knob stub ends on is sampled")
        {
            // The VCO1 PWM block is box(234, 60, 52, 23), so its bottom edge is
            // y=83, and stub(259, 83) drops from the middle of that edge. In
            // reading order the stub is emitted after the block, so its rounded
            // end-cap used to paint the neutral FRAME colour over the coloured
            // border here. All coordinates are the painter's diagram space.
            constexpr float BLOCK_BOTTOM_EDGE = 83.0F;
            constexpr float STUB_X = 259.0F;
            // The painter translates the diagram up over the cropped menustrip
            // band: MENUSTRIP_BAND(32) - CANVAS_PADDING(5). Mirrors the constant
            // in BackgroundRenderer.cpp, which is file-local by design.
            constexpr float CANVAS_TOP_CROP = 27.0F;
            // Just inside the edge: within the border stroke (centred on the
            // edge, so it spans +/- 0.75) and within the stub's end-cap reach.
            constexpr float SAMPLE_INSET = 0.4F;

            const int sampleX = static_cast<int>(STUB_X * SCALE);
            const int sampleY = static_cast<int>(
                (BLOCK_BOTTOM_EDGE - SAMPLE_INSET - CANVAS_TOP_CROP) * SCALE);
            const juce::Colour sampled = canvas.getPixelAt(sampleX, sampleY);

            // The border is a top-bright/bottom-dark relief (RQ-DSN-094), and
            // this sample sits at its bottom edge — so the darkened end.
            const juce::Colour blockBorder =
                tokens::semantic::blockVco.darker(tokens::component::blockFrameRelief);
            const juce::Colour neutralLine = tokens::semantic::diagramFrame;

            const auto distanceTo = [&sampled](const juce::Colour& other)
            {
                const auto dr = sampled.getFloatRed() - other.getFloatRed();
                const auto dg = sampled.getFloatGreen() - other.getFloatGreen();
                const auto db = sampled.getFloatBlue() - other.getFloatBlue();
                return (dr * dr) + (dg * dg) + (db * db);
            };

            THEN("the block's border is what shows, not the neutral line colour")
            {
                REQUIRE(distanceTo(blockBorder) < distanceTo(neutralLine));
            }
        }

        WHEN("a block interior is sampled away from any control")
        {
            // Guards the other half of RQ-GUI-051: the fill must actually be
            // visible over the plate. Sampled inside the VCO1 block, clear of
            // its title text and of the border.
            constexpr float INTERIOR_X = 160.0F;   // VCO1 box is (51, 32, 147, 52)
            constexpr float INTERIOR_Y = 40.0F;
            constexpr float CANVAS_TOP_CROP = 27.0F;

            const juce::Colour sampled = canvas.getPixelAt(
                static_cast<int>(INTERIOR_X * SCALE),
                static_cast<int>((INTERIOR_Y - CANVAS_TOP_CROP) * SCALE));

            THEN("it carries the block's hue rather than the bare plate")
            {
                // The plate is near-neutral (r ~= g ~= b); the VCO hue is blue.
                // A fill that reverted to 0.18, or vanished, would fail this by
                // collapsing the blue-over-red margin.
                const auto blueOverRed = sampled.getFloatBlue() - sampled.getFloatRed();
                REQUIRE(blueOverRed > 0.05F);
            }
        }
    }
}

// The tick junction: a control tick SHALL end on the outer edge of the first
// pixel its target paints — touching it, never crossing into it, never stopping
// short. Two things can break that independently, so there are two scenarios.
//
// The painter states the rows it aims at rather than scanning the control table
// on every repaint (SectionLayout.hpp, same reasoning as SECTION_X_MATRIX), so
// the FIRST scenario is what makes that safe: a control that moves fails here
// instead of quietly unpicking its own tick. The SECOND renders the painter and
// looks at the two pixels either side of the ring's edge, which is the property
// itself rather than the arithmetic behind it.
// [RQ-GUI-071, ADR-JUC-027 (DEC-JUC-112)]

namespace
{
    const ControlSpec* specFor(std::string_view id)
    {
        for (const auto& spec : controlTable())
        {
            if (id == spec.id)
            {
                return &spec;
            }
        }
        return nullptr;
    }
}

SCENARIO("Every tick row is the row the control table actually puts its controls on",
         "[RQ-GUI-071]")
{
    GIVEN("the rows BackgroundRenderer aims its knob ticks at")
    {
        // One flat row per knob rather than a row-to-list mapping: a struct with
        // a std::initializer_list member would hold a backing array whose
        // lifetime rules are a trap worth not writing into a regression test.
        struct Aim
        {
            std::string_view id;
            int canvasY;
        };

        const Aim aims[] = {
            {"VCO1_FREQ", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"VCO1_DETUNE", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"VCO1_PW", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"VCO1_VOLUME", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"VCF_FREQ", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"VCF_RES", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"VCF_VCA1_VOLUME", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"VCF_VCA2_VOLUME", layout::KNOB_ROW_TOP_CANVAS_Y},
            {"FM_AMP", layout::KNOB_ROW_FM_CANVAS_Y},
            {"VCO2_VOLUME", layout::KNOB_ROW_VCO2_VCA_CANVAS_Y},
            {"ENV_X_DELAY", layout::KNOB_ROW_ENV_CANVAS_Y},
            {"ENV_X_ATTACK", layout::KNOB_ROW_ENV_CANVAS_Y},
            {"ENV_X_DECAY", layout::KNOB_ROW_ENV_CANVAS_Y},
            {"ENV_X_SUSTAIN", layout::KNOB_ROW_ENV_CANVAS_Y},
            {"ENV_X_RELEASE", layout::KNOB_ROW_ENV_CANVAS_Y},
            {"ENV_X_VOLUME", layout::KNOB_ROW_ENV_CANVAS_Y},
            {"VCO2_FREQ", layout::KNOB_ROW_VCO2_CANVAS_Y},
            {"VCO2_DETUNE", layout::KNOB_ROW_VCO2_CANVAS_Y},
            {"VCO2_PW", layout::KNOB_ROW_VCO2_CANVAS_Y},
            {"LFO_X_SPEED", layout::KNOB_ROW_LFO_CANVAS_Y},
            {"LFO_X_RETRIG", layout::KNOB_ROW_LFO_CANVAS_Y},
            {"LFO_X_AMP", layout::KNOB_ROW_LFO_CANVAS_Y},
            {"FMLAG_RATE", layout::KNOB_ROW_LAG_CANVAS_Y},
            {"RAMP_X_RATE", layout::KNOB_ROW_RAMP_CANVAS_Y},
            {"TRACK_X_PT1", layout::KNOB_ROW_TRACK_CANVAS_Y},
            {"TRACK_X_PT2", layout::KNOB_ROW_TRACK_CANVAS_Y},
            {"TRACK_X_PT3", layout::KNOB_ROW_TRACK_CANVAS_Y},
            {"TRACK_X_PT4", layout::KNOB_ROW_TRACK_CANVAS_Y},
            {"TRACK_X_PT5", layout::KNOB_ROW_TRACK_CANVAS_Y},
        };

        THEN("each knob really sits on the row its tick is derived from")
        {
            for (const auto& aim : aims)
            {
                const auto* spec = specFor(aim.id);
                REQUIRE(spec != nullptr);
                REQUIRE(spec->kind == ControlKind::KnobControl);
                REQUIRE(spec->y == aim.canvasY);
            }
        }
    }

    GIVEN("the two combo boxes a tick lands on")
    {
        const auto* mode = specFor("VCF_MODE");
        const auto* wave = specFor("LFO_X_WAVESHAPE");
        REQUIRE(mode != nullptr);
        REQUIRE(wave != nullptr);

        THEN("the painter's centres are the combos' own centres, not a carried-over x")
        {
            // Both used to be drawn on an x inherited from the reference bitmap,
            // 6.5 px and 4 px left of the combo the tick belongs to.
            const auto centreOf = [](const ControlSpec& s)
            { return static_cast<float>(s.x) + static_cast<float>(s.width) / 2.0F; };

            REQUIRE(centreOf(*mode) == layout::COMBO_VCF_MODE_CENTRE_X);
            REQUIRE(centreOf(*wave) == layout::COMBO_LFO_WAVE_CENTRE_X);
        }

        THEN("the painter's top edges are the combos' own top edges")
        {
            REQUIRE(mode->y == layout::COMBO_VCF_MODE_CANVAS_Y);
            REQUIRE(wave->y == layout::COMBO_LFO_WAVE_CANVAS_Y);
        }

        THEN("a caption on an odd-width combo rounds to the nearer pixel")
        {
            // VCF_MODE is 127 wide, so its centre is x.5 and a truncating cast
            // would put the words half a pixel left of the tick above them.
            REQUIRE(layout::captionCentreX(layout::COMBO_VCF_MODE_CENTRE_X) == 676);
            REQUIRE(layout::captionCentreX(layout::COMBO_LFO_WAVE_CENTRE_X) == 661);
        }
    }
}

SCENARIO("A knob tick reaches the ring and stops there", "[RQ-GUI-071][RQ-GUI-031]")
{
    GIVEN("the vector background rendered at a magnified scale")
    {
        const juce::ScopedJuceInitialiser_GUI juceInit;

        // The painter draws the DIAGRAM only — knobs are separate components — so
        // the tick is the sole thing in its column here, and "how far down does it
        // reach" is unambiguous.
        constexpr int SCALE = 8;

        juce::Image canvas(juce::Image::ARGB,
                           LOGICAL_CANVAS_WIDTH * SCALE,
                           LOGICAL_CANVAS_HEIGHT * SCALE, true);
        {
            juce::Graphics g{canvas};
            g.addTransform(juce::AffineTransform::scale(static_cast<float>(SCALE)));
            paintVectorBackground(g, defaultBlockPalette());
        }

        // The tick is diagramFrame (near-white) over the plate (near-black); half
        // way between the two brightnesses separates them with room to spare, and
        // is derived from the tokens rather than being a tuned literal.
        const float lineThreshold = (tokens::semantic::diagramFrame.getBrightness()
                                     + tokens::semantic::panelPlateMid.getBrightness())
                                    / 2.0F;
        const auto paintedAt = [&](float canvasX, float canvasY)
        {
            return canvas.getPixelAt(static_cast<int>(canvasX * SCALE),
                                     static_cast<int>(canvasY * SCALE))
                       .getBrightness()
                   > lineThreshold;
        };

        // One knob per row, chosen so both old failure modes are represented: the
        // two VCA VOLUMEs stopped SHORT of the ring, the rest crossed INTO it.
        struct Target
        {
            float centreX;
            int knobCanvasTopY;
        };
        const Target targets[] = {
            {432.0F, layout::KNOB_ROW_TOP_CANVAS_Y},        // VCO1_VOLUME — was 1 px short
            {430.0F, layout::KNOB_ROW_VCO2_VCA_CANVAS_Y},   // VCO2_VOLUME — was 3 px short
            {82.0F, layout::KNOB_ROW_TOP_CANVAS_Y},         // VCO1_FREQ   — crossed by 1 px
            {541.0F, layout::KNOB_ROW_TOP_CANVAS_Y},        // VCF_FREQ
            {106.0F, layout::KNOB_ROW_FM_CANVAS_Y},         // FM_AMP
            {541.0F, layout::KNOB_ROW_ENV_CANVAS_Y},        // ENV_X_DELAY
            {82.0F, layout::KNOB_ROW_VCO2_CANVAS_Y},        // VCO2_FREQ
            {759.0F, layout::KNOB_ROW_LFO_CANVAS_Y},        // LFO_X_RETRIG — crossed by 3 px
            {834.0F, layout::KNOB_ROW_LFO_CANVAS_Y},        // LFO_X_AMP    — crossed by 3 px
            {215.0F, layout::KNOB_ROW_LAG_CANVAS_Y},        // FMLAG_RATE   — crossed by 3 px
            {657.0F, layout::KNOB_ROW_RAMP_CANVAS_Y},       // RAMP_X_RATE
            {126.0F, layout::KNOB_ROW_TRACK_CANVAS_Y},      // TRACK_X_PT1  — crossed by 3 px
            {302.0F, layout::KNOB_ROW_TRACK_CANVAS_Y},      // TRACK_X_PT5
        };

        THEN("it is still painted a pixel above the ring, and not one below it")
        {
            for (const auto& target : targets)
            {
                const auto ringTop = static_cast<float>(target.knobCanvasTopY)
                                     + layout::KNOB_RING_TOP_CANVAS_INSET;

                // Reaches: one px clear of the ring the tick still shows.
                REQUIRE(paintedAt(target.centreX, ringTop - 1.0F));
                // Does not cross: one px inside the ring the diagram is gone.
                REQUIRE_FALSE(paintedAt(target.centreX, ringTop + 1.0F));
            }
        }
    }
}
