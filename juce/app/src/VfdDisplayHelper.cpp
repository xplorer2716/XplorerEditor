#include "VfdDisplayHelper.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/MidiAutomationTable.hpp"
#include "xplorer/app/ModulationHighlight.hpp"

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

    bool VfdDisplayHelper::isActiveModulationDestination(const std::string& parameterName) const
    {
        const auto destination = modulationDestinationForParameter(parameterName);
        if (!destination)
        {
            return false;
        }
        // A matrix entry targets this destination with an active source.
        for (int entryNumber = 1; entryNumber <= model::constants::MODENTRIES_COUNT; ++entryNumber)
        {
            const auto& entry = _controller.getModulationEntryByNumber(entryNumber);
            if (entry.destination == *destination
                && entry.source != model::EnumModulationSourcesModMatrix::NONE)
            {
                return true;
            }
        }
        return false;
    }

    void VfdDisplayHelper::showControlEdit(const std::string& parameterName, const std::string& valueText)
    {
        // Reference: append "." to the name when the parameter is an active
        // modulation destination. [RQ-GUI-020]
        const juce::String displayName = juce::String(parameterDisplayName(parameterName))
                                         + (isActiveModulationDestination(parameterName) ? "." : "");
        const juce::String value(valueText);

        // Single line when it fits, otherwise name on line 2 and value on line 3.
        juce::String line2 = displayName + ":" + value;
        juce::String line3;
        if (line2.length() > _display.maxCharsPerLine())
        {
            line2 = displayName + ":";
            line3 = value;
        }

        // unassignedControlChange() (128, the last "None" entry) is the
        // sentinel the reference stores for parameters with no mapped CC
        // (reference: ccNumber != ControlChangesNames.Names.Length-1); it must
        // not be shown as a real CC number. [RQ-GUI-020, issue #11]
        juce::String ccLine;
        if (const auto cc = _controller.controlChangeAutomationTable().ccNumberFor(parameterName);
            cc && *cc != unassignedControlChange())
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
