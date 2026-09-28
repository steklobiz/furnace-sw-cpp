#include "systick.hpp"
#include "arm_systick.hpp"

namespace drivers::systick {

static volatile uint32_t ticks = 0;
    
void init(uint32_t cpu_hz)
{
    auto& st = arm::systick::regs();

    st.LOAD = (cpu_hz / 1000U) - 1U; // 1 ms tick
    st.VAL  = 0;

    st.CTRL =
        arm::systick::ctrl::CLKSOURCE |
        arm::systick::ctrl::TICKINT   |
        arm::systick::ctrl::ENABLE;
}

uint32_t millis()
{
    return ticks;
}

void delay_ms(uint32_t ms)
{
    const uint32_t start = millis();

    while ((millis() - start) < ms)
    {
    }
}

void irq_handler()
{
    ++ticks;
}

} // namespace drivers::systick