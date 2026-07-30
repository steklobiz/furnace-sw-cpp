#pragma once

#include "core/log.hpp"

namespace platform::log
{

struct Backend
{
    static void write(char c) noexcept
    {
        // TODO: Forward character to USART driver.
    }
};

using Log = core::log::Logger<Backend>;

} // namespace platform::log
