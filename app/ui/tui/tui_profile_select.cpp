#include <cstdint>
#include <iostream>

#include "tui.hpp"

namespace app
{

void Tui::draw_profile_select() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "    SELECT PROFILE\n"
        << "========================\n\n";
}
    
    
void Tui::update_profile_select() noexcept
{
    move_cursor(5,1);

    for (uint8_t i = 0; i < 10; ++i)
    {
        std::cout
            << i
            << ". Profile "
            << unsigned(i)
            << '\n';
    }

    std::cout
        << "\nB. Back\n";
}
    
    
void Tui::process_profile_select_input() noexcept
{
    char key;

    std::cin >> key;


    if (key == 'b' || key == 'B')
    {
        ui_.dispatch(
        {
            Ui::Event::Id::Back
        });

        return;
    }


    if (key < '0' || key > '9')
        return;


    const auto profile =
        static_cast<uint16_t>(key - '0');


    ui_.dispatch(
    {
        Ui::Event::Id::SelectProfile,
        profile
    });
}

    
} // namespace app