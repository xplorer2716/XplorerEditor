#include "xplorer/app/ControlTable.hpp"

#include <array>

namespace xplorer::app
{
    namespace
    {
        constexpr ControlSpec CONTROL_TABLE[] = {
#include "GeneratedControlTable.inc"
        };
    }

    std::span<const ControlSpec> controlTable()
    {
        return CONTROL_TABLE;
    }
}
