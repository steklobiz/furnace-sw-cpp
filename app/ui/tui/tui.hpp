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
    
    struct Button
    {
        char key;
        const char* caption;
        Ui::Action action;
    };
    
    void init(Ui& ui) noexcept;

    void process() noexcept;

private:
    
    // Main page representation
    static constexpr Label main_labels_[] =
    {
        {
            "State:",
            static_cast<uint8_t>(Ui::MainField::State)
        },
        
        {
            "Profile",
            static_cast<uint8_t>(Ui::MainField::ProfileId)            
        },
        
        {
            "Temperature, C:",
            static_cast<uint8_t>(Ui::MainField::Temperature)
        }
    };

    static constexpr Button main_buttons_[] =
    {
        {
            's',
            "Start",
            {Ui::ActionType::StartProfileSelection}
        },
        {
            'e',
            "Edit",
            {Ui::ActionType::EditProfileSelection}
        }
    };

    // Monitor page representation
    static constexpr Label monitor_labels_[] =
    {
        {
            "State:",
            static_cast<uint8_t>(Ui::MonitorField::State)
        },
        {
            "Profile",
            static_cast<uint8_t>(Ui::MonitorField::ProfileId)            
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
    
    static constexpr Button monitor_buttons_[] =
    {
        {
            'x',
            "Stop",
            {Ui::ActionType::StopFurnace}
        }
    };    
    
    // Result page representation
    static constexpr Label result_labels_[] =
    {
        {
            "State:",
            static_cast<uint8_t>(Ui::ResultField::State)
        },

        {
            "Temperature, C:",
            static_cast<uint8_t>(Ui::ResultField::Temperature)
        }
    };    
    
    static constexpr Button result_buttons_[] =
    {
        {
            'r',
            "Reset",
           {Ui::ActionType::ResetFurnace}
        }
    };

        
    void process_input() noexcept;

    void render_main() noexcept;
    void render_monitor() noexcept;
    void render_result() noexcept;
    void render_profile_selection() noexcept;
                    
    Ui* ui_ = nullptr;

    static constexpr std::size_t MainFieldCount =
        static_cast<std::size_t>(Ui::MainField::Count);
    
    static constexpr std::size_t MonitorFieldCount =
        static_cast<std::size_t>(Ui::MonitorField::Count);

    static constexpr std::size_t ResultFieldCount =
        static_cast<std::size_t>(Ui::ResultField::Count);
        
    uint8_t main_versions_[MainFieldCount]{};
    uint8_t monitor_versions_[MonitorFieldCount]{};
    uint8_t result_versions_[ResultFieldCount]{};

    bool main_initialized_ = false;
    bool profile_selection_initialized_ = false;
    bool monitor_initialized_ = false;
    bool result_initialized_ = false; 
       
    Ui::Page rendered_page_ = Ui::Page::Main;
    bool page_initialized_ = false;
};

} // namespace app