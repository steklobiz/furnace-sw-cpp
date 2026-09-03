#pragma once

#include <cstdint>
#include "notification.hpp"


namespace app
{

// TOD: 
// 1.split structure into areas:
// struct Settings
// {
//     ThermalSettings thermal;
//     UiSettings ui;
// };    
//
// usage:
// settings_.edit().thermal
// settings_.view().ui
//
// 2.Think aout adding:
// bool modified() const noexcept;
// void mark_modified() noexcept;    

struct Settings
{
    uint16_t buzzer_state = 1;

    uint16_t display_brightness = 100;

    uint16_t pid_kp = 100;
    uint16_t pid_ki = 20;
    uint16_t pid_kd = 50;

    uint16_t max_temperature_c = 1200;

    bool auto_start = false;

    bool beep_on_finish = true;
    
    uint8_t prestep_outs = 0;
};


class SettingManager
{
public:

    SettingManager() = default;

    // For reading
    const Settings& view() const noexcept;

    // For editing
    Settings& edit() noexcept;

    // Persistent settings
    bool open() noexcept;
    bool save() noexcept;

    // Settings editing transaction
    void begin_edit() noexcept;
    void cancel_edit() noexcept;
    
    // Restore default values
    void reset_defaults() noexcept;

    // Register for settings change notifications
    void set_notify_callback(
        NotificationCallback callback,
        void* context) noexcept;
        
    // Committed getters    
    uint16_t get_pid_kp() const noexcept;
    uint16_t get_pid_ki() const noexcept;
    uint16_t get_pid_kd() const noexcept;    
    uint16_t get_max_temperature() const noexcept;
    uint16_t get_buzzer_state() const noexcept;
    uint16_t get_prestep_outs() const noexcept;
        
    // Access the settings currently being edited. 
    uint16_t get_edit_pid_kp() const noexcept;
    uint16_t get_edit_pid_ki() const noexcept;
    uint16_t get_edit_pid_kd() const noexcept;
    uint16_t get_edit_max_temperature() const noexcept;
    uint16_t get_edit_buzzer_state() const noexcept;
    uint16_t get_edit_prestep_outs() const noexcept;
    
    // Modify the settings currently being edited.
    // Changes are committed only by save().    // ui setters    
    bool set_edit_pid_kp(uint16_t value) noexcept;
    bool set_edit_pid_ki(uint16_t value) noexcept;
    bool set_edit_pid_kd(uint16_t value) noexcept;
    bool set_edit_max_temperature(uint16_t value) noexcept;
    bool set_edit_buzzer_state(uint16_t value) noexcept;
    bool set_edit_prestep_outs(uint16_t value) noexcept;
    
    
private:

    void settings_changed() noexcept;
            
    Settings settings_{}; // committed settings
    Settings edit_settings_{}; // temporary editing copy
    
    NotificationCallback notify_callback_ = nullptr;
    void* notify_context_ = nullptr;
    
};

} // namespace app