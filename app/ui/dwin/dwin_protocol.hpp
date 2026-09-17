#pragma once

#include <cstddef>
#include <cstdint>

namespace app
{

    // Encodes commands using the DWIN/DGUS serial protocol.
    // Does not know anything about UART or the physical display.
    class DwinProtocol
    {
    public:
        static constexpr std::size_t MaxPacketSize = 64;

        struct Packet
        {
            uint8_t data[MaxPacketSize]{};
            std::size_t size = 0;
        };

        DwinProtocol() noexcept = default;

        // Creates a DWIN VP write command containing one 16-bit value.
        [[nodiscard]] Packet write_word(uint16_t address, uint16_t value) const noexcept;
    };

} // namespace app