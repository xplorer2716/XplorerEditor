#include "xplorer/app/ComboBoxSizing.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"

#include <algorithm>

namespace xplorer::app
{
    std::vector<ComboBoxSizingInput> collectComboBoxSizingInputs()
    {
        // [RQ-GUI-047, ADR-JUC-021 (DEC-JUC-041)]
        std::vector<ComboBoxSizingInput> inputs;
        for (const auto& spec : controlTable())
        {
            if (spec.kind != ControlKind::ComboBoxValuedControl)
            {
                continue;
            }
            auto labels = comboLabelsForControl(spec.id);
            if (labels.empty())
            {
                continue; // no items: nothing to fit, cannot constrain the size
            }
            inputs.push_back({spec.id, spec.width, std::move(labels)});
        }
        return inputs;
    }

    std::vector<std::string> comboBoxesOverflowingAt(
        const std::vector<ComboBoxSizingInput>& inputs, float fontSize, int arrowZone,
        int labelBorder, const TextWidthMeasurer& measureWidth)
    {
        // [RQ-GUI-047, RQ-GUI-048, ADR-JUC-022 (DEC-JUC-050)]
        std::vector<std::string> overflowing;
        for (const auto& input : inputs)
        {
            const float available =
                static_cast<float>(input.width - arrowZone - 2 * labelBorder);
            float widest = 0.0F;
            for (const auto& label : input.labels)
            {
                widest = std::max(widest, measureWidth(label, fontSize));
            }
            if (widest > available)
            {
                overflowing.push_back(input.controlId);
            }
        }
        return overflowing;
    }
}
