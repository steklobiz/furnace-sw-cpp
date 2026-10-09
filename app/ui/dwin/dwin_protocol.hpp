#pragma once

#include <cstddef>
#include <cstdint>

// Encodes DWIN/DGUS protocol packets for display output
// and decodes touch-response packets received from the display.
// Does not handle transport or UART communication.
// Protocol-specific addresses and application mappings belong elsewhere.

namespace app
{

// Encodes and decodes DWIN/DGUS serial protocol packets.
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

    struct TouchEvent
    {
        uint16_t address = 0U;
        uint16_t value = 0U;
    };

    DwinProtocol() noexcept = default;

    // Creates a DWIN VP write command containing one 16-bit value.
    [[nodiscard]] Packet write_word(
        uint16_t address,
        uint16_t value) const noexcept;

    // Creates a DWIN VP write command containing a fixed-length string.
    // The remaining bytes are zero-padded to clear any previous text.
    [[nodiscard]] Packet write_string(
        uint16_t address,
        const char* text,
        std::size_t length) const noexcept;

    // Creates a DWIN page-switch command for the specified page ID.
    [[nodiscard]] Packet switch_page(uint16_t page) const noexcept;

    // Decodes a DWIN touch/event packet.
    // Returns true when the packet contains a valid touch event.
    [[nodiscard]] bool decode_touch(
        const uint8_t* data,
        std::size_t size,
        TouchEvent& event) const noexcept;

};

} // namespace app