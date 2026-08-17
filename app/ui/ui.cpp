// ui.cpp

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
Ui::get_field(MonitorField field) const noexcept
{
    switch (field)
    {
        case MonitorField::Temperature:
            return data_->get_item(TcParserItem::Temperature);
    }

    // Unreachable for a valid MonitorField.
    return data_->get_item(TcParserItem::Temperature);
}

} // namespace app