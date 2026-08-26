#include <catch2/catch_test_macros.hpp>

#include "xplorer/app/KnobPresetValues.hpp"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

// The two headless halves of the knob preset-value feature: which preset a
// physical key selects (RQ-GUI-080) and which value a preset holds
// (RQ-GUI-079). No OS call, no JUCE, no display — the fake below stands in for
// the three real per-OS queries (juce/app/src/PianoKeyboardLayoutQuery_*.cpp),
// which cannot be exercised deterministically since nothing here controls a CI
// runner's actual keyboard layout. Same pattern, same reason, as
// PianoKeyMappingTests. [RQ-GUI-079, RQ-GUI-080, ADR-JUC-037]

using namespace xplorer::app;

namespace
{
    /// Scriptable KeyboardLayoutQuery: a fixed table of resolutions, absent
    /// entries reported as unresolvable. [DEC-JUC-116]
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

    /// Every position reports its own name back: what a US-QWERTY layout does.
    FakeKeyboardLayoutQuery usQwertyQuery()
    {
        std::map<char, char32_t> table;
        for (const auto& position : presetKeyPositions())
        {
            table[position.referenceChar] = static_cast<char32_t>(position.referenceChar);
        }
        return FakeKeyboardLayoutQuery(std::move(table));
    }

    // The four accented characters an AZERTY number row produces, written as
    // Unicode escapes rather than as literals: this file is UTF-8 with no BOM,
    // and MSVC would otherwise decode them through the host's ANSI codepage and
    // reject them ("too many characters in constant"). Escapes keep the test
    // independent of the source encoding and of any /utf-8 build switch.
    constexpr char32_t E_ACUTE = 0x00E9;   // U+00E9 LATIN SMALL LETTER E WITH ACUTE
    constexpr char32_t E_GRAVE = 0x00E8;   // U+00E8 LATIN SMALL LETTER E WITH GRAVE
    constexpr char32_t C_CEDILLA = 0x00E7; // U+00E7 LATIN SMALL LETTER C WITH CEDILLA
    constexpr char32_t A_GRAVE = 0x00E0;   // U+00E0 LATIN SMALL LETTER A WITH GRAVE

    /// A French AZERTY layout at the SAME physical positions: the number row
    /// produces punctuation and accented letters, the position US labels '['
    /// is the dead key '^' and resolves to nothing, and the eleventh slot is
    /// reachable only through the position right of '0'. Measured against a
    /// real fr-FR layout (MapVirtualKey), not invented.
    FakeKeyboardLayoutQuery azertyQuery()
    {
        return FakeKeyboardLayoutQuery({
            {'1', U'&'}, {'2', E_ACUTE}, {'3', U'"'},      {'4', U'\''}, {'5', U'('}, {'6', U'-'},
            {'7', E_GRAVE}, {'8', U'_'}, {'9', C_CEDILLA}, {'0', A_GRAVE}, {'-', U')'},
            // '[' deliberately absent: dead key, unresolvable.
        });
    }

    /// The same ten characters in preset order, for the round-trip assertion.
    const std::vector<char32_t>& azertyNumberRow()
    {
        static const std::vector<char32_t> row = {U'&', E_ACUTE, U'"',      U'\'',   U'(',
                                                  U'-', E_GRAVE, U'_',      C_CEDILLA, A_GRAVE};
        return row;
    }
}

SCENARIO("The preset keys resolve to the number row on a US QWERTY layout",
         "[RQ-GUI-080]")
{
    GIVEN("a query that reports every position as itself (US QWERTY)")
    {
        const auto query = usQwertyQuery();

        WHEN("the preset key mapping is resolved")
        {
            const auto resolved = resolvePresetKeyMapping(&query);

            THEN("the ten digits select presets 0..9 in order")
            {
                const std::u32string digits = U"1234567890";
                for (std::size_t i = 0; i < digits.size(); ++i)
                {
                    const auto index = presetIndexForCharacter(resolved, digits[i]);
                    REQUIRE(index.has_value());
                    CHECK(*index == static_cast<int>(i));
                }
            }

            THEN("both eleventh-slot positions select the same preset")
            {
                const auto viaMinus = presetIndexForCharacter(resolved, U'-');
                const auto viaBracket = presetIndexForCharacter(resolved, U'[');
                REQUIRE(viaMinus.has_value());
                REQUIRE(viaBracket.has_value());
                CHECK(*viaMinus == BIPOLAR_PRESET_COUNT - 1);
                CHECK(*viaBracket == BIPOLAR_PRESET_COUNT - 1);
            }
        }
    }
}

SCENARIO("The preset keys follow a non-QWERTY layout by position, not by character",
         "[RQ-GUI-080]")
{
    GIVEN("a French AZERTY layout")
    {
        const auto query = azertyQuery();

        WHEN("the preset key mapping is resolved")
        {
            const auto resolved = resolvePresetKeyMapping(&query);

            THEN("the same physical keys select the same presets, under other characters")
            {
                const auto& azerty = azertyNumberRow();
                for (std::size_t i = 0; i < azerty.size(); ++i)
                {
                    const auto index = presetIndexForCharacter(resolved, azerty[i]);
                    REQUIRE(index.has_value());
                    CHECK(*index == static_cast<int>(i));
                }
            }

            THEN("the QWERTY digits themselves select nothing")
            {
                // The proof that this is positional: '1' is not bound at all
                // here, because no AZERTY position produces it unshifted.
                CHECK_FALSE(presetIndexForCharacter(resolved, U'1').has_value());
                CHECK_FALSE(presetIndexForCharacter(resolved, U'5').has_value());
            }

            THEN("the eleventh slot survives the dead key on the other position")
            {
                // '[' is AZERTY's dead '^' and resolves to nothing, but the
                // slot is still reachable through ')'. This is the whole point
                // of binding two positions to it. [DEC-JUC-127]
                const auto index = presetIndexForCharacter(resolved, U')');
                REQUIRE(index.has_value());
                CHECK(*index == BIPOLAR_PRESET_COUNT - 1);
            }
        }
    }
}

