// data_aggregator.cpp

#include "data_aggregator.hpp"

namespace app
{

namespace
{

template<class Enum, class Source>
struct Mapping
{
    Enum item;
    uint16_t (Source::*get)() const noexcept;
};

template<class Enum, class T, std::size_t N>
void update(
    Enum id,
    T value,
    T (&items)[N]) noexcept
{
    items[static_cast<std::size_t>(id)] = value;
}

template<class Enum, class Source, std::size_t MappingCount,
         std::size_t ItemCount>
void refresh_source(
    const Source* source,
    const Mapping<Enum, Source> (&mappings)[MappingCount],
    uint16_t (&items)[ItemCount]) noexcept
{
    for (const auto& mapping : mappings)
    {
        update(
            mapping.item,
            (source->*mapping.get)(),
            items);
    }
}

constexpr Mapping<FurnaceItem, Furnace> furnace_mapping[]
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


constexpr Mapping<TcParserItem, TcParser> tc_parser_mapping[] =
{
    {TcParserItem::Temperature, &TcParser::average}
};


constexpr Mapping<ProfileItem, ProfileManager> profile_mapping[] =
{
    {ProfileItem::StartProfileId, &ProfileManager::start_profile_id},
    {ProfileItem::EditProfileId,  &ProfileManager::edit_profile_id}
};

constexpr Mapping<SettingItem, SettingManager> setting_mapping[] =
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

    refresh();

}


void DataAggregator::notification_callback(
        void* context,
        const Notification& notification) noexcept
{
    auto* self = static_cast<DataAggregator*>(context);

    switch (notification.type)
    {
        case NotificationType::DataReady:
            self->refresh();

            if (notification.context == self->furnace_)
                self->collect_sample();
            break;

        case NotificationType::SettingsChanged:
        case NotificationType::StartProfileChanged:
        case NotificationType::EditProfileChanged:
            self->refresh();
            break;

        case NotificationType::ProfileStarted:
            self->clear_history();
            self->add_event(DataSource::Furnace, notification);
            break;

        case NotificationType::StepStarted:
        case NotificationType::ProfileFinished:
        case NotificationType::ProfileStopped:
        case NotificationType::OutputSet:
        case NotificationType::OutputReset:
            self->add_event(DataSource::Furnace, notification);
            break;

        case NotificationType::Error:
            self->add_event(DataSource::Alarm, notification);
            break;

        default:
            break;
    }
}


void DataAggregator::refresh() noexcept
{
    refresh_source(tc_parser_, tc_parser_mapping, tc_parser_items_);
    refresh_source(furnace_, furnace_mapping, furnace_items_);
    refresh_source(profiles_, profile_mapping, profile_items_);
    refresh_source(settings_, setting_mapping, setting_items_);

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