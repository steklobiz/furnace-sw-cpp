#include <cstdio>
#include <thread>
#include <chrono>
#include <iostream>
#include "hal.hpp"
#include "thermal_model.hpp"


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


} // namespace hal