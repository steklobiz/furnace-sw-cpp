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
};

} // namespace app