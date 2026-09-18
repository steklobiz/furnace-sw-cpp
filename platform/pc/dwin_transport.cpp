#include "dwin_transport.hpp"

#include <cstdio>

namespace app
{

void DwinTransport::init() noexcept
{
}

void DwinTransport::send(
    const uint8_t* data,
    std::size_t size) noexcept
{
    if (data == nullptr)
    {
        return;
    }

    for (std::size_t i = 0; i < size; ++i)
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
    (void)data;
    (void)capacity;

    size = 0U;
    return false;
}

} // namespace app