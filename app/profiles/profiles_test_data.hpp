// profiles_test_data.hpp
#pragma once

#include <cstddef>
#include "profiles.hpp"

namespace app
{

inline constexpr Profile test_profiles[] =
{
    // Profile 0
    {
        {{
            {50,  10, 0x01},
            {50,  10, 0x02},
            {100, 10, 0x00},
            {100, 10, 0x00},
            {50,  10, 0x00},
        }}
    },

    // Profile 1
    {
        {{
            {100, 10, 0x01},
            {150, 20, 0x01},
            {200, 30, 0x02},
            {150, 20, 0x00},
            {100, 10, 0x00},
        }}
    },

    // Profile 2
    {
        {{
            {25,  5,  0x01},
            {50,  5,  0x01},
            {75,  5,  0x02},
            {100, 5,  0x02},
        }}
    }
};

inline constexpr std::size_t test_profile_count =
    sizeof(test_profiles) / sizeof(test_profiles[0]);

}