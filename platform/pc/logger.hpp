// logger_backend.hpp
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
}

// Direct alias outside any namespace
using Log = core::log::Logger<platform::log::Backend>;

// Also expose Tag and Level for convenience
using core::log::Tag;
using core::log::Level;