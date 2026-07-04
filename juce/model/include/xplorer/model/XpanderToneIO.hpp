#pragma once

// Port of XPanderToneReader/Writer, SinglePatchIterator and the sysex file
// type detection (reference XpanderController.DetermineSysexFileType).
// [RQ-MOD-041..043]

#include "midiapp/model/ToneIO.hpp"
#include "xpl/midi/SysexStreamIterator.hpp"

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace xplorer::model
{
    enum class SysexFileType
    {
        SingleTone,
        AllDataDump,
        Unknown,
    };

    /// Yields only the SysEx frames that carry the single-patch intro.
    class SinglePatchIterator
    {
    public:
        explicit SinglePatchIterator(std::span<const std::uint8_t> data)
            : _iterator(data)
        {
        }

        [[nodiscard]] static bool isSinglePatch(std::span<const std::uint8_t> frame);

        std::optional<std::vector<std::uint8_t>> next();

    private:
        xpl::midi::SysexStreamIterator _iterator;
    };

    /// Classifies a .syx file: one single-patch frame -> SingleTone; several
    /// frames or one unrecognized frame -> AllDataDump; unreadable/empty ->
    /// Unknown. Same rules as the reference. [RQ-MOD-043]
    [[nodiscard]] SysexFileType determineSysexFileType(const std::string& fileName);

    class XpanderToneReader final : public midiapp::model::IToneReader
    {
    public:
        /// Loads the first single patch of the file into the tone.
        /// @throws ToneException (missing file, no patch, decode error).
        void readTone(const std::string& filename, midiapp::model::AbstractTone& tone) override;

        /// All single patches of a bank file as (name, tone) pairs. [RQ-MOD-041]
        [[nodiscard]] std::vector<std::pair<std::string, std::unique_ptr<midiapp::model::AbstractTone>>>
        readTones(const std::string& filename) override;
    };

    class XpanderToneWriter final : public midiapp::model::IToneWriter
    {
    public:
        /// Writes the tone as a single-patch .syx. [RQ-MOD-042]
        void writeTone(const std::string& filename, const midiapp::model::AbstractTone& tone) override;
    };
}
