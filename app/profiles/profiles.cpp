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
    if (profile_id == selected_profile_id_)
    {
        return true;
    }
    
    // TODO:
    // load profile from EEPROM
    
    selected_profile_id_ = profile_id;
    
    notify(NotificationType::ProfileChanged);
    
    return true;
}


bool
ProfileManager::save() noexcept
{
    // TODO:
    // save current_profile_ to EEPROM

    return true;
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