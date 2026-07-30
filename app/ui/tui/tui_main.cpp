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
        << "Temperature:\n"
        << "Profile ID:\n";
        
}
    
    
void Tui::update_main() noexcept
{
    for (std::size_t i = 0;
         i < static_cast<std::size_t>(Ui::MainField::Count);
         ++i)
    {
        const auto field =
            static_cast<Ui::MainField>(i);

        if (main_field_changed(field))
        {
            draw_main_field(field);
        }
    }


    main_cache_ = ui_.main_page();

    main_cache_initialized_ = true;


    move_cursor(15,1);
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

void Tui::draw_main_field(
    Ui::MainField field) noexcept
{
    const auto& page = ui_.main_page();

    switch (field)
    {
    case Ui::MainField::State:

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

        break;


    case Ui::MainField::Temperature:

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

        break;
        
    case Ui::MainField::ProfileId:

        move_cursor(
            MainLayout::ProfileIdRow,
            MainLayout::ValueColumn);

        std::cout
            << "          ";

        move_cursor(
            MainLayout::ProfileIdRow,
            MainLayout::ValueColumn);

        if (page.profile_id > 0)
        {
            std::cout << static_cast<unsigned>(page.profile_id);
        }
        else
        {
            std::cout << "(none)";
        }

        break;

    case Ui::MainField::Buttons:

        move_cursor(
            MainLayout::ButtonsRow,
            1);

        for (uint8_t i = 0; i < page.count; ++i)
        {
            std::cout
                << static_cast<unsigned>(i + 1)
                << ". "
                << main_action_name(
                       page.buttons[i].action)
                << "          \n";
        }

        break;


    default:

        break;
    }
}

bool Tui::main_field_changed(
    Ui::MainField field) const noexcept
{
    const auto& page = ui_.main_page();

    if (!main_cache_initialized_)
        return true;


    switch(field)
    {
    case Ui::MainField::State:

        return page.state != main_cache_.state;


    case Ui::MainField::Temperature:

        return page.temperature != main_cache_.temperature;

    
    case Ui::MainField::ProfileId:

        return page.profile_id != main_cache_.profile_id;            

    case Ui::MainField::Buttons:

        return buttons_changed(page);
        
    default:

        return false;
    }
}

const char* Tui::main_action_name(
    Ui::Button::Action action) noexcept
{
    switch(action)
    {
    case Ui::Button::Action::Start:
        return "Start";

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