// tui.cpp

#include "tui.hpp"

#include <cstdio>

namespace app
{

void Tui::init(Ui& ui) noexcept
{
    ui_ = &ui;
}


void Tui::process() noexcept
{
    switch (ui_->page())
    {
        case Ui::Page::Main:
            render_main();
            break;

        case Ui::Page::Monitor:
            render_monitor();
            break;
    }
}


void Tui::render_main() noexcept
{
    render_main_field(Ui::MainField::Temperature);
    
    main_initialized_ = true;
}


void Tui::render_monitor() noexcept
{
    render_monitor_field(Ui::MonitorField::State);
    render_monitor_field(Ui::MonitorField::CurrentStep);
    render_monitor_field(Ui::MonitorField::Temperature);
    
    monitor_initialized_ = true;
}


void Tui::render_main_field(
    Ui::MainField field) noexcept
{
    const auto& item = ui_->get_field(field);

    const std::size_t index =
        static_cast<std::size_t>(field);

    if (main_initialized_ &&
        main_versions_[index] == item.version)
    {
        return;
    }

    main_versions_[index] = item.version;

    switch (field)
    {
        case Ui::MainField::Temperature:
            std::printf(
                "Temperature: %u C\n",
                static_cast<unsigned>(item.value));
            break;

        case Ui::MainField::Count:
            break;
    }
}

void Tui::render_monitor_field(
    Ui::MonitorField field) noexcept
{
    const auto& item = ui_->get_field(field);

    const std::size_t index =
        static_cast<std::size_t>(field);

    if (monitor_initialized_ &&
        monitor_versions_[index] == item.version)
    {
        return;
    }

    monitor_versions_[index] = item.version;

    switch (field)
    {
        case Ui::MonitorField::State:
            std::printf(
                "State: %u\n",
                static_cast<unsigned>(item.value));
            break;

        case Ui::MonitorField::CurrentStep:
            std::printf(
                "Step: %u\n",
                static_cast<unsigned>(item.value));
            break;

        case Ui::MonitorField::Temperature:
            std::printf(
                "Temperature: %u C\n",
                static_cast<unsigned>(item.value));
            break;

        case Ui::MonitorField::Count:
            break;
    }
}

} // namespace app