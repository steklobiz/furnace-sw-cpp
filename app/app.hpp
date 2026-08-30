#pragma once

#include <cstddef>

#include "profiles.hpp"
#include "furnace.hpp"
#include "tc_parser.hpp"
#include "ui.hpp"
#include "tui.hpp"
#include "scheduler.hpp"
#include "hal.hpp"
#include "logger.hpp"
#include "alarm.hpp"
#include "pid.hpp"
#include "data_aggregator.hpp"

#ifdef PLATFORM_PC
#include "trace.hpp"
#endif

namespace app
{

// Define tag
namespace
{

constexpr Tag tag
{
    "App",
    Level::Info
};

}

    
class App
{
public:

    App() = default;
    
    bool init() noexcept;

    void run() noexcept;
    
private:

    static void ui_command_callback(
        void* context,
        Ui::Action action) noexcept;

    void reset_furnace() noexcept;

    static constexpr uint32_t loop_delay_ms = 5;
/*    
#ifdef PLATFORM_PC


    // Records the current PID state in the PID trace buffer.
    void trace_pid() noexcept
    {
        const auto& debug = furnace_.pid().debug();
        
        trace_.add_pid({
            hal::tick_s(),
            furnace_.setpoint(),
            furnace_.current_temperature(),
            debug.error,
            debug.p,
            debug.i,
            debug.d,
            debug.output
        });
    }

    // Records the current furnace state in the furnace trace buffer.
    void trace_furnace() noexcept
    {
        trace_.add_furnace({
            hal::tick_s(),
            furnace_.current_temperature(),
            furnace_.setpoint(),
            furnace_.power()
        });
    }
    
        
#endif
*/    
    using Trace = platform::trace::Trace<100, 100>;
    
private:

    ProfileManager profiles_ ;
    
    SettingManager settings_;

    TcParser tc_parser_;

    DataAggregator data_aggregator_;
        
    Furnace furnace_;
    
    AlarmDispatcher alarm_;

    Ui ui_;

    Tui tui_;
    
    core::Scheduler scheduler_;
    
    core::Pid pid_;
/*
#ifdef PLATFORM_PC
    Trace trace_;
#endif
  */  
};

} // namespace app