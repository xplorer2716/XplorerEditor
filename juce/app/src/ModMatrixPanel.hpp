#pragma once

// Modulation matrix view: 20 rows of {source combo, amount knob, destination
// combo, quantize checkbox}, wired to the controller matrix API. Not part of
// the parameter-binding registry (matrix edits are dedicated controller
// operations, not plain parameters). Port of ModulationMatrixManager.
// [RQ-GUI-015..017, RQ-GUI-020, ADR-JUC-006, ADR-JUC-036]

#include "ModMatrixComboBox.hpp"
#include "NumericEntryKnob.hpp"
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

        /// Fan-out for an ATTEMPTED edit that could not happen: the user
        /// opened a row's source dropdown while its destination is at the
        /// RQ-GUI-016 6-source cap and the row itself contributes no source.
        /// Distinct from setEditHandler above -- nothing changed here, so it
        /// is not "after the controller update". The app wires this to the
        /// VFD's "MAX SRC COUNT REACHED FOR" notice. [RQ-GUI-020,
        /// ADR-JUC-036 (DEC-JUC-123)]
        void setMaxSourceReachedHandler(std::function<void(int)> handler)
        {
            _maxSourceReachedHandler = std::move(handler);
        }

        // Cross-reference highlight (reference ModulationMatrixHighlight). The
        // matched combos are flagged, and the LookAndFeel renders the flag as a
        // thicker, brighter frame in the combo's own block hue — the background
        // is left alone because it carries the block identity of RQ-GUI-052.
        // [RQ-GUI-018 (rendering amended), RQ-GUI-052, ADR-JUC-028]
        /// Highlight source combos currently set to sourceValue.
        void highlightSources(int sourceValue);
        /// Highlight destination combos set to destValue whose row has an
        /// active source (≠ NONE).
        void highlightDestinations(int destValue);
        /// Clear the highlight flag on every combo.
        void clearHighlight();

    private:
        struct Row
        {
            std::unique_ptr<ModMatrixComboBox> source;
            // NumericEntryKnob, not a plain juce::Slider: the double-click ->
            // inline numeric entry of RQ-GUI-034 applies to every rotary knob,
            // amount included. [RQ-GUI-015, PLAN-GUI-009 (TASK-GUI-036)]
            std::unique_ptr<NumericEntryKnob> amount;
            std::unique_ptr<ModMatrixComboBox> destination;
            std::unique_ptr<juce::ToggleButton> quantize;
        };

        void buildRow(juce::Component& parent, int entryNumber);
        [[nodiscard]] const ControlSpec* specFor(const std::string& id) const;
        void onSourceChanged(int entryNumber);
        void onAmountChanged(int entryNumber);
        void onDestinationChanged(int entryNumber);
        void onQuantizeChanged(int entryNumber);

        /// Re-resolves the block identity of a row's two combos from their
        /// currently selected values. [RQ-GUI-052, ADR-JUC-028 (DEC-JUC-082)]
        void applyBlockIdentity(int entryNumber);

        /// Rebuilds every row's source and destination item lists from live
        /// controller availability, preserving each combo's current selected
        /// value. Called after anything that can change any destination's
        /// saturation, so an over-the-cap choice is never present in a list
        /// to begin with -- reachable by neither a pointer click nor an
        /// arrow key. [RQ-GUI-016, ADR-JUC-036 (DEC-JUC-122)]
        void refreshComboAvailability();
        void refreshSourceComboItems(int entryNumber);
        void refreshDestinationComboItems(int entryNumber);

        /// The source combo's ADR-JUC-036 "about to show popup" hook: fires
        /// setMaxSourceReachedHandler when this row cannot offer anything
        /// but NONE.
        void onSourcePopupAboutToShow(int entryNumber);

        controller::XpanderController& _controller;
        std::array<Row, 20> _rows;
        std::array<int, 20> _currentDestination{}; // tracks old destination for change ops
        std::function<void(int)> _editHandler;
        std::function<void(int)> _maxSourceReachedHandler;
        bool _refreshing = false;
    };
}
