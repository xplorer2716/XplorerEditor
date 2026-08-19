#include "ModMatrixPanel.hpp"

#include "xplorer/app/ControlMetadata.hpp"
#include "xplorer/app/ModulationHighlight.hpp"
#include "xplorer/model/ModulationMatrixEntry.hpp"

namespace xplorer::app
{
    using model::ModulationMatrixEntry;

    ModMatrixPanel::ModMatrixPanel(juce::Component& parent, controller::XpanderController& controller)
        : _controller(controller)
    {
        for (int entryNumber = 1; entryNumber <= 20; ++entryNumber)
        {
            buildRow(parent, entryNumber);
        }
        refreshAll();
    }

    const ControlSpec* ModMatrixPanel::specFor(const std::string& id) const
    {
        for (const auto& spec : controlTable())
        {
            if (id == spec.id)
            {
                return &spec;
            }
        }
        return nullptr;
    }

    void ModMatrixPanel::buildRow(juce::Component& parent, int entryNumber)
    {
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];
        const auto suffix = std::to_string(entryNumber);

        // Source combo (EnumModulationSourcesModMatrix; ordinal == value).
        // ModMatrixComboBox, not a raw juce::ComboBox: it carries the block tint
        // and cross-reference-highlight state the LookAndFeel paints from
        // (RQ-GUI-052), and inherits HoverRepaintingComboBox so RQ-GUI-041's
        // hover state cannot go stale — these rows are not parameter-bound
        // (composite controller operations, see the header), so nothing else
        // would repaint them. [ADR-JUC-017 (DEC-JUC-040), issue #21,
        // ADR-JUC-028 (DEC-JUC-080)]
        if (const auto* spec = specFor("MOD_SRC_" + suffix))
        {
            row.source = std::make_unique<ModMatrixComboBox>();
            const auto labels = comboLabelsForControl(spec->tag);
            for (std::size_t i = 0; i < labels.size(); ++i)
            {
                row.source->addItem(labels[i], static_cast<int>(i) + 1);
            }
            row.source->setBounds(spec->x, spec->y, spec->width, spec->height);
            row.source->onChange = [this, entryNumber] { onSourceChanged(entryNumber); };
            row.source->setOnAboutToShowPopup([this, entryNumber] { onSourcePopupAboutToShow(entryNumber); });
            parent.addAndMakeVisible(*row.source);
        }

        // Amount knob (-63..63). NumericEntryKnob so double-click opens the
        // same inline numeric entry every other rotary knob has.
        // [RQ-GUI-034, PLAN-GUI-009 (TASK-GUI-036)]
        if (const auto* spec = specFor("MOD_AMNT_SRC_" + suffix))
        {
            row.amount = std::make_unique<NumericEntryKnob>(juce::Slider::RotaryHorizontalVerticalDrag,
                                                             juce::Slider::NoTextBox);
            row.amount->setRange(ModulationMatrixEntry::MIN_AMOUNT, ModulationMatrixEntry::MAX_AMOUNT, 1);
            row.amount->setBounds(spec->x, spec->y, spec->width, spec->height);
            row.amount->onValueChange = [this, entryNumber] { onAmountChanged(entryNumber); };
            parent.addAndMakeVisible(*row.amount);
        }

        // Destination combo (EnumModulationDestinations; ordinal == value).
        if (const auto* spec = specFor("MOD_DEST_" + suffix))
        {
            row.destination = std::make_unique<ModMatrixComboBox>();
            const auto labels = comboLabelsForControl(spec->tag);
            for (std::size_t i = 0; i < labels.size(); ++i)
            {
                row.destination->addItem(labels[i], static_cast<int>(i) + 1);
            }
            row.destination->setBounds(spec->x, spec->y, spec->width, spec->height);
            row.destination->onChange = [this, entryNumber] { onDestinationChanged(entryNumber); };
            parent.addAndMakeVisible(*row.destination);
        }

