#pragma once

#include <cstddef>
#include <cstdint>

// Transports encoded DWIN/DGUS packets between the application and display.
// Provides a platform-independent interface for sending and receiving packets.
// Does not interpret DWIN protocol data or know about application/UI logic.
// Uses HAL for platform-specific communication.

namespace app
{

// Sends encoded DWIN protocol packets to the display.
// The transport does not know about Ui or DWIN screen fields.
class DwinTransport
{
public:
    static constexpr std::size_t MaxPacketSize = 64;

    DwinTransport() noexcept = default;

    void init() noexcept;

    void send(
        const uint8_t* data,
        std::size_t size) noexcept;

    bool receive(
        uint8_t* data,
        std::size_t capacity,
        std::size_t& size) noexcept;

private:
    uint8_t packet_[MaxPacketSize]{};
    std::size_t packet_size_ = 0U;
    std::size_t packet_expected_ = 0U;

};

} // namespace app