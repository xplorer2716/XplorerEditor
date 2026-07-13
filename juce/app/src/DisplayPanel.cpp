#include "DisplayPanel.hpp"

namespace xplorer::app
{
    DisplayPanel::DisplayPanel()
    {
        setLines({});
    }

    void DisplayPanel::setLines(juce::StringArray lines)
    {
        _lines = std::move(lines);
        repaint();
    }

    void DisplayPanel::paint(juce::Graphics& g)
    {
        // Green-on-black, monospaced — evokes the hardware VFD. [RQ-GUI-020]
        g.fillAll(juce::Colour::fromRGB(6, 20, 6));
        g.setColour(juce::Colour::fromRGB(120, 255, 120));

        const auto bounds = getLocalBounds().reduced(6);
        const int rowHeight = juce::jmax(1, bounds.getHeight() / LINE_COUNT);
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                             juce::jmax(9.0F, static_cast<float>(rowHeight) * 0.8F),
                             juce::Font::bold));

        for (int row = 0; row < LINE_COUNT && row < _lines.size(); ++row)
        {
            g.drawText(_lines[row], bounds.getX(), bounds.getY() + row * rowHeight,
                       bounds.getWidth(), rowHeight, juce::Justification::centredLeft);
        }
    }
}
