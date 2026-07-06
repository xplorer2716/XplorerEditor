#pragma once

// Modulation matrix view: 20 rows of {source combo, amount knob, destination
// combo, quantize checkbox}, wired to the controller matrix API. Not part of
// the parameter-binding registry (matrix edits are dedicated controller
// operations, not plain parameters). Port of ModulationMatrixManager.
// [RQ-GUI-015..017, ADR-006]

#include "xplorer/app/ControlTable.hpp"
#include "xplorer/controller/XpanderController.hpp"

#include <juce_gui_extra/juce_gui_extra.h>

#include <array>
#include <memory>

namespace xplorer::app
{
    class ModMatrixPanel
    {
    public:
        ModMatrixPanel(juce::Component& parent, controller::XpanderController& controller);

        /// Refreshes one 1-based entry from the model (on entry-change event).
        void refreshRow(int entryNumber);
        /// Refreshes all 20 rows (on full-tone change).
        void refreshAll();

    private:
        struct Row
        {
            std::unique_ptr<juce::ComboBox> source;
            std::unique_ptr<juce::Slider> amount;
            std::unique_ptr<juce::ComboBox> destination;
            std::unique_ptr<juce::ToggleButton> quantize;
        };

        void buildRow(juce::Component& parent, int entryNumber);
        [[nodiscard]] const ControlSpec* specFor(const std::string& id) const;
        void onSourceChanged(int entryNumber);
        void onAmountChanged(int entryNumber);
        void onDestinationChanged(int entryNumber);
        void onQuantizeChanged(int entryNumber);

        controller::XpanderController& _controller;
        std::array<Row, 20> _rows;
        std::array<int, 20> _currentDestination{}; // tracks old destination for change ops
        bool _refreshing = false;
    };
}
