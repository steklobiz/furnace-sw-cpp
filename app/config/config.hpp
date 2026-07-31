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