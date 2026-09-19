#include "dwin_transport.hpp"

#include "ring_buffer.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace
{

    constexpr std::size_t RxCapacity = 128U;

    core::RingBuffer<uint8_t, RxCapacity> rx_buffer;

} // namespace

namespace app
{

    void DwinTransport::init() noexcept
    {
        rx_buffer.clear();
    }

    void DwinTransport::send(
        const uint8_t* data,
        std::size_t size) noexcept
    {
        if (data == nullptr)
        {
            return;
        }

        for (std::size_t i = 0U; i < size; ++i)
        {
            std::printf("%02X ", static_cast<unsigned int>(data[i]));
        }

        std::printf("\n");
    }

    bool DwinTransport::receive(
        uint8_t* data,
        std::size_t capacity,
        std::size_t& size) noexcept
    {
        // Temporary implementation. Packet framing will be added next.
        (void)data;
        (void)capacity;

        size = 0U;
        return false;
    }

} // namespace app

namespace app::test
{

    void feed_dwin_bytes(
        const uint8_t* data,
        std::size_t size) noexcept
    {
        if (data == nullptr)
        {
            return;
        }

        for (std::size_t i = 0U; i < size; ++i)
        {
            (void)rx_buffer.push(data[i]);
        }
    }

} // namespace app::test