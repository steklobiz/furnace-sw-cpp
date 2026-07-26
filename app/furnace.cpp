#include "furnace.hpp"
#include "hal.hpp"
#include <iostream>

namespace app
{
    

Furnace::Furnace()
{
    std::cout << "Furnace is ready..."  << std::endl;
}

const char* Furnace::state_name(State state) noexcept
{
    switch (state)
    {
    case State::Idle:
        return "Idle";

    case State::Running:
        return "Running";

    case State::Waiting:
        return "Waiting";

    case State::Finished:
        return "Finished";

    case State::Stopped:
        return "Stopped";

    case State::Error:
        return "Error";

    default:
        return "Unknown";
    }
}

void Furnace::load_profile(uint8_t) noexcept
{
    
};


//------------------------------------------------------
// State handlers
//------------------------------------------------------

Furnace::State Furnace::idle(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Start:
    
        start_profile();

        return State::Running;

    default:

        return State::Idle;
    }
}

Furnace::State Furnace::running(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Stop:
    
         hal::reset_outputs();
    
        return State::Stopped;
        
    default:
        
        ++profile_elapsed_s_;
        ++step_elapsed_s_;
        
        // Calculate the current target temperature for the active step.
        update_temperature();
                
        if (is_step_finished())
            return next_step();
    
        return State::Running;
    }
}

Furnace::State Furnace::waiting(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Stop:
    
        return State::Stopped;
        
    case Event::Start:

        enter_step();
        
        return State::Running;        

    default:

        return State::Waiting;
    }
}


Furnace::State Furnace::finished(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Reset:
        return State::Idle;
    default:
        return State::Finished;
    }
}

Furnace::State Furnace::stopped(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Reset:
        return State::Idle;
    default:
        return State::Stopped;
    }    
};    

Furnace::State Furnace::error(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Reset:
        return State::Idle;
    default:
        return State::Error;
    }        
};

//------------------------------------------------------
// Helper functions
//------------------------------------------------------

// Starting profile from Idle state:
// - reset profile
// - first step starts from ambient
void Furnace::start_profile() noexcept
{
    current_step_ = 0;
    
    profile_elapsed_s_ = 0;
    step_elapsed_s_ = 0;

    step_start_temperature_c_ = ambient_temperature_c_;
    
    enter_step();
};


// Step's beginning
// - initialize timing
// - apply outputs
// - anything common to every step
void Furnace::enter_step() noexcept
{    
    hal::set_outputs(profile_[current_step_].flags);
        
    std::cout << "setpoint: "<< static_cast<unsigned>(profile_[current_step_].setpoint_c)
        << "| duration: " << static_cast<unsigned>(profile_[current_step_].duration_sec)
        << "| flags: " << static_cast<unsigned>(profile_[current_step_].flags)
        << std::endl;
};

// Step's end. Switching from current step to next one
Furnace::State Furnace::next_step() noexcept
{
    // Current step is finished.
    // Move to the next one.
    ++current_step_;

    // Case 1:
    // We reached the maximum number of steps.
    // Profile is finished.
    if (current_step_ >= MAX_RPOFILE_STEPS)
    {
        return State::Finished;
    }

    const Step& step = profile_[current_step_];

    // Case 2:
    // 0-0 marker means end of profile.
    if (step.setpoint_c == 0 &&
        step.duration_sec == 0)
    {
        return State::Finished;
    }

    // New step starts from the previous step target.
    step_start_temperature_c_ =
        profile_[current_step_ - 1].setpoint_c;

    step_elapsed_s_ = 0;    
        
    enter_step();

    return State::Running;
}


void Furnace::update_temperature() noexcept
{
    const Step& step = profile_[current_step_];

    current_temperature_c_ =
        step_start_temperature_c_
        + (step.setpoint_c - step_start_temperature_c_)
        * static_cast<int32_t>(step_elapsed_s_)
        / step.duration_sec;

    std::cout
        << "step: "
        << unsigned(current_step_)
        << "| time: "
        << step_elapsed_s_
        << "| temp: "
        << current_temperature_c_
        << '\n';
}

// Chacks if current step finished
bool Furnace::is_step_finished() const noexcept
{
    const Step& step = profile_[current_step_];

    return step_elapsed_s_ >= step.duration_sec;
}


// Getters for UI output

Furnace::State Furnace::state() const noexcept
{
    return fsm_.state();
}

Furnace::StepType Furnace::step_type() const noexcept
{
    const Step& step = profile_[current_step_];

    if (step.setpoint_c > step_start_temperature_c_)
        return StepType::Heating;

    if (step.setpoint_c < step_start_temperature_c_)
        return StepType::Cooling;

    return StepType::Holding;
}

uint16_t Furnace::current_temp() const noexcept
{
    return current_temperature_c_;
};

uint16_t Furnace::current_step() const noexcept
{
    return current_step_;
};

uint16_t Furnace::setpoint() const noexcept
{
    return profile_[current_step_].setpoint_c; 
};
    
uint32_t Furnace::profile_elapsed() const noexcept
{
    return profile_elapsed_s_;    
};

uint32_t Furnace::step_elapsed() const noexcept
{
    return step_elapsed_s_;
};

uint8_t Furnace::outputs() const noexcept
{
    return profile_[current_step_].flags;
};


} // namespace app