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
                
        ui_.init(
            furnace_, 
            profiles_, 
            settings_);
            
        tui_.init(ui_);

        
        // Temporary test profile.
        // Later this will probably come from EEPROM / flash storage.
        Profile& p = profiles_.edit();

        p.steps[0] = {50, 10, 0x01};
        p.steps[1] = {50, 10, 0x02};
        p.steps[2] = {100, 10, 0x00};
        p.steps[3] = {100, 10, 0x00};
        p.steps[4] = {50, 10, 0x00};
        p.steps[5] = {0, 0, 0x00};

        // Registration order defines execution order.
        // Scheduler stores:
        //     object pointer
        //     generated callback function
        // No wrappers are required.

        scheduler_.every<Furnace, &Furnace::process>(
            1000,
            furnace_);
            
#ifdef PLATFORM_PC

        scheduler_.every<App, &App::update_simulation>(
            1000,
            *this);    

        scheduler_.every<App, &App::trace_pid>(
            1000,
            *this);
        
        scheduler_.every<App, &App::trace_furnace>(
            1000,
            *this);
    
        scheduler_.once<App, &App::debug_dump_trace>(
            30000,
            *this);
                        
#endif    
                    
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
    
#ifdef PLATFORM_PC

    void update_simulation() noexcept
    {
        hal::update();
    }

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

    void trace_furnace() noexcept
    {
        trace_.add_furnace({
            hal::tick_s(),
            furnace_.current_temperature(),
            furnace_.setpoint(),
            furnace_.pid_output()
        });
    }
    
    void debug_dump_trace() noexcept
    {
        Log::info(tag, "----- PID TRACE -----");
    
        const auto& pid_trace = trace_.pid();
    
        for (std::size_t i = 0; i < pid_trace.size(); ++i)
        {
            const auto& sample = pid_trace.from_newest(i);
    
            Log::info(
                tag,
                "time=", sample.time_s,
                " sp=", sample.setpoint,
                " meas=", sample.measurement,
                " err=", sample.error,
                " p=", sample.p,
                " i=", sample.i,
                " d=", sample.d,
                " out=", sample.output);
        }
        
        debug_dump_furnace_trace();
    }
    
    void debug_dump_furnace_trace() noexcept
    {
        Log::info(tag, "----- FURNACE TRACE -----");
    
        const auto& furnace_trace = trace_.furnace();
    
        for (std::size_t i = 0; i < furnace_trace.size(); ++i)
        {
            const auto& sample = furnace_trace.from_newest(i);
    
            Log::info(
                tag,
                "time=", sample.time_s,
                " temp=", sample.temperature,
                " sp=", sample.setpoint,
                " out=", sample.output);
        }
    }
        
#endif
    
    using Trace = platform::trace::Trace<100, 100>;
    
private:

    ProfileManager profiles_ ;
    
    SettingManager settings_;

    TcParser tc_parser_;
    
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