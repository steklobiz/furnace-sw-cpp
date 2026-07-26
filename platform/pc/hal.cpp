#include <cstdio>
#include <thread>
#include <chrono>
#include <iostream>
#include "hal.hpp"
#include "thermal_model.hpp"

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
    
    
void set_outputs(uint8_t byte)
{
    std::cout << "Outs: "<< (byte >> 2 & 1) << "|" << (byte >> 1 & 1) << "|" << (byte >> 0 & 1) << std::endl;
    
}

void reset_outputs()
{
    std::cout << "Outs resetting..." << std::endl;
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