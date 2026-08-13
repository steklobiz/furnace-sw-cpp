#pragma once

#include <cstdint>
#include <array>

#include "fsm.hpp"
#include "furnace.hpp"
#include "profiles.hpp"
#include "settings.hpp"

namespace app
{

class Ui
{
public:

    enum class State : uint8_t
    {
        Main,
        Monitor,
        ProfileSelect,
        Result,
    
        Count
    };

    struct Event
    {
        enum class Id : uint8_t
        {
            None,
            
            Start, 
            Stop,
            Reset,

            SelectProfile,

            OpenMonitor,
                        
            Back
        };

        constexpr Event(Id id) noexcept
            :
            id(id)
        {
        }

        constexpr Event(Id id, uint16_t data) noexcept
            :
            id(id),
            data(data)
        {
        }

        Id id;

        uint16_t data = 0;
    };

    struct Button
    {
        enum class Action : uint8_t
        {
            None,
    
            Start,
            Stop,
            Reset,
            
            SelectProfile,

            Monitor,
            Settings,
            EditProfile,
            Back
        };
    
        Action action;
    };
    
    // Page models
    struct MainPage
    {
        uint8_t profile_id;
        
        Furnace::State state;
        uint16_t temperature;
    
        std::array<Button, 6> buttons;
        uint8_t count = 0;
    };

    struct MonitorPage
    {
        Furnace::State state;
    
        Furnace::StepType step_type;
    
        uint16_t temperature;
        uint16_t setpoint;
    
        uint16_t step;
    
        uint32_t profile_elapsed;
        uint32_t step_elapsed;
    
        uint8_t power;
        uint8_t outputs;
    };

    struct ResultPage
    {
        Furnace::State state;
    
        uint32_t duration;
    
        uint16_t temperature;
    
        uint8_t alarm_id;
    };
    
    // Page fields    
    enum class MonitorField : uint8_t
    {
        State,
        Step,
        StepType,
        Temperature,
        Setpoint,
        ProfileElapsed,
        StepElapsed,
        Power,
        Outputs,

        Count
    };
    
    enum class MainField : uint8_t
    {
        State,
        Temperature,
        ProfileId,
        Buttons,
        
        Count
    };
            
    Ui() noexcept = default;

    void init(Furnace& furnace, ProfileManager& profiles, SettingManager& settings) noexcept;
    
    void process() noexcept;

    const MainPage& main_page() const noexcept;
    
    const MonitorPage& monitor_page() const noexcept;
    
    const ResultPage& result_page() const noexcept;
    
    State state() const noexcept
    {
        return fsm_.state();
    }
    
    // Temporary.
    // Later the UI will generate its own events from
    // keyboard/DWIN input.
    void dispatch(const Event& event) noexcept
    {
        fsm_.dispatch(event);
    }

    bool page_changed() const noexcept
    {
        return page_changed_;
    }

    void clear_page_changed() noexcept
    {
        page_changed_ = false;
    }

private:

    using Fsm =
        core::Fsm<Ui, State, Event>;

    //--------------------------------------------------
    // State handlers
    //--------------------------------------------------

    State main(const Event& event) noexcept;

    State monitor(const Event& event) noexcept;

    State profile_select(const Event& event) noexcept;
    
    //--------------------------------------------------
    // Enter callbacks
    //--------------------------------------------------

    void enter_main() noexcept;

    void enter_monitor() noexcept;

    void enter_profile_select() noexcept;

    //------------------------------------------------------
    // Helper functions
    //------------------------------------------------------

    void update() noexcept;
        
    void update_main() noexcept;

    void update_monitor() noexcept;

    void check_furnace_state() noexcept;
    
    const Button& main_button(
        uint8_t index) const noexcept;    
    
private:

    static const Fsm::EnterTable enters_;
    static const Fsm::Tables tables_;
    
    
    Furnace* furnace_ = nullptr;
    
    ProfileManager* profiles_ = nullptr;
    SettingManager* settings_ = nullptr;
    
    Furnace::State previous_furnace_state_;
    
    Fsm fsm_;

    bool page_changed_ = true;
    
    MainPage main_page_;
    MonitorPage monitor_page_;
    ResultPage result_page_;
};

} // namespace app