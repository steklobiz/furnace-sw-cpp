#include <iostream>

#include "tui.hpp"

namespace app
{

void Tui::draw_result() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "        RESULT\n"
        << "========================\n\n"

        << "State:\n"
        << "Duration:\n"
        << "Temperature:\n"
        << "Alarm ID:\n\n"

        << "1. Reset\n";
}


void Tui::process_result_input() noexcept
{
    char key;

    if (!read_key(key))
    {
        return;
    }

    if (key != '1')
    {
        return;
    }

    ui_->dispatch(
        Ui::Event::Id::Reset);
}

} // namespace app