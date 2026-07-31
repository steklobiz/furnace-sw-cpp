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

} // namespace app