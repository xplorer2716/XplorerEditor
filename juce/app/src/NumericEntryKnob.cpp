#include "NumericEntryKnob.hpp"

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
            // setValue fires onValueChange, so callers' own wiring (parameter
            // send + VFD, or the matrix's controller call) runs unchanged.
            setValue(_entryEditor->getText().getIntValue(), juce::sendNotificationSync);
        }
        dismissTextEntry();
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
