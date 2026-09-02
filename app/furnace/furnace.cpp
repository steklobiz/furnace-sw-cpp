// furnace.cpp

#include "config.hpp"
#include "furnace.hpp"
#include "tc_parser.hpp"
#include "hal.hpp"
#include "logger.hpp"
#include "pid.hpp"

namespace app {

namespace {
/*
// Tag for Log()
constexpr Tag tag
{
    "FURN",
    Level::Info
};
*/  

}
    
void 
Furnace::init(
    ProfileManager& profiles, 
    SettingManager& settings, 
    TcParser& tc_parser,
    core::Pid& pid) noexcept
{
    profiles_ = &profiles;
    settings_ = &settings;
    tc_parser_ = &tc_parser;
    pid_ = &pid;
    
    fsm_.init(*this, State::Idle, fsm_tables_);
    
}
        
void Furnace::set_notify_callback(
    NotificationCallback callback,
    void* context) noexcept
{
    notify_callback_ = callback;
    notify_context_ = context;
}


void Furnace::process()
{
    fsm_.dispatch(Event::Tick);

    const int32_t temperature = hal::get_temperature();

    current_temperature_c_ =
        static_cast<int16_t>(temperature);

    if (fsm_.state() == State::Running)
    {
        pid_output_ =
            static_cast<uint8_t>(update_pid(temperature));
    }
    else
    {
        pid_output_ = 0;
    }

    hal::set_heater_power(pid_output_);

    // Emit a DataReady notification after all
    // published Furnace data has been updated.
    notify(
        NotificationType::DataReady,
        0);            
}

const char* 
Furnace::state_name(State state) noexcept
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

const char* 
Furnace::step_type_name(StepType type) noexcept
{
    switch(type)
    {
    case StepType::Heating:

        return "Heating";

    case StepType::Holding:

        return "Holding";

    case StepType::Cooling:

        return "Cooling";


    default:

        return "Unknown";
    }
}


//------------------------------------------------------
// State handlers
//------------------------------------------------------

Furnace::State 
Furnace::idle(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Start:
        
        start_profile();
        if (settings_->get_prestep_outs() != 0)
            return State::Waiting;

        return State::Running;
        
    case Event::Error:
        
        set_outs(0); // do i need to reset heaters?
            
        return State::Error;
        

    default:

        return State::Idle;
    }
}

Furnace::State 
Furnace::running(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Stop:
        
        set_outs(0); // do i need to reset heaters?
        
        notify(
            NotificationType::ProfileStopped,
            current_step_);
        
        return State::Stopped;
    
    case Event::Error:
    
        set_outs(0); // do i need to reset heaters?
            
        return State::Error;
        
    default:
        
        ++profile_elapsed_s_;
        ++step_elapsed_s_;
                        
        if (is_step_finished())
            return next_step();
    
        return State::Running;
    }
}

Furnace::State
Furnace::waiting(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Stop:

        set_outs(0);

        notify(
        NotificationType::ProfileStopped,
        current_step_);
        
        return State::Stopped;

    case Event::Error:

        set_outs(0);

        return State::Error;

    case Event::Reset:
    
        enter_step();

        return State::Running;

    default:

        return State::Waiting;
    }
}

Furnace::State 
Furnace::finished(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Error:
    
        set_outs(0); // do i need to reset heaters?
            
        return State::Error;

    case Event::Reset:

        return State::Idle;

    default:

        return State::Finished;
    }
}

Furnace::State 
Furnace::stopped(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Error:
    
        set_outs(0); // do i need to reset heaters?
            
        return State::Error;
        
    case Event::Reset:
        return State::Idle;
    default:
        return State::Stopped;
    }    
};    

Furnace::State 
Furnace::error(const Event& event) noexcept
{
    switch (event)
    {
    case Event::Reset:

        return State::Idle;

    default:

        return State::Error;
        
    }        
};

void Furnace::reset() noexcept
{
    fsm_.dispatch(Event::Reset);
}

//------------------------------------------------------
// Helper functions
//------------------------------------------------------

