// tui.hpp
#pragma once

#include <cstdint>

#include "ui.hpp"

// TUI renderer presents the current UI page through a terminal interface.
// It obtains page fields from Ui, converts their native values into
// terminal representation, and tracks the last rendered version of
// each field independently from UI and DataAggregator state.

namespace app
{

class Tui
{
public:
    struct Label
    {
        const char* caption;
        uint8_t field;
    };

    void init(Ui& ui) noexcept;

    void process() noexcept;

private:

    static constexpr Label main_labels_[] =
    {
        {
            "Temperature:",
            static_cast<uint8_t>(Ui::MainField::Temperature)
        }
    };

    static constexpr Label monitor_labels_[] =
    {
        {
            "State:",
            static_cast<uint8_t>(Ui::MonitorField::State)
        },
        {
            "Step:",
            static_cast<uint8_t>(Ui::MonitorField::CurrentStep)
        },
        {
            "Temperature, C:",
            static_cast<uint8_t>(Ui::MonitorField::Temperature)
        }
    };
    
    void process_input() noexcept;

    void render_main() noexcept;
    void render_monitor() noexcept;

    void render_main_field(
        Ui::MainField field,
        const char* caption) noexcept;
    
    void render_monitor_field(
        Ui::MonitorField field,
        const char* caption) noexcept;
                
    Ui* ui_ = nullptr;

    static constexpr std::size_t MainFieldCount =
        static_cast<std::size_t>(Ui::MainField::Count);
    
    static constexpr std::size_t MonitorFieldCount =
        static_cast<std::size_t>(Ui::MonitorField::Count);
    
    uint8_t main_versions_[MainFieldCount]{};
    uint8_t monitor_versions_[MonitorFieldCount]{};

    bool main_initialized_ = false;
    bool monitor_initialized_ = false;
    Ui::Page rendered_page_ = Ui::Page::Main;
    bool page_initialized_ = false;
};

} // namespace app