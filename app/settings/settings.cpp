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
    return edit_settings_;
}

bool
SettingManager::open() noexcept
{
    // TODO:
    // Load settings from EEPROM.

    edit_settings_ = settings_;
     
    return true;
}


bool
SettingManager::save() noexcept
{
    settings_ = edit_settings_;

    // TODO:
    // Save settings_ to EEPROM.

    settings_changed();

    return true;
}


void
SettingManager::begin_edit() noexcept
{
    edit_settings_ = settings_;
}

void
SettingManager::cancel_edit() noexcept
{
    edit_settings_ = settings_;
}

void
SettingManager::reset_defaults() noexcept
{
    settings_ = Settings{};
    edit_settings_ = settings_;
}

void SettingManager::set_notify_callback(
    NotificationCallback callback,
    void* context) noexcept
{
    notify_callback_ = callback;
    notify_context_ = context;
}

// -----------------------------------------------------------------------------
// Committed settings
// -----------------------------------------------------------------------------

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
SettingManager::get_max_temperature() const noexcept
{
    return settings_.max_temperature_c;
}

uint16_t 
SettingManager::get_buzzer_state() const noexcept
{
    return settings_.buzzer_state ? 1 : 0;
}

uint16_t 
SettingManager::get_prestep_outs() const noexcept
{
    return settings_.prestep_outs;
}

// -----------------------------------------------------------------------------
// Edit settings access
// -----------------------------------------------------------------------------

uint16_t 
SettingManager::get_edit_pid_kp() const noexcept
{
    return edit_settings_.pid_kp;
}

uint16_t 
SettingManager::get_edit_pid_ki() const noexcept
{
    return edit_settings_.pid_ki;
}

uint16_t 
SettingManager::get_edit_pid_kd() const noexcept
{
    return edit_settings_.pid_kd;
}

uint16_t 
SettingManager::get_edit_max_temperature() const noexcept
{
    return edit_settings_.max_temperature_c;
}

uint16_t 
SettingManager::get_edit_buzzer_state() const noexcept
{
    return edit_settings_.buzzer_state ? 1 : 0;
}

uint16_t
SettingManager::get_edit_prestep_outs() const noexcept
{
    return edit_settings_.prestep_outs;
}

// -----------------------------------------------------------------------------
// Edit settings modification
// -----------------------------------------------------------------------------

bool
SettingManager::set_edit_pid_kp(uint16_t value) noexcept
{
    edit_settings_.pid_kp = value;
    return true;
}


bool
SettingManager::set_edit_pid_ki(uint16_t value) noexcept
{
    edit_settings_.pid_ki = value;
    return true;
}

bool
SettingManager::set_edit_pid_kd(uint16_t value) noexcept
{
    edit_settings_.pid_kd = value;
    return true;
}

bool
SettingManager::set_edit_max_temperature(uint16_t value) noexcept
{
    edit_settings_.max_temperature_c = value;
    return true;
}

bool
SettingManager::set_edit_buzzer_state(uint16_t value) noexcept
{
    if (value > 1)
        return false;

    edit_settings_.buzzer_state = value;
    return true;
}

bool
SettingManager::set_edit_prestep_outs(uint16_t value) noexcept
{
    if (value > 0xFF)
        return false;

    edit_settings_.prestep_outs = value;
    return true;
}

// -----------------------------------------------------------------------------
// Private helpers
// -----------------------------------------------------------------------------

void
SettingManager::settings_changed() noexcept
{
    if (notify_callback_ != nullptr)
    {
        notify_callback_(
            notify_context_,
            Notification{
                this,
                NotificationType::SettingsChanged,
                0
            });
    }
}


} // namespace app