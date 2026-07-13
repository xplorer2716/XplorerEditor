#include "VfdDisplayHelper.hpp"

#include "xplorer/app/ControlMetadata.hpp"

namespace xplorer::app
{
    namespace
    {
        // Enum display labels for the matrix (ordinal == enum value); the row-1
        // control ids resolve to the ModulationSourcesModMatrix / Modulation-
        // Destination label sets. Cached: they never change.
        const std::vector<std::string>& sourceLabels()
        {
            static const std::vector<std::string> labels = comboLabelsForControl("MOD_SRC_1");
            return labels;
        }
        const std::vector<std::string>& destinationLabels()
        {
            static const std::vector<std::string> labels = comboLabelsForControl("MOD_DEST_1");
            return labels;
        }
        juce::String labelAt(const std::vector<std::string>& labels, int index)
        {
            return (index >= 0 && index < static_cast<int>(labels.size()))
                       ? juce::String(labels[static_cast<std::size_t>(index)])
                       : juce::String();
        }
    }

    VfdDisplayHelper::VfdDisplayHelper(DisplayPanel& display, controller::XpanderController& controller)
        : _display(display), _controller(controller)
    {
        showToneInfo();
    }

    juce::String VfdDisplayHelper::toneLine() const
    {
        // Reference: "* S{prog:00} {name} *".
        return juce::String::formatted("* S%02d ", _controller.currentProgramNumber())
               + juce::String(_controller.toneName()) + " *";
    }

    void VfdDisplayHelper::showToneInfo()
    {
        _display.setLines({toneLine()});
    }

    void VfdDisplayHelper::showControlEdit(const std::string& parameterName, const std::string& valueText)
    {
        const juce::String displayName(parameterDisplayName(parameterName));
        const juce::String value(valueText);

        // Single line when it fits, otherwise name on line 2 and value on line 3.
        juce::String line2 = displayName + ":" + value;
        juce::String line3;
        if (line2.length() > _display.maxCharsPerLine())
        {
            line2 = displayName + ":";
            line3 = value;
        }

        juce::String ccLine;
        if (const auto cc = _controller.controlChangeAutomationTable().ccNumberFor(parameterName))
        {
            ccLine = juce::String::formatted("MIDI CC: %03d", *cc);
        }

        _display.setLines({toneLine(), line2, line3, juce::String(), ccLine});
    }

    void VfdDisplayHelper::showModulationEntry(const model::ModulationMatrixEntry& entry,
                                               bool maxSourceReached)
    {
        juce::String line2;
        juce::String line3;
        if (entry.source != model::EnumModulationSourcesModMatrix::NONE)
        {
            line2 = labelAt(sourceLabels(), static_cast<int>(entry.source)) + " TO "
                    + labelAt(destinationLabels(), static_cast<int>(entry.destination)) + ":";
            line3 = juce::String::formatted("AMNT:%02d QTZ:", entry.amount())
                    + (entry.quantize() == 1 ? "Y" : "N");
        }
        else if (maxSourceReached)
        {
            line2 = "MAX SRC COUNT REACHED FOR";
            line3 = labelAt(destinationLabels(), static_cast<int>(entry.destination));
        }

        _display.setLines({toneLine(), line2, line3});
    }
}
