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
};


class SettingManager
{
public:

    SettingManager() = default;

    // For reading
    const Settings& view() const noexcept;

    // For editing
    Settings& edit() noexcept;

    bool open() noexcept;

    bool save() noexcept;

    void reset_defaults() noexcept;


    void set_notify_callback(
        NotificationCallback callback,
        void* context) noexcept;
        
    // ui getters
    uint16_t get_pid_kp() const noexcept;
    uint16_t get_pid_ki() const noexcept;
    uint16_t get_pid_kd() const noexcept;
    uint16_t get_max_temperature_c() const noexcept;
    uint16_t get_buzzer_state() const noexcept;

    // ui setters    
    bool set_buzzer_state(uint16_t value) noexcept;
    bool set_pid_kp(uint16_t value) noexcept;
    bool set_pid_ki(uint16_t value) noexcept;
    bool set_pid_kd(uint16_t value) noexcept;
    bool set_max_temperature_c(uint16_t value) noexcept;
    
private:

    void settings_changed() noexcept;
    void notify(
        NotificationType type,
        uint16_t argument = 0) noexcept;
        
    Settings settings_;
    NotificationCallback notify_callback_ = nullptr;
    void* notify_context_ = nullptr;
    
};

} // namespace app