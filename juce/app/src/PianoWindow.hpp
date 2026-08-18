#pragma once

// Piano-keyboard window sending Note On/Off to the synth. Port of the
// MidiApp PianoControlForm. The computer-keyboard-to-note mapping is
// resolved from the active OS keyboard layout each time the window becomes
// visible, replacing JUCE's US-QWERTY-only default. [RQ-GUI-028, RQ-GUI-074,
// ADR-JUC-035]

#include "PianoLabelledKeyboardComponent.hpp"

#include "xplorer/controller/XpanderController.hpp"

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace xplorer::app
{
    class PianoWindow final : public juce::DocumentWindow, private juce::MidiKeyboardState::Listener
    {
    public:
        explicit PianoWindow(controller::XpanderController& controller);
        ~PianoWindow() override;

        void closeButtonPressed() override;
        void visibilityChanged() override;

    private:
        void handleNoteOn(juce::MidiKeyboardState*, int channel, int midiNote, float velocity) override;
        void handleNoteOff(juce::MidiKeyboardState*, int channel, int midiNote, float velocity) override;

        /// Resolves the key mapping from the current OS layout and installs
        /// it, or leaves JUCE's built-in default untouched if no query is
        /// available. Called on every open, not just the first — the window
        /// is reused across opens (see MainComponent::_pianoWindow), so this
        /// is what makes "resolved at window open" true after the first time
        /// too. [RQ-GUI-074, ADR-JUC-035 (DEC-JUC-117)]
        void applyKeyboardLayoutMapping();

        controller::XpanderController& _controller;
        juce::MidiKeyboardState _keyboardState;
        PianoLabelledKeyboardComponent _keyboard;
    };
}
