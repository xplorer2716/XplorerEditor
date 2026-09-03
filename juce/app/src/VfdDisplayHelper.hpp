#pragma once

// VFD content builder: ports Xplorer.View.VfdDisplayHelper. Formats the
// program/name line plus the last-edited control or modulation entry, and
// pushes the resulting multi-line text into the DisplayPanel. Rendering stays
// the simplified text block (owner decision); only the content follows the
// reference. [RQ-GUI-020]

#include "DisplayPanel.hpp"

#include "xplorer/controller/XpanderController.hpp"
#include "xplorer/model/ModulationMatrixEntry.hpp"

#include <string>

namespace xplorer::app
{
    class VfdDisplayHelper
    {
    public:
        VfdDisplayHelper(DisplayPanel& display, controller::XpanderController& controller);

        /// Program number + patch name only (on load / program change).
        void showToneInfo();

        /// A control was tweaked: "NAME:value" (+ MIDI CC line), value already
        /// formatted per control type by the caller.
        void showControlEdit(const std::string& parameterName, const std::string& valueText);

        /// A modulation matrix entry changed: "SRC TO DEST:" / "AMNT:.. QTZ:..",
        /// or the max-source-reached notice.
        void showModulationEntry(const model::ModulationMatrixEntry& entry, bool maxSourceReached);

        /// Character test: display alphabet and symbols for VFD alignment
        /// verification. Each argument is wrapped independently onto as many
        /// display rows as it needs, at the panel's actual column count
        /// (DisplayPanel::maxCharsPerLine(), not a hard-coded width), so
        /// content longer than one row never gets silently clipped to it.
        /// [RQ-GUI-082]
        void displayCharacters(const std::string& line1, const std::string& line2);

    private:
        [[nodiscard]] juce::String toneLine() const;
        [[nodiscard]] bool isActiveModulationDestination(const std::string& parameterName) const;

        DisplayPanel& _display;
        controller::XpanderController& _controller;
    };
}
