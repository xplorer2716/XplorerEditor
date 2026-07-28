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

    SharedComboBoxFontSize computeSharedComboBoxFontSize(
        const std::vector<ComboBoxSizingInput>& inputs, float baseSize, float minSize,
        int arrowZone, int labelMargin, const TextWidthMeasurer& measureWidth)
    {
        // Per box: start from baseSize and shrink by the ratio its widest label
        // overflows the available text area by — the exact per-instance formula
        // this policy replaces. The shared size is the most constraining of
        // those, so no box anywhere ends up ellipsized. A box whose UNCLAMPED
        // candidate is under the floor is reported for widening instead, which
        // is a property of that box alone and independent of the shared value.
        // [RQ-GUI-047, ADR-JUC-021 (DEC-JUC-042, DEC-JUC-043)]
        SharedComboBoxFontSize result;
        result.sharedSize = baseSize;

        for (const auto& input : inputs)
        {
            const float availableWidth =
                static_cast<float>(input.width - arrowZone - labelMargin);

            float widest = 0.0F;
            for (const auto& label : input.labels)
            {
                widest = std::max(widest, measureWidth(label, baseSize));
            }

            // No room at all, or an unmeasurable (empty-width) label set: the
            // box cannot fit anything, so it is a widening candidate and must
            // not drag the shared size to zero.
            if (availableWidth <= 0.0F || widest <= 0.0F)
            {
                if (availableWidth <= 0.0F)
                {
                    result.idsBelowFloor.push_back(input.controlId);
                }
                continue;
            }

            const float candidate =
                widest > availableWidth ? baseSize * (availableWidth / widest) : baseSize;
            if (candidate < minSize)
            {
                result.idsBelowFloor.push_back(input.controlId);
            }
            result.sharedSize = std::min(result.sharedSize, candidate);
        }

        result.sharedSize = std::clamp(result.sharedSize, minSize, baseSize);
        return result;
    }
}
