#pragma once

#include "xplorer/model/XpanderConstants.hpp"

namespace xplorer::model
{
    /// Port of Xplorer.Model.ModulationMatrixEntry: one of the 20 matrix
    /// slots (source, clamped amount, quantize flag, destination, and the
    /// per-destination source id). [RQ-MOD-030]
    class ModulationMatrixEntry
    {
    public:
        static constexpr int MIN_AMOUNT = -63;
        static constexpr int MAX_AMOUNT = 63;
        static constexpr int MIN_QUANTIZE = 0;
        static constexpr int MAX_QUANTIZE = 1;
        static constexpr int UNDEFINED_ID_SOURCE = -1;

        EnumModulationDestinations destination = EnumModulationDestinations::VCO1_FRQ;
        EnumModulationSourcesModMatrix source = EnumModulationSourcesModMatrix::NONE;
        int idSource = UNDEFINED_ID_SOURCE;

        [[nodiscard]] int amount() const { return _amount; }
        void setAmount(int value)
        {
            _amount = value < MIN_AMOUNT ? MIN_AMOUNT : (value > MAX_AMOUNT ? MAX_AMOUNT : value);
        }

        [[nodiscard]] int quantize() const { return _quantize; }
        void setQuantize(int value)
        {
            _quantize = value < MIN_QUANTIZE ? MIN_QUANTIZE : (value > MAX_QUANTIZE ? MAX_QUANTIZE : value);
        }

        void reset()
        {
            idSource = UNDEFINED_ID_SOURCE;
            source = EnumModulationSourcesModMatrix::NONE;
            _amount = 0;
            _quantize = 0;
        }

    private:
        int _amount = 0;
        int _quantize = 0;
    };
}
