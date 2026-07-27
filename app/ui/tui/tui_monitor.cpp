#include <cstdint>
#include <iostream>
#include "tui.hpp"

namespace app
{

void Tui::draw_monitor() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "        MONITOR\n"
        << "========================\n\n"

        << "State:\n"
        << "Step:\n"
        << "Type:\n"
        << "Temperature:\n"
        << "Setpoint:\n"
        << "Profile time:\n"
        << "Step time:\n"
        << "Outputs:\n\n"

        << "1. Back\n"
        << "2. Stop\n";
}
    
    
void Tui::update_monitor() noexcept
{
    const auto& page = ui_.monitor_page();

    if (!monitor_cache_valid_ ||
        page.state != monitor_cache_.state)
    {
        move_cursor(
            MonitorLayout::StateRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << Furnace::state_name(page.state)
            << "        ";
    }
    
    if (!monitor_cache_valid_ ||
    page.step != monitor_cache_.step)
    {
        move_cursor(
            MonitorLayout::StepRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << page.step
            << "        ";
    }

  if (!monitor_cache_valid_ ||
    page.step_type != monitor_cache_.step_type)
    {
        move_cursor(
            MonitorLayout::TypeRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << Furnace::step_type_name(page.step_type)
            << "        ";
    }  
    
    if (!monitor_cache_valid_ ||
    page.temperature != monitor_cache_.temperature)
    {
        move_cursor(
            MonitorLayout::TemperatureRow,
            MonitorLayout::ValueColumn);

        std::cout
            << page.temperature
            << " C     ";
    }


    if (!monitor_cache_valid_ ||
        page.setpoint != monitor_cache_.setpoint)
    {
        move_cursor(
            MonitorLayout::SetpointRow,
            MonitorLayout::ValueColumn);

        std::cout
            << page.setpoint
            << " C     ";
    }


    if (!monitor_cache_valid_ ||
        page.profile_elapsed != monitor_cache_.profile_elapsed)
    {
        move_cursor(
            MonitorLayout::ProfileTimeRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << page.profile_elapsed
            << " s     ";
    }
    

    if (!monitor_cache_valid_ ||
    page.step_elapsed != monitor_cache_.step_elapsed)
    {
        move_cursor(
            MonitorLayout::StepTimeRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << page.step_elapsed
            << " s     ";
    }

    
    if (!monitor_cache_valid_ ||
    page.outputs != monitor_cache_.outputs)
    {
        move_cursor(
            MonitorLayout::OutputsRow,
            MonitorLayout::ValueColumn);
    
        std::cout
            << unsigned(page.outputs)
            << "        ";
    }    
    
    
    monitor_cache_ = page;
    monitor_cache_valid_ = true;
}
    
void Tui::process_monitor_input() noexcept
{
    char key;

    std::cin >> key;


    switch(key)
    {
    case '1':

        ui_.dispatch(
        {
            Ui::Event::Id::Back
        });

        break;


    case '2':

        ui_.dispatch(
        {
            Ui::Event::Id::Stop
        });

        break;


    default:

        break;
    }
}        

    
} // namespace app