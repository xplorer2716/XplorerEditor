#pragma once

// Piano-keyboard window sending Note On/Off to the synth. Port of the
// MidiApp PianoControlForm. [RQ-GUI-028]

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

    private:
        void handleNoteOn(juce::MidiKeyboardState*, int channel, int midiNote, float velocity) override;
        void handleNoteOff(juce::MidiKeyboardState*, int channel, int midiNote, float velocity) override;

        controller::XpanderController& _controller;
        juce::MidiKeyboardState _keyboardState;
        juce::MidiKeyboardComponent _keyboard;
    };
}
