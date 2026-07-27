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
        << "========================\n";
}
    
    
void Tui::update_main() noexcept
{
    const auto& page = ui_.main_page();

    move_cursor(5, 1);

    for (uint8_t i = 0; i < page.count; ++i)
    {
        std::cout
            << (i + 1)
            << ". "
            << main_action_name(page.buttons[i].action)
            << "            \n";
    }
}    
    
void Tui::process_main_input() noexcept
{
    char key;

    std::cin >> key;


    if (key < '1' || key > '9')
        return;


    const auto index =
        static_cast<uint8_t>(key - '1');


    const auto& page =
        ui_.main_page();


    if (index >= page.count)
        return;


    const auto action =
        page.buttons[index].action;


    switch(action)
    {
    case Ui::Button::Action::StartProfile:

        ui_.dispatch(
        {
            Ui::Event::Id::StartProfile
        });

        break;


    case Ui::Button::Action::Monitor:

        ui_.dispatch(
        {
            Ui::Event::Id::OpenMonitor
        });

        break;


    case Ui::Button::Action::Stop:

        ui_.dispatch(
        {
            Ui::Event::Id::Stop
        });

        break;


    case Ui::Button::Action::Reset:

        ui_.dispatch(
        {
            Ui::Event::Id::Reset
        });

        break;


    case Ui::Button::Action::Back:

        ui_.dispatch(
        {
            Ui::Event::Id::Back
        });

        break;


    default:

        break;
    }
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