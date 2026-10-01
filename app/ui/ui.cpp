// ui.cpp

#include "ui.hpp"
#include <cstdio>

namespace app
{

namespace
{
    
constexpr Ui::FieldMapping main_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::State)},
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Step)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Power)},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs)}
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
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs)},
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

    context_ = Context::Main;
    current_step_ = 0;
    profile_selection_page_ = 0;
}

void Ui::process() noexcept
{
    switch (furnace_->state())
    {
    case static_cast<uint16_t>(Furnace::State::Waiting):
    case static_cast<uint16_t>(Furnace::State::Finished):
    case static_cast<uint16_t>(Furnace::State::Stopped):
    case static_cast<uint16_t>(Furnace::State::Error):

        context_ = Context::Result;
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


Ui::Context Ui::context() const noexcept
{
    return context_;
}


bool Ui::get_field(
            const Context context,
            const uint8_t field,
            uint16_t& value) const noexcept
{

    // Profile selection fields are derived from the current
    // collection page rather than DataAggregator data.
    if (context == Context::ProfileSelection)
    {
        const auto profile_id =
            profile_selection_page_ * ProfilesPerPage + field;

        const auto count = profiles_->profile_count();

        if (profile_id >= count)
            return false;

        value = static_cast<uint16_t>(profile_id);
        return true;
    }


    const auto page_index =
        static_cast<std::size_t>(context);

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

std::size_t Ui::profile_selection_page() const noexcept
{
    return profile_selection_page_;
}

std::size_t Ui::profile_selection_page_count() const noexcept
{
    const auto count = profiles_->profile_count();

    if (count == 0)
        return 0;

    return (count + ProfilesPerPage - 1) / ProfilesPerPage;
}

const Profile*
Ui::profile_at_slot(const std::size_t slot) const noexcept
{
    if (slot >= ProfilesPerPage)
        return nullptr;

    const auto profile_id =
        profile_selection_page_ * ProfilesPerPage + slot;

    if (profile_id >= profiles_->profile_count())
        return nullptr;

    return &profiles_->profile(profile_id);
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

std::size_t Ui::event_page() const noexcept
{
    return event_page_;
}

std::size_t Ui::event_page_count() const noexcept
{
    const auto count = event_count();

    if (count == 0)
        return 0;

    return (count + EventsPerPage - 1) / EventsPerPage;
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

    profile_selection_page_ = 0;
    context_ = Context::ProfileSelection;
}


void Ui::edit_profile_selection(uint16_t) noexcept
{
    profile_selection_mode_ =
        ProfileSelectionMode::Edit;

    profile_selection_page_ = 0;
    context_ = Context::ProfileSelection;
}

void Ui::select_profile(const uint16_t slot) noexcept
{
    if (slot >= ProfilesPerPage)
        return;

    const auto profile_id =
        profile_selection_page_ * ProfilesPerPage + slot;

    if (profile_id >= profiles_->profile_count())
        return;

    if (profile_selection_mode_ ==
        ProfileSelectionMode::Start)
    {
        confirm_start_profile(
            static_cast<uint16_t>(profile_id));
    }
    else
    {
        confirm_edit_profile(
            static_cast<uint16_t>(profile_id));
    }
}

void Ui::confirm_start_profile(uint16_t profile_id) noexcept
{
    if (!profiles_->select_for_start(profile_id))
        return;

    furnace_->start();
    context_ = Context::Monitor;
}


void Ui::confirm_edit_profile(uint16_t profile_id) noexcept
{
    if (!profiles_->select_for_edit(profile_id))
        return;

    current_step_ = 0;
    context_ = Context::ProfileEditor;
}

const Settings& Ui::get_edit_settings() const noexcept
{
    return settings_->edit();
}


void Ui::open_settings(uint16_t) noexcept
{
    settings_->begin_edit();
    context_ = Context::Settings;
}

void Ui::save_settings(uint16_t) noexcept
{
    settings_->save();
    context_ = Context::Main;
}

void Ui::cancel_settings(uint16_t) noexcept
{
    settings_->cancel_edit();
    context_ = Context::Main;
}

void Ui::previous_step() noexcept
{
    if (current_step_ == 0)
        current_step_ = config::profiles::max_steps - 1;
    else
        --current_step_;
}

void Ui::next_step() noexcept
{
    if (static_cast<std::size_t>(current_step_) + 1 >= config::profiles::max_steps)
        current_step_ = 0;
    else
        ++current_step_;
}

void Ui::previous_profile_selection_page() noexcept
{
    const auto page_count = profile_selection_page_count();

    if (page_count == 0)
        return;

    if (profile_selection_page_ == 0)
        profile_selection_page_ = page_count - 1;
    else
        --profile_selection_page_;
}

void Ui::next_profile_selection_page() noexcept
{
    const auto page_count = profile_selection_page_count();

    if (page_count == 0)
        return;

    if (profile_selection_page_ + 1 == page_count)
        profile_selection_page_ = 0;
    else
        ++profile_selection_page_;
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
    context_ = Context::Main;
}


void Ui::cancel_profile(uint16_t) noexcept
{
    context_ = Context::ProfileSelection;
}

void Ui::stop_furnace(uint16_t) noexcept
{
    furnace_->stop();
    context_ = Context::Result;
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

    context_ = Context::Main;
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

    context_ = Context::Monitor;
}

void Ui::show_events(uint16_t) noexcept
{
    event_page_ = 0;
    context_ = Context::Events;
}

void Ui::next_event_page() noexcept
{
    const auto page_count = event_page_count();

    if (page_count == 0)
        return;

    if (event_page_ + 1 >= page_count)
        event_page_ = 0;
    else
        ++event_page_;
}

void Ui::previous_event_page() noexcept
{
    const auto page_count = event_page_count();

    if (page_count == 0)
        return;

    if (event_page_ == 0)
        event_page_ = page_count - 1;
    else
        --event_page_;
}

void Ui::ask_stop_profile(uint16_t) noexcept
{
    context_ = Context::Question;
}

void Ui::confirm_question(uint16_t) noexcept
{
    furnace_->stop();
}

void Ui::cancel_question(uint16_t) noexcept
{
    context_ = Context::Monitor;
}

void Ui::show_samples(uint16_t) noexcept
{
    context_ = Context::Samples;
}

void Ui::previous(uint16_t) noexcept
{
    switch (context_)
    {
        case Context::ProfileSelection:
            previous_profile_selection_page();
            break;

        case Context::ProfileEditor:
            previous_step();
            break;

        case Context::Events:
            previous_event_page();
            break;

        default:
            break;
    }
}

void Ui::next(uint16_t) noexcept
{
    switch (context_)
    {
        case Context::ProfileSelection:
            next_profile_selection_page();
            break;

        case Context::ProfileEditor:
            next_step();
            break;

        case Context::Events:
            next_event_page();
            break;

        default:
            break;
    }
}

void Ui::back(uint16_t) noexcept
{
    switch (context_)
    {
        case Context::ProfileSelection:
        case Context::Settings:
        case Context::ProfileEditor:
        case Context::Monitor:
        case Context::Result:
        case Context::Events:
        case Context::Samples:
            context_ = Context::Main;
            break;    
            
        case Context::Main:
        case Context::Count:
        case Context::Question:
            break;
    }
}

} // namespace app