#include "BoundControls.hpp"

namespace xplorer::app
{
    BoundKnob::BoundKnob(ParameterBindingRegistry& registry, std::string parameterName,
                         int minValue, int maxValue, int step)
        : BoundControl(registry, std::move(parameterName))
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        // No permanent text box: the value is on the VFD (RQ-GUI-020); the
        // rotary fills its full bounds. A bubble shows the value while turning,
        // and a double-click opens an inline entry. [RQ-GUI-034]
        setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        setPopupDisplayEnabled(true, true, nullptr); // desktop bubble (crisp under the canvas scale)
        setRange(minValue, maxValue, juce::jmax(1, step));

        onDragStart = [this] { _registry.onControlEditBegan(_parameterName); };
        onDragEnd = [this] { _registry.onControlEditEnded(); };
        onValueChange = [this] { _registry.onControlEdited(_parameterName, static_cast<int>(getValue())); };
    }

    void BoundKnob::mouseDoubleClick(const juce::MouseEvent&)
    {
        auto* parent = getParentComponent();
        if (parent == nullptr || _entryEditor != nullptr)
        {
            return;
        }
        auto editor = std::make_unique<juce::TextEditor>();
        editor->setBounds(getBounds()); // knob's bounds within the shared parent
        editor->setJustification(juce::Justification::centred);
        editor->setInputRestrictions(0, "0123456789-");
        editor->setText(juce::String(static_cast<int>(getValue())), false);
        editor->selectAll();
        editor->onReturnKey = [this] { applyTextEntry(); };
        editor->onEscapeKey = [this] { dismissTextEntry(); };
        editor->onFocusLost = [this] { dismissTextEntry(); };
        parent->addAndMakeVisible(*editor);
        editor->grabKeyboardFocus();
        _entryEditor = std::move(editor);
    }

    void BoundKnob::applyTextEntry()
    {
        if (_entryEditor != nullptr)
        {
            // setValue fires onValueChange → onControlEdited (send + VFD).
            setValue(_entryEditor->getText().getIntValue(), juce::sendNotificationSync);
        }
        dismissTextEntry();
    }

    void BoundKnob::dismissTextEntry()
    {
        // Defer deletion so we never destroy the editor inside its own callback.
        juce::MessageManager::callAsync(
            [safe = juce::Component::SafePointer<BoundKnob>(this)]
            {
                if (safe != nullptr)
                {
                    safe->_entryEditor.reset();
                }
            });
    }

    void BoundKnob::setDisplayedValue(int value)
    {
        setValue(value, juce::dontSendNotification); // [RQ-GUI-003]
    }

    std::string BoundKnob::displayText() const
    {
        // Reference: KnobControl.Value.ToString("00") — two-digit minimum.
        return juce::String::formatted("%02d", static_cast<int>(getValue())).toStdString();
    }

    BoundComboBox::BoundComboBox(ParameterBindingRegistry& registry, std::string parameterName,
                                 const std::vector<std::pair<std::string, int>>& options)
        : BoundControl(registry, std::move(parameterName))
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

    std::string BoundComboBox::displayText() const
    {
        // Reference: the selected ComboBoxValuedControlItem's label.
        return getText().toStdString();
    }

    BoundCheckBox::BoundCheckBox(ParameterBindingRegistry& registry, std::string parameterName,
                                 const juce::String& text)
        : juce::ToggleButton(text), BoundControl(registry, std::move(parameterName))
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

    std::string BoundCheckBox::displayText() const
    {
        // Reference: CheckBoxValuedControl -> "Y" / "N".
        return getToggleState() ? "Y" : "N";
    }
}
