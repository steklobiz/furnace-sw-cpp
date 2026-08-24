#include "settings.hpp"

namespace app
{

const Settings&
SettingManager::view() const noexcept
{
    return settings_;
}


Settings&
SettingManager::edit() noexcept
{
    return settings_;
}


bool
SettingManager::open() noexcept
{
    // TODO:
    // Load settings from EEPROM.

    return true;
}


bool
SettingManager::save() noexcept
{
    // TODO:
    // Save settings to EEPROM.

    return true;
}


void
SettingManager::reset_defaults() noexcept
{
    settings_ = Settings{};
}

uint16_t 
SettingManager::get_pid_kp() const noexcept
{
    return settings_.pid_kp;
}

uint16_t 
SettingManager::get_pid_ki() const noexcept
{
    return settings_.pid_ki;
}

uint16_t 
SettingManager::get_pid_kd() const noexcept
{
    return settings_.pid_kd;
}

uint16_t 
SettingManager::get_max_temperature_c() const noexcept
{
    return settings_.max_temperature_c;
}

uint16_t 
SettingManager::get_buzzer_state() const noexcept
{
    return settings_.buzzer_state ? 1 : 0;
}

} // namespace app