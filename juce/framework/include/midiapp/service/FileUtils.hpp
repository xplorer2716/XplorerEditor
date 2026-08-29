#pragma once

// Port of MidiApp.MidiController.Service.FileUtils. [RQ-CTL-003, RQ-CTL-004]

#include <string>

namespace midiapp::service
{
    inline constexpr auto SYSEX_FILE_EXTENSION_WITH_DOT = ".syx";

    /// Sanitizes `name` (removes filesystem-invalid characters plus ":.)&",
    /// as the reference) and appends 1, 2, ... until the file name is unique
    /// in `directoryName`. Returns the file name including the extension.
    [[nodiscard]] std::string makeUniqueFilenameFromString(const std::string& name,
                                                           const std::string& extension,
                                                           const std::string& directoryName);

    /// Trims trailing space characters — the padding character used by the
    /// synth's fixed-width on-wire tone-name storage (`XpanderTone::
    /// setToneName`) — so a short tone name does not carry meaningless
    /// trailing spaces into a file name built via
    /// `makeUniqueFilenameFromString` (space is otherwise a legal file-name
    /// character, so the sanitizer above does not remove it). [RQ-GUI-077]
    [[nodiscard]] std::string trimTrailingSpaces(const std::string& value);
}
