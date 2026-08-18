#pragma once

// Layout-independent computer-keyboard mapping for the piano window. The
// physical-position table and the resolution algorithm are UI-framework-free
// and OS-free — headless-testable against a fake KeyboardLayoutQuery. The
// concrete per-OS queries (Windows/Linux/macOS) live in the XplorerApp target
// (juce/app/src/PianoKeyboardLayoutQuery_*.cpp), never here: this file must
// stay buildable in the headless configuration (RQ-BLD-025).
// [RQ-GUI-074, ADR-JUC-035 (DEC-JUC-114, DEC-JUC-116, DEC-JUC-118)]

#include <cstdint>
#include <optional>
#include <vector>

namespace xplorer::app
{
    /// One of the seventeen physical keys JUCE's own default piano mapping
    /// already uses (`awsedftgyhujkolp;`), reused here as a stable NAME for a
    /// physical position rather than as a character to match. `referenceChar`
    /// identifies the position — "the key physically labelled this way on a
    /// US-QWERTY keyboard" — never the character a query should return.
    /// `noteOffsetFromC` is the same 0..16 semitone offset JUCE's constructor
    /// passes to setKeyPressForNote(). [DEC-JUC-114]
    struct PianoKeyPosition
    {
        char referenceChar;
        int noteOffsetFromC;
    };

    /// The seventeen positions in JUCE's own reference order: ten white keys
    /// (home row) then interleaved with the seven black keys (upper row),
    /// skipping the Mi-Fa and Si-Do steps a piano has no black key for. This
    /// table is the ENTIRE definition of "what a piano looks like typed on a
    /// keyboard" — it never varies with the user's layout.
    [[nodiscard]] const std::vector<PianoKeyPosition>& pianoKeyPositions();

    /// Answers, for one physical position, which character it currently
    /// produces under whatever keyboard layout is active. Implementations are
    /// per-OS and live outside this headless library. A position that cannot
    /// be resolved to a usable character (dead key, non-printable, query
    /// failure) SHALL return std::nullopt rather than a guess. [RQ-GUI-074]
    class KeyboardLayoutQuery
    {
    public:
        virtual ~KeyboardLayoutQuery() = default;

        [[nodiscard]] virtual std::optional<char32_t> characterForPosition(char referenceChar) const = 0;
    };

    /// One resolved binding: a character, bound to the note it plays.
    struct ResolvedPianoKey
    {
        char32_t character;
        int noteOffsetFromC;
    };

    /// Resolves every position in pianoKeyPositions() through the given
    /// query. Positions the query cannot resolve are simply absent from the
    /// result — never guessed. [RQ-GUI-074]
    [[nodiscard]] std::vector<ResolvedPianoKey> resolvePianoKeyMapping(const KeyboardLayoutQuery& query);

    /// The mapping to install, or std::nullopt if JUCE's own built-in default
    /// should be left untouched. nullopt covers BOTH failure modes of
    /// DEC-JUC-118: `query` itself unavailable (nullptr — the platform could
    /// not even start a query, e.g. no X11 display), and a query that
    /// resolved nothing at all (every position failed). A PARTIAL mapping
    /// (some positions resolved, some not) is still returned — it is strictly
    /// more useful than the built-in default on a non-QWERTY layout. [RQ-GUI-074]
    [[nodiscard]] std::optional<std::vector<ResolvedPianoKey>> buildPianoKeyMapping(
        const KeyboardLayoutQuery* query);
}
