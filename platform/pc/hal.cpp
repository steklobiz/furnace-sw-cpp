#include <cstdio>
#include <thread>
#include <chrono>
#include <iostream>
#include "hal.hpp"
#include "thermal_model.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
    void enable_vt_processing() noexcept
    {
#ifdef _WIN32
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);

        if (output == INVALID_HANDLE_VALUE)
        {
            return;
        }

        DWORD mode = 0;

        if (!GetConsoleMode(output, &mode))
        {
            return;
        }

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        static_cast<void>(
            SetConsoleMode(output, mode)
        );
#endif
    }
constexpr std::size_t DwinRxBufferSize = 256U;

uint8_t dwin_rx_buffer[DwinRxBufferSize]{};
std::size_t dwin_rx_read = 0U;
std::size_t dwin_rx_write = 0U;
constexpr uint8_t DwinHeader1 = 0x5AU;
constexpr uint8_t DwinHeader2 = 0xA5U;
constexpr uint8_t DwinCommandWriteVp = 0x82U;
constexpr uint8_t DwinWriteWordPayload = 5U;
constexpr std::size_t DwinWriteWordSize = 8U;

}

// need to be namespace platform::hal {
namespace hal {

    simulator::ThermalParams params{
        10000,
        1000,
        2
    };

    simulator::ThermalModel model(params,
         25,
         25
    );

    uint8_t current_duty = 0;

void init()
{
    enable_vt_processing();
}


void set_outs(uint8_t byte)
{
    // TODO: set ouputs
    static_cast<void>(byte);
}

void reset_outs()
{
    // Do I need it?
}

void delay_ms(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint32_t tick_ms()
{
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

uint32_t tick_s()
{
    return(tick_ms() / 1000);
}

uint16_t get_temperature()
{
    return model.temperature();
};


void set_heater_power(uint8_t duty)
{
    current_duty = duty;
};

void update()
{
    model.update(current_duty);
};

void dwin_send(
    const uint8_t* data,
    std::size_t size) noexcept
{
    if (data == nullptr)
    {
        return;
    }

    if (size == DwinWriteWordSize &&
        data[0] == DwinHeader1 &&
        data[1] == DwinHeader2 &&
        data[2] == DwinWriteWordPayload &&
        data[3] == DwinCommandWriteVp)
    {
        const unsigned int address =
            (static_cast<unsigned int>(data[4]) << 8U) |
             static_cast<unsigned int>(data[5]);

        const unsigned int value =
            (static_cast<unsigned int>(data[6]) << 8U) |
             static_cast<unsigned int>(data[7]);

        std::printf(
            "DWIN VP 0x%04X = %u\n",
            address,
            value);

        return;
    }

    std::printf("DWIN RAW:");

    for (std::size_t i = 0U; i < size; ++i)
    {
        std::printf(
            " %02X",
            static_cast<unsigned int>(data[i]));
    }

    std::printf("\n");
}

bool dwin_receive(
    uint8_t* data,
    std::size_t capacity,
    std::size_t& size) noexcept
{
    size = 0U;

    if (data == nullptr || capacity == 0U)
    {
        return false;
    }

    while (size < capacity && dwin_rx_read != dwin_rx_write)
    {
        data[size] = dwin_rx_buffer[dwin_rx_read];

        dwin_rx_read =
            (dwin_rx_read + 1U) % DwinRxBufferSize;

        ++size;
    }

    return size > 0U;
}

void test_feed_dwin_bytes(
    const uint8_t* data,
    std::size_t size) noexcept {
    if (data == nullptr)
    {
        return;
    }

    for (std::size_t i = 0U; i < size; ++i)
    {
        const std::size_t next =
            (dwin_rx_write + 1U) % DwinRxBufferSize;

        if (next == dwin_rx_read)
        {
            break;
        }

        dwin_rx_buffer[dwin_rx_write] = data[i];
        dwin_rx_write = next;
    }
}

} // namespace hal