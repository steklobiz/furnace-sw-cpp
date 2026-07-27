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

            draw_main();

            break;

        case Ui::State::Monitor:

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
    char key;

    std::cin >> key;


    if (ui_.state() == Ui::State::Main)
    {
        if (key >= '1' && key <= '9')
        {
            const uint8_t index =
                static_cast<uint8_t>(key - '1');

            const auto& page =
                ui_.main_page();


            if (index < page.count)
            {
                execute_action(
                    page.buttons[index].action);
            }
        }
    }
}


//------------------------------------------------------
// Common UI helpers
//------------------------------------------------------


Ui::Event::Id Tui::event_from_action(
    Ui::Button::Action action) noexcept
{
    switch(action)
    {
    case Ui::Button::Action::StartProfile:
        return Ui::Event::Id::StartProfile;

    case Ui::Button::Action::Monitor:
        return Ui::Event::Id::OpenMonitor;

    case Ui::Button::Action::Back:
        return Ui::Event::Id::Back;

    default:
        return Ui::Event::Id::None;
    }
}


void Tui::execute_action(
    Ui::Button::Action action) noexcept
{
    switch(action)
    {
    case Ui::Button::Action::StartProfile:

        ui_.dispatch(
            Ui::Event(
                Ui::Event::Id::StartProfile));

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


} // namespace app