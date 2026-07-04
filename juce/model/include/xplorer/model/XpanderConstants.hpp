#pragma once

// Port of Xplorer.Common.XpanderConstants: structural constants, enums and
// page mapping tables. All values are protocol facts from the Oberheim
// Xpander/Matrix-12 MIDI spec and must match the reference exactly.
// [RQ-MOD-001..003]

#include <array>
#include <cstdint>

namespace xplorer::model
{
    namespace constants
    {
        inline constexpr int SINGLE_TONES_MAX_COUNT = 100;
        inline constexpr int MULTI_PATCHES_MAX_COUNT = 100;
        inline constexpr int TONE_NAME_LENGTH = 8;
        inline constexpr int MODENTRIES_COUNT = 20;
        inline constexpr int MAX_MODULATION_SOURCE = 6;
        inline constexpr int VCO_COUNT = 2;
        inline constexpr int LFO_COUNT = 5;
        inline constexpr int ENV_COUNT = 5;
        inline constexpr int TRACK_COUNT = 3;
        inline constexpr int TRACK_POINTS_COUNTS = 5;
        inline constexpr int RAMP_COUNT = 4;

        // Synthesizer type/display command, see MIDI spec. [RQ-CTL-061]
        inline constexpr int DISPLAY_CONTROL_COMMAND_XPANDER = 0x05;
        inline constexpr int DISPLAY_CONTROL_COMMAND_MATRIX12 = 0x06;
    }

    enum class EnumModulationFlags : std::uint8_t // bitfield
    {
        MODFLAG_KEYBD = 0x01,
        MODFLAG_LAG = 0x02,
        MODFLAG_LEV_1 = 0x04,
        MODFLAG_VIB = 0x08,
    };

    enum class EnumVCOWaveFlags : std::uint8_t // bitfield
    {
        VCOWAVEFLAG_TRI = 0x01,
        VCOWAVEFLAG_SAW = 0x02,
        VCOWAVEFLAG_PULSE = 0x04,
        VCOWAVEFLAG_SYNC = 0x08,
        VCOWAVEFLAG_NOISE = 0x10,
    };

    enum class EnumFMDestinationTypes
    {
        FMDEST_VCO,
        FMDESTFM_VCF,
    };

    enum class EnumLagModeFlags : std::uint8_t // bitfield
    {
        LAGMODE_LEGATO = 0x01,
        LAGMODE_EXPO = 0x02,
        LAGMODE_EQUAL_TIME = 0x04,
    };

    enum class EnumTriggerTypes
    {
        TRIGGER_OFF,
        TRIGGER_SINGLE,
        TRIGGER_MULTI,
        TRIGGER_EXTRIG,
    };

    enum class LagFlags : std::uint8_t // bitfield
    {
        LAGF_LAG = 0x01,
    };

    enum class EnumEnveloppeModeFlags : std::uint8_t // bitfield
    {
        ENVMODE_RESET = 0x01,
        ENVMODE_INVALID_VALUE = 0x02, // original MIDI spec did not mention this unused bit
        ENVMODE_MULTI = 0x04,         // SINGLE if not MULTI
        ENVMODE_GATED = 0x08,
        ENVMODE_EXTRIG = 0x10,
        ENVMODE_LFOTRIG = 0x20,
        ENVMODE_DADR = 0x40,
        ENVMODE_FREERUN = 0x80,
    };

    enum class EnumLFOTriggerCodes
    {
        LFOTRIG_LFO1,
        LFOTRIG_LFO2,
        LFOTRIG_LFO3,
        LFOTRIG_LFO4,
        LFOTRIG_LFO5,
        LFOTRIG_VIB,
    };

    enum class EnumRampFlags : std::uint8_t // bitfield
    {
        RAMPF_GATED = 0x01,
        RAMPF_LFOTRIG = 0x02,
        RAMPF_EXTRIG = 0x04,
        RAMPF_MULTI = 0x08, // SINGLE if not MULTI
    };

    enum class EnumModulationSources
    {
        KBD, LAG, VEL, RVEL, PRES,
        TRK1, TRK2, TRK3,
        RMP1, RMP2, RMP3, RMP4,
        ENV1, ENV2, ENV3, ENV4, ENV5,
        PED1, PED2,
        LFO1, LFO2, LFO3, LFO4, LFO5,
        VIB, LEV1, LEV2,
    };

