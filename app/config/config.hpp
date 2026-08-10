// app/config.hpp

// Factory software constants structure

#pragma once

#include <cstdint>

// Application level config constants
namespace app::config
{
    namespace profiles
    {
        inline constexpr uint8_t count = 10;
        inline constexpr uint8_t max_steps = 10;
    }
    namespace pid
    {
        inline constexpr uint8_t output_max_power = 100;
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