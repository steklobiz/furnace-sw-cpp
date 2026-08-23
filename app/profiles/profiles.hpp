// profiles.hpp
#pragma once

#include <array>
#include <cstdint>
#include "notification.hpp"

// Profile data and profile management.
// Provides runtime and editing profile copies.
// Handles loading and saving profiles through persistent storage.
// Notifies consumers when the editing profile is loaded or changed.

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


inline constexpr uint8_t invalid_profile_id = 0xFF;

// Temporary test profiles.
// TODO: Remove when persistent profile storage is implemented.

inline constexpr Profile test_profiles[] =
{
    // Profile 0
    {
        {{
            {50,  10, 0x01},
            {50,  10, 0x02},
            {100, 10, 0x00},
            {100, 10, 0x00},
            {50,  10, 0x00},
        }}
    },

    // Profile 1
    {
        {{
            {100, 10, 0x01},
            {150, 20, 0x01},
            {200, 30, 0x02},
            {150, 20, 0x00},
            {100, 10, 0x00},
        }}
    },

    // Profile 2
    {
        {{
            {25,  5,  0x01},
            {50,  5,  0x01},
            {75,  5,  0x02},
            {100, 5,  0x02},
        }}
    }
};

inline constexpr std::size_t test_profile_count =
    sizeof(test_profiles) / sizeof(test_profiles[0]);



class ProfileManager
{
public:

    ProfileManager() = default;

    // Runtime profile used by Furnace.
    const Profile& start_profile() const noexcept;

    // Profile currently being edited.
    Profile& edit_profile() noexcept;
    const Profile& edit_profile() const noexcept;

    // UI getters (uint16_only)
    // Returns the selected profile ID for starting. 
    uint16_t start_profile_id() const noexcept; 
    // Returns the selected profile ID for editing. 
    uint16_t edit_profile_id() const noexcept;
    
    // Loads a profile for starting.
    bool select_for_start(uint8_t profile_id) noexcept;
    // Loads a profile for editing.
    bool select_for_edit(uint8_t profile_id) noexcept;

    // Saves the currently edited profile.
    bool save_edit() noexcept;

    // Clears the profile selected for starting. 
    void clear_start_selection() noexcept;
    
    void set_notify_callback(NotificationCallback callback, void* context) noexcept;

private:
    
    void notify(NotificationType type,
            uint16_t argument = 0) noexcept;    

    uint8_t start_profile_id_ = invalid_profile_id; 
    uint8_t edit_profile_id_ = invalid_profile_id;            
            
    Profile start_profile_{};
    Profile edit_profile_{};
        
    NotificationCallback notify_callback_ = nullptr;
    void* notify_context_ = nullptr;
};

}