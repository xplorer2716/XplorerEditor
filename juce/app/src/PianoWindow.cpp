#include "PianoWindow.hpp"

#include "PianoKeyboardLayoutQuery.hpp"

namespace xplorer::app
{
    namespace
    {
        // Owner request, 2026-08-18: the whole piano reads too small,
        // including the RQ-GUI-075 key-mapping labels; scaling the window and
        // the key width together keeps the same visible note range while
        // giving the labels proportionally more room. [RQ-GUI-075]
        constexpr int BASE_WINDOW_WIDTH = 760;
        constexpr int BASE_WINDOW_HEIGHT = 90;
        constexpr float BASE_KEY_WIDTH = 16.0F; // JUCE's own default (KeyboardComponentBase)
        constexpr float PIANO_SCALE = 1.5F;
    }

    PianoWindow::PianoWindow(controller::XpanderController& controller)
        : juce::DocumentWindow("Piano", juce::Colours::black, juce::DocumentWindow::closeButton),
          _controller(controller),
          _keyboard(_keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        _keyboardState.addListener(this);
        _keyboard.setKeyWidth(BASE_KEY_WIDTH * PIANO_SCALE);
        setContentNonOwned(&_keyboard, false);
        setUsingNativeTitleBar(true);
        setResizable(true, false);
        centreWithSize(static_cast<int>(BASE_WINDOW_WIDTH * PIANO_SCALE),
                      static_cast<int>(BASE_WINDOW_HEIGHT * PIANO_SCALE));
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
