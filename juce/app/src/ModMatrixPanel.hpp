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
#include <functional>
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

        /// Fan-out for a user edit of a matrix row (source/amount/dest/qtz),
        /// invoked with the 1-based entry number after the controller update.
        /// The app wires this to the VFD, like the reference. [RQ-GUI-020]
        void setEditHandler(std::function<void(int)> handler) { _editHandler = std::move(handler); }

        // Hover highlight (reference ModulationMatrixHighlight). The colour is
        // derived from the live LookAndFeel, not cached, so it tracks the LED
        // setting (ADR-011). [RQ-GUI-018]
        /// Highlight source combos currently set to sourceValue.
        void highlightSources(int sourceValue);
        /// Highlight destination combos set to destValue whose row has an
        /// active source (≠ NONE).
        void highlightDestinations(int destValue);
        /// Restore every combo's default background.
        void clearHighlight();

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

        [[nodiscard]] juce::Colour highlightColour() const;

        controller::XpanderController& _controller;
        std::array<Row, 20> _rows;
        std::array<int, 20> _currentDestination{}; // tracks old destination for change ops
        std::function<void(int)> _editHandler;
        juce::Colour _defaultComboBackground;
        bool _refreshing = false;
    };
}
