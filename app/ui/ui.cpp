#include "ui.hpp"

namespace app
{

void Ui::init(
    Furnace& furnace,
    ProfileManager& profiles,
    SettingManager& settings,
    DataAggregator& data) noexcept
{
    furnace_ = &furnace;
    profiles_ = &profiles;
    settings_ = &settings;
    data_ = &data;

    temperature_version_ = data_->temperature().version;
    monitor_.temperature = data_->temperature().value;
}

void Ui::process() noexcept
{
    const auto& temperature = data_->temperature();

    if (temperature.version != temperature_version_)
    {
        monitor_.temperature = temperature.value;
        temperature_version_ = temperature.version;
    }
}

const Ui::Monitor& Ui::monitor() const noexcept
{
    return monitor_;
}

} // namespace app