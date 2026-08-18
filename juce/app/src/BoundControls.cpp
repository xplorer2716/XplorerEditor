#include "BoundControls.hpp"

#include "DesignTokens.hpp"

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
        // Not "label": juce::ComboBox has a member of that name, and this file
        // is now compiled into the warnings-as-errors test target. [RQ-GUI-053]
        for (const auto& [itemLabel, value] : options)
        {
            addItem(itemLabel, row++);
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

    BoundRadioGroup::BoundRadioGroup(ParameterBindingRegistry& registry, std::string parameterName,
                                     const std::vector<std::pair<std::string, int>>& options)
        : BoundControl(registry, std::move(parameterName))
    {
        // A non-zero id local to this container makes the child toggles mutually
        // exclusive and tells the LookAndFeel to draw them as circular radios
        // rather than tick boxes. [RQ-GUI-038, ADR-JUC-016]
        constexpr int RADIO_GROUP_ID = 1;
        for (const auto& [label, value] : options)
        {
            auto button = std::make_unique<juce::ToggleButton>(label);
            button->setRadioGroupId(RADIO_GROUP_ID);
            button->onClick = [this, value] { onOptionClicked(value); };
            addAndMakeVisible(*button);
            _options.push_back({std::move(button), value});
        }
    }

    void BoundRadioGroup::onOptionClicked(int value)
    {
        // setRadioGroupId already cleared the sibling; only the selected button
        // fires with getToggleState()==true. Ignore the incidental deselect click.
        for (const auto& option : _options)
        {
            if (option.value == value && !option.button->getToggleState())
            {
                return;
            }
        }
        _registry.onControlEditBegan(_parameterName);
        _registry.onControlEdited(_parameterName, value);
        _registry.onControlEditEnded();
    }

    void BoundRadioGroup::resized()
    {
        // One even slot per option. How tall an option is depends on the
        // orientation, because two GEOMETRY SOURCES meet here: the design system
        // owns shared metrics, the control table owns extracted reference
        // positions (out of the token system, RQ-DSN §2). Stacked, the panel
        // spans several rows and the design-system row height governs; side by
        // side, the panel is one row and its own extracted height governs. Each
        // branch says which, below.
        // [RQ-GUI-040, RQ-GUI-053, ADR-JUC-016 (DEC-JUC-086, DEC-JUC-094),
        // ADR-JUC-014]
        auto area = getLocalBounds();
        if (_options.empty())
        {
            return;
        }
        const int optionCount = static_cast<int>(_options.size());

        // Orientation comes from the panel's own extracted bounds, never from a
        // declared field: the reference geometry already says which way its
        // designer laid the radios out. A panel too short to stack its options
        // at the shared control-row height was drawn side by side there --
        // ENV/RAMP SINGLE-MULTI (129x24, 126x22) -- while FM_DESTINATION (82x42)
        // and LAG_TIMING (79x47) have the room and stay stacked. This test is
        // the one place the token legitimately answers a design-system question:
        // "is there room for N standard rows?".
        // [RQ-GUI-053, RQ-GUI-040, ADR-JUC-016 (DEC-JUC-086), ADR-JUC-014]
        if (area.getHeight() < optionCount * tokens::semantic::controlRowHeight)
        {
            // Side by side: the panel IS the row, and its height is EXTRACTED
            // reference geometry (129x24, 126x22) -- not a design-system row.
            // The reference drew it straddling the 17 px control row (ENV X:
            // panel y=313 h=24 against check boxes y=316 h=17, so 3 px above and
            // 4 px below), i.e. the centring is already encoded in these bounds.
            // Imposing controlRowHeight here pinned the button to the panel top
            // and threw that centring away, putting the radio indicator 3 px
            // above its sibling check boxes. Taking the full height hands the
            // centring back to drawToggleButton, which centres the indicator in
            // whatever bounds it is given -- no arithmetic of our own, and no
            // token applied to geometry the design system does not own
            // (RQ-DSN §2). [RQ-GUI-053, ADR-JUC-016 (DEC-JUC-094)]
            const int slotWidth = area.getWidth() / optionCount;
            for (const auto& option : _options)
            {
                option.button->setBounds(area.removeFromLeft(slotWidth));
            }
            return;
        }

        // Stacked: the panel spans SEVERAL control rows, so each option must be
        // one design-system row at its slot top -- otherwise it stretches over
        // its whole slot and drifts off the row of its sibling check boxes.
        // This is the one orientation where controlRowHeight governs an option's
        // height. [RQ-GUI-040, TASK-JUC-108, ADR-JUC-016 (DEC-JUC-086, DEC-JUC-094)]

        const int slotHeight = area.getHeight() / optionCount;
        for (const auto& option : _options)
        {
            option.button->setBounds(
                area.removeFromTop(slotHeight).withHeight(tokens::semantic::controlRowHeight));
        }
    }

    void BoundRadioGroup::setDisplayedValue(int value)
    {
        for (const auto& option : _options)
        {
            option.button->setToggleState(option.value == value, juce::dontSendNotification);
        }
    }

    std::string BoundRadioGroup::displayText() const
    {
        // Reference: the selected option's label (mirrors the combo/radio VFD text).
        for (const auto& option : _options)
        {
            if (option.button->getToggleState())
            {
                return option.button->getButtonText().toStdString();
            }
        }
        return {};
    }
}
