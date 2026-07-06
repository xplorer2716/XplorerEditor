#pragma once

// Simplified VFD display: fixed-width multiline text showing the program
// number + patch name and the last edited parameter. Functional-first
// (RQ-GUI-020); hardware-accurate dot-matrix rendering is the skin pass
// (TASK-JUCE-069). Port of VfdDisplayHelper's text content. [RQ-GUI-020]

#include <juce_gui_extra/juce_gui_extra.h>

#include <string>

namespace xplorer::app
{
    class DisplayPanel final : public juce::Component
    {
    public:
        DisplayPanel();

        /// Line 1: "* Snn NAME *" (reference format).
        void showToneInfo(int programNumber, const std::string& toneName);
        /// Line 2: "NAME:value" on a parameter edit.
        void showParameter(const std::string& parameterName, int value);

        void paint(juce::Graphics& g) override;

    private:
        juce::String _line1;
        juce::String _line2;
    };
}
