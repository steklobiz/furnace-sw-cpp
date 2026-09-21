#include "dwin_transport.hpp"

#include "hal.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace app
{

void DwinTransport::init() noexcept
{
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

    hal::dwin_send(data, size);
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

    uint8_t rx_data[MaxPacketSize]{};
    std::size_t rx_size = 0U;

    if (!hal::dwin_receive(
            rx_data,
            sizeof(rx_data),
            rx_size))
    {
        return false;
    }

    for (std::size_t i = 0U; i < rx_size; ++i)
    {
        const uint8_t byte = rx_data[i];

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
            for (std::size_t j = 0U; j < packet_size_; ++j)
            {
                data[j] = packet_[j];
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