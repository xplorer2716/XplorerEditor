#include "xplorer/app/ControlTable.hpp"

#include <array>
#include <cmath>

namespace xplorer::app
{
    namespace
    {
        constexpr auto CONTROL_TABLE = std::to_array<ControlSpec>({
#include "GeneratedControlTable.inc"
        });

        int roundToPixels(double value)
        {
            return static_cast<int>(std::lround(value));
        }
    }

    std::span<const ControlSpec> controlTable()
    {
        return CONTROL_TABLE;
    }

    // The height comes from the width through the ratio of the two canvas
    // constants, never through a decimal: a future canvas relayout then stays
    // correct by construction, and no magic number appears.
    // [RQ-SCL-001, RQ-SCL-002, ADR-JUC-025 (DEC-JUC-063)]
    WindowSize windowSizeForScale(float scale)
    {
        const auto width = roundToPixels(static_cast<double>(scale) * WINDOW_WIDTH_AT_1X);
        const auto canvasHeight = roundToPixels(static_cast<double>(width) * LOGICAL_CANVAS_HEIGHT
                                                / LOGICAL_CANVAS_WIDTH);
        return {width, canvasHeight + MENU_BAR_HEIGHT};
    }
}
