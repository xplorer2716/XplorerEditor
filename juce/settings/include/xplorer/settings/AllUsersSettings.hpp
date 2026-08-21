#pragma once

// Port of Xplorer.Controller.Service.Settings.AllUsersSettings. [RQ-SET-002, RQ-SET-003]

#include "xplorer/model/XpanderConstants.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace xplorer::settings
{
    struct AllUsersSettings
    {
        struct MidiConfiguration
        {
            std::string automationInputDeviceName;
            std::string synthInputDeviceName;
            std::string synthOutputDeviceName;
            int sysexTransmitDelay = 0;
            int midiChannel = 0;
            int editingProgramNumber = 0;
            bool smartAllNotesOff = false;
            bool synthTypeIsMatrix12 = false;
            /// "PARAMETER_NAME;CC" entries; CC 128 means not assigned.
            std::vector<std::string> automationTable;
        };

        struct UiConfiguration
        {
            /// Number of user-themeable functional-block colours. Array index
            /// order is the app-side BlockId order: VCO, LAG, TRACK, VCF, ENV,
            /// LFO, RAMP, MATRIX. [RQ-SET-007, ADR-JUC-020 (DEC-JUC-039)]
            static constexpr std::size_t BLOCK_COLOUR_COUNT = 8;

            /// 32-bit ARGB, as .NET Color.ToArgb() persists it.
            int knobLedBorderColor = 0;
            /// Individually optional per-block ARGB overrides; an unset entry
            /// means "use the design-system default", so files from earlier
            /// versions and imported .NET files (RQ-SET-006) load unchanged.
            /// Reset-to-defaults CLEARS entries — defaults are never written
            /// out as literals. [RQ-SET-007, ADR-JUC-020 (DEC-JUC-039)]
            std::array<std::optional<int>, BLOCK_COLOUR_COUNT> blockColours{};
        };

        struct RandomizerConfiguration
        {
            model::EnumRandomVCO2 vco2FmNoiseSync{};      // [Flags]
            model::EnumRandomVCOFreq vcoFreq{};
            model::EnumRandomVCODetune vcoDetune{};
            model::EnumRandomVCAEnv vca2Env{};
            model::EnumRandomModMatrix modulationMatrix{}; // [Flags]
        };

        MidiConfiguration midiConfig;
        UiConfiguration uiConfig;
        RandomizerConfiguration randomizerConfig;
    };

    /// Reference defaults (DefaultAllusersSettings). [RQ-SET-004]
    [[nodiscard]] AllUsersSettings defaultAllUsersSettings();
    [[nodiscard]] AllUsersSettings::RandomizerConfiguration defaultRandomizerConfiguration();
}
