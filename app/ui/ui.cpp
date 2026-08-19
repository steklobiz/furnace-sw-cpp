// ui.cpp
//
// UI state and field mapping layer.
//
// Role in the architecture:
//   - Ui is the application-facing facade over the data available to
//     the UI. It holds the currently active page and maps logical
//     page fields (MainField / MonitorField) to concrete items owned
//     by the DataAggregator.
//   - Renderers (such as Tui) never talk to DataAggregator directly.
//     They ask Ui for a field by its logical name and receive the
//     matching DataItem reference (value + version).
//   - Field->source resolution is defined statically in the
//     main_fields_[] and monitor_fields_[] and result_fields_[] tables 
//     in ui.hpp: each entry stores a DataSource id and a field id within 
//     that source.

#include "ui.hpp"

namespace app
{

// Binds the UI to the application modules it needs.
// Must be called once before process()/get_field(); the pointers stay
// valid for the lifetime of all involved objects.
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


// Updates UI state and processes pending UI actions.
// Called periodically by the application. Placeholder for
// page switching and user-action handling; renderers poll page() and
// get_field() independently.
void Ui::process() noexcept
{
    // UI state and actions will be implemented here.
}


// Returns the current data item for a Main-page field.
// The returned item contains both the current value and its version.            
const DataItem<uint16_t>&
Ui::get_field(MainField field) const noexcept
{
    const auto& mapping =
        main_fields_[static_cast<std::size_t>(field)];

    // Delegate to the aggregator. The returned item is shared
    // (reference) and its version increments on every value change,
    // which renderers use to detect "publish on change".
    return data_->get_item(
        mapping.source,
        mapping.field);
}

// Returns the current data item for a Monitor-page field.
// Same mechanism as the Main-field overload, using monitor_fields_[].
const DataItem<uint16_t>&
Ui::get_field(MonitorField field) const noexcept
{
    const auto& mapping =
        monitor_fields_[static_cast<std::size_t>(field)];

    return data_->get_item(
        mapping.source,
        mapping.field);
}

// Returns the current data item for a Result-page field.
// Same mechanism as the Main-field overload, using result_fields_[].
const DataItem<uint16_t>&
Ui::get_field(ResultField field) const noexcept
{
    const auto& mapping =
        result_fields_[static_cast<std::size_t>(field)];

    return data_->get_item(
        mapping.source,
        mapping.field);
}



// Returns the currently active page. Renderers use this to decide
// which page to draw on each cycle.
Ui::Page Ui::page() const noexcept
{
    return page_;
}


void Ui::execute(const Action& action) noexcept
{
    switch (action.type)
    {
        case ActionType::StartProfileSelection:
            profile_selection_action_ =
                ActionType::StartProfileConfirm;
            page_ = Page::ProfileSelection;
            break;
        
        case ActionType::EditProfileSelection:
            profile_selection_action_ =
                ActionType::EditProfileConfirm;
            page_ = Page::ProfileSelection;
            break;
    
        case ActionType::StartProfileConfirm:
            profiles_->open(
                static_cast<uint8_t>(action.argument));

            furnace_->start();
            page_ = Page::Monitor;
            break;

        case ActionType::EditProfileConfirm:
            profiles_->open(
                static_cast<uint8_t>(action.argument));

            // Profile editor will be added later.
            // For now return to Main.
            page_ = Page::Main;
            break;

        case ActionType::StopFurnace:
            furnace_->stop();
            page_ = Page::Result;
            break;

        case ActionType::ResetFurnace:
            furnace_->reset();
            page_ = Page::Main;
            break;

        case ActionType::Back:
            page_ = Page::Main;
            break;

        default:
            break;
    }
}

} // namespace app