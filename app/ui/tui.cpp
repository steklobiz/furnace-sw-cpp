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
// Public methods
//------------------------------------------------------

void Tui::process() noexcept
{
    render();

    process_input();
}

//------------------------------------------------------
// Render
//------------------------------------------------------

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

//------------------------------------------------------
// Input
//------------------------------------------------------

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

void Tui::process_monitor_input() noexcept
{
    char key;

    std::cin >> key;


    switch(key)
    {
    case '1':

        ui_.dispatch(
        {
            Ui::Event::Id::Back
        });

        break;


    case '2':

        ui_.dispatch(
        {
            Ui::Event::Id::Stop
        });

        break;


    default:

        break;
    }
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


//------------------------------------------------------
// Page display
//------------------------------------------------------

void Tui::draw_main() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "       MAIN PAGE\n"
        << "========================\n";
}

void Tui::draw_monitor() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "        MONITOR\n"
        << "========================\n\n"

        << "State:\n"
        << "Step:\n"
        << "Type:\n"
        << "Temperature:\n"
        << "Setpoint:\n"
        << "Profile time:\n"
        << "Step time:\n"
        << "Outputs:\n\n"

        << "1. Back\n"
        << "2. Stop\n";
}

void Tui::draw_profile_select() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "    SELECT PROFILE\n"
        << "========================\n\n";
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
            << action_name(page.buttons[i].action)
            << "            \n";
    }
}

void Tui::update_monitor() noexcept
{
    const auto& page = ui_.monitor_page();

    if (!monitor_cache_valid_ ||
        page.state != monitor_cache_.state)
    {
        move_cursor(
            MonitorLayout::StateRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << Furnace::state_name(page.state)
            << "        ";
    }
    
    if (!monitor_cache_valid_ ||
    page.step != monitor_cache_.step)
    {
        move_cursor(
            MonitorLayout::StepRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << page.step
            << "        ";
    }

  if (!monitor_cache_valid_ ||
    page.step_type != monitor_cache_.step_type)
    {
        move_cursor(
            MonitorLayout::TypeRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << Furnace::step_type_name(page.step_type)
            << "        ";
    }  
    
    if (!monitor_cache_valid_ ||
    page.temperature != monitor_cache_.temperature)
    {
        move_cursor(
            MonitorLayout::TemperatureRow,
            MonitorLayout::ValueColumn);

        std::cout
            << page.temperature
            << " C     ";
    }


    if (!monitor_cache_valid_ ||
        page.setpoint != monitor_cache_.setpoint)
    {
        move_cursor(
            MonitorLayout::SetpointRow,
            MonitorLayout::ValueColumn);

        std::cout
            << page.setpoint
            << " C     ";
    }


    if (!monitor_cache_valid_ ||
        page.profile_elapsed != monitor_cache_.profile_elapsed)
    {
        move_cursor(
            MonitorLayout::ProfileTimeRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << page.profile_elapsed
            << " s     ";
    }
    

    if (!monitor_cache_valid_ ||
    page.step_elapsed != monitor_cache_.step_elapsed)
    {
        move_cursor(
            MonitorLayout::StepTimeRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << page.step_elapsed
            << " s     ";
    }

    
    if (!monitor_cache_valid_ ||
    page.outputs != monitor_cache_.outputs)
    {
        move_cursor(
            MonitorLayout::OutputsRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << unsigned(page.outputs)
            << "        ";
    }    
    
    
    monitor_cache_ = page;
    monitor_cache_valid_ = true;
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


//------------------------------------------------------
// Helpers
//------------------------------------------------------

const char* Tui::action_name(
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

const char* Tui::state_name(
    Furnace::State state) noexcept
{
    switch(state)
    {
    case Furnace::State::Idle:
        return "Idle";

    case Furnace::State::Running:
        return "Running";

    case Furnace::State::Waiting:
        return "Waiting";

    case Furnace::State::Finished:
        return "Finished";

    case Furnace::State::Stopped:
        return "Stopped";

    case Furnace::State::Error:
        return "Error";

    default:
        return "";
    }
}

const char* Tui::step_type_name(
    Furnace::StepType type) noexcept
{
    switch(type)
    {
    case Furnace::StepType::Heating:
        return "Heating";

    case Furnace::StepType::Holding:
        return "Holding";

    case Furnace::StepType::Cooling:
        return "Cooling";

    default:
        return "";
    }
}

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

//------------------------------------------------------
// Cursor helpers
//------------------------------------------------------

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