SCENARIO("An unusable layout query disables the gesture rather than guessing",
         "[RQ-GUI-080]")
{
    GIVEN("no query at all (platform could not start one)")
    {
        WHEN("the preset key mapping is resolved")
        {
            const auto resolved = resolvePresetKeyMapping(nullptr);

            THEN("nothing is bound, so no key can fire the wrong preset")
            {
                CHECK(resolved.empty());
                CHECK_FALSE(presetIndexForCharacter(resolved, U'1').has_value());
            }
        }
    }
}

SCENARIO("A unipolar knob offers ten presets spanning its range", "[RQ-GUI-079]")
{
    GIVEN("the 0..63 range every VCO volume, PW and envelope knob has")
    {
        WHEN("its preset values are derived")
        {
            const auto values = presetValuesForRange(0, 63);

            THEN("there are ten, from the minimum to the maximum in equal steps")
            {
                REQUIRE(values.size() == static_cast<std::size_t>(UNIPOLAR_PRESET_COUNT));
                // Identical to the reference's own serialized table for these
                // knobs (MainForm.Designer.cs VCO1_VOLUME.PredefinedValues).
                const std::vector<int> expected = {0, 7, 14, 21, 28, 35, 42, 49, 56, 63};
                CHECK(values == expected);
            }
        }
    }

    GIVEN("the 0..127 range of the filter frequency knob")
    {
        WHEN("its preset values are derived")
        {
            const auto values = presetValuesForRange(0, 127);

            THEN("the ends are exact and the interior is rounded to nearest")
            {
                REQUIRE(values.size() == static_cast<std::size_t>(UNIPOLAR_PRESET_COUNT));
                CHECK(values.front() == 0);
                CHECK(values.back() == 127);
                // Round-to-nearest, not the reference's Math.Ceiling: 14 rather
                // than 15 at the first step. [DEC-JUC-129]
                CHECK(values[1] == 14);
            }
        }
    }
}

SCENARIO("A bipolar knob offers eleven presets, centred exactly on zero", "[RQ-GUI-079]")
{
    GIVEN("the -63..63 range of a modulation-matrix amount knob")
    {
        WHEN("its preset values are derived")
        {
            const auto values = presetValuesForRange(-63, 63);

            THEN("there are eleven and the middle one is exactly zero")
            {
                REQUIRE(values.size() == static_cast<std::size_t>(BIPOLAR_PRESET_COUNT));
                CHECK(values[BIPOLAR_PRESET_COUNT / 2] == 0);
            }

            THEN("the distribution is a mirror about zero")
            {
                // The reference produced -12 against +13 here, because it
                // computed each half separately and took Math.Ceiling of both.
                // [DEC-JUC-129]
                for (std::size_t i = 0; i < values.size(); ++i)
                {
                    CHECK(values[i] == -values[values.size() - 1 - i]);
                }
                const std::vector<int> expected = {-63, -50, -38, -25, -13, 0, 13, 25, 38, 50, 63};
                CHECK(values == expected);
            }
        }
    }

    GIVEN("the -31..31 range of a VCO detune knob")
    {
        WHEN("its preset values are derived")
        {
            const auto values = presetValuesForRange(-31, 31);

            THEN("it is eleven values, mirrored, ends exact")
            {
                REQUIRE(values.size() == static_cast<std::size_t>(BIPOLAR_PRESET_COUNT));
                CHECK(values.front() == -31);
                CHECK(values.back() == 31);
                CHECK(values[BIPOLAR_PRESET_COUNT / 2] == 0);
                for (std::size_t i = 0; i < values.size(); ++i)
                {
                    CHECK(values[i] == -values[values.size() - 1 - i]);
                }
            }
        }
    }
}

SCENARIO("The VCO frequency knobs carry the reference's harmonic presets", "[RQ-GUI-079]")
{
    GIVEN("the two VCO frequency parameters")
    {
        WHEN("their preset override is looked up")
        {
            THEN("both carry the reference's semitone table")
            {
                // MainForm.Overrides.cs SetKnobControlsPredefinedValues(): a
                // stack of major triads, root to three octaves. [DEC-JUC-130]
                const std::vector<int> expected = {0, 4, 7, 12, 16, 19, 24, 28, 31, 36};
                const auto* vco1 = harmonicPresetValuesFor("VCO1_FREQ");
                const auto* vco2 = harmonicPresetValuesFor("VCO2_FREQ");
                REQUIRE(vco1 != nullptr);
                REQUIRE(vco2 != nullptr);
                CHECK(*vco1 == expected);
                CHECK(*vco2 == expected);
            }

            THEN("every value stays inside the parameter's own 0..63 range")
            {
                const auto* vco1 = harmonicPresetValuesFor("VCO1_FREQ");
                REQUIRE(vco1 != nullptr);
                for (const int value : *vco1)
                {
                    CHECK(value >= 0);
                    CHECK(value <= 63);
                }
            }
        }
    }

    GIVEN("any other knob parameter")
    {
        WHEN("its preset override is looked up")
        {
            THEN("there is none, so it derives presets from its range")
            {
                CHECK(harmonicPresetValuesFor("VCO1_VOLUME") == nullptr);
                CHECK(harmonicPresetValuesFor("VCF_FREQ") == nullptr);
                CHECK(harmonicPresetValuesFor("MOD_AMNT_SRC_1") == nullptr);
            }
        }
    }
}
