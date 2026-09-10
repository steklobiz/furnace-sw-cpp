// ui.cpp

#include "ui.hpp"

namespace app
{

namespace
{
    
constexpr Ui::FieldMapping main_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::State)},
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature)}
};


constexpr Ui::FieldMapping monitor_fields[] =
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

constexpr Ui::FieldMapping settings_fields[] =
{
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::Buzzer)},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PidKp)},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PidKi)},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PidKd)},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::MaxTemperature)},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PrestepOuts)}
};

constexpr Ui::FieldMapping result_fields[] =
{
    {DataSource::Furnace,static_cast<uint8_t>(FurnaceItem::State)},
    {DataSource::Furnace,static_cast<uint8_t>(FurnaceItem::Temperature)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs)}
};

constexpr Ui::PageDescriptor page_descriptors[] =
{
    {main_fields,    std::size(main_fields)},       // Main page
    {nullptr,        0},                            // ProfileSelection page
    {settings_fields, std::size(settings_fields)},  // Settings page
    {nullptr,        0},                            // ProfileEditor page
    {monitor_fields, std::size(monitor_fields)},    // Monitor page
    {result_fields,  std::size(result_fields)},     // Result page
    {nullptr,        0},                            // Events page
    {nullptr,        0},                            // Samples page
    {nullptr,        0}                             // Question page
};

} // namespace


void Ui::init(
    DataAggregator& data,
    Furnace& furnace,
    ProfileManager& profiles,
    SettingManager& settings) noexcept
{
    data_ = &data;
    profiles_ = &profiles;
    settings_ = &settings;
    furnace_ = &furnace;

    page_ = Page::Main;
    current_step_ = 0;
}

void Ui::process() noexcept
{
    switch (furnace_->state())
    {
    case static_cast<uint16_t>(Furnace::State::Waiting):
    case static_cast<uint16_t>(Furnace::State::Finished):
    case static_cast<uint16_t>(Furnace::State::Stopped):
    case static_cast<uint16_t>(Furnace::State::Error):

        page_ = Page::Result;
        break;

    default:

        break;
    }
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


Ui::Page Ui::page() const noexcept
{
    return page_;
}


bool Ui::get_field(
            const Page page,
            const uint8_t field,
            uint16_t& value) const noexcept
{
    const auto page_index =
        static_cast<std::size_t>(page);

    if (page_index >= std::size(page_descriptors))
        return false;

    const auto& descriptor = page_descriptors[page_index];

    if (field >= descriptor.field_count)
        return false;

    const auto& mapping = descriptor.fields[field];

    switch (mapping.source)
    {
        case DataSource::TcParser:
            value = data_->tc_parser_item(
                static_cast<TcParserItem>(mapping.field));
            return true;

        case DataSource::Furnace:
            value = data_->furnace_item(
                static_cast<FurnaceItem>(mapping.field));
            return true;

        case DataSource::Profile:
            value = data_->profile_item(
                static_cast<ProfileItem>(mapping.field));
            return true;

        case DataSource::Setting:
            value = data_->setting_item(
                static_cast<SettingItem>(mapping.field));
            return true;

        case DataSource::Alarm:
        case DataSource::Count:
            return false;
    }

    return false;
}

const Profile&
Ui::get_edit_profile() const noexcept
{
    return data_->profile();
}

std::size_t Ui::event_count() const noexcept
{
    return data_->event_count();
}

const DataAggregator::Event&
Ui::event_from_newest(const std::size_t index) const noexcept
{
    return data_->event_from_newest(index);
}

std::size_t Ui::sample_count() const noexcept
{
    return data_->sample_count();
}

const DataAggregator::FurnaceSample&
Ui::sample_from_newest(const std::size_t index) const noexcept
{
    return data_->sample_from_newest(index);
}

uint8_t Ui::current_step() const noexcept
{
    return current_step_;
}

void Ui::set_command_callback(
    CommandCallback callback,
    void* context) noexcept
{
    command_callback_ = callback;
    command_context_ = context;
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

const Settings& Ui::get_edit_settings() const noexcept
{
    return settings_->edit();
}


void Ui::open_settings(uint16_t) noexcept
{
    settings_->begin_edit();
    page_ = Page::Settings;
}

void Ui::save_settings(uint16_t) noexcept
{
    settings_->save();
    page_ = Page::Main;
}

void Ui::cancel_settings(uint16_t) noexcept
{
    settings_->cancel_edit();
    page_ = Page::Main;
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

void Ui::edit_buzzer(const uint16_t value) noexcept
{
    settings_->set_edit_buzzer_state(value);    
};

void Ui::edit_pid_kp(const uint16_t value) noexcept
{
    settings_->set_edit_pid_kp(value);
};

void Ui::edit_pid_ki(const uint16_t value) noexcept
{
    settings_->set_edit_pid_ki(value);    
};

void Ui::edit_pid_kd(const uint16_t value) noexcept
{
    settings_->set_edit_pid_kd(value);
};

void Ui::edit_max_temperature(const uint16_t value) noexcept
{
    settings_->set_edit_max_temperature(value);
};

void Ui::edit_prestep_outs(const uint16_t value) noexcept
{
    settings_->set_edit_prestep_outs(value);
}

void Ui::previous_step(uint16_t) noexcept
{
    if (current_step_ > 0)
        --current_step_;
}

void Ui::edit_setpoint(const uint16_t value) noexcept
{
    profiles_->set_edit_setpoint(
        current_step_,
        value);
}

void Ui::edit_duration(const uint16_t value) noexcept
{
    profiles_->set_edit_duration(
        current_step_,
        value);
}

void Ui::edit_outs(uint16_t value) noexcept
{
    profiles_->set_edit_outs(
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

void Ui::request_reset_furnace(uint16_t argument) noexcept
{
    // Send command to an App
    if (command_callback_ != nullptr)
    {
        command_callback_(
            command_context_,
            {
                ActionType::ResetFurnace,
                argument
            });
    }

    page_ = Page::Main;
}

void Ui::request_continue_furnace(
    uint16_t) noexcept
{
    if (command_callback_ != nullptr)
    {
        command_callback_(
            command_context_,
            {
                ActionType::ContinueFurnace,
                0
            });
    }

    page_ = Page::Monitor;
}

void Ui::show_events(uint16_t) noexcept
{
    page_ = Page::Events;
}

void Ui::ask_stop_profile(uint16_t) noexcept
{
    page_ = Page::Question;
}

void Ui::confirm_question(uint16_t) noexcept
{
    furnace_->stop();
}

void Ui::cancel_question(uint16_t) noexcept
{
    page_ = Page::Monitor;
}

void Ui::show_samples(uint16_t) noexcept
{
    page_ = Page::Samples;
}

void Ui::back(uint16_t) noexcept
{
    switch (page_)
    {
        case Page::ProfileSelection:
        case Page::Settings:            
        case Page::ProfileEditor:     
        case Page::Monitor:
        case Page::Result:
        case Page::Events:
        case Page::Samples:
            page_ = Page::Main;
            break;    
            
        case Page::Main:
        case Page::Count:
        case Page::Question:        
            break;
    }
}

} // namespace app