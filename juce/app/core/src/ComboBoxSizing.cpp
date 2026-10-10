#include "xplorer/app/ComboBoxSizing.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ControlTable.hpp"

#include <utility>

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
            inputs.emplace_back(spec.id, spec.width, std::move(labels));
        }
        return inputs;
    }
}
