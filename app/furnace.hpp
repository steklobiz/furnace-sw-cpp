#pragma once

#include <cstdint>
#include "fsm.hpp"
#include "profiles.hpp"
#include "settings.hpp"

namespace app {
    


class Furnace
{
public:

    enum class State : uint8_t
    {
        Idle = 0,
        Running,
        Waiting,
        Finished,
        Stopped,
        Error,

        Count
    };
    
    enum class StepType: uint8_t
    {
        Heating,
        Holding,
        Cooling
    };        

    Furnace(ProfileManager& profiles, SettingManager& settings) noexcept;
    
    // Should be executed by scheduler once a second
    void process() { fsm_.dispatch(Event::Tick); }

    void start()   { fsm_.dispatch(Event::Start); }
    void stop()    { fsm_.dispatch(Event::Stop);  }
    void reset()   { fsm_.dispatch(Event::Reset); }
    
    static const char* state_name(State state) noexcept; // only for TUI
    static const char* step_type_name(StepType type) noexcept; // only for TUI
        
    // UI getters 
    State state() const noexcept;
    StepType step_type() const noexcept;
    uint16_t current_temp() const noexcept;
    uint16_t current_step() const noexcept;
    uint16_t setpoint() const noexcept;
    uint32_t profile_elapsed() const noexcept;
    uint32_t step_elapsed() const noexcept;
    uint8_t outputs() const noexcept;
        
private:

    static constexpr uint8_t MAX_RPOFILE_STEPS = 5;
    static constexpr int8_t ambient_temperature_c_ = 25;

    enum class Event : uint8_t
    {
        Tick,
        Start,
        Stop,
        Reset
    };        

    enum class StepAction : uint8_t
    {
        Continue,
        WaitForOperator,
    };
    /*
    struct Step{
        uint8_t setpoint_c;
        uint8_t duration_sec; 
        uint8_t flags;         //  outputs + actions
    };     
      */  
    using Fsm = core::Fsm<Furnace, State, Event>;

    //------------------------------------------------------
    // State handlers
    //------------------------------------------------------

    State idle(const Event&) noexcept;
    State running(const Event&) noexcept;
    State waiting(const Event&) noexcept;
    State finished(const Event&) noexcept;
    State stopped(const Event&) noexcept;    
    State error(const Event&) noexcept;

    //------------------------------------------------------
    // Optional enter callbacks
    //------------------------------------------------------


    //------------------------------------------------------
    // Optional exit callbacks
    //------------------------------------------------------

    //------------------------------------------------------
    // Helper functions
    //------------------------------------------------------
        
    // Initializes runtime data, resets the current step index,
    // starts the profile timer, and enters the first step.
    void start_profile() noexcept;

    // Applies step outputs, captures the starting temperature,
    // and starts step timing.
    // This function is called when a step actually begins.
    void enter_step() noexcept;
    
    // Increments the step index and decides whether the next step
    // should start immediately, wait for operator confirmation,
    // or finish the profile.
    State next_step() noexcept;

    
    // Calculates the current temperature setpoint according to
    // the active step and updates heaters/fans through the
    // temperature controller.
    void update_temperature() noexcept;
    
    // Returns true when the current step duration has elapsed
    // according to the recipe execution rules.
    bool is_step_finished() const noexcept;

    
    //------------------------------------------------------
    // FSM tables
    //------------------------------------------------------

    using StateTable = Fsm::StateTable;
    using EnterTable = Fsm::EnterTable;
    using ExitTable  = Fsm::ExitTable;

    static constexpr StateTable handlers_
    {
        &Furnace::idle,
        &Furnace::running,
        &Furnace::waiting,
        &Furnace::finished,
        &Furnace::stopped,
        &Furnace::error
    };

    static constexpr Fsm::Tables fsm_tables_
    {
        handlers_,
    };

    //------------------------------------------------------
    // Data
    //------------------------------------------------------

    Fsm fsm_
    {
        *this,
        State::Idle,
        fsm_tables_
    };

    ProfileManager& profiles_;
    SettingManager& settings_;
    
    uint8_t current_step_ = 0;

    // Elapsed time 
    uint32_t profile_elapsed_s_;
    uint32_t step_elapsed_s_;
    
    // Temperature at the beginning of the current step.
    int16_t step_start_temperature_c_;
    
    // Current calculated temperature (update each tick). will be used as a target fo PID later
    int16_t current_temperature_c_; 
};

} // namespace app 