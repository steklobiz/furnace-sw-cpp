#include <cstdint>
#include <iostream>
#include <array>
#include "tui.hpp"


namespace app
{
    
namespace 
{ 
        
struct Field
{
    uint8_t row;
    uint8_t column;
    uint8_t width;
};    
  
constexpr std::size_t index(
    Ui::MonitorField field) noexcept
{
    return static_cast<std::size_t>(field);
}

static constexpr std::array<
    Field,
    index(Ui::MonitorField::Count)>
monitor_layout_
{
    Field{5,18,10},   // State
    Field{6,18,4},    // Step
    Field{7,18,10},   // StepType
    Field{8,18,6},    // Temperature
    Field{9,18,6},    // Setpoint
    Field{10,18,8},   // ProfileElapsed
    Field{11,18,8},   // StepElapsed
    Field{12,18,4}    // Outputs
};
  
constexpr auto const& layout(
    Ui::MonitorField field) noexcept
{
    return monitor_layout_[index(field)];
}
         
} // anonymus namespace
   
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
    for (std::size_t i = 0;
         i < static_cast<std::size_t>(Ui::MonitorField::Count);
         ++i)
    {
        const auto field =
            static_cast<Ui::MonitorField>(i);

        if (monitor_field_changed(field))
        {
            draw_monitor_field(field);
        }
    }

    monitor_cache_ = ui_.monitor_page();

    monitor_cache_initialized_ = true;
    
    move_cursor(20, 1);
}

void Tui::draw_monitor_field(
    Ui::MonitorField field) noexcept
{
    const auto& page = ui_.monitor_page();

    clear_field(field);

    switch (field)
    {
    case Ui::MonitorField::State:

        std::cout
            << Furnace::state_name(
                   page.state);

        break;


    case Ui::MonitorField::Step:

        std::cout
            << page.step;

        break;


    case Ui::MonitorField::StepType:

        std::cout
            << Furnace::step_type_name(
                   page.step_type);

        break;


    case Ui::MonitorField::Temperature:

        std::cout
            << page.temperature
            << " C";

        break;


    case Ui::MonitorField::Setpoint:

        std::cout
            << page.setpoint
            << " C";

        break;


    case Ui::MonitorField::ProfileElapsed:

        std::cout
            << page.profile_elapsed
            << " s";

        break;


    case Ui::MonitorField::StepElapsed:

        std::cout
            << page.step_elapsed
            << " s";

        break;


    case Ui::MonitorField::Outputs:

        std::cout
            << static_cast<unsigned>(
                   page.outputs);

        break;


    default:

        break;
    }
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

void Tui::clear_field(
    Ui::MonitorField field) noexcept
{
    const auto& f = layout(field);

    move_cursor(
        f.row,
        f.column);

    for (uint8_t i = 0; i < f.width; ++i)
    {
        std::cout << ' ';
    }

    move_cursor(
        f.row,
        f.column);
}

bool Tui::monitor_field_changed(
    Ui::MonitorField field) const noexcept
{
    const auto& page = ui_.monitor_page();

    if (!monitor_cache_initialized_)
        return true;

    switch (field)
    {
    case Ui::MonitorField::State:
        return page.state != monitor_cache_.state;

    case Ui::MonitorField::Step:
        return page.step != monitor_cache_.step;

    case Ui::MonitorField::StepType:
        return page.step_type != monitor_cache_.step_type;

    case Ui::MonitorField::Temperature:
        return page.temperature != monitor_cache_.temperature;

    case Ui::MonitorField::Setpoint:
        return page.setpoint != monitor_cache_.setpoint;

    case Ui::MonitorField::ProfileElapsed:
        return page.profile_elapsed != monitor_cache_.profile_elapsed;

    case Ui::MonitorField::StepElapsed:
        return page.step_elapsed != monitor_cache_.step_elapsed;

    case Ui::MonitorField::Outputs:
        return page.outputs != monitor_cache_.outputs;

    default:
        return false;
    }
}
    
} // namespace app