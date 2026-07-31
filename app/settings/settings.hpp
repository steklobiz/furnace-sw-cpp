#pragma once

#include <cstdint>

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
    uint16_t buzzer_volume = 100;

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

private:

    Settings settings_;
};

} // namespace app