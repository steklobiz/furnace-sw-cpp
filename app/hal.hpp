// app/hal.hpp
#pragma once

#include <cstdint>

namespace hal {

void init();    
    
void set_outputs(uint8_t);

// resetting outputs to default state
void reset_outputs();

void delay_ms(uint32_t ms);

uint32_t tick_ms();

uint32_t tick_s();

// Get current temperature
uint16_t get_temperature();

void set_heater_power(uint8_t);

void update();

} // namespace hal
