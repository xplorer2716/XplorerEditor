#include "PianoWindow.hpp"

namespace xplorer::app
{
    PianoWindow::PianoWindow(controller::XpanderController& controller)
        : juce::DocumentWindow("Piano", juce::Colours::black, juce::DocumentWindow::closeButton),
          _controller(controller),
          _keyboard(_keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        _keyboardState.addListener(this);
        setContentNonOwned(&_keyboard, false);
        setUsingNativeTitleBar(true);
        setResizable(true, false);
        centreWithSize(760, 90);
        setVisible(true);
    }

    PianoWindow::~PianoWindow()
    {
        _keyboardState.removeListener(this);
    }

    void PianoWindow::closeButtonPressed()
    {
        setVisible(false); // owner keeps the instance; reused on next open
    }

    void PianoWindow::handleNoteOn(juce::MidiKeyboardState*, int, int midiNote, float)
    {
        _controller.playNote(true, midiNote); // [RQ-GUI-028, RQ-MID-010]
    }

    void PianoWindow::handleNoteOff(juce::MidiKeyboardState*, int, int midiNote, float)
    {
        _controller.playNote(false, midiNote);
    }
}
