// profiles.hpp
#pragma once

#include <array>
#include <cstdint>
#include "notification.hpp"

namespace app
{

struct Step
{
    uint16_t setpoint_c = 0;
    uint16_t duration = 0;
    uint8_t flags = 0;

    friend constexpr bool operator==(
        const Step& lhs,
        const Step& rhs) noexcept
    {
        return lhs.setpoint_c == rhs.setpoint_c
            && lhs.duration == rhs.duration
            && lhs.flags == rhs.flags;
    }

    friend constexpr bool operator!=(
        const Step& lhs,
        const Step& rhs) noexcept
    {
        return !(lhs == rhs);
    }
};


struct Profile
{
    static constexpr uint8_t MaxSteps = 16;

    std::array<Step, MaxSteps> steps{};
    
    friend bool operator==(
        const Profile& lhs,
        const Profile& rhs) noexcept
    {
        return lhs.steps == rhs.steps;
    }

    friend bool operator!=(
        const Profile& lhs,
        const Profile& rhs) noexcept
    {
        return !(lhs == rhs);
    }
};


class ProfileManager
{
public:

    ProfileManager() = default;

    // Runtime profile used by Furnace.
    const Profile& start_profile() const noexcept;

    // Profile currently being edited.
    Profile& edit_profile() noexcept;
    const Profile& edit_profile() const noexcept;

    bool load_for_start(uint8_t profile_id) noexcept;
    bool load_for_edit(uint8_t profile_id) noexcept;

    bool save_edit() noexcept;

    void set_notify_callback(NotificationCallback callback, void* context) noexcept;

private:
    
    void notify(NotificationType type,
            uint16_t argument = 0) noexcept;    

    Profile start_profile_;
    Profile edit_profile_;
        
    NotificationCallback notify_callback_ = nullptr;
    void* notify_context_ = nullptr;
};

}