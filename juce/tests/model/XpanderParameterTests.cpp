#include <catch2/catch_test_macros.hpp>

#include "xplorer/model/PacketizedBinary.hpp"
#include "xplorer/model/XpanderConstants.hpp"
#include "xplorer/model/XpanderParameter.hpp"

#include <cstdint>
#include <vector>

using namespace xplorer::model;
using xpl::midi::MidiMessage;
using Bytes = std::vector<std::uint8_t>;

namespace
{
    /// Parameter-edit frame template from the reference source comment:
    /// F0 10 02 0A 00 <buttonId> 00 00 00 <value> <sign> F7
    MidiMessage makeParameterFrame(std::uint8_t buttonId)
    {
        return MidiMessage::sysEx(
            Bytes{0xF0, 0x10, 0x02, 0x0A, 0x00, buttonId, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7});
    }
}

SCENARIO("Unsigned parameter edits are byte-exact", "[RQ-MOD-010][RQ-TST-003]")
{
    GIVEN("a VCO1 frequency parameter (page 0x20, subpage 2, button 8)")
    {
        XpanderParameter parameter("VCO1_FREQ", 0x20, 2, 0, 63, 1, makeParameterFrame(0x08), 0);

        WHEN("the value becomes 0x63-like max 63")
        {
            parameter.setValue(63);

            THEN("the frame matches the reference layout")
            {
                CHECK(parameter.message().toBytes()
                      == Bytes{0xF0, 0x10, 0x02, 0x0A, 0x00, 0x08, 0x00, 0x00, 0x00, 63, 0x00, 0xF7});
                CHECK(parameter.buttonId() == 0x08);
                CHECK(parameter.message().size() == XpanderParameter::SYSEX_MESSAGE_LENGTH);
            }
        }

        THEN("the page-select frame is F0 10 02 0B 20 02 F7")
        {
            CHECK(parameter.pageSelectMessage().toBytes()
                  == Bytes{0xF0, 0x10, 0x02, 0x0B, 0x20, 0x02, 0xF7});
        }

        WHEN("mirroring a synth edit with setValueUnchanged")
        {
            parameter.setValueUnchanged(10);

            THEN("the value updates but nothing is queued for transmission")
            {
                CHECK(parameter.value() == 10);
                CHECK_FALSE(parameter.changed());
                CHECK(parameter.message()[XpanderParameter::SYSEX_VALUE_INDEX] == 10);
            }
        }
    }
}

SCENARIO("Signed parameter encodes negatives as 0x80-|v| plus sign byte", "[RQ-MOD-011][RQ-TST-003]")
{
    GIVEN("a detune parameter [-31..31]")
    {
        XpanderSignedParameter parameter("VCO2_DETUNE", 0x21, 3, -31, 31, 1, makeParameterFrame(0x0B), 0);

        WHEN("the value is -5")
        {
            parameter.setValue(-5);

            THEN("value byte is 0x7B and the sign byte is 1")
            {
                CHECK(parameter.message()[XpanderParameter::SYSEX_VALUE_INDEX] == 0x80 - 5);
                CHECK(parameter.message()[XpanderParameter::SYSEX_VALUE_INDEX + 1] == 0x01);
            }
        }

        WHEN("the value returns positive")
        {
            parameter.setValue(-5);
            parameter.setValue(7);

            THEN("value byte is plain and the sign byte resets")
            {
                CHECK(parameter.message()[XpanderParameter::SYSEX_VALUE_INDEX] == 7);
                CHECK(parameter.message()[XpanderParameter::SYSEX_VALUE_INDEX + 1] == 0x00);
            }
        }

        WHEN("constructed directly with a negative value")
        {
            XpanderSignedParameter negative("N", 0x21, 3, -31, 31, 1, makeParameterFrame(0x0B), -31);

            THEN("the initial message is already signed-encoded")
            {
                CHECK(negative.message()[XpanderParameter::SYSEX_VALUE_INDEX] == 0x80 - 31);
                CHECK(negative.message()[XpanderParameter::SYSEX_VALUE_INDEX + 1] == 0x01);
            }
        }
    }
}

