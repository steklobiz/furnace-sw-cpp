// app/config.hpp

// Factory software constants structure

#pragma once

#include <cstdint>


namespace app::config
{
    namespace profiles
    {
        inline constexpr uint8_t count = 10;
        inline constexpr uint8_t max_steps = 10;
    }
    namespace pid
    {
        // do not replace int32_t with uint8_t. All pid arithmetic based on int32_t
        inline constexpr int32_t scale = 1000;
        
        // Uutput limits (heater duty range 0..100).
        inline constexpr int32_t output_min_power = 100;
        inline constexpr int32_t output_max_power = 100;
    }
    namespace history
    {
        inline constexpr uint8_t event_capacity = 10;
        inline constexpr uint8_t sample_capacity = 60;
        inline constexpr uint32_t sample_period_s = 1; 
    }
    
    
inline constexpr uint8_t tc_count      = 2;

inline constexpr uint8_t output_count      = 4;

inline constexpr uint32_t ui_buffer_size  = 1024;
inline constexpr uint32_t event_queue_size = 32;
}

/*
namespace app::config
{
    namespace profiles
    {
        inline constexpr uint8_t count = 16;
        inline constexpr uint8_t max_steps = 32;
    }

    namespace logging
    {
        inline constexpr std::size_t buffer_size = 1024;
    }

    namespace ui
    {
        inline constexpr uint8_t output_count = 4;
    }
}
*/