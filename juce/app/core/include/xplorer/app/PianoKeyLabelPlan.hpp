#pragma once

// Piano-window key-label decision (UI-framework-free, headless-tested),
// mirroring ComboBoxSizing.hpp's shape: the fit check needs real font
// metrics, so the measurer is INJECTED and this module stays JUCE-free
// (builds and is tested by the always-run headless CI job, RQ-BLD-025). The
// JUCE side (PianoLabelledKeyboardComponent) supplies a real
// GlyphArrangement-backed measurer; tests supply a fake one.
// [RQ-GUI-075, ADR-JUC-035 (DEC-JUC-119..121)]

#include "xplorer/app/PianoKeyMapping.hpp"

#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

namespace xplorer::app
{
    /// Measures how wide `character` renders at `fontSize`, in the same units
    /// as the key width passed to pianoKeyLabelFor. This is the type-erased
    /// form, for callers that hold a measurer as a value; pianoKeyLabelFor
    /// takes any callable of this shape, so a function or lambda reaches it
    /// without being wrapped. [RQ-QLT-017, TASK-QLT-014]
    using PianoLabelWidthMeasurer = std::function<float(char32_t character, float fontSize)>;

    /// Decides whether `midiNoteNumber` shows its binding character.
    ///
    /// `mappingBaseMidiNote` converts a ResolvedPianoKey's noteOffsetFromC
    /// into an absolute note — the same `12 * keyMappingOctave` JUCE's
    /// setKeyPressForNote uses internally, computed once by the caller so
    /// this module needs no knowledge of JUCE's default octave.
    ///
    /// White keys (`isBlackKey == false`) always show a resolved binding — no
    /// fit check, since JUCE draws them full-width. Black keys additionally
    /// require the measured width to fit within `keyWidthPixels`; if it does
    /// not, the label is dropped (DEC-JUC-121) rather than clipped or shrunk.
    /// A note absent from `mapping` returns std::nullopt on either kind of
    /// key. [RQ-GUI-075, RQ-QLT-017, TASK-QLT-014]
    template <typename Measurer>
        requires std::is_invocable_r_v<float, const Measurer&, char32_t, float>
    [[nodiscard]] std::optional<char32_t> pianoKeyLabelFor(
        int midiNoteNumber, const std::vector<ResolvedPianoKey>& mapping, int mappingBaseMidiNote,
        bool isBlackKey, float keyWidthPixels, float fontSize, const Measurer& measureWidth)
    {
        for (const auto& key : mapping)
        {
            if (mappingBaseMidiNote + key.noteOffsetFromC != midiNoteNumber)
            {
                continue;
            }
            if (isBlackKey && measureWidth(key.character, fontSize) > keyWidthPixels)
            {
                return std::nullopt; // too narrow to fit legibly [DEC-JUC-121]
            }
            return key.character;
        }
        return std::nullopt; // this note carries no binding
    }
}
