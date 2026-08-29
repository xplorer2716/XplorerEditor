#pragma once

// Keyboard preset-value entry for rotary knobs (RQ-GUI-079): WHICH physical
// keys select a preset (RQ-GUI-080) and WHICH values the presets hold. Both
// halves are UI-framework-free and OS-free, so they stay headless-testable
// against a fake KeyboardLayoutQuery and keep building with
// -DXPL_BUILD_APP=OFF (RQ-BLD-025) — the same split ADR-JUC-035 gave the piano
// window's mapping, for the same reason.
// [RQ-GUI-079, RQ-GUI-080, ADR-JUC-037, ADR-JUC-035 (DEC-JUC-116)]

#include "xplorer/app/KeyboardLayoutQuery.hpp"

#include <optional>
#include <string>
#include <vector>

namespace xplorer::app
{
    /// How many presets a knob offers, by range shape. A knob whose range is
    /// symmetric about zero gets an ODD count so one preset lands exactly on
    /// the centre value; every other knob gets the ten number-row keys.
    /// [RQ-GUI-079, ADR-JUC-037 (DEC-JUC-128)]
    inline constexpr int UNIPOLAR_PRESET_COUNT = 10;
    inline constexpr int BIPOLAR_PRESET_COUNT = 11;

    /// One physical key position bound to one preset slot. `referenceChar`
    /// names the position — "the key physically labelled this way on a
    /// US-QWERTY keyboard" — and is never matched as a character; the active
    /// layout decides what it actually produces. Two positions MAY share a
    /// `presetIndex`. [ADR-JUC-037 (DEC-JUC-126, DEC-JUC-127)]
    struct PresetKeyPosition
    {
        char referenceChar;
        int presetIndex;
    };

    /// The number row, left to right, plus BOTH candidates for the eleventh
    /// slot. This table is the entire definition of "which keys set a knob".
    [[nodiscard]] const std::vector<PresetKeyPosition>& presetKeyPositions();

    /// One resolved binding: the character the layout actually produces at a
    /// position, and the preset slot it selects.
    struct ResolvedPresetKey
    {
        char32_t character;
        int presetIndex;
    };

    /// Resolves every position through the given query. Positions the query
    /// cannot resolve are simply absent — never guessed (DEC-JUC-118's rule,
    /// inherited). A null query resolves nothing, which disables the gesture
    /// rather than binding it wrongly. [RQ-GUI-080]
    [[nodiscard]] std::vector<ResolvedPresetKey> resolvePresetKeyMapping(const KeyboardLayoutQuery* query);

    /// The preset slot a pressed character selects, or nullopt when the
    /// character is not one of the resolved preset keys.
    [[nodiscard]] std::optional<int> presetIndexForCharacter(const std::vector<ResolvedPresetKey>& mapping,
                                                             char32_t character);

    /// The preset values a knob with this range offers, evenly spaced from
    /// minValue to maxValue inclusive. Symmetric ranges get
    /// BIPOLAR_PRESET_COUNT values (index 5 is exactly the centre); every
    /// other range gets UNIPOLAR_PRESET_COUNT.
    /// [RQ-GUI-079, ADR-JUC-037 (DEC-JUC-128, DEC-JUC-129)]
    [[nodiscard]] std::vector<int> presetValuesForRange(int minValue, int maxValue);

    /// The musically-spaced override for the parameters that have one, or
    /// nullptr when the parameter takes the range-derived values above.
    /// [RQ-GUI-079, ADR-JUC-037 (DEC-JUC-130)]
    [[nodiscard]] const std::vector<int>* harmonicPresetValuesFor(const std::string& parameterName);
}
