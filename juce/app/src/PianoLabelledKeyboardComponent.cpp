#include "PianoLabelledKeyboardComponent.hpp"

#include "DesignTokens.hpp"

#include "xplorer/app/PianoKeyLabelPlan.hpp"

namespace xplorer::app
{
    namespace
    {
        // Uppercased for legibility (owner request, 2026-08-18) — display
        // only: the resolved character itself (matched against KeyPress)
        // never changes case. Centralised here so the fit check
        // (measureCharacterWidth) and the actual drawText agree on what
        // glyph is measured, since upper-case letters run wider than lower.
        juce::String toDisplayString(char32_t character)
        {
            return juce::String::charToString(static_cast<juce::juce_wchar>(character)).toUpperCase();
        }

        // The one JUCE-touching piece of pianoKeyLabelFor's injected
        // measurer: GlyphArrangement, not the deprecated
        // Font::getStringWidthFloat — same metric-accurate call the codebase
        // already uses for combo-box and section-header fit checks.
        // [RQ-GUI-047, RQ-GUI-062]
        float measureCharacterWidth(char32_t character, float fontSize)
        {
            const auto font = juce::Font{juce::FontOptions{fontSize}};
            return juce::GlyphArrangement::getStringWidth(font, toDisplayString(character));
        }

        // JUCE's own drawWhiteNote prints the octave marker at
        // Justification::centredBottom inside area.withTrimmedBottom(2.0f),
        // in a font sized jmin(12.0f, keyWidth*0.9F) — capped, not measured,
        // since that computation is private to JUCE's implementation. This is
        // a deliberately generous estimate of the room it needs, so the
        // binding label stacked above it (DEC-JUC-120) clears it on every
        // realistic key width; exact spacing is a visual nicety to retune on
        // screen, not part of what RQ-GUI-075 requires.
        constexpr float ESTIMATED_OCTAVE_MARKER_HEIGHT = 14.0f;
    }

    PianoLabelledKeyboardComponent::PianoLabelledKeyboardComponent(juce::MidiKeyboardState& keyboardState,
                                                                    Orientation keyboardOrientation)
        : juce::MidiKeyboardComponent(keyboardState, keyboardOrientation)
    {
    }

    void PianoLabelledKeyboardComponent::setKeyLabelMapping(std::vector<ResolvedPianoKey> mapping,
                                                             int mappingBaseMidiNote)
    {
        _mapping = std::move(mapping);
        _mappingBaseMidiNote = mappingBaseMidiNote;
        repaint();
    }

    void PianoLabelledKeyboardComponent::drawWhiteNote(int midiNoteNumber, juce::Graphics& g,
                                                       juce::Rectangle<float> area, bool isDown, bool isOver,
                                                       juce::Colour lineColour, juce::Colour textColour)
    {
        // Base first: JUCE keeps owning fill, pressed/hover state, the
        // separator line and its own octave marker. [DEC-JUC-119]
        juce::MidiKeyboardComponent::drawWhiteNote(midiNoteNumber, g, area, isDown, isOver, lineColour,
                                                   textColour);

        const auto label =
            pianoKeyLabelFor(midiNoteNumber, _mapping, _mappingBaseMidiNote, /*isBlackKey*/ false,
                             area.getWidth(), tokens::semantic::pianoKeyLabelSize, measureCharacterWidth);
        if (!label)
        {
            return;
        }

        // Reserve room above JUCE's own octave marker on the two Cs that
        // carry one, so binding character and marker never overlap.
        // [DEC-JUC-120]
        const bool hasOctaveMarker = getWhiteNoteText(midiNoteNumber).isNotEmpty();
        auto labelArea = area.withTrimmedLeft(1.0f)
                             .withTrimmedBottom(hasOctaveMarker ? ESTIMATED_OCTAVE_MARKER_HEIGHT : 2.0f);

        g.setColour(tokens::semantic::pianoKeyLabelOnWhite);
        g.setFont(juce::Font{juce::FontOptions{tokens::semantic::pianoKeyLabelSize}});
        g.drawText(toDisplayString(*label), labelArea, juce::Justification::centredBottom, false);
    }

    void PianoLabelledKeyboardComponent::drawBlackNote(int midiNoteNumber, juce::Graphics& g,
                                                       juce::Rectangle<float> area, bool isDown, bool isOver,
                                                       juce::Colour noteFillColour)
    {
        juce::MidiKeyboardComponent::drawBlackNote(midiNoteNumber, g, area, isDown, isOver, noteFillColour);

        // Black keys never carry an octave marker (RQ-GUI-075), so the fit
        // check is against the key's own width, reduced by the same 1px
        // margin the drawn text area below uses.
        const auto label = pianoKeyLabelFor(midiNoteNumber, _mapping, _mappingBaseMidiNote,
                                            /*isBlackKey*/ true, area.getWidth() - 2.0f,
                                            tokens::semantic::pianoKeyLabelSize, measureCharacterWidth);
        if (!label)
        {
            return; // dropped: too narrow to fit legibly [DEC-JUC-121]
        }

        g.setColour(tokens::semantic::pianoKeyLabelOnBlack);
        g.setFont(juce::Font{juce::FontOptions{tokens::semantic::pianoKeyLabelSize}});
        g.drawText(toDisplayString(*label), area.reduced(1.0f), juce::Justification::centredBottom, false);
    }
}
