#pragma once

#include <array>
#include <cstdint>

namespace app
{

struct Step
{
    uint16_t setpoint_c = 0;

    uint16_t duration = 0;

    uint8_t flags = 0;
};


struct Profile
{
    static constexpr uint8_t MaxSteps = 16;

    std::array<Step, MaxSteps> steps{};
};


class Profiles
{
public:

    Profiles() = default;


    const Profile& current() const noexcept;

    Profile& current() noexcept;


    uint8_t selected() const noexcept;


    bool open(
        uint8_t profile_id) noexcept;


    bool save() noexcept;


private:

    uint8_t selected_profile_id_ = 0;

    Profile current_profile_;
};

}