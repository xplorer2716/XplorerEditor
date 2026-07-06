#include "DisplayPanel.hpp"

#include <iomanip>
#include <sstream>

namespace xplorer::app
{
    DisplayPanel::DisplayPanel()
    {
        showToneInfo(0, "");
    }

    void DisplayPanel::showToneInfo(int programNumber, const std::string& toneName)
    {
        std::ostringstream line;
        line << "* S" << std::setw(2) << std::setfill('0') << programNumber << ' ' << toneName << " *";
        _line1 = juce::String(line.str());
        repaint();
    }

    void DisplayPanel::showParameter(const std::string& parameterName, int value)
    {
        _line2 = juce::String(parameterName) + ":" + juce::String(value);
        repaint();
    }

    void DisplayPanel::paint(juce::Graphics& g)
    {
        // Green-on-black, monospaced — evokes the hardware VFD. [RQ-GUI-020]
        g.fillAll(juce::Colour::fromRGB(6, 20, 6));
        g.setColour(juce::Colour::fromRGB(120, 255, 120));
        g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                             juce::jmax(10.0F, static_cast<float>(getHeight()) * 0.22F),
                             juce::Font::bold));
        auto bounds = getLocalBounds().reduced(6);
        g.drawText(_line1, bounds.removeFromTop(bounds.getHeight() / 3), juce::Justification::centredLeft);
        g.drawText(_line2, bounds.removeFromTop(bounds.getHeight() / 2), juce::Justification::centredLeft);
    }
}