        // Quantize checkbox.
        if (const auto* spec = specFor("MOD_QUANTIZE_" + suffix))
        {
            row.quantize = std::make_unique<juce::ToggleButton>();
            row.quantize->setBounds(spec->x, spec->y, spec->width, spec->height);
            row.quantize->onClick = [this, entryNumber] { onQuantizeChanged(entryNumber); };
            parent.addAndMakeVisible(*row.quantize);
        }
    }

    void ModMatrixPanel::refreshRow(int entryNumber)
    {
        if (entryNumber < 1 || entryNumber > 20)
        {
            return;
        }
        const auto& entry = _controller.getModulationEntryByNumber(entryNumber);
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];

        _refreshing = true;
        if (row.source != nullptr)
        {
            row.source->setSelectedId(static_cast<int>(entry.source) + 1, juce::dontSendNotification);
        }
        if (row.amount != nullptr)
        {
            row.amount->setValue(entry.amount(), juce::dontSendNotification);
        }
        if (row.destination != nullptr)
        {
            row.destination->setSelectedId(static_cast<int>(entry.destination) + 1,
                                           juce::dontSendNotification);
        }
        if (row.quantize != nullptr)
        {
            row.quantize->setToggleState(entry.quantize() != 0, juce::dontSendNotification);
        }
        // The block tint follows the SELECTED VALUE, so it is re-resolved here
        // rather than at construction: this is the one path every external
        // change goes through (patch load, synth edit, automation, full tone
        // change), and skipping it would leave the matrix correct until the
        // first load and quietly wrong afterwards.
        // [RQ-GUI-052, ADR-JUC-028 (DEC-JUC-082)]
        applyBlockIdentity(entryNumber);
        _currentDestination[static_cast<std::size_t>(entryNumber - 1)] = static_cast<int>(entry.destination);
        _refreshing = false;
        // Whatever just changed for this entry can change every OTHER row's
        // combo availability too (shared destinations). Runs once per
        // refreshRow call, including the 20 calls refreshAll makes -- cheap
        // (small lists, not a render loop). [ADR-JUC-036 (DEC-JUC-122)]
        refreshComboAvailability();
    }

    void ModMatrixPanel::refreshAll()
    {
        for (int entryNumber = 1; entryNumber <= 20; ++entryNumber)
        {
            refreshRow(entryNumber);
        }
    }

    namespace
    {
        int comboValue(const juce::ComboBox& combo) { return combo.getSelectedId() - 1; }
    }

    void ModMatrixPanel::applyBlockIdentity(int entryNumber)
    {
        const auto& entry = _controller.getModulationEntryByNumber(entryNumber);
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];

        // Only the identity is pushed to the control — the hue is resolved from
        // the live palette when the LookAndFeel paints, so a user re-theme needs
        // no invalidation here. [RQ-DSN-095, ADR-JUC-028 (DEC-JUC-080)]
        if (row.source != nullptr)
        {
            row.source->setBlockId(modulationSourceBlock(entry.source));
        }
        if (row.destination != nullptr)
        {
            row.destination->setBlockId(modulationDestinationBlock(entry.destination));
        }
    }

    void ModMatrixPanel::refreshDestinationComboItems(int entryNumber)
    {
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];
        const auto* spec = specFor("MOD_DEST_" + std::to_string(entryNumber));
        if (row.destination == nullptr || spec == nullptr)
        {
            return;
        }
        const auto labels = comboLabelsForControl(spec->tag);
        const auto available = _controller.getAvailableModulationDestinationsForEntry(entryNumber);
        // getAvailableModulationDestinationsForEntry always includes this
        // entry's OWN current destination even when it is itself at the cap
        // (that is what "available" means for the row's own assignment), so
        // the reselect below always finds a match -- no reference-style
        // fall-back-to-first-item branch is reachable here.
        const int currentValue = comboValue(*row.destination);
        row.destination->clear(juce::dontSendNotification);
        for (const auto destination : available)
        {
            const auto index = static_cast<std::size_t>(destination);
            if (index < labels.size())
            {
                row.destination->addItem(labels[index], static_cast<int>(index) + 1);
            }
        }
        row.destination->setSelectedId(currentValue + 1, juce::dontSendNotification);
    }

    void ModMatrixPanel::refreshSourceComboItems(int entryNumber)
    {
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];
        const auto* spec = specFor("MOD_SRC_" + std::to_string(entryNumber));
        if (row.source == nullptr || spec == nullptr)
        {
            return;
        }
        const auto labels = comboLabelsForControl(spec->tag);
        const int currentValue = comboValue(*row.source);
        row.source->clear(juce::dontSendNotification);
        if (_controller.sourceAvailabilityForEntry(entryNumber))
        {
            for (std::size_t i = 0; i < labels.size(); ++i)
            {
                row.source->addItem(labels[i], static_cast<int>(i) + 1);
            }
        }
        else
        {
            // Destination at the RQ-GUI-016 6-source cap and this row
            // contributes no source: only NONE is selectable (reference
            // OnModSourceDropDown "else" branch) -- structurally, not just
            // by convention, so neither a click nor an arrow key can reach
            // anything else. [ADR-JUC-036 (DEC-JUC-122)]
            const auto noneIndex = static_cast<std::size_t>(model::EnumModulationSourcesModMatrix::NONE);
            if (noneIndex < labels.size())
            {
                row.source->addItem(labels[noneIndex], static_cast<int>(noneIndex) + 1);
            }
        }
        // sourceAvailabilityForEntry is false only when entry.source == NONE
        // already, so currentValue is always the item just added above.
        row.source->setSelectedId(currentValue + 1, juce::dontSendNotification);
    }

    void ModMatrixPanel::refreshComboAvailability()
    {
        for (int entryNumber = 1; entryNumber <= 20; ++entryNumber)
        {
            refreshDestinationComboItems(entryNumber);
            refreshSourceComboItems(entryNumber);
        }
    }

    void ModMatrixPanel::onSourcePopupAboutToShow(int entryNumber)
    {
        if (!_controller.sourceAvailabilityForEntry(entryNumber) && _maxSourceReachedHandler)
        {
            _maxSourceReachedHandler(entryNumber);
        }
    }

    // The highlight now sets a FLAG the LookAndFeel renders as a thicker,
    // brighter frame; it no longer repaints the background, which carries the
    // block identity of RQ-GUI-052. The row-matching rules below are the
    // reference behaviour and are unchanged.
    // [RQ-GUI-018 (amended), RQ-GUI-052, ADR-JUC-028 (DEC-JUC-078)]
    void ModMatrixPanel::highlightSources(int sourceValue)
    {
        for (int entryNumber = 1; entryNumber <= 20; ++entryNumber)
        {
            const auto& entry = _controller.getModulationEntryByNumber(entryNumber);
            auto& combo = _rows[static_cast<std::size_t>(entryNumber - 1)].source;
            if (combo != nullptr && static_cast<int>(entry.source) == sourceValue)
            {
                combo->setHighlighted(true);
            }
        }
    }

    void ModMatrixPanel::highlightDestinations(int destValue)
    {
        for (int entryNumber = 1; entryNumber <= 20; ++entryNumber)
        {
            const auto& entry = _controller.getModulationEntryByNumber(entryNumber);
            auto& combo = _rows[static_cast<std::size_t>(entryNumber - 1)].destination;
            if (combo != nullptr && static_cast<int>(entry.destination) == destValue
                && entry.source != model::EnumModulationSourcesModMatrix::NONE)
            {
                combo->setHighlighted(true);
            }
        }
    }

    void ModMatrixPanel::clearHighlight()
    {
        for (auto& row : _rows)
        {
            if (row.source != nullptr)
            {
                row.source->setHighlighted(false);
            }
            if (row.destination != nullptr)
            {
                row.destination->setHighlighted(false);
            }
        }
    }

    void ModMatrixPanel::onSourceChanged(int entryNumber)
    {
        if (_refreshing)
        {
            return;
        }
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];
        _controller.changeModulationSource(comboValue(*row.source),
                                           static_cast<int>(row.amount->getValue()),
                                           row.quantize->getToggleState() ? 1 : 0,
                                           comboValue(*row.destination), entryNumber);
        applyBlockIdentity(entryNumber);
        refreshComboAvailability(); // [ADR-JUC-036 (DEC-JUC-122)]
        if (_editHandler)
        {
            _editHandler(entryNumber);
        }
    }

    void ModMatrixPanel::onAmountChanged(int entryNumber)
    {
        if (_refreshing)
        {
            return;
        }
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];
        _controller.changeModulationSourceAmount(comboValue(*row.source),
                                                 static_cast<int>(row.amount->getValue()),
                                                 comboValue(*row.destination), entryNumber);
        if (_editHandler)
        {
            _editHandler(entryNumber);
        }
    }

    void ModMatrixPanel::onDestinationChanged(int entryNumber)
    {
        if (_refreshing)
        {
            return;
        }
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];
        const int oldDestination = _currentDestination[static_cast<std::size_t>(entryNumber - 1)];
        const int newDestination = comboValue(*row.destination);
        if (oldDestination == newDestination)
        {
            return;
        }
        _controller.changeModulationDestination(comboValue(*row.source),
                                                static_cast<int>(row.amount->getValue()),
                                                row.quantize->getToggleState() ? 1 : 0,
                                                oldDestination, newDestination, entryNumber);
        _currentDestination[static_cast<std::size_t>(entryNumber - 1)] = newDestination;
        applyBlockIdentity(entryNumber);
        refreshComboAvailability(); // [ADR-JUC-036 (DEC-JUC-122)]
        if (_editHandler)
        {
            _editHandler(entryNumber);
        }
    }

    void ModMatrixPanel::onQuantizeChanged(int entryNumber)
    {
        if (_refreshing)
        {
            return;
        }
        auto& row = _rows[static_cast<std::size_t>(entryNumber - 1)];
        _controller.changeModulationSourceQuantize(comboValue(*row.source), comboValue(*row.destination),
                                                   row.quantize->getToggleState() ? 1 : 0, entryNumber);
        if (_editHandler)
        {
            _editHandler(entryNumber);
        }
    }
}
