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

} // namespace app