#pragma once

#include <cstdint>

#include "data_aggregator.hpp"
#include "furnace.hpp"
#include "profiles.hpp"
#include "settings.hpp"

namespace app
{

class Ui
{
public:
    struct Monitor
    {
        int16_t temperature = 0;
    };

    void init(
        Furnace& furnace,
        ProfileManager& profiles,
        SettingManager& settings,
        DataAggregator& data) noexcept;

    void process() noexcept;

    const Monitor& monitor() const noexcept;

private:
    Furnace* furnace_ = nullptr;
    ProfileManager* profiles_ = nullptr;
    SettingManager* settings_ = nullptr;
    DataAggregator* data_ = nullptr;

    Monitor monitor_{};
    uint8_t temperature_version_ = 0;
};

} // namespace app