// Starting profile from Idle state:
// - reset profile
// - first step starts from ambient
void
Furnace::start_profile() noexcept
{
    current_step_ = 0;

    profile_elapsed_s_ = 0;
    step_elapsed_s_ = 0;

    step_start_temperature_c_ = ambient_temperature_c_;

    notify(
        NotificationType::ProfileStarted,
        current_step_);

    const uint8_t prestep_outs =
        settings_->get_prestep_outs();

    if (prestep_outs != 0)
    {
        set_outs(prestep_outs);
    }
    else
    {
        enter_step();
    }
}
// Step's beginning
// - apply outputs
// - anything common to every step
void 
Furnace::enter_step() noexcept
{   
    const auto& step =
        profiles_->start_profile().steps[current_step_];
            
    notify(
        NotificationType::StepStarted,
        current_step_);        
        
    set_outs(step.outs);    
};


// Step's end. Switching from current step to next one
Furnace::State 
Furnace::next_step() noexcept
{
    // Current step is finished.
    // Move to the next one.
    ++current_step_;

    const auto& step =
        profiles_->start_profile().steps[current_step_];    
            
        
    // Case 1: We reached the maximum number of steps.
    if ((current_step_ >= app::config::profiles::max_steps) ||
        // Case 2: 0-0 marker means end of profile.
        (step.setpoint_c == 0 && step.duration == 0))    
    {
        // Profile is finished.
        set_outs(0);

        notify(
                NotificationType::ProfileFinished,
                0);
                
        return State::Finished;
    }


    // New step starts from the previous step target.
    step_start_temperature_c_ =
        profiles_->start_profile()
            .steps[current_step_ - 1]
            .setpoint_c;        
            
    step_elapsed_s_ = 0;    
        
    enter_step();

    return State::Running;
}


void 
Furnace::update_temperature() noexcept
{
    const auto& step =
        profiles_->start_profile().steps[current_step_];    
    
    current_temperature_c_ =
        step_start_temperature_c_
        + (step.setpoint_c - step_start_temperature_c_)
        * static_cast<int32_t>(step_elapsed_s_)
        / step.duration;

}

// Chacks if current step finished
bool 
Furnace::is_step_finished() const noexcept
{
    const auto& step =
        profiles_->start_profile().steps[current_step_];    

    return step_elapsed_s_ >= step.duration;
}


void Furnace::set_outs(uint8_t outs) noexcept
{
    const uint8_t changed = outs_ ^ outs;

    if (changed == 0)
        return;

    const uint8_t set = changed & outs;
    const uint8_t reset = changed & outs_;

    outs_ = outs;

    hal::set_outs(outs_);

    for (uint8_t id = 0; id < 8; ++id)
    {
        const uint8_t mask =
            static_cast<uint8_t>(1u << id);

        if (set & mask)
        {
            notify(
                NotificationType::OutputSet,
                id);
        }

        if (reset & mask)
        {
            notify(
                NotificationType::OutputReset,
                id);
        }
    }
}

int32_t Furnace::update_pid(int32_t temperature) noexcept
{
    return pid_->update(
        setpoint(),
        temperature,
        1000);
}

void Furnace::notify(
    NotificationType type,
    uint16_t argument) noexcept
{
    if (notify_callback_ == nullptr)
        return;

    notify_callback_(
        notify_context_,
        {
            this,
            type,
            argument
        });
}

//------------------------------------------------------
// Getters for UI output and History
//------------------------------------------------------

uint16_t 
Furnace::state() const noexcept
{
    return static_cast<uint16_t>(fsm_.state());
}

uint16_t
Furnace::step_type() const noexcept
{
    const auto& step =
        profiles_->start_profile().steps[current_step_];

    if (step.setpoint_c > step_start_temperature_c_)
        return static_cast<uint16_t>(StepType::Heating);

    if (step.setpoint_c < step_start_temperature_c_)
        return static_cast<uint16_t>(StepType::Cooling);

    return static_cast<uint16_t>(StepType::Holding);
}

uint16_t 
Furnace::current_temperature() const noexcept
{
    return current_temperature_c_;
};

uint16_t 
Furnace::current_step() const noexcept
{
    return current_step_;
};

uint16_t 
Furnace::setpoint() const noexcept
{
    return profiles_->start_profile().steps[current_step_].setpoint_c;
};
    
uint16_t 
Furnace::profile_elapsed() const noexcept
{
    return profile_elapsed_s_;    
};

uint16_t 
Furnace::step_elapsed() const noexcept
{
    return step_elapsed_s_;
};

uint16_t 
Furnace::outputs() const noexcept
{
    return outs_;
}

uint16_t 
Furnace::power() const noexcept
{
    return static_cast<uint16_t>(pid_output_);
}

#ifdef PLATFORM_PC

const core::Pid& Furnace::pid() const noexcept
{
    return *pid_;
}

#endif

} // namespace app