#pragma once

#include <iostream>

namespace platform
{
namespace log
{

struct Backend
{
    static void write(char c) noexcept
    {
        std::cout.put(c);
    }
};

} // namespace log
} // namespace platform