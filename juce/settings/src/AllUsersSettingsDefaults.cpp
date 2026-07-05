#include "xplorer/settings/AllUsersSettings.hpp"

#include <cstdint>

namespace xplorer::settings
{
    using namespace xplorer::model;

    namespace
    {
        // .NET Color.FromArgb(102, 181, 227).ToArgb() — opaque light blue.
        constexpr auto DEFAULT_KNOB_COLOR = static_cast<std::int32_t>(0xFF66B5E3);

        const std::vector<std::string> DEFAULT_AUTOMATION_TABLE{
#include "DefaultAutomationTable.inc"
        };
    }

    AllUsersSettings::RandomizerConfiguration defaultRandomizerConfiguration()
    {
        // Reference GetRandomizerConfigurationDefault: randomize everything.
        AllUsersSettings::RandomizerConfiguration config;
        config.vcoFreq = EnumRandomVCOFreq::Free;
        config.vcoDetune = EnumRandomVCODetune::Free;
        config.vco2FmNoiseSync = static_cast<EnumRandomVCO2>(
            static_cast<int>(EnumRandomVCO2::EnableFM)
            | static_cast<int>(EnumRandomVCO2::EnableNoise)
            | static_cast<int>(EnumRandomVCO2::EnableSync));
        config.vca2Env = EnumRandomVCAEnv::Free;
        config.modulationMatrix = static_cast<EnumRandomModMatrix>(
            static_cast<int>(EnumRandomModMatrix::EnableAmount)
            | static_cast<int>(EnumRandomModMatrix::EnableSourcesAndDestinations)
            | static_cast<int>(EnumRandomModMatrix::EnableQuantize));
        return config;
    }

    AllUsersSettings defaultAllUsersSettings()
    {
        AllUsersSettings settings;

        settings.midiConfig.editingProgramNumber = constants::SINGLE_TONES_MAX_COUNT - 1;
        settings.midiConfig.midiChannel = 1;
        settings.midiConfig.synthTypeIsMatrix12 = false;
        settings.midiConfig.sysexTransmitDelay = 30;
        settings.midiConfig.smartAllNotesOff = true;
        settings.midiConfig.automationTable = DEFAULT_AUTOMATION_TABLE;

        settings.uiConfig.knobLedBorderColor = DEFAULT_KNOB_COLOR;
        settings.uiConfig.knobMovementIsLinear = true;

        settings.randomizerConfig = defaultRandomizerConfiguration();
        return settings;
    }
}
