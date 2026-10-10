#include "dwin_protocol.hpp"

namespace app
{

    namespace
    {
        constexpr uint8_t Header1 = 0x5AU;
        constexpr uint8_t Header2 = 0xA5U;

        constexpr uint8_t WriteVp = 0x82U;
        constexpr uint8_t ReadVp = 0x83U;

        constexpr uint8_t TouchPacketLength = 0x06U;
        constexpr uint8_t TouchDataLength = 0x01U;
        constexpr std::size_t TouchPacketSize = 9U;
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

DwinProtocol::Packet DwinProtocol::write_string(
    const uint16_t address,
    const char* text,
    const std::size_t length) const noexcept
{
    Packet packet{};

    // Reject invalid input and strings that exceed the packet buffer.
    constexpr std::size_t HeaderSize = 3U;
    constexpr std::size_t CommandAndAddressSize = 3U;

    if (text == nullptr ||
        length == 0U ||
        length > MaxPacketSize - HeaderSize - CommandAndAddressSize ||
        length > 255U - CommandAndAddressSize)
    {
        return packet;
    }

    // Packet header and command.
    packet.data[0] = Header1;
    packet.data[1] = Header2;
    packet.data[2] = static_cast<uint8_t>(
        CommandAndAddressSize + length);
    packet.data[3] = WriteVp;

    // VP address, big endian.
    packet.data[4] = static_cast<uint8_t>(address >> 8U);
    packet.data[5] = static_cast<uint8_t>(address & 0xFFU);

    // Copy string data and zero-fill after the first null character.
    bool terminated = false;

    for (std::size_t i = 0U; i < length; ++i)
    {
        if (terminated || text[i] == '\0')
        {
            packet.data[6U + i] = 0U;
            terminated = true;
        }
        else
        {
            packet.data[6U + i] =
                static_cast<uint8_t>(text[i]);
        }
    }

    // Total packet size includes the 3-byte header and command/address.
    packet.size = HeaderSize + CommandAndAddressSize + length;

    return packet;
}

DwinProtocol::Packet DwinProtocol::switch_page(uint16_t page) const noexcept
    {
        Packet packet{};

        // 5A A5 07 82 00 84 5A 01 [page]
        packet.data[0] = Header1;
        packet.data[1] = Header2;
        packet.data[2] = 7U;
        packet.data[3] = WriteVp;
        packet.data[4] = 0x00U;
        packet.data[5] = 0x84U;
        packet.data[6] = 0x5AU;
        packet.data[7] = 0x01U;
        packet.data[8] = static_cast<uint8_t>(page >> 8U);
        packet.data[9] = static_cast<uint8_t>(page & 0xFFU);
        packet.size = 10U;

        return packet;
    }


bool DwinProtocol::decode_touch(
    const uint8_t* data,
    std::size_t size,
    TouchEvent& event) const noexcept
{
    constexpr uint8_t PacketLength = 0x06U;
    constexpr uint8_t DataLength = 0x01U;
    constexpr std::size_t PacketSize = 9U;

    if (data == nullptr || size != PacketSize)
    {
        return false;
    }

    if (data[0] != Header1 ||
        data[1] != Header2 ||
        data[2] != PacketLength ||
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