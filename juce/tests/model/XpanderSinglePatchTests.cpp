#include <catch2/catch_test_macros.hpp>

#include "midiapp/model/ToneIO.hpp"
#include "xpl/midi/SysexStreamIterator.hpp"
#include "xplorer/model/XpanderSinglePatch.hpp"

#include <fstream>
#include <vector>

using namespace xplorer::model;
using Bytes = std::vector<std::uint8_t>;

namespace
{
    Bytes loadFixtureDump()
    {
        std::ifstream stream(std::string(XPL_FIXTURES_DIR) + "/single_patch_oberheim.syx", std::ios::binary);
        REQUIRE(stream.good());
        const Bytes fileContent((std::istreambuf_iterator<char>(stream)), {});
        auto messages = xpl::midi::SysexStreamIterator::allMessages(fileContent);
        REQUIRE(!messages.empty());
        return messages.front();
    }
}

SCENARIO("A real hardware dump round-trips bit-exactly", "[RQ-MOD-022][RQ-TST-002]")
{
    GIVEN("the OBERHEIM single-patch dump (program 0x63)")
    {
        const auto dump = loadFixtureDump();
        REQUIRE(dump.size() == XpanderSinglePatch::SINGLE_PATCH_LENGTH);

        WHEN("decoded then re-encoded with the same program number")
        {
            XpanderSinglePatch patch;
            patch.fromByteArray(dump);
            const auto reEncoded = patch.toByteArray(dump[XpanderSinglePatch::SINGLE_PATCH_INTRO_LENGTH - 1]);

            THEN("the 399 bytes are identical")
            {
                CHECK(reEncoded == dump);
            }

            THEN("the decoded name is OBERHEIM")
            {
                CHECK(patch.patchName() == "OBERHEIM");
            }
        }
    }
}

SCENARIO("The patch name is read from the top of a raw dump", "[RQ-MOD-023]")
{
    GIVEN("the fixture dump")
    {
        const auto dump = loadFixtureDump();

        THEN("getNameFromByteArray decodes OBERHEIM")
        {
            CHECK(XpanderSinglePatch::getNameFromByteArray(dump) == "OBERHEIM");
        }

        THEN("a wrong-sized buffer yields an empty name")
        {
            CHECK(XpanderSinglePatch::getNameFromByteArray(Bytes{0x01, 0x02}).empty());
        }
    }
}

SCENARIO("Decoding rejects invalid buffers like the reference", "[RQ-MOD-022]")
{
    GIVEN("a too-short buffer")
    {
        const Bytes tooShort(10, 0x00);
        XpanderSinglePatch patch;

        THEN("fromByteArray throws a tone exception")
        {
            CHECK_THROWS_AS(patch.fromByteArray(tooShort), midiapp::model::ToneException);
        }
    }

    GIVEN("a large buffer without any patch intro")
    {
        const Bytes noIntro(XpanderSinglePatch::SINGLE_PATCH_LENGTH, 0x55);
        XpanderSinglePatch patch;

        THEN("fromByteArray throws")
        {
            CHECK_THROWS_AS(patch.fromByteArray(noIntro), midiapp::model::ToneException);
        }
    }

    GIVEN("a dump with leading garbage before the intro")
    {
        auto dump = loadFixtureDump();
        Bytes shifted{0x00, 0x11, 0x22};
        shifted.insert(shifted.end(), dump.begin(), dump.end());
        XpanderSinglePatch patch;

        THEN("the intro is located anywhere in the buffer")
        {
            CHECK_NOTHROW(patch.fromByteArray(shifted));
            CHECK(patch.patchName() == "OBERHEIM");
        }
    }
}

SCENARIO("Modulation entries encode amount, sign and quantize in one byte", "[RQ-MOD-022]")
{
    GIVEN("a patch with a signed quantized modulation entry")
    {
        XpanderSinglePatch patch;
        patch.modulationEntries[0].source = 2;
        patch.modulationEntries[0].amount = -21;
        patch.modulationEntries[0].quantize = true;
        patch.modulationEntries[0].dest = 6;

        WHEN("encoded and decoded again")
        {
            const auto data = patch.toByteArray(0);
            XpanderSinglePatch decoded;
            decoded.fromByteArray(data);

            THEN("the entry round-trips")
            {
                CHECK(decoded.modulationEntries[0].source == 2);
                CHECK(decoded.modulationEntries[0].amount == -21);
                CHECK(decoded.modulationEntries[0].quantize);
                CHECK(decoded.modulationEntries[0].dest == 6);
            }
        }
    }
}
