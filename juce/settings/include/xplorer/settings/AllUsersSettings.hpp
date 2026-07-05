#pragma once

// Port of Xplorer.Controller.Service.Settings.AllUsersSettings. [RQ-SET-002, RQ-SET-003]

#include "xplorer/model/XpanderConstants.hpp"

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
            /// 32-bit ARGB, as .NET Color.ToArgb() persists it.
            int knobLedBorderColor = 0;
            bool knobMovementIsLinear = false;
            bool knobStyleIsStandard = false;
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
