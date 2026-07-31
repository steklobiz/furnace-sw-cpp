// profiles.cpp
#include "profiles.hpp"

namespace app {

const Profile&
ProfileManager::view() const noexcept
{
    return current_profile_;
}


Profile&
ProfileManager::edit() noexcept
{
    return current_profile_;
}


uint8_t
ProfileManager::selected_id() const noexcept
{
    return selected_profile_id_;
}


bool
ProfileManager::open(uint8_t profile_id) noexcept
{
    selected_profile_id_ = profile_id;

    // TODO:
    // load profile from EEPROM

    return true;
}


bool
ProfileManager::save() noexcept
{
    // TODO:
    // save current_profile_ to EEPROM

    return true;
}

} // namespace app