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
}


void Ui::process() noexcept
{
    // UI state and actions will be implemented here.
}


const DataItem<uint16_t>&
Ui::get_field(MainField field) const noexcept
{
    const auto& mapping =
        main_fields_[static_cast<std::size_t>(field)];

    return data_->get_item(
        mapping.source,
        mapping.field);
}

const DataItem<uint16_t>&
Ui::get_field(MonitorField field) const noexcept
{
    const auto& mapping =
        monitor_fields_[static_cast<std::size_t>(field)];

    return data_->get_item(
        mapping.source,
        mapping.field);
}

Ui::Page Ui::page() const noexcept
{
    return page_;
}

} // namespace app