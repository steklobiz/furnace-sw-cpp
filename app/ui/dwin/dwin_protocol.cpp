#include "dwin_protocol.hpp"

namespace app
{

    namespace
    {

        constexpr uint8_t Header1 = 0x5AU;
        constexpr uint8_t Header2 = 0xA5U;
        constexpr uint8_t WriteVp = 0x82U;

    } // namespace

    DwinProtocol::Packet DwinProtocol::write_word(
        uint16_t address,
        uint16_t value) const noexcept
    {
        Packet packet{};

        // 5A A5
        packet.data[0] = Header1;
        packet.data[1] = Header2;

        // Length:
        //   command      1 byte
        //   VP address   2 bytes
        //   value        2 bytes
        packet.data[2] = 5U;

        // Command.
        packet.data[3] = WriteVp;

        // VP address, big endian.
        packet.data[4] = static_cast<uint8_t>(address >> 8U);
        packet.data[5] = static_cast<uint8_t>(address & 0xFFU);

        // Value, big endian.
        packet.data[6] = static_cast<uint8_t>(value >> 8U);
        packet.data[7] = static_cast<uint8_t>(value & 0xFFU);

        packet.size = 8U;

        return packet;
    }

    bool DwinProtocol::decode_touch(
        const uint8_t* data,
        std::size_t size,
        TouchEvent& event) const noexcept
    {
        constexpr std::size_t PacketSize = 9U;
        constexpr uint8_t Header1 = 0x5AU;
        constexpr uint8_t Header2 = 0xA5U;
        constexpr uint8_t ReadVp = 0x83U;
        constexpr uint8_t DataLength = 1U;

        if (data == nullptr || size != PacketSize)
        {
            return false;
        }

        if (data[0] != Header1 ||
            data[1] != Header2 ||
            data[2] != 0x06U ||
            data[3] != ReadVp ||
            data[6] != DataLength)
        {
            return false;
        }

        event.address =
            static_cast<uint16_t>(
                (static_cast<uint16_t>(data[4]) << 8U) |
                static_cast<uint16_t>(data[5]));

        event.value =
            static_cast<uint16_t>(
                (static_cast<uint16_t>(data[7]) << 8U) |
                static_cast<uint16_t>(data[8]));

        return true;
    }

} // namespace app