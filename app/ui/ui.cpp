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
    {DataSource::TcParser, static_cast<uint8_t>(TcParserItem::Temperature)}
};


static constexpr Ui::FieldMapping monitor_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::State)},
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Step)},
    {DataSource::TcParser, static_cast<uint8_t>(TcParserItem::Temperature)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Setpoint)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::StepElapsed)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::ProfileElapsed)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Power)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs)}
};


static constexpr Ui::FieldMapping result_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::State)}
};


static constexpr Ui::PageDescriptor page_descriptors[] =
{
    {main_fields,    std::size(main_fields)},
    {nullptr,         0},
    {nullptr,         0},
    {monitor_fields, std::size(monitor_fields)},
    {result_fields,  std::size(result_fields)}
};

} // namespace


const DataItem<uint16_t> Ui::null_item_{};


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


const DataItem<uint16_t>&
Ui::get_field(
    Page page,
    uint8_t field) const noexcept
{
    const auto page_index =
        static_cast<std::size_t>(page);

    if (page_index >=
        static_cast<std::size_t>(Page::Count))
    {
        return null_item_;
    }

const auto& descriptor =
    page_descriptors[page_index];
    
    if (descriptor.fields == nullptr ||
        field >= descriptor.field_count)
    {
        return null_item_;
    }

    const auto& mapping =
        descriptor.fields[field];

    return data_->get_item(
        static_cast<uint8_t>(mapping.source),
        mapping.field);
}


const DataItem<Profile>&
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
    switch (action.type)
    {
        case ActionType::StartProfileSelection:
            start_profile_selection();
            break;

        case ActionType::EditProfileSelection:
            edit_profile_selection();
            break;
            
        case ActionType::SelectProfile:
            select_profile(action.argument);
            break;
            
        case ActionType::StartProfileConfirm:
            confirm_start_profile(
                static_cast<uint8_t>(action.argument));
            break;

        case ActionType::EditProfileConfirm:
            confirm_edit_profile(
                static_cast<uint8_t>(action.argument));
            break;

        case ActionType::NextStep:
            next_step();
            break;

        case ActionType::PreviousStep:
            previous_step();
            break;

        case ActionType::SaveProfile:
            save_profile();
            break;

        case ActionType::CancelProfile:
            cancel_profile();
            break;

        case ActionType::Back:
            back();
            break;

        case ActionType::None:
        case ActionType::EditSetpoint:
        case ActionType::EditDuration:
        case ActionType::EditFlags:
        case ActionType::StopFurnace:
        case ActionType::ResetFurnace:
            break;
    }
}


void Ui::start_profile_selection() noexcept
{
    profile_selection_mode_ =
        ProfileSelectionMode::Start;

    page_ = Page::ProfileSelection;
}


void Ui::edit_profile_selection() noexcept
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

void Ui::confirm_start_profile(uint8_t profile_id) noexcept
{
    if (!profiles_->select_for_start(profile_id))
        return;

    furnace_->start();
    page_ = Page::Monitor;
}


void Ui::confirm_edit_profile(uint8_t profile_id) noexcept
{
    if (!profiles_->select_for_edit(profile_id))
        return;

    current_step_ = 0;
    page_ = Page::ProfileEditor;
}


void Ui::next_step() noexcept
{
    const auto& profile =
        data_->profile().value;

    if (static_cast<std::size_t>(current_step_) + 1 <
        profile.steps.size())
    {
        ++current_step_;
    }
}

void Ui::previous_step() noexcept
{
    if (current_step_ > 0)
        --current_step_;
}


void Ui::save_profile() noexcept
{
    profiles_->save_edit();
    page_ = Page::Main;
}


void Ui::cancel_profile() noexcept
{
    page_ = Page::ProfileSelection;
}


void Ui::back() noexcept
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