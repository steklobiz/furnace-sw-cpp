// app/hal.hpp
#pragma once

#include <cstdint>

namespace hal {

void init();    

void set_outs(uint8_t);

// resetting outputs to default state
void reset_outs();

void delay_ms(uint32_t ms);

uint32_t tick_ms();

uint32_t tick_s();

// Get current temperature
uint16_t get_temperature();

void set_heater_power(uint8_t);

void update();

// Send raw bytes to the DWIN display.
void dwin_send(
    const uint8_t* data,
    std::size_t size) noexcept;

// Retrieve raw bytes received from the display.
bool dwin_receive(
    uint8_t* data,
    std::size_t capacity,
    std::size_t& size) noexcept;

void test_feed_dwin_bytes(
    const uint8_t* data,
    std::size_t size) noexcept;

} // namespace hal