SCENARIO("Mod-matrix parameter starts unarmed and exposes its source id", "[RQ-MOD-012]")
{
    GIVEN("a mod matrix amount parameter")
    {
        XpanderModMatrixParameter parameter("MODMATRIX_AMOUNT_0", 0x6B, 0, -63, 63, 1,
                                            makeParameterFrame(0x00), 5);

        THEN("construction cleared the changed flag despite the non-zero value")
        {
            CHECK(parameter.value() == 5);
            CHECK_FALSE(parameter.changed());
        }

        WHEN("the source id is set")
        {
            parameter.setIdSource(3);

            THEN("it lands at byte 5 of the frame")
            {
                CHECK(parameter.idSource() == 3);
                CHECK(parameter.message()[XpanderModMatrixParameter::ID_SOURCE_INDEX] == 3);
            }
        }
    }
}

SCENARIO("Full-tone parameter always transmits, clones included", "[RQ-MOD-013]")
{
    GIVEN("a full-tone parameter wrapping a dump")
    {
        XpanderFullToneParameter parameter(MidiMessage::sysEx(Bytes{0xF0, 0x10, 0x02, 0x01, 0x00, 0xF7}));

        THEN("it is armed at construction and after cloning")
        {
            CHECK(parameter.changed());
            parameter.setChanged(false);
            const auto copy = parameter.clone();
            CHECK(copy->changed());
        }
    }
}

SCENARIO("Nibble packetization round-trips and matches the wire format", "[RQ-MOD-040][RQ-TST-003]")
{
    GIVEN("a writer over an empty sink")
    {
        Bytes sink;
        PacketizedBinaryWriter writer(sink);

        WHEN("writing 0x00, 0x7F, 0x80, 0xFF")
        {
            writer.writeByte(0x00);
            writer.writeByte(0x7F);
            writer.writeByte(0x80);
            writer.writeByte(0xFF);

            THEN("each byte becomes low-7-bits then bit-7, little endian")
            {
                CHECK(sink == Bytes{0x00, 0x00, 0x7F, 0x00, 0x00, 0x01, 0x7F, 0x01});
            }

            THEN("the reader restores the original bytes")
            {
                PacketizedBinaryReader reader(sink);
                CHECK(reader.readByte() == 0x00);
                CHECK(reader.readByte() == 0x7F);
                CHECK(reader.readByte() == 0x80);
                CHECK(reader.readByte() == 0xFF);
            }
        }

        WHEN("writing signed bytes")
        {
            writer.writeSByte(-1);

            THEN("they round-trip through the same packing")
            {
                PacketizedBinaryReader reader(sink);
                CHECK(reader.readSByte() == -1);
            }
        }
    }

    GIVEN("a truncated packed stream")
    {
        const Bytes packed{0x01};

        THEN("reading past the end throws")
        {
            PacketizedBinaryReader reader(packed);
            CHECK_THROWS_AS(reader.readByte(), std::out_of_range);
        }
    }
}

SCENARIO("Constant tables match the reference", "[RQ-MOD-001][RQ-MOD-003]")
{
    THEN("structural constants hold")
    {
        CHECK(constants::MODENTRIES_COUNT == 20);
        CHECK(constants::MAX_MODULATION_SOURCE == 6);
        CHECK(constants::TONE_NAME_LENGTH == 8);
        CHECK(PAGE_SUBPAGE_FOR_MODULATION_DESTINATION.size() == 47);
    }

    THEN("destination -> page/subpage -> destination round-trips")
    {
        const auto& vcfEntry =
            PAGE_SUBPAGE_FOR_MODULATION_DESTINATION[static_cast<std::size_t>(EnumModulationDestinations::VCF_FRQ)];
        CHECK(vcfEntry.page == EnumPages::VCF_VCA_X);
        CHECK(vcfEntry.subPage == 2);
        CHECK(modulationDestinationForPageSubPage(static_cast<int>(EnumPages::VCF_VCA_X), 2)
              == EnumModulationDestinations::VCF_FRQ);
        CHECK(modulationDestinationForPageSubPage(static_cast<int>(EnumPages::FM_LAG_X), 6)
              == EnumModulationDestinations::LAG_RATE);
    }
}
