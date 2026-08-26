#pragma once

// The single platform-dependent question every layout-independent keyboard
// feature asks: "which character does this physical key position currently
// produce?". It lives in a header of its own because TWO features now ask it —
// the piano window's note mapping (RQ-GUI-074) and the rotary knobs'
// preset-value keys (RQ-GUI-080) — and neither should have to include the
// other's header to reach the interface.
//
// The concrete per-OS queries (Windows/Linux/macOS) live in the XplorerApp
// target (juce/app/src/PianoKeyboardLayoutQuery_*.cpp), never here: this file
// must stay buildable in the headless configuration (RQ-BLD-025).
// [RQ-GUI-074, RQ-GUI-080, ADR-JUC-035 (DEC-JUC-116), ADR-JUC-037 (DEC-JUC-131)]

#include <optional>

namespace xplorer::app
{
    /// Answers, for one physical position, which character it currently
    /// produces under whatever keyboard layout is active. A position that
    /// cannot be resolved to a usable character (dead key, non-printable,
    /// query failure) SHALL return std::nullopt rather than a guess.
    ///
    /// Positions are NAMED by the character they carry on a US-QWERTY
    /// keyboard — "the key physically labelled this way" — and that name is
    /// never matched as a character. [RQ-GUI-074, DEC-JUC-114]
    class KeyboardLayoutQuery
    {
    public:
        virtual ~KeyboardLayoutQuery() = default;

        [[nodiscard]] virtual std::optional<char32_t> characterForPosition(char referenceChar) const = 0;
    };
}
