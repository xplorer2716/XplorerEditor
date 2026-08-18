#pragma once

// Piano keyboard component that prints, on every mapped key, the computer-
// keyboard character that plays it — extending juce::MidiKeyboardComponent's
// own drawWhiteNote/drawBlackNote hooks rather than replacing them, so JUCE
// keeps owning fill, pressed/hover state and its own octave markers.
// [RQ-GUI-075, ADR-JUC-035 (DEC-JUC-119)]

#include "xplorer/app/PianoKeyMapping.hpp"

#include <juce_audio_utils/juce_audio_utils.h>

namespace xplorer::app
{
    class PianoLabelledKeyboardComponent final : public juce::MidiKeyboardComponent
    {
    public:
        PianoLabelledKeyboardComponent(juce::MidiKeyboardState& keyboardState, Orientation keyboardOrientation);

        /// Replaces the bindings the labels are drawn from.
        /// `mappingBaseMidiNote` is 12 * the base octave setKeyPressForNote was
        /// installed against — always 12*6, since this feature never calls
        /// setKeyPressBaseOctave. An empty mapping shows no labels at all.
        /// [RQ-GUI-075]
        void setKeyLabelMapping(std::vector<ResolvedPianoKey> mapping, int mappingBaseMidiNote);

    private:
        void drawWhiteNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown,
                           bool isOver, juce::Colour lineColour, juce::Colour textColour) override;
        void drawBlackNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area, bool isDown,
                           bool isOver, juce::Colour noteFillColour) override;

        std::vector<ResolvedPianoKey> _mapping;
        int _mappingBaseMidiNote = 0;
    };
}
