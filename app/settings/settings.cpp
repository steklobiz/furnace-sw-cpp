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

bool SettingManager::set_pid_kp(uint16_t value) noexcept
{
    settings_.pid_kp = value;
    settings_changed();
    return true;
}

bool SettingManager::set_pid_kp(uint16_t value) noexcept
{
    settings_.pid_kp = value;
    settings_changed();

    return true;
} 

bool SettingManager::set_pid_ki(uint16_t value) noexcept
{
    settings_.pid_ki = value;
    settings_changed();

    return true;
}

bool SettingManager::set_pid_kd(uint16_t value) noexcept
{
    settings_.pid_kd = value;
    settings_changed();

    return true;
}

bool SettingManager::set_max_temperature_c(uint16_t value) noexcept
{
    settings_.max_temperature_c = value;
    settings_changed();

    return true;
}

bool SettingManager::set_buzzer_state(uint16_t value) noexcept
{
    if (value > 1)
        return false;

    settings_.buzzer_state = value;
    settings_changed();

    return true;
}

void SettingManager::settings_changed() noexcept
{
    if (notify_callback_)
    {
        notify_callback_(
            notify_context_,
            NotificationType::SettingsChanged,
            0);
    }
}

} // namespace app