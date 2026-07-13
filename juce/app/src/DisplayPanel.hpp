#pragma once

// Simplified VFD display: a fixed-width multiline text block. Functional-first
// (RQ-GUI-020); hardware-accurate dot-matrix rendering is the skin pass
// (TASK-JUCE-069). The line content is built by VfdDisplayHelper, a port of
// the reference VfdDisplayHelper. [RQ-GUI-020]

#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class DisplayPanel final : public juce::Component
    {
    public:
        /// Characters per line before the helper wraps to the next line,
        /// mirroring the reference VacuumFluoDisplayControl.MaxCharsPerLine.
        static constexpr int MAX_CHARS_PER_LINE = 24;
        /// Number of text rows the display shows (reference layout).
        static constexpr int LINE_COUNT = 5;

        DisplayPanel();

        /// Replaces the whole text block (up to LINE_COUNT lines).
        void setLines(juce::StringArray lines);

        void paint(juce::Graphics& g) override;

    private:
        juce::StringArray _lines;
    };
}
