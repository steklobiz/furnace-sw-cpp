#pragma once

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

        platform::trace::Trace<1000, 1000> trace_;
        
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
            
        scheduler_.every<App, &App::update_simulation>(
            1000,
            *this);    
        
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

    void update_simulation() noexcept
    {
        hal::update();
    }
    
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
    
};

} // namespace app