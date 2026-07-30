#pragma once

#include <iostream>

#include "log.hpp"

namespace platform::log
{

struct Backend
{
    static void write(char c) noexcept
    {
        std::cout.put(c);
    }
};

using Log = core::log::Logger<Backend>;

} // namespace platform::log
