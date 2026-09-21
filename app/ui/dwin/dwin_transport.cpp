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
    packet_size_ = 0U;
    packet_expected_ = 0U;
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
    size = 0U;

    if (data == nullptr || capacity < 3U)
    {
        return false;
    }

    uint8_t byte = 0U;

    while (rx_buffer.pop(byte))
    {
        if (packet_size_ == 0U)
        {
            if (byte == 0x5AU)
            {
                packet_[packet_size_++] = byte;
            }

            continue;
        }

        if (packet_size_ == 1U)
        {
            if (byte == 0xA5U)
            {
                packet_[packet_size_++] = byte;
            }
            else if (byte == 0x5AU)
            {
                // Possible new packet header.
                packet_[0] = byte;
            }
            else
            {
                packet_size_ = 0U;
            }

            continue;
        }

        packet_[packet_size_++] = byte;

        if (packet_size_ == 3U)
        {
            const std::size_t payload_length =
                static_cast<std::size_t>(packet_[2]);

            packet_expected_ = 3U + payload_length;

            if (packet_expected_ > MaxPacketSize ||
                packet_expected_ > capacity)
            {
                packet_size_ = 0U;
                packet_expected_ = 0U;
            }

            continue;
        }

        if (packet_size_ == packet_expected_)
        {
            for (std::size_t i = 0U; i < packet_size_; ++i)
            {
                data[i] = packet_[i];
            }

            size = packet_size_;

            packet_size_ = 0U;
            packet_expected_ = 0U;

            return true;
        }
    }

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