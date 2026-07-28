#include <cstdint>
#include <iostream>
#include "tui.hpp"

namespace app
{

void Tui::draw_main() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "       MAIN PAGE\n"
        << "========================\n\n"
        
        << "State:\n"
        << "Temperature:\n";
}
    
    
void Tui::update_main() noexcept
{
    const auto& page =
        ui_.main_page();


    move_cursor(
        MainLayout::StateRow,
        MainLayout::ValueColumn);

    std::cout
        << "          ";

    move_cursor(
        MainLayout::StateRow,
        MainLayout::ValueColumn);

    std::cout
        << Furnace::state_name(page.state);


    move_cursor(
        MainLayout::TemperatureRow,
        MainLayout::ValueColumn);

    std::cout
        << "          ";

    move_cursor(
        MainLayout::TemperatureRow,
        MainLayout::ValueColumn);

    std::cout
        << page.temperature
        << " C";

    move_cursor(
        15,
        1);
}    
    
void Tui::process_main_input() noexcept
{
    char key;

    std::cin >> key;


    if (key < '1' || key > '9')
    {
        return;
    }


    const auto index =
        static_cast<uint8_t>(key - '1');

    const auto& page =
        ui_.main_page();


    if (index >= page.count)
    {
        return;
    }


    execute_action(
        page.buttons[index].action);
}

const char* Tui::main_action_name(
    Ui::Button::Action action) noexcept
{
    switch(action)
    {
    case Ui::Button::Action::StartProfile:
        return "Start profile";

    case Ui::Button::Action::Monitor:
        return "Monitor";

    case Ui::Button::Action::Stop:
        return "Stop";

    case Ui::Button::Action::Reset:
        return "Reset";

    case Ui::Button::Action::Settings:
        return "Settings";

    case Ui::Button::Action::EditProfile:
        return "Edit profile";

    case Ui::Button::Action::Back:
        return "Back";

    default:
        return "";
    }
}

    
} // namespace app