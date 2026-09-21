#include "dwin_transport.hpp"
#include "hal.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace
{

bool check(
    bool condition,
    const char* message) noexcept
{
    if (!condition)
    {
        std::printf("FAIL: %s\n", message);
        return false;
    }

    return true;
}

bool packet_equals(
    const uint8_t* actual,
    std::size_t actual_size,
    const uint8_t* expected,
    std::size_t expected_size) noexcept
{
    if (actual_size != expected_size)
    {
        return false;
    }

    for (std::size_t i = 0U; i < expected_size; ++i)
    {
        if (actual[i] != expected[i])
        {
            return false;
        }
    }

    return true;
}

bool test_complete_packet()
{
    app::DwinTransport transport;
    transport.init();

    constexpr uint8_t packet[] =
    {
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x00, 0x01, 0x00, 0x01
    };

    hal::test_feed_dwin_bytes(packet, sizeof(packet));

    uint8_t received[16]{};
    std::size_t size = 0U;

    if (!check(
            transport.receive(received, sizeof(received), size),
            "complete packet should be received"))
    {
        return false;
    }

    if (!check(
            packet_equals(
                received,
                size,
                packet,
                sizeof(packet)),
            "received packet should match"))
    {
        return false;
    }

    if (!check(
            !transport.receive(received, sizeof(received), size),
            "RX should be empty"))
    {
        return false;
    }

    return true;
}

bool test_partial_packet()
{
    app::DwinTransport transport;
    transport.init();

    constexpr uint8_t first[] =
    {
        0x5A, 0xA5, 0x06, 0x83, 0x20
    };

    constexpr uint8_t second[] =
    {
        0x00, 0x01, 0x00, 0x01
    };

    constexpr uint8_t expected[] =
    {
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x00, 0x01, 0x00, 0x01
    };

    hal::test_feed_dwin_bytes(first, sizeof(first));

    uint8_t received[16]{};
    std::size_t size = 0U;

    if (!check(
            !transport.receive(received, sizeof(received), size),
            "partial packet should not be returned"))
    {
        return false;
    }

    if (!check(
            size == 0U,
            "size should be zero for incomplete packet"))
    {
        return false;
    }

    hal::test_feed_dwin_bytes(second, sizeof(second));

    if (!check(
            transport.receive(received, sizeof(received), size),
            "completed packet should be returned"))
    {
        return false;
    }

    return check(
        packet_equals(
            received,
            size,
            expected,
            sizeof(expected)),
        "reassembled packet should match");
}

bool test_two_packets()
{
    app::DwinTransport transport;
    transport.init();

    constexpr uint8_t packets[] =
    {
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x00, 0x01, 0x00, 0x01,

        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x01, 0x01, 0x00, 0x01
    };

    constexpr uint8_t expected_first[] =
    {
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x00, 0x01, 0x00, 0x01
    };

    constexpr uint8_t expected_second[] =
    {
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x01, 0x01, 0x00, 0x01
    };

    hal::test_feed_dwin_bytes(packets, sizeof(packets));

    uint8_t received[16]{};
    std::size_t size = 0U;

    if (!check(
            transport.receive(received, sizeof(received), size),
            "first packet should be received"))
    {
        return false;
    }

    if (!check(
            packet_equals(
                received,
                size,
                expected_first,
                sizeof(expected_first)),
            "first packet should match"))
    {
        return false;
    }

    if (!check(
            transport.receive(received, sizeof(received), size),
            "second packet should be received"))
    {
        return false;
    }

    if (!check(
            packet_equals(
                received,
                size,
                expected_second,
                sizeof(expected_second)),
            "second packet should match"))
    {
        return false;
    }

    return check(
        !transport.receive(received, sizeof(received), size),
        "RX should be empty after two packets");
}

bool test_empty_rx()
{
    app::DwinTransport transport;
    transport.init();

    uint8_t received[16]{};
    std::size_t size = 123U;

    if (!check(
            !transport.receive(received, sizeof(received), size),
            "empty RX should return false"))
    {
        return false;
    }

    return check(
        size == 0U,
        "size should be zero for empty RX");
}

bool test_garbage_before_packet()
{
    app::DwinTransport transport;
    transport.init();

    constexpr uint8_t data[] =
    {
        0x11, 0x22, 0x33,
        0x5A, 0x00,
        0x44,
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x00, 0x01, 0x00, 0x01
    };

    constexpr uint8_t expected[] =
    {
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x00, 0x01, 0x00, 0x01
    };

    hal::test_feed_dwin_bytes(data, sizeof(data));

    uint8_t received[16]{};
    std::size_t size = 0U;

    if (!check(
            transport.receive(received, sizeof(received), size),
            "valid packet after garbage should be received"))
    {
        return false;
    }

    return check(
        packet_equals(
            received,
            size,
            expected,
            sizeof(expected)),
        "packet after garbage should match");
}

bool test_output_buffer_too_small()
{
    app::DwinTransport transport;
    transport.init();

    constexpr uint8_t packet[] =
    {
        0x5A, 0xA5, 0x06, 0x83,
        0x20, 0x00, 0x01, 0x00, 0x01
    };

    hal::test_feed_dwin_bytes(packet, sizeof(packet));

    uint8_t received[4]{};
    std::size_t size = 0U;

    if (!check(
            !transport.receive(received, sizeof(received), size),
            "small output buffer should fail"))
    {
        return false;
    }

    return check(
        size == 0U,
        "size should be zero when output buffer is too small");
}

} // namespace

int main()
{
    bool passed = true;

    passed = test_complete_packet() && passed;
    passed = test_partial_packet() && passed;
    passed = test_two_packets() && passed;
    passed = test_empty_rx() && passed;
    passed = test_garbage_before_packet() && passed;
    passed = test_output_buffer_too_small() && passed;

    if (passed)
    {
        std::printf("DwinTransport tests: PASS\n");
        return 0;
    }

    std::printf("DwinTransport tests: FAIL\n");
    return 1;
}