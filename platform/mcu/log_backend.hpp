#pragma once

namespace platform
{
namespace log
{

struct Backend
{
    static void write(char c) noexcept
    {
        usart.write(c);
    }
};

} // namespace log
} // namespace platform