#include "xplorer/app/ControlMetadata.hpp"

#include <array>
#include <span>
#include <string_view>

namespace xplorer::app
{
    namespace
    {
        struct EnumLabelSet
        {
            std::string_view key;
            std::vector<std::string> labels;
        };

        const std::vector<EnumLabelSet>& enumLabelSets()
        {
            static const std::vector<EnumLabelSet> sets = {
#include "GeneratedEnumLabels.inc"
            };
            return sets;
        }

        struct ComboEnum
        {
            std::string_view controlId;
            std::string_view enumKey;
        };

        constexpr ComboEnum COMBO_ENUM_MAP[] = {
#include "GeneratedComboEnumMap.inc"
        };

        // Every RadioButtonPanel of the control table -- the two fixed-block
        // panels and the two page-family trigger panels. Values are
        // authoritative (child-radio Tag integers from the reference); labels
        // are derived from the child-radio names (the reference bakes them into
        // the background bitmap). Keeping all four here is what
        // "every RadioButtonPanel spec resolves to options" asserts: the two
        // ENV/RAMP rows were missing, so those controls were declared in the
        // table and never built (issue #31).
        // [RQ-GUI-032, RQ-GUI-053, ADR-JUC-016 (DEC-JUC-087)]
        struct RadioPanel
        {
            std::string_view controlId;
            std::vector<std::pair<std::string, int>> options;
        };

        const std::vector<RadioPanel>& radioPanels()
        {
            static const std::vector<RadioPanel> panels = {
                {"FM_DESTINATION", {{"VCO1", 0}, {"VCF", 1}}},
                {"LAG_TIMING_LINEAR_EXPO", {{"LINEAR", 0}, {"EXPO", 1}}},
                // rdSINGLE/rdRampXSingle Tag "0", rdMULTI/rdRampXMulti Tag "1".
                {"ENV_X_TRIG_SINGLE_MULTI", {{"SINGLE", 0}, {"MULTI", 1}}},
                {"RAMP_X_TRIG_SINGLE_MULTI", {{"SINGLE", 0}, {"MULTI", 1}}},
            };
            return panels;
        }

        struct ParameterName
        {
            std::string_view tag;
            std::string_view displayName;
        };

        constexpr ParameterName PARAMETER_NAMES[] = {
#include "GeneratedParameterNames.inc"
        };
    }

    std::vector<std::string> comboLabelsForControl(const std::string& controlId)
    {
        std::string_view enumKey;
        for (const auto& entry : COMBO_ENUM_MAP)
        {
            if (entry.controlId == controlId)
            {
                enumKey = entry.enumKey;
                break;
            }
        }
        if (enumKey.empty())
        {
            return {};
        }
        for (const auto& set : enumLabelSets())
        {
            if (set.key == enumKey)
            {
                return set.labels;
            }
        }
        return {};
    }

    std::vector<std::pair<std::string, int>> radioPanelOptions(const std::string& controlId)
    {
        for (const auto& panel : radioPanels())
        {
            if (panel.controlId == controlId)
            {
                return panel.options;
            }
        }
        return {};
    }

    std::string parameterDisplayName(const std::string& parameterName)
    {
        for (const auto& entry : PARAMETER_NAMES)
        {
            if (entry.tag == parameterName)
            {
                return std::string(entry.displayName);
            }
        }
        return parameterName; // fall back to the raw tag, like the reference
    }
}
