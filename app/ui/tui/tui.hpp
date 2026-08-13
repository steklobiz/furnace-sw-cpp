// tui.hpp
#pragma once

#include "ui.hpp"
#include "trace.hpp"
#include <iostream> // for cout

namespace app
{

class Tui
{
public:

    Tui() noexcept = default;

    void init(
        Ui& ui,
        platform::trace::Trace<100, 100>& trace) noexcept;
                
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
    // Debug trace
    //------------------------------------------------------ 

    void draw_trace() noexcept;
    void update_trace() noexcept;
    void process_trace_input() noexcept;
        
        
    //------------------------------------------------------
    // Profile select page functions
    //------------------------------------------------------

    void draw_profile_select() noexcept;
    void update_profile_select() noexcept;
    void process_profile_select_input() noexcept;    
    
    //------------------------------------------------------
    // Result page functions
    //------------------------------------------------------
    
    void draw_result() noexcept;
    void update_result() noexcept;
    void process_result_input() noexcept;
    
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
          
    bool read_key(char& key) noexcept;     
    
            
private:
    
    using Trace = platform::trace::Trace<100, 100>;    

    enum class DebugPage : uint8_t
    {
        None,
        Trace
    };

    struct MainLayout
    {
        static constexpr uint8_t ValueColumn = 14;
    
        static constexpr uint8_t StateRow       = 5;
        static constexpr uint8_t TemperatureRow = 6;
        static constexpr uint8_t ProfileIdRow   = 7;
    
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
        static constexpr uint8_t PowerRow         = 12;
        static constexpr uint8_t OutputsRow       = 13;
    };

    struct TraceLayout
    {
        static constexpr uint8_t FurnaceStartRow = 6;
        static constexpr uint8_t FurnaceRows = 10;
    
        static constexpr uint8_t PidStartRow =
            FurnaceStartRow + FurnaceRows + 2;
    
        static constexpr uint8_t PidRows = 10;
    };

    struct ProfileSelectLayout{}; // ???
    
    Ui::MainPage main_cache_;
    bool main_cache_initialized_ = false;
    
    Ui::MonitorPage monitor_cache_;
    bool monitor_cache_initialized_ = false;
    
    DebugPage debug_page_ = DebugPage::None;    
    
    Ui* ui_ = nullptr;
    Trace* trace_ = nullptr;
    
};

} // namespace app