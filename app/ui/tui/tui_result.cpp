#include <iostream>

#include "tui.hpp"

namespace app
{

void Tui::draw_result() noexcept
{
    clear_screen();

    const auto& page = ui_->result_page();

    std::cout
        << "========================\n"
        << "        RESULT\n"
        << "========================\n\n"

        << "State:        ";

    // Render state here

    std::cout
        << "\n"
        << "Duration:     "
        << page.duration
        << " s\n"

        << "Temperature:  "
        << page.temperature
        << " C\n"

        << "Alarm ID:     "
        << static_cast<unsigned>(page.alarm_id)
        << "\n\n"

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