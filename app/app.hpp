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
#include "history.hpp"
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
    
    bool init()
    {
        Log::info(
            tag,
            "Application initialized");
       
        const Settings& settings = settings_.view();

        pid_.init({
            settings.pid_kp,
            settings.pid_ki,
            settings.pid_kd
        });    
        
        tc_parser_.init();
            
        // Connect objects (dependency injection)
        furnace_.init(
            profiles_, 
            settings_, 
            tc_parser_, 
            history_,
            pid_);
        
        alarm_.init(
            tc_parser_,
            furnace_    
        ); 
                
        data_aggregator_.init(
             tc_parser_,
             furnace_, 
             profiles_);
        
        ui_.init(
            furnace_, 
            profiles_, 
            settings_,
            data_aggregator_);
            
        tui_.init(
            ui_);
        
  
        // Registration order defines execution order.
        // Scheduler stores:
        //     object pointer
        //     generated callback function
        // No wrappers are required.

        scheduler_.every<Furnace, &Furnace::process>(
            1000,
            furnace_);
/*            
#ifdef PLATFORM_PC

        scheduler_.every<App, &App::trace_pid>(
            1000,
            *this);
        
        scheduler_.every<App, &App::trace_furnace>(
            1000,
            *this);
                            
#endif    
  */                  
        scheduler_.every<Ui, &Ui::process>(
            100,
            ui_);
        
        scheduler_.every<Tui, &Tui::process>(
            100,
            tui_);
            
        return true; // replace with false if anything fails    
    }

    
    void run() noexcept
    {
        while (true)
        {
            scheduler_.run();
   
            hal::update();
             
            hal::delay_ms(loop_delay_ms);
        }
    }
    
private:

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
        
    History history_;
    
    Furnace furnace_;
    
    AlarmDispatcher alarm_;

    Ui ui_;

    Tui tui_;
    
    core::Scheduler scheduler_;
    
    core::Pid pid_;

#ifdef PLATFORM_PC
    Trace trace_;
#endif
    
};

} // namespace app