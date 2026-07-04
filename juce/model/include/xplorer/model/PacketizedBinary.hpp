#pragma once

// Port of PacketizedBinaryReader/Writer: the Oberheim dump encoding stores
// each data byte as a 16-bit little-endian packet — low byte = bits 0-6,
// high byte = bit 7. [RQ-MOD-040]

#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace xplorer::model
{
    class PacketizedBinaryReader
    {
    public:
        explicit PacketizedBinaryReader(std::span<const std::uint8_t> packed)
            : _packed(packed)
        {
        }

        /// @throws std::out_of_range past the end of the packed data.
        [[nodiscard]] std::uint8_t readByte()
        {
            if (_position + 2 > _packed.size())
            {
                throw std::out_of_range("packetized read past end of data");
            }
            const std::uint16_t packet =
                static_cast<std::uint16_t>(_packed[_position] | (_packed[_position + 1] << 8));
            _position += 2;
            return unpack(packet);
        }

        [[nodiscard]] std::int8_t readSByte() { return static_cast<std::int8_t>(readByte()); }

        [[nodiscard]] std::vector<std::uint8_t> readBytes(std::size_t count)
        {
            std::vector<std::uint8_t> unpacked(count);
            for (auto& byte : unpacked)
            {
                byte = readByte();
            }
            return unpacked;
        }

        [[nodiscard]] std::size_t position() const { return _position; }

    private:
        static std::uint8_t unpack(std::uint16_t packet)
        {
            return static_cast<std::uint8_t>(((packet & 0x100) >> 1) | (packet & 0xFF));
        }

        std::span<const std::uint8_t> _packed;
        std::size_t _position = 0;
    };

    class PacketizedBinaryWriter
    {
    public:
        explicit PacketizedBinaryWriter(std::vector<std::uint8_t>& sink)
            : _sink(sink)
        {
        }

        void writeByte(std::uint8_t data)
        {
            const std::uint16_t packet = pack(data);
            _sink.push_back(static_cast<std::uint8_t>(packet & 0xFF));        // low byte first
            _sink.push_back(static_cast<std::uint8_t>((packet >> 8) & 0xFF)); // (little endian)
        }

        void writeSByte(std::int8_t data) { writeByte(static_cast<std::uint8_t>(data)); }

        void writeBytes(std::span<const std::uint8_t> bytes)
        {
            for (const auto byte : bytes)
            {
                writeByte(byte);
            }
        }

    private:
        static std::uint16_t pack(std::uint8_t data)
        {
            return static_cast<std::uint16_t>(((data & 0x80) << 1) | (data & 0x7F));
        }

        std::vector<std::uint8_t>& _sink;
    };
}
