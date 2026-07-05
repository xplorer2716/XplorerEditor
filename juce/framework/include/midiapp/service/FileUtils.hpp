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
}
