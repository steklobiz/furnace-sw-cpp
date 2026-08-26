// ui.cpp

#include "ui.hpp"

namespace app
{

namespace
{
    
static constexpr Ui::FieldMapping main_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::State)},
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature)}
};


static constexpr Ui::FieldMapping monitor_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::State)},
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Step)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::StepType)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Setpoint)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::StepElapsed)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::ProfileElapsed)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Power)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs)}
};


static constexpr Ui::FieldMapping result_fields[] =
{
    {DataSource::Furnace,static_cast<uint8_t>(FurnaceItem::State)},
    {DataSource::Furnace,static_cast<uint8_t>(FurnaceItem::Temperature)}
};

static constexpr Ui::PageDescriptor page_descriptors[] =
{
    {main_fields,    std::size(main_fields)},    // Main page
    {nullptr,         0},    // ProfileSelection page
    {nullptr,         0},    // ProfileEditor page
    {monitor_fields, std::size(monitor_fields)},    // Monitor page
    {result_fields,  std::size(result_fields)}    // Result page
};

} // namespace


void Ui::init(
    DataAggregator& data,
    ProfileManager& profiles,
    Furnace& furnace) noexcept
{
    data_ = &data;
    profiles_ = &profiles;
    furnace_ = &furnace;

    page_ = Page::Main;
    current_step_ = 0;
}

Ui::Page Ui::page() const noexcept
{
    return page_;
}


    uint16_t Ui::get_field(
        Ui::Page page,
        uint8_t field) const noexcept    
    {
    const auto page_index =
        static_cast<std::size_t>(page);

    if (page_index >= static_cast<std::size_t>(Page::Count))
        return 0;

    const auto& descriptor =
        page_descriptors[page_index];

    if (descriptor.fields == nullptr ||
        field >= descriptor.field_count)
    {
        return 0;
    }

    const auto& mapping =
        descriptor.fields[field];

    return data_->get_item(
        static_cast<uint8_t>(mapping.source),
        mapping.field);
}

const Profile&
Ui::get_edit_profile() const noexcept
{
    return data_->profile();
}


uint8_t Ui::current_step() const noexcept
{
    return current_step_;
}


void Ui::execute(Action action) noexcept
{
    for (const auto& mapping : action_mapping)
    {
        if (mapping.type == action.type)
        {
            (this->*mapping.callback)(action.argument);
            return;
        }
    }
}

void Ui::start_profile_selection(uint16_t) noexcept
{
    profile_selection_mode_ =
        ProfileSelectionMode::Start;

    page_ = Page::ProfileSelection;
}


void Ui::edit_profile_selection(uint16_t) noexcept
{
    profile_selection_mode_ =
        ProfileSelectionMode::Edit;

    page_ = Page::ProfileSelection;
}

void Ui::select_profile(uint16_t id) noexcept
{
    const auto profile_id =
        static_cast<uint8_t>(id);

    if (profile_selection_mode_ ==
        ProfileSelectionMode::Start)
    {
        confirm_start_profile(profile_id);
    }
    else
    {
        confirm_edit_profile(profile_id);
    }
}

void Ui::confirm_start_profile(uint16_t profile_id) noexcept
{
    if (!profiles_->select_for_start(profile_id))
        return;

    furnace_->start();
    page_ = Page::Monitor;
}


void Ui::confirm_edit_profile(uint16_t profile_id) noexcept
{
    if (!profiles_->select_for_edit(profile_id))
        return;

    current_step_ = 0;
    page_ = Page::ProfileEditor;
}


void Ui::next_step(uint16_t) noexcept
{
    const auto& profile =
        data_->profile();

    if (static_cast<std::size_t>(current_step_) + 1 <
        profile.steps.size())
    {
        ++current_step_;
    }
}

void Ui::previous_step(uint16_t) noexcept
{
    if (current_step_ > 0)
        --current_step_;
}

void Ui::edit_setpoint(uint16_t value) noexcept
{
    profiles_->set_edit_setpoint(
        current_step_,
        value);
}

void Ui::edit_duration(uint16_t value) noexcept
{
    profiles_->set_edit_duration(
        current_step_,
        value);
}

void Ui::edit_flags(uint16_t value) noexcept
{
    profiles_->set_edit_flags(
        current_step_,
        value);
}

void Ui::save_profile(uint16_t) noexcept
{
    profiles_->save_edit();
    page_ = Page::Main;
}


void Ui::cancel_profile(uint16_t) noexcept
{
    page_ = Page::ProfileSelection;
}

void Ui::stop_furnace(uint16_t) noexcept
{
    furnace_->stop();
    page_ = Page::Result;
}

void Ui::reset_furnace(uint16_t) noexcept
{
    furnace_->reset();
    page_ = Page::Main;
}

void Ui::back(uint16_t) noexcept
{
    switch (page_)
    {
        case Page::ProfileSelection:
            page_ = Page::Main;
            break;

        case Page::ProfileEditor:
            page_ = Page::ProfileSelection;
            break;

        case Page::Monitor:
            page_ = Page::Main;
            break;

        case Page::Result:
            page_ = Page::Main;
            break;

        case Page::Main:
        case Page::Count:
            break;
    }
}

} // namespace app