#include "xplorer/app/PianoKeyMapping.hpp"

namespace xplorer::app
{
    const std::vector<PianoKeyPosition>& pianoKeyPositions()
    {
        // JUCE's own default, reference `juce_MidiKeyboardComponent.cpp`:
        //   const std::string_view keys { "awsedftgyhujkolp;" };
        //   for (const char& c : keys) setKeyPressForNote({c,0,0}, offset);
        // Reusing that exact order and offsets is what makes a US-QWERTY
        // resolution byte-identical to today's behaviour. [DEC-JUC-114]
        static const std::vector<PianoKeyPosition> positions = {
            {'a', 0},  {'w', 1},  {'s', 2},  {'e', 3},  {'d', 4},  {'f', 5},  {'t', 6},
            {'g', 7},  {'y', 8},  {'h', 9},  {'u', 10}, {'j', 11}, {'k', 12}, {'o', 13},
            {'l', 14}, {'p', 15}, {';', 16},
        };
        return positions;
    }

    std::vector<ResolvedPianoKey> resolvePianoKeyMapping(const KeyboardLayoutQuery& query)
    {
        std::vector<ResolvedPianoKey> resolved;
        for (const auto& position : pianoKeyPositions())
        {
            if (const auto character = query.characterForPosition(position.referenceChar))
            {
                resolved.push_back({*character, position.noteOffsetFromC});
            }
        }
        return resolved;
    }

    std::optional<std::vector<ResolvedPianoKey>> buildPianoKeyMapping(const KeyboardLayoutQuery* query)
    {
        if (query == nullptr)
        {
            return std::nullopt; // query unavailable altogether [DEC-JUC-118]
        }
        auto resolved = resolvePianoKeyMapping(*query);
        if (resolved.empty())
        {
            return std::nullopt; // every position failed: as good as unavailable
        }
        return resolved;
    }
}
