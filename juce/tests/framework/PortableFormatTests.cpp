#include <catch2/catch_test_macros.hpp>

#include "midiapp/service/PortableFormat.hpp"

using namespace midiapp::service;

SCENARIO("formatStr() substitutes bare {} placeholders", "[RQ-BLD-025]")
{
    GIVEN("a format string with no placeholder")
    {
        THEN("it is returned unchanged")
        {
            CHECK(formatStr("no placeholder") == "no placeholder");
        }
    }

    GIVEN("a format string with one placeholder")
    {
        THEN("the argument is substituted in place")
        {
            CHECK(formatStr("MOD_AMNT_SRC_{}", 3) == "MOD_AMNT_SRC_3");
        }
    }

    GIVEN("a format string with several placeholders and mixed argument types")
    {
        THEN("each argument is substituted in order")
        {
            CHECK(formatStr("TRACK_{}_POINT_{}", 1, 2) == "TRACK_1_POINT_2");
            CHECK(formatStr("{} to {}: {}", std::string("a.syx"), std::string("b.syx"), "boom")
                  == "a.syx to b.syx: boom");
        }
    }
}

SCENARIO("formatFallback() is exercised directly, independent of which toolchain format() selects",
         "[RQ-BLD-025]")
{
    GIVEN("the fallback formatter, which is always compiled")
    {
        THEN("it substitutes bare {} placeholders the same way format() does")
        {
            CHECK(formatFallback("no placeholder") == "no placeholder");
            CHECK(formatFallback("MOD_AMNT_SRC_{}", 3) == "MOD_AMNT_SRC_3");
            CHECK(formatFallback("TRACK_{}_POINT_{}", 1, 2) == "TRACK_1_POINT_2");
        }

        THEN("trailing literal text after the last placeholder is preserved")
        {
            CHECK(formatFallback("value={} unit", 42) == "value=42 unit");
        }

        THEN("extra arguments beyond the placeholder count are simply not consumed")
        {
            CHECK(formatFallback("only {}", 1, 2) == "only 1");
        }
    }
}
