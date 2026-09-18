#pragma once

#include <cstddef>
#include <cstdint>

namespace app
{

// Sends encoded DWIN protocol packets to the display.
// The transport does not know about Ui or DWIN screen fields.
class DwinTransport
{
public:
    DwinTransport() noexcept = default;

    void init() noexcept;

    void send(const uint8_t* data, std::size_t size) noexcept;

    bool receive(
        uint8_t* data,
        std::size_t capacity,
        std::size_t& size) noexcept;
};

} // namespace app