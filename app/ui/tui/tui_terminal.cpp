#include <cstdint>
#include <iostream>

#include "tui.hpp"

namespace app
{

void Tui::clear_screen() noexcept
{
    std::cout << "\033[2J\033[H";
}

void Tui::move_cursor(
    uint8_t row,
    uint8_t column) noexcept
{
    std::cout
        << "\033["
        << unsigned(row)
        << ';'
        << unsigned(column)
        << 'H';
}

    
} // namespace app