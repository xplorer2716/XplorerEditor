#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/PianoKeyMapping.hpp"

#include <map>

// The layout-independent half of RQ-GUI-074: given a KeyboardLayoutQuery, does
// resolvePianoKeyMapping() build the right bindings? This is pure C++, no OS
// call, no JUCE, no display — the fake below stands in for the three real
// per-OS queries (juce/app/src/PianoKeyboardLayoutQuery_*.cpp), which cannot
// be exercised deterministically since nothing here controls a CI runner's
// actual keyboard layout. [RQ-GUI-074, ADR-JUC-035 (DEC-JUC-114, DEC-JUC-116)]

using namespace xplorer::app;

namespace
{
    /// Scriptable KeyboardLayoutQuery: a fixed table of resolutions, absent
    /// entries reported as unresolvable. Mirrors MockMidiBackend's role for
    /// MidiBackend. [DEC-JUC-116]
    class FakeKeyboardLayoutQuery final : public KeyboardLayoutQuery
    {
    public:
        explicit FakeKeyboardLayoutQuery(std::map<char, char32_t> table) : _table(std::move(table)) {}

        [[nodiscard]] std::optional<char32_t> characterForPosition(char referenceChar) const override
        {
            const auto it = _table.find(referenceChar);
            return it != _table.end() ? std::optional<char32_t>{it->second} : std::nullopt;
        }

    private:
        std::map<char, char32_t> _table;
    };

    /// referenceChar -> referenceChar, i.e. a query that reports the position
    /// table's own names back: what a genuine US-QWERTY layout resolves to.
    FakeKeyboardLayoutQuery usQwertyQuery()
    {
        std::map<char, char32_t> table;
        for (const auto& position : pianoKeyPositions())
        {
            table[position.referenceChar] = static_cast<char32_t>(position.referenceChar);
        }
        return FakeKeyboardLayoutQuery(std::move(table));
    }
}

SCENARIO("The mapping resolves identically to JUCE's default on a US QWERTY layout",
         "[RQ-GUI-074]")
{
    GIVEN("a query that reports every position as itself (US QWERTY)")
    {
        const auto query = usQwertyQuery();

        WHEN("the mapping is resolved")
        {
            const auto resolved = resolvePianoKeyMapping(query);

            THEN("all seventeen positions resolve, matching JUCE's own reference string")
            {
                REQUIRE(resolved.size() == 17);
                const std::u32string expected = U"awsedftgyhujkolp;";
                for (std::size_t i = 0; i < resolved.size(); ++i)
                {
                    CHECK(resolved[i].character == expected[i]);
                    CHECK(resolved[i].noteOffsetFromC == static_cast<int>(i));
                }
            }
        }
    }
}

SCENARIO("The mapping follows a non-QWERTY layout by position, not by character",
         "[RQ-GUI-074]")
{
    GIVEN("a French AZERTY layout (only the letters JUCE's positions actually differ on)")
    {
        // AZERTY vs QWERTY, at the SAME physical positions used here: A<->Q,
        // Z<->W, M<->; (semicolon's physical neighbour). Every other letter in
        // the 17 is on the same key on both layouts.
        auto table = std::map<char, char32_t>{
            {'a', U'q'}, {'w', U'z'}, {'s', U's'}, {'e', U'e'}, {'d', U'd'}, {'f', U'f'},
            {'t', U't'}, {'g', U'g'}, {'y', U'y'}, {'h', U'h'}, {'u', U'u'}, {'j', U'j'},
            {'k', U'k'}, {'o', U'o'}, {'l', U'l'}, {'p', U'p'}, {';', U'm'},
        };
        const FakeKeyboardLayoutQuery query(table);

        WHEN("the mapping is resolved")
        {
            const auto resolved = resolvePianoKeyMapping(query);

            THEN("the home-row C is now 'q', and the note offsets are untouched")
            {
                REQUIRE(resolved.size() == 17);
                CHECK(resolved[0].character == U'q');   // was 'a', offset 0 (C)
                CHECK(resolved[0].noteOffsetFromC == 0);
                CHECK(resolved[1].character == U'z');   // was 'w', offset 1 (C#)
                CHECK(resolved[16].character == U'm');  // was ';', offset 16 (E)
            }
        }
    }
}

SCENARIO("An unresolvable position is left out, not guessed", "[RQ-GUI-074]")
{
    GIVEN("a query that fails on exactly one position")
    {
        auto query = usQwertyQuery();
        // Rebuild without 'g' to simulate one failed position.
        std::map<char, char32_t> table;
        for (const auto& position : pianoKeyPositions())
        {
            if (position.referenceChar != 'g')
            {
                table[position.referenceChar] = static_cast<char32_t>(position.referenceChar);
            }
        }
        const FakeKeyboardLayoutQuery partial(table);

        WHEN("the mapping is resolved")
        {
            const auto resolved = resolvePianoKeyMapping(partial);

            THEN("sixteen positions resolve and the seventeenth is simply absent")
            {
                REQUIRE(resolved.size() == 16);
                for (const auto& key : resolved)
                {
                    CHECK(key.character != U'g');
                }
            }
        }
    }
}

SCENARIO("buildPianoKeyMapping leaves JUCE's default alone when the query is unusable",
         "[RQ-GUI-074][DEC-JUC-118]")
{
    GIVEN("no query at all (platform could not start one)")
    {
        WHEN("the mapping is built")
        {
            const auto result = buildPianoKeyMapping(nullptr);

            THEN("the caller is told to keep JUCE's own default")
            {
                CHECK_FALSE(result.has_value());
            }
        }
    }

    GIVEN("a query that resolves nothing at all")
    {
        const FakeKeyboardLayoutQuery empty({});

        WHEN("the mapping is built")
        {
            const auto result = buildPianoKeyMapping(&empty);

            THEN("the caller is told to keep JUCE's own default")
            {
                CHECK_FALSE(result.has_value());
            }
        }
    }

    GIVEN("a query that resolves at least one position")
    {
        const auto query = usQwertyQuery();

        WHEN("the mapping is built")
        {
            const auto result = buildPianoKeyMapping(&query);

            THEN("a mapping to install is returned")
            {
                REQUIRE(result.has_value());
                CHECK(result->size() == 17);
            }
        }
    }
}