    enum class EnumModulationSourcesModMatrix
    {
        KBD, LAG, VEL, RVEL, PRES,
        TRK1, TRK2, TRK3,
        RMP1, RMP2, RMP3, RMP4,
        ENV1, ENV2, ENV3, ENV4, ENV5,
        PED1, PED2,
        LFO1, LFO2, LFO3, LFO4, LFO5,
        VIB, LEV1, LEV2,
        NONE = 0x1B,
    };

    enum class EnumModulationDestinations
    {
        VCO1_FRQ, VCO1_PW, VCO1_VOL,
        VCO2_FRQ, VCO2_PW, VCO2_VOL,
        VCF_FRQ, VCF_RES, VCA1_VOL, VCA2_VOL,
        LFO1_SPD, LFO1_AMP,
        LFO2_SPD, LFO2_AMP,
        LFO3_SPD, LFO3_AMP,
        LFO4_SPD, LFO4_AMP,
        LFO5_SPD, LFO5_AMP,
        ENV1_DLY, ENV1_ATK, ENV1_DCY, ENV1_REL, ENV1_AMP,
        ENV2_DLY, ENV2_ATK, ENV2_DCY, ENV2_REL, ENV2_AMP,
        ENV3_DLY, ENV3_ATK, ENV3_DCY, ENV3_REL, ENV3_AMP,
        ENV4_DLY, ENV4_ATK, ENV4_DCY, ENV4_REL, ENV4_AMP,
        ENV5_DLY, ENV5_ATK, ENV5_DCY, ENV5_REL, ENV5_AMP,
        FM_AMP, LAG_RATE,
    };

    enum class EnumLFOTriggerSources
    {
        LFO1, LFO2, LFO3, LFO4, LFO5, VIB,
    };

    enum class EnumLFORetrigModes
    {
        OFF, SINGLE, MULTI, EXTRIG,
    };

    enum class EnumVCFFilterModes
    {
        _1_POLE_LOW, _2_POLE_LOW, _3_POLE_LOW, _4_POLE_LOW,
        _1_POLE_HIGH, _2_POLE_HIGH, _3_POLE_HIGH,
        _2_POLE_BAND, _4_POLE_BAND,
        _2_POLE_NOTCH, _3_POLE_PHASE,
        _2_HIGH_1_LOW, _3_HIGH_1_LOW,
        _2_NOTCH_1_LOW, _3_PHASE_1_LOW,
    };

    enum class EnumLFOWaveShapes
    {
        TRIANGLE, UP_SAW, DOWN_SAW, SQUARE, RANDOM, NOISE, SAMPLE,
    };

    enum class EnumModulationEditCommands
    {
        UNKNOWN = -1,
        ADDSOURCE = 0x00,
        DELETESOURCE,
        CHANGESOURCE,
        SETUNSIGNEDVALUE,
        DIALVALUEAMOUNTOFCHANGE,
        SETQUANTIZE,
        TOGGLEQUANTIZE,
        SETSIGN,
    };

    enum class EnumPages
    {
        UNKNOWN = -1,
        MASTER_X = 0x00,
        TUNE_X = 0x01,
        CHAIN = 0x02,
        MASTER_MISC_M12 = 0x03,
        GATE_XP_X = 0x03,
        VOICE_ON_OFF = 0x04,
        MIDI = 0x05,
        CASSETTE = 0x06,
        SEND_CASSETTE = 0x07,
        CHECK_CASSETTE = 0x08,
        FROM_CASSETTE = 0x09,
        LOAD_ALL = 0x0F,
        LOAD_ONE = 0x10,
        LOAD_SINGLE = 0x11,
        LOAD_MULTI = 0x12,
        LOAD_CHAIN = 0x13,
        START_LOAD = 0x14,
        LOAD_GLOBAL = 0x15,
        MIDI_BASIC_CHANNEL = 0x16,
        MIDI_CONTROLLERS = 0x17,
        MIDI_ENABLES = 0x18,
        MIDI_SEND_PATCH = 0x19,
        MIDI_RESET = 0x1A,
        MIDI_MUTE = 0x1B,
        VERSION_X = 0x1D,
        VCO_1_X = 0x20,
        VCO_2_X = 0x21,
        VCF_VCA_X = 0x22,
        FM_LAG_X = 0x23,
        ENV_1 = 0x28,
        ENV_2 = 0x29,
        ENV_3 = 0x2A,
        ENV_4 = 0x2B,
        ENV_5 = 0x2C,
        LFO_1 = 0x30,
        LFO_2 = 0x31,
        LFO_3 = 0x32,
        LFO_4 = 0x33,
        LFO_5 = 0x34,
        TRACK_1 = 0x38,
        TRACK_2 = 0x39,
        TRACK_3 = 0x3A,
        RAMP_1 = 0x40,
        RAMP_2 = 0x41,
        RAMP_3 = 0x42,
        RAMP_4 = 0x43,
        ZONE_1_M12_X = 0x50,
        ZONE_2_M12_X = 0x51,
        ZONE_3_M12_X = 0x52,
        ZONE_4_M12_X = 0x53,
        ZONE_5_M12_X = 0x54,
        MISC_NAME_M12 = 0x58,
        NAME_XP = 0x59,
        TRANSPOSE = 0x60,
        PAN = 0x61,
        VASSIGN_CVMIDI = 0x62,
        ZONE_XP = 0x63,
        ZONE_INPUT_XP = 0x64,
        ZONE_LIMITS_XP = 0x65,
        ZONE_MODE_XP = 0x66,
        VOLUME = 0x67,
        VIB = 0x68,
        NAME_M12 = 0x69,
        DETUNE = 0x6A,
        MOD_ROUTINGS = 0x6B,
    };

