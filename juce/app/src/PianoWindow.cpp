#include "PianoWindow.hpp"

#include "PianoKeyboardLayoutQuery.hpp"

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

    void PianoWindow::visibilityChanged()
    {
        // Fires on every setVisible(true) — the ctor's own call at first
        // open, and MainComponent's on every reopen of the reused instance —
        // so "resolved at window open" (DEC-JUC-117) holds after the first
        // time too, with no separate call for the caller to remember.
        if (isVisible())
        {
            applyKeyboardLayoutMapping();
        }
    }

    void PianoWindow::applyKeyboardLayoutMapping()
    {
        // JUCE's own default installs setKeyPressForNote at keyMappingOctave
        // 6 (juce_MidiKeyboardComponent.h); this feature never calls
        // setKeyPressBaseOctave, so the base stays that same constant.
        constexpr int KEY_MAPPING_BASE_MIDI_NOTE = 12 * 6;

        const auto query = makeNativeKeyboardLayoutQuery();
        const auto mapping = buildPianoKeyMapping(query.get());
        if (!mapping)
        {
            // JUCE's built-in default is left untouched [DEC-JUC-118] — and
            // it IS the identity of pianoKeyPositions(): each reference
            // character is exactly what JUCE matches for that note. Labelling
            // from that identity keeps RQ-GUI-075 true even in this fallback.
            std::vector<ResolvedPianoKey> identity;
            for (const auto& position : pianoKeyPositions())
            {
                identity.push_back({static_cast<char32_t>(position.referenceChar), position.noteOffsetFromC});
            }
            _keyboard.setKeyLabelMapping(std::move(identity), KEY_MAPPING_BASE_MIDI_NOTE);
            return;
        }

        _keyboard.clearKeyMappings();
        for (const auto& key : *mapping)
        {
            // textCharacter left at 0, keyCode carries the character —
            // matching exactly how JUCE's own default does it
            // ({c,0,0} in juce_MidiKeyboardComponent.cpp), so
            // isCurrentlyDown()'s VkKeyScan-style matching keeps working.
            _keyboard.setKeyPressForNote(
                juce::KeyPress(static_cast<int>(key.character), juce::ModifierKeys(), 0),
                key.noteOffsetFromC);
        }
        _keyboard.setKeyLabelMapping(*mapping, KEY_MAPPING_BASE_MIDI_NOTE);
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
