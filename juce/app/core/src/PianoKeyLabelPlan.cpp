#include "xplorer/app/PianoKeyLabelPlan.hpp"

namespace xplorer::app
{
    std::optional<char32_t> pianoKeyLabelFor(int midiNoteNumber, const std::vector<ResolvedPianoKey>& mapping,
                                             int mappingBaseMidiNote, bool isBlackKey, float keyWidthPixels,
                                             float fontSize, const PianoLabelWidthMeasurer& measureWidth)
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
