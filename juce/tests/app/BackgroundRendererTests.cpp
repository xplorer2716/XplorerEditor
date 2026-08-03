#include <catch2/catch_test_macros.hpp>

#include "BackgroundRenderer.hpp"
#include "BlockPalette.hpp"
#include "DesignTokens.hpp"

#include "xplorer/app/ControlTable.hpp"

#include <juce_graphics/juce_graphics.h>

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
            // the shared one is that no combo arrow, focus ring or selector
            // outline moves. If this value ever changes, that promise is broken
            // and the widgets need re-reviewing.
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
