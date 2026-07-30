#include <cstdint>
#include <iostream>

#include "tui.hpp"

namespace app
{

//------------------------------------------------------
// Construction
//------------------------------------------------------

Tui::Tui(Ui& ui)
    :
    ui_(ui)
{
}

//------------------------------------------------------
// Common UI logic
//------------------------------------------------------

void Tui::process() noexcept
{
    render();

    process_input();
}

void Tui::render() noexcept
{
    if (ui_.page_changed())
    {
        switch (ui_.state())
        {
        case Ui::State::Main:

            main_cache_initialized_ = false;

            draw_main();

            break;

        case Ui::State::Monitor:
        
            main_cache_initialized_ = false;

            draw_monitor();

            break;

        case Ui::State::ProfileSelect:

            draw_profile_select();

            break;

        default:
            break;
        }

        ui_.clear_page_changed();
    }

    switch (ui_.state())
    {
    case Ui::State::Main:

        update_main();

        break;

    case Ui::State::Monitor:

        update_monitor();

        break;

    case Ui::State::ProfileSelect:

        update_profile_select();

        break;

    default:
        break;
    }
}

void Tui::process_input() noexcept
{
    switch (ui_.state())
    {
    case Ui::State::Main:

        process_main_input();

        break;


    case Ui::State::Monitor:

        process_monitor_input();

        break;


    case Ui::State::ProfileSelect:

        process_profile_select_input();

        break;


    default:

        break;
    }
}


//------------------------------------------------------
// Common UI helpers
//------------------------------------------------------


void Tui::execute_action(
    Ui::Button::Action action) noexcept
{
    switch(action)
    {
    case Ui::Button::Action::Start:

        ui_.dispatch(
            Ui::Event(
                Ui::Event::Id::Start));

        break;


    case Ui::Button::Action::Monitor:

        ui_.dispatch(
            Ui::Event(
                Ui::Event::Id::OpenMonitor));

        break;


    case Ui::Button::Action::Stop:

        ui_.dispatch(
            Ui::Event(
                Ui::Event::Id::Stop));

        break;


    case Ui::Button::Action::Reset:

        ui_.dispatch(
            Ui::Event(
                Ui::Event::Id::Reset));

        break;


    default:

        break;
    }
}

bool Tui::buttons_changed(
    const Ui::MainPage& page) const noexcept
{
    if (page.count != main_cache_.count)
        return true;

    for (uint8_t i = 0; i < page.count; ++i)
    {
        if (page.buttons[i].action !=
            main_cache_.buttons[i].action)
        {
            return true;
        }
    }

    return false;
}

} // namespace app