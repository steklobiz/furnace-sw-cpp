// tui_trace.hpp
#include "tui.hpp"

namespace app
{

void Tui::draw_trace() noexcept
{
    clear_screen();

    std::cout
        << "========================\n"
        << "         TRACE\n"
        << "========================\n\n"

        << "----- FURNACE TRACE -----\n";

    for (uint8_t i = 0; i < TraceLayout::FurnaceRows; ++i)
    {
        std::cout << '\n';
    }

    std::cout
        << "\n----- PID TRACE -----\n";

    for (uint8_t i = 0; i < TraceLayout::PidRows; ++i)
    {
        std::cout << '\n';
    }

    std::cout
        << "\nPress any key to return\n";
}


void Tui::process_trace_input() noexcept
{
    char key;

    if (!read_key(key))
    {
        return;
    }

    debug_page_ = DebugPage::None;

    draw_main();

    main_cache_initialized_ = false;
}

void Tui::update_trace() noexcept
{
    const auto& furnace_trace = trace_->furnace();

    for (uint8_t i = 0;
         i < TraceLayout::FurnaceRows && i < furnace_trace.size();
         ++i)
    {
        const auto& sample = furnace_trace.from_newest(i);

    move_cursor(
        TraceLayout::FurnaceStartRow + i,
        0);
    
        std::cout
            << "time=" << sample.time_s
            << " temp=" << sample.temperature
            << " sp=" << sample.setpoint
            << " out=" << static_cast<int>(sample.output)
            << "                    ";
    }

    const auto& pid_trace = trace_->pid();

    const uint8_t pid_start_row =
        TraceLayout::PidStartRow;
    
    for (uint8_t i = 0;
         i < TraceLayout::PidRows && i < pid_trace.size();
         ++i)
    {
        const auto& sample = pid_trace.from_newest(i);

        move_cursor(
            pid_start_row + i,
            0);

        std::cout
            << "time=" << sample.time_s
            << " sp=" << sample.setpoint
            << " meas=" << sample.measurement
            << " err=" << sample.error
            << " p=" << sample.p
            << " i=" << sample.i
            << " d=" << sample.d
            << " out=" << sample.output
            << "                    ";
    }
}


}

