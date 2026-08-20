// profiles.cpp
#include "profiles.hpp"

namespace app {

const Profile&
ProfileManager::start_profile() const noexcept
{
    return start_profile_;
}


Profile&
ProfileManager::edit_profile() noexcept
{
    return edit_profile_;
}


const Profile&
ProfileManager::edit_profile() const noexcept
{
    return edit_profile_;
}

uint8_t
ProfileManager::start_profile_id() const noexcept
{
    return start_profile_id_;
}


uint8_t
ProfileManager::edit_profile_id() const noexcept
{
    return edit_profile_id_;
}

bool
ProfileManager::select_for_start(uint8_t profile_id) noexcept
{
    if (profile_id >= test_profile_count)
    {
        return false;
    }

    // TODO:
    // load profile from EEPROM

    start_profile_id_ = profile_id;
    
    start_profile_ = test_profiles[profile_id];
    
    notify(
        NotificationType::StartProfileChanged,
        profile_id);

    return true;
}


bool
ProfileManager::select_for_edit(uint8_t profile_id) noexcept
{
    if (profile_id >= test_profile_count)
    {
        return false;
    }

    // TODO:
    // load profile from EEPROM

    edit_profile_id_ = profile_id;
    
    edit_profile_ = test_profiles[profile_id];

    notify(
        NotificationType::EditProfileChanged,
        profile_id);

    return true;
}

bool
ProfileManager::save_edit() noexcept
{
    // TODO:
    // save edit_profile_ to EEPROM

    return true;
}

void
ProfileManager::clear_start_selection() noexcept
{
    start_profile_id_ = invalid_profile_id;
    start_profile_ = Profile{};

    notify(NotificationType::StartProfileChanged);
}

void
ProfileManager::set_notify_callback(
    NotificationCallback callback,
    void* context) noexcept
{
    notify_callback_ = callback;
    notify_context_ = context;
}

void
ProfileManager::notify(
    NotificationType type,
    uint16_t argument) noexcept
{
    if (notify_callback_ != nullptr)
    {
        notify_callback_(
            notify_context_,
            Notification{
                this,
                type,
                argument
            });
    }
}

} // namespace app