    enum class EnumRotaryEncoders
    {
        FIRST = 0x18,
        SECOND = 0x19,
        THIRD = 0x1A,
        FOURTH = 0x1B,
        FIFTH = 0x1C,
        SIXTH = 0x1D,
    };

    struct PageSubPage
    {
        EnumPages page;
        int subPage;
    };

    /// Page/sub-page for each modulation destination (same order as
    /// EnumModulationDestinations). [RQ-MOD-003]
    inline constexpr std::array<PageSubPage, static_cast<std::size_t>(EnumModulationDestinations::LAG_RATE) + 1>
        PAGE_SUBPAGE_FOR_MODULATION_DESTINATION{{
            {EnumPages::VCO_1_X, 2}, {EnumPages::VCO_1_X, 5}, {EnumPages::VCO_1_X, 7},
            {EnumPages::VCO_2_X, 2}, {EnumPages::VCO_2_X, 5}, {EnumPages::VCO_2_X, 7},
            {EnumPages::VCF_VCA_X, 2}, {EnumPages::VCF_VCA_X, 3}, {EnumPages::VCF_VCA_X, 6}, {EnumPages::VCF_VCA_X, 7},
            {EnumPages::LFO_1, 2}, {EnumPages::LFO_1, 7},
            {EnumPages::LFO_2, 2}, {EnumPages::LFO_2, 7},
            {EnumPages::LFO_3, 2}, {EnumPages::LFO_3, 7},
            {EnumPages::LFO_4, 2}, {EnumPages::LFO_4, 7},
            {EnumPages::LFO_5, 2}, {EnumPages::LFO_5, 7},
            {EnumPages::ENV_1, 2}, {EnumPages::ENV_1, 3}, {EnumPages::ENV_1, 4}, {EnumPages::ENV_1, 6}, {EnumPages::ENV_1, 7},
            {EnumPages::ENV_2, 2}, {EnumPages::ENV_2, 3}, {EnumPages::ENV_2, 4}, {EnumPages::ENV_2, 6}, {EnumPages::ENV_2, 7},
            {EnumPages::ENV_3, 2}, {EnumPages::ENV_3, 3}, {EnumPages::ENV_3, 4}, {EnumPages::ENV_3, 6}, {EnumPages::ENV_3, 7},
            {EnumPages::ENV_4, 2}, {EnumPages::ENV_4, 3}, {EnumPages::ENV_4, 4}, {EnumPages::ENV_4, 6}, {EnumPages::ENV_4, 7},
            {EnumPages::ENV_5, 2}, {EnumPages::ENV_5, 3}, {EnumPages::ENV_5, 4}, {EnumPages::ENV_5, 6}, {EnumPages::ENV_5, 7},
            {EnumPages::FM_LAG_X, 2}, {EnumPages::FM_LAG_X, 6},
        }};

    /// Reverse lookup; returns the first enumerator (VCO1_FRQ) when no match,
    /// as the reference TryGetValue leaves the default value. [RQ-MOD-003]
    [[nodiscard]] constexpr EnumModulationDestinations modulationDestinationForPageSubPage(int page, int subPage)
    {
        for (std::size_t index = 0; index < PAGE_SUBPAGE_FOR_MODULATION_DESTINATION.size(); ++index)
        {
            const auto& entry = PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[index];
            if (static_cast<int>(entry.page) == page && entry.subPage == subPage)
            {
                return static_cast<EnumModulationDestinations>(index);
            }
        }
        return EnumModulationDestinations::VCO1_FRQ;
    }
}
