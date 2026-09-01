// DisplayPanel content normalisation. [RQ-GUI-049, PLAN-GUI-015]

#include "DisplayPanel.hpp"

#include <catch2/catch_test_macros.hpp>

#include <juce_gui_extra/juce_gui_extra.h>

#include <string>

using namespace xplorer::app;

namespace
{
    // Reference logical bounds of the extracted VFD control (ADR-JUC-007).
    constexpr int PANEL_WIDTH = 267;
    constexpr int PANEL_HEIGHT = 82;

    std::string signature(const juce::Image& image)
    {
        const juce::Image::BitmapData pixels(image, juce::Image::BitmapData::readOnly);
        std::string sig;
        sig.reserve(static_cast<std::size_t>(image.getWidth() * image.getHeight()));
        for (int y = 0; y < image.getHeight(); ++y)
        {
            for (int x = 0; x < image.getWidth(); ++x)
            {
                sig += static_cast<char>(pixels.getPixelColour(x, y).getGreen());
            }
        }
        return sig;
    }
}

SCENARIO("DisplayPanel uppercases every line before rendering", "[RQ-GUI-049]")
{
    const juce::ScopedJuceInitialiser_GUI juceInit;

    GIVEN("a panel at the reference bounds")
    {
        DisplayPanel panel;
        panel.setBounds(0, 0, PANEL_WIDTH, PANEL_HEIGHT);

        WHEN("the same text is set once in lowercase and once in uppercase")
        {
            panel.setLines({"tone name"});
            const auto lower = signature(panel.createComponentSnapshot(panel.getLocalBounds()));

            panel.setLines({"placeholder"}); // force the next call to be a real change
            panel.setLines({"TONE NAME"});
            const auto upper = signature(panel.createComponentSnapshot(panel.getLocalBounds()));

            THEN("they render identically")
            {
                // The physical Xpander VFD has no lowercase capability;
                // DisplayPanel::setLines uppercases so a lowercase tone name
                // (e.g. from an externally authored .syx file) displays the
                // way the real unit would, rather than in a distinct
                // lowercase glyph style.
                REQUIRE(lower == upper);
            }
        }

        WHEN("mixed-case text is set")
        {
            panel.setLines({"Xpander"});
            const auto mixed = signature(panel.createComponentSnapshot(panel.getLocalBounds()));

            panel.setLines({"placeholder"});
            panel.setLines({"XPANDER"});
            const auto upper = signature(panel.createComponentSnapshot(panel.getLocalBounds()));

            THEN("it renders as if it had been all-uppercase")
            {
                REQUIRE(mixed == upper);
            }
        }
    }
}
