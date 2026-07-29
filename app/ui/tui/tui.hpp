#pragma once

#include "ui.hpp"

namespace app
{

class Tui
{
public:

    explicit Tui(Ui& ui);

    void process() noexcept;
    
private:

    void render() noexcept;
    void process_input() noexcept; 

    //------------------------------------------------------
    // Main page functions
    //------------------------------------------------------

    void draw_main() noexcept;
    void update_main() noexcept;
    void process_main_input() noexcept;    
    
    void draw_main_field(
        Ui::MainField field) noexcept;
        
    bool main_field_changed(
        Ui::MainField field) const noexcept;
    
    static const char* main_action_name(
        Ui::Button::Action action) noexcept;

    //------------------------------------------------------
    // Monitor page functions
    //------------------------------------------------------
        
    void draw_monitor() noexcept;
    void update_monitor() noexcept;
    void process_monitor_input() noexcept;
    
    void draw_monitor_field(
        Ui::MonitorField field) noexcept;    
        
    bool monitor_field_changed(
        Ui::MonitorField field) const noexcept;
                    
    //------------------------------------------------------
    // Profile select page functions
    //------------------------------------------------------

    void draw_profile_select() noexcept;
    void update_profile_select() noexcept;
    void process_profile_select_input() noexcept;    
    
    //------------------------------------------------------
    // Common helper functions
    //------------------------------------------------------
    
    static const char* step_type_name(
        Furnace::StepType type) noexcept;
                       
    void execute_action(
        Ui::Button::Action action) noexcept;
        
    void move_cursor(
        uint8_t row, uint8_t column) noexcept;
        
    static void clear_screen() noexcept;
    
    void clear_field(
        Ui::MonitorField field) noexcept;
                
    bool buttons_changed(
        const Ui::MainPage& page) const noexcept;    
        
private:

    struct MainLayout
    {
        static constexpr uint8_t ValueColumn = 14;
    
        static constexpr uint8_t StateRow       = 5;
        static constexpr uint8_t TemperatureRow = 6;
    
        static constexpr uint8_t ButtonsRow    = 9;
    };
        
    struct MonitorLayout
    {
        static constexpr uint8_t ValueColumn = 18;
    
        static constexpr uint8_t StateRow         = 5;
        static constexpr uint8_t StepRow          = 6;
        static constexpr uint8_t TypeRow          = 7;
        static constexpr uint8_t TemperatureRow   = 8;
        static constexpr uint8_t SetpointRow      = 9;
        static constexpr uint8_t ProfileTimeRow   = 10;
        static constexpr uint8_t StepTimeRow      = 11;
        static constexpr uint8_t OutputsRow       = 12;
    };

    struct ProfileSelectLayout{}; // ???
    
    Ui::MainPage main_cache_;
    bool main_cache_initialized_ = false;
    
    Ui::MonitorPage monitor_cache_;
    bool monitor_cache_initialized_ = false;
    
    Ui& ui_;
    
};

} // namespace app