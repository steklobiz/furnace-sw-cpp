// data_aggregator.cpp

#include "data_aggregator.hpp"

namespace app
{

namespace
{

struct FurnaceMapping
{
    FurnaceItem item;
    uint16_t (Furnace::*get)() const noexcept;
};


struct TcParserMapping
{
    TcParserItem item;
    uint16_t (TcParser::*get)() const noexcept;
};


struct ProfileMapping
{
    ProfileItem item;
    uint16_t (ProfileManager::*get)() const noexcept;
};

struct SettingMapping
{
    SettingItem item;
    uint16_t (SettingManager::*get)() const noexcept;
};

template<class Enum, class T, std::size_t N>
void update(
    Enum id,
    T value,
    T (&items)[N]) noexcept
{
    items[static_cast<std::size_t>(id)] = value;
}

constexpr FurnaceMapping furnace_mapping[] =
{
    {FurnaceItem::State,          &Furnace::state},
    {FurnaceItem::Step,           &Furnace::current_step},
    {FurnaceItem::StepType,       &Furnace::step_type},
    {FurnaceItem::Temperature,    &Furnace::current_temperature},
    {FurnaceItem::Setpoint,       &Furnace::setpoint},
    {FurnaceItem::StepElapsed,    &Furnace::step_elapsed},
    {FurnaceItem::ProfileElapsed, &Furnace::profile_elapsed},
    {FurnaceItem::Power,          &Furnace::power},
    {FurnaceItem::Outputs,        &Furnace::outputs}
};


constexpr TcParserMapping tc_parser_mapping[] =
{
    {TcParserItem::Temperature, &TcParser::average}
};


constexpr ProfileMapping profile_mapping[] =
{
    {ProfileItem::StartProfileId, &ProfileManager::start_profile_id},
    {ProfileItem::EditProfileId,  &ProfileManager::edit_profile_id}
};

constexpr SettingMapping setting_mapping[] =
{
    {SettingItem::Buzzer,          &SettingManager::get_buzzer_state},
    {SettingItem::PidKp,           &SettingManager::get_pid_kp},
    {SettingItem::PidKi,           &SettingManager::get_pid_ki},
    {SettingItem::PidKd,           &SettingManager::get_pid_kd},
    {SettingItem::MaxTemperature,  &SettingManager::get_max_temperature},
    {SettingItem::PrestepOuts,     &SettingManager::get_prestep_outs}
};

} // namespace


void DataAggregator::init(
    TcParser& tc_parser,
    Furnace& furnace,
    ProfileManager& profiles,
    SettingManager& settings,
    AlarmDispatcher& alarms) noexcept
{
    tc_parser_ = &tc_parser;
    furnace_ = &furnace;
    profiles_ = &profiles;
    settings_ = &settings;
    alarms_ = &alarms;

    tc_parser_->set_notify_callback(
        notification_callback,
        this);

    furnace_->set_notify_callback(
        notification_callback,
        this);

    settings_->set_notify_callback(
        notification_callback,
        this);

    profiles_->set_notify_callback(
        notification_callback,
        this);

    alarms_->set_notify_callback(
        notification_callback,
        this);

    update_tc_parser();
    update_furnace();
    update_settings();
    update_profile();
}


void DataAggregator::notification_callback(
        void* context,
        const Notification& notification) noexcept
{
    static_cast<DataAggregator*>(context)->capture(notification);
}

void DataAggregator::capture(
        const Notification& notification) noexcept
{
    switch (notification.type)
    {
        case NotificationType::DataReady:
            if (notification.context == furnace_)
            {
                update_furnace();
                collect_sample();
            }
            else if (notification.context == tc_parser_)
            {
                update_tc_parser();
            }
            break;

        case NotificationType::SettingsChanged:
            update_settings();
            break;

        case NotificationType::StartProfileChanged:
        case NotificationType::EditProfileChanged:
            update_profile();
            break;

        case NotificationType::ProfileStarted:
            clear_history();
            add_event(DataSource::Furnace, notification);
            break;

        case NotificationType::StepStarted:
        case NotificationType::ProfileFinished:
        case NotificationType::ProfileStopped:
        case NotificationType::OutputSet:
        case NotificationType::OutputReset:
            add_event(DataSource::Furnace, notification);
            break;

        case NotificationType::Error:
            add_event(DataSource::Alarm, notification);
            break;

        default:
            break;
    }
}


void DataAggregator::update_tc_parser() noexcept
{
    for (const auto& mapping : tc_parser_mapping)
    {
        update(
            mapping.item,
            (tc_parser_->*mapping.get)(),
            tc_parser_items_);
    }
}


void DataAggregator::update_furnace() noexcept
{
    for (const auto& mapping : furnace_mapping)
    {
        update(
            mapping.item,
            (furnace_->*mapping.get)(),
            furnace_items_);
    }
}

void DataAggregator::update_settings() noexcept
{
    for (const auto& mapping : setting_mapping)
    {
        update(
            mapping.item,
            (settings_->*mapping.get)(),
            setting_items_);
    }
}


void DataAggregator::update_profile() noexcept
{
    for (const auto& mapping : profile_mapping)
    {
        update(
            mapping.item,
            (profiles_->*mapping.get)(),
            profile_items_);
    }

    profile_ = profiles_->edit_profile();
}


uint16_t DataAggregator::furnace_item(
        const FurnaceItem item) const noexcept
{
    return furnace_items_[static_cast<std::size_t>(item)];
}


uint16_t DataAggregator::tc_parser_item(
        const TcParserItem item) const noexcept
{
    return tc_parser_items_[static_cast<std::size_t>(item)];
}


uint16_t DataAggregator::profile_item(
        const ProfileItem item) const noexcept
{
    return profile_items_[static_cast<std::size_t>(item)];
}


uint16_t DataAggregator::setting_item(
        const SettingItem item) const noexcept
{
    return setting_items_[static_cast<std::size_t>(item)];
}



const Profile&
DataAggregator::profile() const noexcept
{
    return profile_;
}

void DataAggregator::add_event(
    DataSource source,
    const Notification& notification) noexcept
{

    const auto elapsed_s =
        furnace_item(FurnaceItem::ProfileElapsed);

    events_.push_overwrite({
        elapsed_s,
        source,
        notification.type,
        notification.argument
    });
}

std::size_t DataAggregator::event_count() const noexcept
{
    return events_.size();
}

const DataAggregator::Event& DataAggregator::event_from_newest(
    const std::size_t index) const noexcept
{
    return events_.from_newest(index);
}

std::size_t DataAggregator::sample_count() const noexcept
{
    return samples_.size();
}

const DataAggregator::FurnaceSample& DataAggregator::sample_from_newest(
    const std::size_t index) const noexcept
{
    return samples_.from_newest(index);
}

void DataAggregator::collect_sample() noexcept
{
    const uint16_t elapsed_s =
        furnace_item(FurnaceItem::ProfileElapsed);

    if (elapsed_s < next_sample_s_)
        return;

    const auto temperature =
        static_cast<int16_t>(
            furnace_item(FurnaceItem::Temperature));

    const auto output =
        static_cast<uint8_t>(
            furnace_item(FurnaceItem::Power));

    samples_.push_overwrite({
        elapsed_s,
        temperature,
        output
    });

    next_sample_s_ =
        elapsed_s + config::history::sample_period_s;
}


void DataAggregator::clear_history() noexcept
{
    events_.clear();
    samples_.clear();
    
    next_sample_s_ = 0;
}

} // namespace app