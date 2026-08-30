#include "app.hpp"

namespace app {

bool App::init() noexcept
{
//     Log::info(
//         tag,
//         "Application initialized");
   
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
        pid_);
    
    alarm_.init(
        tc_parser_,
        furnace_,
        settings_    
    ); 
            
    data_aggregator_.init(
         tc_parser_,
         furnace_, 
         profiles_,
         settings_,
         alarm_);
    
    ui_.init(
        data_aggregator_,
        furnace_,            
        profiles_,
        settings_
        );
        
    ui_.set_command_callback(
        &App::ui_command_callback,
        this);
        
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
        
    scheduler_.every<TcParser, &TcParser::update>(
        500,
        tc_parser_);
        
    scheduler_.every<AlarmDispatcher, &AlarmDispatcher::process>(
    500,
    alarm_);
    
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

void App::run() noexcept
{
    while (true)
    {
        scheduler_.run();

        hal::update();
         
        hal::delay_ms(loop_delay_ms);
    }
}
    
void App::reset_furnace() noexcept
{
    alarm_.clear_all();
    furnace_.reset();
}

void App::ui_command_callback(
    void* context,
    Ui::Action action) noexcept
{
    auto& app =
        *static_cast<App*>(context);

    switch (action.type)
    {
    case Ui::ActionType::ResetFurnace:
        app.reset_furnace();
        break;

    case Ui::ActionType::ContinueFurnace:
        app.furnace_.start();
        break;
        
    default:
        break;
    }
}

} // namespace app