#include "ModMatrixPanel.hpp"

#include "xplorer/app/ControlMetadata.hpp"
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
        // Default combo background from the LookAndFeel, captured before any
        // highlight so clearHighlight can restore it.
        _defaultComboBackground = _rows[0].source != nullptr
                                      ? _rows[0].source->findColour(juce::ComboBox::backgroundColourId)
                                      : juce::Colour::fromRGB(30, 36, 44);
        _highlightColour = _defaultComboBackground.brighter(0.5F); // until the app sets the LED colour
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
        if (const auto* spec = specFor("MOD_SRC_" + suffix))
        {
            row.source = std::make_unique<juce::ComboBox>();
            const auto labels = comboLabelsForControl(spec->tag);
            for (std::size_t i = 0; i < labels.size(); ++i)
            {
                row.source->addItem(labels[i], static_cast<int>(i) + 1);
            }
            row.source->setBounds(spec->x, spec->y, spec->width, spec->height);
            row.source->onChange = [this, entryNumber] { onSourceChanged(entryNumber); };
            parent.addAndMakeVisible(*row.source);
        }

        // Amount knob (-63..63).
        if (const auto* spec = specFor("MOD_AMNT_SRC_" + suffix))
        {
            row.amount = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag,
                                                        juce::Slider::NoTextBox);
            row.amount->setRange(ModulationMatrixEntry::MIN_AMOUNT, ModulationMatrixEntry::MAX_AMOUNT, 1);
            row.amount->setBounds(spec->x, spec->y, spec->width, spec->height);
            row.amount->onValueChange = [this, entryNumber] { onAmountChanged(entryNumber); };
            parent.addAndMakeVisible(*row.amount);
        }

        // Destination combo (EnumModulationDestinations; ordinal == value).
        if (const auto* spec = specFor("MOD_DEST_" + suffix))
        {
            row.destination = std::make_unique<juce::ComboBox>();
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
        _currentDestination[static_cast<std::size_t>(entryNumber - 1)] = static_cast<int>(entry.destination);
        _refreshing = false;
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

    void ModMatrixPanel::highlightSources(int sourceValue)
    {
        for (int entryNumber = 1; entryNumber <= 20; ++entryNumber)
        {
            const auto& entry = _controller.getModulationEntryByNumber(entryNumber);
            auto& combo = _rows[static_cast<std::size_t>(entryNumber - 1)].source;
            if (combo != nullptr && static_cast<int>(entry.source) == sourceValue)
            {
                combo->setColour(juce::ComboBox::backgroundColourId, _highlightColour);
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
                combo->setColour(juce::ComboBox::backgroundColourId, _highlightColour);
            }
        }
    }

    void ModMatrixPanel::clearHighlight()
    {
        for (auto& row : _rows)
        {
            if (row.source != nullptr)
            {
                row.source->setColour(juce::ComboBox::backgroundColourId, _defaultComboBackground);
            }
            if (row.destination != nullptr)
            {
                row.destination->setColour(juce::ComboBox::backgroundColourId, _defaultComboBackground);
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
