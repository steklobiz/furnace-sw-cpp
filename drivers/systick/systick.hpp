#pragma once

#include <cstdint>

namespace drivers::systick {

void init(uint32_t cpu_hz);

uint32_t millis();

void delay_ms(uint32_t ms);

void irq_handler();

} // namespace drivers::systick