#include "ui.hpp"
#include "profiles.hpp"

#include <iostream>

namespace app
{    
    
//------------------------------------------------------
// Static tables
//------------------------------------------------------

const Ui::Fsm::EnterTable Ui::enters_
{
    &Ui::enter_main,
    &Ui::enter_monitor,
    &Ui::enter_profile_select
};

const Ui::Fsm::Tables Ui::tables_
{
    {
        &Ui::main,
        &Ui::monitor,
        &Ui::profile_select
    },

    &enters_
};

//------------------------------------------------------
// Construction
//------------------------------------------------------

Ui::Ui(Furnace& furnace, Profiles& profiles) noexcept
    :
    furnace_(furnace),
    profiles_(profiles),
    previous_furnace_state_(furnace.state()),
    fsm_(
    *this,
    State::Main,
    tables_)
{
}

//------------------------------------------------------
// Public methods
//------------------------------------------------------

void Ui::process() noexcept
{
    check_furnace_state();

    update();
}

const Ui::MainPage& Ui::main_page() const noexcept
{
    return main_page_;
}

const Ui::MonitorPage& Ui::monitor_page() const noexcept
{
    return monitor_page_;
}

//------------------------------------------------------
// State handlers
//------------------------------------------------------

Ui::State Ui::main(const Event& event) noexcept
{
    switch(event.id)
    {
    case Event::Id::StartProfile:
        return State::ProfileSelect;

    case Event::Id::OpenMonitor:
        return State::Monitor;

    case Event::Id::Stop:

        furnace_.stop();
    
        return State::Main;    

    case Event::Id::Reset:

        furnace_.reset();
    
        return State::Main;    
    
        
    default:
        return State::Main;
    }
}


Ui::State Ui::monitor(const Event& event) noexcept
{
    switch(event.id)
    {
    case Event::Id::Back:

        return State::Main;


    case Event::Id::Stop:

        furnace_.stop();

        return State::Main;


    default:

        return State::Monitor;
    }
}

Ui::State Ui::profile_select(
    const Event& event) noexcept
{
    switch(event.id)
    {
    case Event::Id::SelectProfile:

        profiles_.open(event.data);
 
        furnace_.start();

        return State::Monitor;


    case Event::Id::Back:
        return State::Main;


    default:
        return State::ProfileSelect;
    }
}

//------------------------------------------------------
// Enter callbacks
//------------------------------------------------------

void Ui::enter_main() noexcept
{
    page_changed_ = true;

    update_main();
}

void Ui::enter_monitor() noexcept
{
    page_changed_ = true;
}

void Ui::enter_profile_select() noexcept
{
    page_changed_ = true;
}

//------------------------------------------------------
// Helper functions
//------------------------------------------------------


void Ui::update() noexcept
{
    switch(fsm_.state())
    {
    case State::Main:

        update_main();

        break;

    case State::Monitor:

        update_monitor();

        break;

    default:
        break;
    }
}

void Ui::update_main() noexcept
{
    
    main_page_.profile_id =
        profiles_.selected_id();

    main_page_.state =
        furnace_.state();

    main_page_.temperature =
        furnace_.current_temp();
        
    main_page_.count = 0;

    switch (furnace_.state())
    {
    case Furnace::State::Idle:

        main_page_.buttons[0] =
        {
            Button::Action::StartProfile
        };

        main_page_.buttons[1] =
        {
            Button::Action::EditProfile
        };

        main_page_.buttons[2] =
        {
            Button::Action::Settings
        };

        main_page_.count = 3;

        break;


    case Furnace::State::Running:

        main_page_.buttons[0] =
        {
            Button::Action::Monitor
        };

        main_page_.buttons[1] =
        {
            Button::Action::Stop
        };

        main_page_.count = 2;

        break;

    case Furnace::State::Stopped:
    
        main_page_.buttons[0] =
        {
            Button::Action::Reset
        };
    
        main_page_.count = 1;
    
        break;        

    case Furnace::State::Finished:

        main_page_.buttons[0] =
        {
            Button::Action::Reset
        };

        main_page_.count = 1;

        break;


    default:
        break;
    }
}

void Ui::update_monitor() noexcept
{
    monitor_page_.state =
        furnace_.state();

    monitor_page_.step_type =
        furnace_.step_type();

    monitor_page_.step =
        furnace_.current_step();

    monitor_page_.temperature =
        furnace_.current_temp();

    monitor_page_.setpoint =
        furnace_.setpoint();

    monitor_page_.profile_elapsed =
        furnace_.profile_elapsed();

    monitor_page_.step_elapsed =
        furnace_.step_elapsed();

    monitor_page_.outputs =
        furnace_.outputs();
}

void Ui::check_furnace_state() noexcept
{
    const auto state = furnace_.state();

    if (state != previous_furnace_state_)
    {
        previous_furnace_state_ = state;

        page_changed_ = true;
    }
}


const Ui::Button& Ui::main_button(
    uint8_t index) const noexcept
{
    return main_page_.buttons[index];
}

} // namespace app