#include "BoundControls.hpp"

namespace xplorer::app
{
    BoundKnob::BoundKnob(ParameterBindingRegistry& registry, std::string parameterName,
                         int minValue, int maxValue, int step)
        : _registry(registry), _parameterName(std::move(parameterName))
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::TextBoxBelow, false, 44, 14);
        setRange(minValue, maxValue, juce::jmax(1, step));

        onDragStart = [this] { _registry.onControlEditBegan(_parameterName); };
        onDragEnd = [this] { _registry.onControlEditEnded(); };
        onValueChange = [this] { _registry.onControlEdited(_parameterName, static_cast<int>(getValue())); };
    }

    void BoundKnob::setDisplayedValue(int value)
    {
        // Do not fire onValueChange back into the registry; the registry's
        // anti-echo guard covers re-entrancy, but avoiding the notification
        // keeps things clean. [RQ-GUI-003]
        setValue(value, juce::dontSendNotification);
    }

    BoundComboBox::BoundComboBox(ParameterBindingRegistry& registry, std::string parameterName,
                                 const std::vector<std::pair<std::string, int>>& options)
        : _registry(registry), _parameterName(std::move(parameterName))
    {
        int row = 1;
        for (const auto& [label, value] : options)
        {
            addItem(label, row++);
            _valueByRow.push_back(value);
        }
        onChange = [this]
        {
            const int index = getSelectedItemIndex();
            if (index >= 0 && index < static_cast<int>(_valueByRow.size()))
            {
                _registry.onControlEditBegan(_parameterName);
                _registry.onControlEdited(_parameterName, _valueByRow[static_cast<std::size_t>(index)]);
                _registry.onControlEditEnded();
            }
        };
    }

    void BoundComboBox::setDisplayedValue(int value)
    {
        for (std::size_t i = 0; i < _valueByRow.size(); ++i)
        {
            if (_valueByRow[i] == value)
            {
                setSelectedItemIndex(static_cast<int>(i), juce::dontSendNotification);
                return;
            }
        }
    }

    BoundCheckBox::BoundCheckBox(ParameterBindingRegistry& registry, std::string parameterName,
                                 const juce::String& text)
        : juce::ToggleButton(text), _registry(registry), _parameterName(std::move(parameterName))
    {
        onClick = [this]
        {
            _registry.onControlEditBegan(_parameterName);
            _registry.onControlEdited(_parameterName, getToggleState() ? 1 : 0);
            _registry.onControlEditEnded();
        };
    }

    void BoundCheckBox::setDisplayedValue(int value)
    {
        setToggleState(value != 0, juce::dontSendNotification);
    }
}
