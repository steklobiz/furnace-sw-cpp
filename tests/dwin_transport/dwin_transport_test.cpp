#include "dwin_protocol.hpp"
#include "dwin_transport.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace app::test
{

    void feed_dwin_bytes(
        const uint8_t* data,
        std::size_t size) noexcept;

} // namespace app::test

int main()
{
    app::DwinTransport transport;
    transport.init();

    uint8_t packet[app::DwinProtocol::MaxPacketSize]{};
    std::size_t size = 0U;

    const uint8_t bytes[] = {
        0x5A, 0xA5, 0x06,
        0x83, 0x20, 0x00,
        0x01, 0x00, 0x01
    };

    app::test::feed_dwin_bytes(bytes, sizeof(bytes));

    assert(!transport.receive(packet, sizeof(packet), size));

    // receive() is not implemented yet, so this test should currently fail.
    // Remove this comment once framing is implemented.

    std::printf("DwinTransport test setup: OK\n");

    return 0;
}