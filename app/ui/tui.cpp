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
    if (!ui_.page_changed())
        return;


    switch(ui_.state())
    {
    case Ui::State::Main:

        show_main();

        break;


    case Ui::State::Monitor:

        show_monitor();

        break;


    case Ui::State::ProfileSelect:

        show_profile_select();

        break;


    default:
        break;
    }


    ui_.clear_page_changed();
}

//------------------------------------------------------
// Input
//------------------------------------------------------

void Tui::process_input() noexcept
{
    switch(ui_.state())
    {
    case Ui::State::Main:

        process_main_input();
        break;


    case Ui::State::ProfileSelect:

        process_profile_select_input();
        break;


    case Ui::State::Monitor:

        process_monitor_input();
        break;


    default:
        break;
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


void Tui::show_main() noexcept
{
    const auto& page = ui_.main_page();

    std::cout
        << "\n========================\n"
        << "        MAIN PAGE       \n"
        << "========================\n\n";


    for (uint8_t i = 0; i < page.count; ++i)
    {
        std::cout
            << (i + 1)
            << ". "
            << action_name(
                   page.buttons[i].action)
            << '\n';
    }

    std::cout << '\n';
}

void Tui::show_monitor() noexcept
{
    const auto& page =
        ui_.monitor_page();


    std::cout
        << "\n========================\n"
        << "        MONITOR         \n"
        << "========================\n\n";


    std::cout
        << "State       : "
        << state_name(page.state)
        << '\n';


    std::cout
        << "Step        : "
        << page.step
        << '\n';


    std::cout
        << "Type        : "
        << step_type_name(page.step_type)
        << '\n';


    std::cout
        << "Temperature : "
        << page.temperature
        << " C\n";


    std::cout
        << "Setpoint    : "
        << page.setpoint
        << " C\n";


    std::cout
        << "Profile time: "
        << page.profile_elapsed
        << " s\n";


    std::cout
        << "Step time   : "
        << page.step_elapsed
        << " s\n";


    std::cout
        << "Outputs     : "
        << static_cast<unsigned>(
               page.outputs)
        << "\n\n";
}

void Tui::show_profile_select() noexcept
{
    std::cout
        << "\n========================\n"
        << "    SELECT PROFILE      \n"
        << "========================\n\n";


    for (uint8_t i = 0; i < 10; ++i)
    {
        std::cout
            << "  "
            << static_cast<unsigned>(i)
            << ". Profile "
            << static_cast<unsigned>(i)
            << '\n';
    }


    std::cout
        << "\n  b. Back\n";
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

} // namespace app