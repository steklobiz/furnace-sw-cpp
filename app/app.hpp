#pragma once

#include "profiles.hpp"
#include "furnace.hpp"
#include "ui.hpp"
#include "tui.hpp"

#include "scheduler.hpp"
#include "hal.hpp"
#include "log.hpp"

namespace app
{

// Define tag
namespace
{

constexpr core::log::Tag tag
{
    "APP",
    core::log::Level::Info
};

}
    
    
class App
{
public:


    App() = default;
    
    bool init()
    {
    core::log::info(
        tag,
        "Application initialized");

    core::log::info(
        tag,
        "Step=",
        3,
        " Temp=",
        125);
                /*        
        // Connect objects (dependency injection)
        furnace_.init(profiles_, settings_);
        ui_.init(furnace_, profiles_, settings_);
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

        // furnace_.process()
        scheduler_.every<Furnace, &Furnace::process>(
            1000,
            furnace_);
        // ui_.process()        
        scheduler_.every<Ui, &Ui::process>(
            100,
            ui_);
        // tui_.process()        
        scheduler_.every<Tui, &Tui::process>(
            100,
            tui_);
*/    
        return true; // replace with false if anything fails    
    }
    

    void run() noexcept
    {
        while(true)
        {
            // Execute ready scheduler tasks.
            scheduler_.run();

            // Small delay to avoid a busy loop.
            // Scheduler precision depends on this value.
            hal::delay_ms(loop_delay_ms);
        }
    }

private:

    static constexpr uint32_t loop_delay_ms = 5;

private:

    ProfileManager profiles_ ;
    
    SettingManager settings_;


    Furnace furnace_;

    Ui ui_;

    Tui tui_;


    core::Scheduler scheduler_;

};

} // namespace app