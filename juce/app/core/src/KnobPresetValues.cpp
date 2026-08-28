#include "xplorer/app/KnobPresetValues.hpp"

#include <cstddef>

namespace xplorer::app
{
    namespace
    {
        /// The eleventh preset slot, shared by two physical positions.
        /// [ADR-JUC-037 (DEC-JUC-127)]
        constexpr int ELEVENTH_PRESET_INDEX = BIPOLAR_PRESET_COUNT - 1;

        /// The reference's harmonic override, MainForm.Overrides.cs
        /// SetKnobControlsPredefinedValues(): the VCO frequency knobs step
        /// through a stack of major triads in semitones — root, major third,
        /// fifth, octave, and on to three octaves — rather than the linear
        /// tenths presetValuesForRange() derives from the 0..63 range.
        /// [ADR-JUC-037 (DEC-JUC-130)]
        const std::vector<int>& harmonicSemitonePresets()
        {
            static const std::vector<int> values = {0, 4, 7, 12, 16, 19, 24, 28, 31, 36};
            return values;
        }

        /// The parameters carrying that override. Named here rather than
        /// tested for at the call site, so adding a third knob is a data
        /// change. [ADR-JUC-037 (DEC-JUC-130)]
        bool takesHarmonicPresets(const std::string& parameterName)
        {
            return parameterName == "VCO1_FREQ" || parameterName == "VCO2_FREQ";
        }

        /// True when the range is symmetric about zero, which is what earns a
        /// knob its eleventh preset. [DEC-JUC-128]
        bool isSymmetricAboutZero(int minValue, int maxValue)
        {
            return minValue < 0 && maxValue > 0 && -minValue == maxValue;
        }

        /// share(span, i, steps) = span * i / steps, rounded to nearest.
        ///
        /// Integer arithmetic on purpose: the reference computed each half of a
        /// symmetric range separately in floating point and took Math.Ceiling
        /// of each, which is what made its ±63 presets land on -12 against +13.
        /// One expression over the WHOLE span cannot drift that way, and since
        /// a symmetric span is even, span*i never ends in 5 — so the half-up
        /// tie never occurs and share(i) + share(steps-i) == span exactly.
        /// [DEC-JUC-129]
        int share(int span, int index, int steps)
        {
            const auto numerator = static_cast<long long>(span) * index + steps / 2;
            return static_cast<int>(numerator / steps);
        }
    }

    const std::vector<PresetKeyPosition>& presetKeyPositions()
    {
        // The number row, left to right, named by its US-QWERTY labels — the
        // same ten physical keys the reference reached through Keys.D1..D0.
        //
        // The eleventh slot is bound TWICE, and that is the point: the
        // reference asked for Keys.OemOpenBrackets, a virtual-key code Windows
        // places on a DIFFERENT physical key per layout ('[' after P on US,
        // ')' right of 0 on AZERTY). A positional seam cannot express "wherever
        // that VK went", so both candidate positions select the same preset:
        // every layout finds at least one, no layout loses its key, and the
        // reference's US/AZERTY divergence disappears instead of being
        // inherited. A layout where a position is a dead key (AZERTY's '^' at
        // the '[' position) simply fails to resolve and falls out.
        // [ADR-JUC-037 (DEC-JUC-126, DEC-JUC-127)]
        static const std::vector<PresetKeyPosition> positions = {
            {'1', 0}, {'2', 1}, {'3', 2}, {'4', 3},  {'5', 4}, {'6', 5},
            {'7', 6}, {'8', 7}, {'9', 8}, {'0', 9},
            {'-', ELEVENTH_PRESET_INDEX}, {'[', ELEVENTH_PRESET_INDEX},
        };
        return positions;
    }

    std::vector<ResolvedPresetKey> resolvePresetKeyMapping(const KeyboardLayoutQuery* query)
    {
        std::vector<ResolvedPresetKey> resolved;
        if (query == nullptr)
        {
            // No query at all: the gesture is disabled rather than bound to
            // guessed characters, which on a non-QWERTY layout would fire the
            // wrong preset — the failure ordering RQ-GUI-080 inherits from
            // DEC-JUC-118.
            return resolved;
        }
        for (const auto& position : presetKeyPositions())
        {
            if (const auto character = query->characterForPosition(position.referenceChar))
            {
                resolved.push_back({*character, position.presetIndex});
            }
        }
        return resolved;
    }

    std::optional<int> presetIndexForCharacter(const std::vector<ResolvedPresetKey>& mapping,
                                               char32_t character)
    {
        for (const auto& key : mapping)
        {
            if (key.character == character)
            {
                return key.presetIndex;
            }
        }
        return std::nullopt;
    }

    std::vector<int> presetValuesForRange(int minValue, int maxValue)
    {
        std::vector<int> values;
        if (maxValue <= minValue)
        {
            return values; // a degenerate range offers no presets
        }

        const int count =
            isSymmetricAboutZero(minValue, maxValue) ? BIPOLAR_PRESET_COUNT : UNIPOLAR_PRESET_COUNT;
        const int steps = count - 1;
        const int span = maxValue - minValue;

        values.reserve(static_cast<std::size_t>(count));
        for (int i = 0; i < count; ++i)
        {
            values.push_back(minValue + share(span, i, steps));
        }
        return values;
    }

    const std::vector<int>* harmonicPresetValuesFor(const std::string& parameterName)
    {
        return takesHarmonicPresets(parameterName) ? &harmonicSemitonePresets() : nullptr;
    }
}
