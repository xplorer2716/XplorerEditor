#include "NumericEntryKnob.hpp"

#include "xplorer/app/KnobPresetValues.hpp"

#include <cstddef>
#include <utility>

namespace xplorer::app
{
    void NumericEntryKnob::openNumericEntry()
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

    void NumericEntryKnob::applyTextEntry()
    {
        if (_entryEditor != nullptr)
        {
            applyValue(_entryEditor->getText().getIntValue());
        }
        dismissTextEntry();
    }

    void NumericEntryKnob::applyValue(int value)
    {
        // setValue fires onValueChange, so callers' own wiring (parameter
        // send + VFD, or the matrix's controller call) runs unchanged.
        setValue(value, juce::sendNotificationSync);
    }

    void NumericEntryKnob::setPresetValues(std::vector<int> values)
    {
        _presetValues = std::move(values);
    }

    const std::vector<int>& NumericEntryKnob::presetValues()
    {
        if (_presetValues.empty())
        {
            _presetValues = presetValuesForRange(static_cast<int>(getMinimum()),
                                                 static_cast<int>(getMaximum()));
        }
        return _presetValues;
    }

    bool NumericEntryKnob::applyPresetValue(int index)
    {
        const auto& values = presetValues();
        if (index < 0 || index >= static_cast<int>(values.size()))
        {
            return false;
        }
        applyValue(values[static_cast<std::size_t>(index)]);
        return true;
    }

    void NumericEntryKnob::dismissTextEntry()
    {
        // Defer deletion so we never destroy the editor inside its own callback.
        juce::MessageManager::callAsync(
            [safe = juce::Component::SafePointer<NumericEntryKnob>(this)]
            {
                if (safe != nullptr)
                {
                    safe->_entryEditor.reset();
                }
            });
    }
}
