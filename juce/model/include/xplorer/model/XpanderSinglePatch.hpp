#pragma once

// Port of Xplorer.Model.XPanderSinglePatch: binary image of a single-patch
// program dump. Layout (399 bytes total): 5-byte intro F0 10 02 01 00,
// 1 program-number byte, 196 packetized data bytes (2 bytes each), 1 EOX.
// [RQ-MOD-022, RQ-MOD-043]

#include "xplorer/model/XpanderConstants.hpp"

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace xplorer::model
{
    class XpanderSinglePatch
    {
    public:
        static constexpr int SINGLE_PATCH_LENGTH = 399; // 6 + 196*2 + EOX
        static constexpr int SINGLE_PATCH_INTRO_LENGTH = 6;
        static constexpr std::uint8_t UNUSED_ENTRY_SOURCE_VALUE = 0x1F;
        static constexpr std::uint8_t UNUSED_ENTRY_DEST_VALUE = 0x3F;
        /// F0 10 02 01 00 — followed by the program number byte.
        static constexpr std::array<std::uint8_t, SINGLE_PATCH_INTRO_LENGTH - 1> PATCH_INTRO{
            0xF0, 0x10, 0x02, 0x01, 0x00};

        struct VCO
        {
            std::uint8_t freq = 0;
            std::int8_t detune = 0;
            std::uint8_t pw = 0;
            std::uint8_t vol = 0;
            std::uint8_t mod = 0;  // standard modulation flags
            std::uint8_t wave = 0; // wave shape flags
        };

        struct VCF
        {
            std::uint8_t freq = 0;
            std::uint8_t res = 0;
            std::uint8_t fmode = 0; // filter mode code
            std::uint8_t vca1 = 0;
            std::uint8_t vca2 = 0;
            std::uint8_t mod = 0;
        };

        struct FMAndLAG
        {
            std::uint8_t f_amp = 0;
            std::uint8_t f_dest = 0;
            std::uint8_t lag_in = 0;
            std::uint8_t lag_rate = 0;
            std::uint8_t lag_mode = 0;
        };

        struct LFO
        {
            std::uint8_t speed = 0;
            std::uint8_t retrig_mode = 0;
            std::uint8_t lag = 0;
            std::uint8_t wave = 0;
            std::uint8_t retrig = 0; // retrigger point
            std::uint8_t sample = 0; // sample source
            std::uint8_t amp = 0;
        };

        struct Enveloppe
        {
            std::uint8_t flags = 0;
            std::uint8_t lfotrig = 0;
            std::uint8_t delay = 0;
            std::uint8_t attack = 0;
            std::uint8_t decay = 0;
            std::uint8_t sustain = 0;
            std::uint8_t release = 0;
            std::uint8_t amp = 0;
        };

        struct Track
        {
            std::uint8_t input = 0;
            std::array<std::uint8_t, constants::TRACK_POINTS_COUNTS> points{};
        };

        struct Ramp
        {
            std::uint8_t rate = 0;
            std::uint8_t flags = 0;
            std::uint8_t lfotrig = 0;
        };

        struct ModulationEntry
        {
            std::uint8_t source = 0;
            bool quantize = false;
            std::int8_t amount = 0;
            std::uint8_t dest = 0;
        };

        std::array<VCO, constants::VCO_COUNT> vcos{};
        VCF vcf{};
        FMAndLAG fmAndLag{};
        std::array<LFO, constants::LFO_COUNT> lfos{};
        std::array<Enveloppe, constants::ENV_COUNT> enveloppes{};
        std::array<Track, constants::TRACK_COUNT> tracks{};
        std::array<Ramp, constants::RAMP_COUNT> ramps{};
        std::array<ModulationEntry, constants::MODENTRIES_COUNT> modulationEntries{};
        std::array<std::uint8_t, constants::TONE_NAME_LENGTH> name{};

        /// Locates the patch intro anywhere in `data` and decodes the
        /// packetized payload. @throws midiapp::model::ToneException on
        /// missing/short data (reference NonFatalException). [RQ-MOD-022]
        void fromByteArray(std::span<const std::uint8_t> data);

        /// Encodes the 399-byte dump for the given program number. [RQ-MOD-022]
        [[nodiscard]] std::vector<std::uint8_t> toByteArray(int programNumber) const;

        /// Patch name from a raw dump (empty string when the size is wrong),
        /// ASCII, located at the top end of the array. [RQ-MOD-023]
        [[nodiscard]] static std::string getNameFromByteArray(std::span<const std::uint8_t> bytes);

        [[nodiscard]] std::string patchName() const;
        void setPatchName(const std::string& newName);
    };
}
