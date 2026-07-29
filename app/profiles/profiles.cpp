#include "profiles.hpp"

namespace app
{

const Profile&
Profiles::view() const noexcept
{
    return current_profile_;
}


Profile&
Profiles::edit() noexcept
{
    return current_profile_;
}


uint8_t
Profiles::selected_id() const noexcept
{
    return selected_profile_id_;
}


bool
Profiles::open(
    uint8_t profile_id) noexcept
{
    selected_profile_id_ = profile_id;

    // TODO:
    // load profile from EEPROM

    return true;
}


bool
Profiles::save() noexcept
{
    // TODO:
    // save current_profile_ to EEPROM

    return true;
}

}