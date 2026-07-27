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

    // Main page
    void draw_main() noexcept;
    void update_main() noexcept;
    void process_main_input() noexcept;
    static const char* main_action_name(
        Ui::Button::Action action) noexcept;

    // Monitor page
    void draw_monitor() noexcept;
    void update_monitor() noexcept;
    void process_monitor_input() noexcept;

    // Profile select page
    void draw_profile_select() noexcept;
    void update_profile_select() noexcept;
    void process_profile_select_input() noexcept;    
    


    static const char* state_name(
        Furnace::State state) noexcept;

    static const char* step_type_name(
        Furnace::StepType type) noexcept;
               
    Ui::Event::Id event_from_action(
        Ui::Button::Action action) noexcept;
        
    void execute_action(
        Ui::Button::Action action) noexcept;
        
    void move_cursor(
        uint8_t row, uint8_t column) noexcept;
        
    static void clear_screen() noexcept;

        
private:

    struct MainLayout{};
    
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
    
    Ui::MonitorPage monitor_cache_;
    
    bool monitor_cache_valid_ = false;
    
    Ui& ui_;
    
};

} // namespace app