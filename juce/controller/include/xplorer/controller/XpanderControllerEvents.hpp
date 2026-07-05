#pragma once

// Ports of the Xplorer.Controller.Events argument types, as plain structs
// consumed through std::function handlers. [RQ-CTL-070]

#include "xplorer/model/ModulationMatrixEntry.hpp"
#include "xplorer/model/XpanderConstants.hpp"

#include <array>
#include <map>
#include <string>

namespace xplorer::controller
{
    enum class EnumModulationParameter
    {
        MODULATIONSOURCE,
        MODULATIONDESTINATION,
        MODULATIONAMOUNT,
        MODULATIONQUANTIZE,
        ALL,
        NONE,
    };

    enum class EnumMidiDevice
    {
        SynthInputDevice,
        SynthOutputDevice,
        AutomationInputDevice,
    };

    struct FullToneChangeEvent
    {
        std::map<std::string, int> parameterMap;
        std::array<model::ModulationMatrixEntry, model::constants::MODENTRIES_COUNT> modulationMatrix;
    };

    struct PageChangeEvent
    {
        model::EnumPages page;
        int subPage;
    };

    struct ModulationEntryChangeEvent
    {
        model::ModulationMatrixEntry entry;
        int entryNumber; // 1-based, as the reference
        EnumModulationParameter parameter;
    };

    struct AllDataDumpProgressionEvent
    {
        bool isWaitingForAllDataDumpRequest;
        int singlePatchCount;
        int multiPatchCount;
    };
}
