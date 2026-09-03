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

static constexpr FurnaceMapping furnace_mapping[] =
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


static constexpr TcParserMapping tc_parser_mapping[] =
{
    {TcParserItem::Temperature, &TcParser::average}
};


static constexpr ProfileMapping profile_mapping[] =
{
    {ProfileItem::StartProfileId, &ProfileManager::start_profile_id},
    {ProfileItem::EditProfileId,  &ProfileManager::edit_profile_id}
};

static constexpr SettingMapping setting_mapping[] =
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
    
    source_descriptors_[
        static_cast<std::size_t>(DataSource::TcParser)] =
    {
        tc_parser_items_,
        std::size(tc_parser_items_)
    };

    source_descriptors_[
        static_cast<std::size_t>(DataSource::Furnace)] =
    {
        furnace_items_,
        std::size(furnace_items_)
    };

    source_descriptors_[
        static_cast<std::size_t>(DataSource::Profile)] =
    {
        profile_items_,
        std::size(profile_items_)
    };

    source_descriptors_[
    static_cast<std::size_t>(DataSource::Setting)] =
    {
        setting_items_,
        std::size(setting_items_)
    };

    
    tc_parser_->set_notify_callback(
        tc_parser_callback,
        this);

    furnace_->set_notify_callback(
        furnace_callback,
        this);
        
    settings_->set_notify_callback(
        settings_callback,
        this);
        
    profiles_->set_notify_callback(
        profile_callback,
        this);
        
    alarms_->set_notify_callback(
        alarm_callback,
        this);
            
    update_tc_parser();
    update_furnace();
    update_settings();
    update_profile();
}


void DataAggregator::tc_parser_callback(
    void* context,
    const Notification& notification) noexcept
{
    if (notification.type != NotificationType::DataReady)
        return;

    auto& aggregator =
        *static_cast<DataAggregator*>(context);

    aggregator.update_tc_parser();
}


void DataAggregator::furnace_callback(
    void* context,
    const Notification& notification) noexcept
{
    auto& aggregator =
        *static_cast<DataAggregator*>(context);

    switch (notification.type)
    {
    case NotificationType::DataReady:
        aggregator.update_furnace();
        aggregator.collect_sample();
        break;

    case NotificationType::ProfileStarted:
    case NotificationType::StepStarted:
    case NotificationType::ProfileFinished:
    case NotificationType::ProfileStopped:
    case NotificationType::OutputSet:
    case NotificationType::OutputReset:
        aggregator.add_event(
            DataSource::Furnace,
            notification);
        break;

    default:
        break;
    }
}

void DataAggregator::settings_callback(
    void* context,
    const Notification& notification) noexcept
{
    if (notification.type != NotificationType::SettingsChanged)
        return;

    auto& aggregator =
        *static_cast<DataAggregator*>(context);

    aggregator.update_settings();
}


void DataAggregator::profile_callback(
    void* context,
    const Notification& notification) noexcept
{
    if (notification.type != NotificationType::StartProfileChanged &&
        notification.type != NotificationType::EditProfileChanged)
    {
        return;
    }

    auto& aggregator =
        *static_cast<DataAggregator*>(context);

    aggregator.update_profile();
}

void DataAggregator::alarm_callback(
    void* context,
    const Notification& notification) noexcept
{
    auto& aggregator =
        *static_cast<DataAggregator*>(context);

    aggregator.add_event(
        DataSource::Alarm,
        notification);
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


const uint16_t&
DataAggregator::get_item(
    uint8_t source,
    uint8_t field) const noexcept
{
    if (source >=
        static_cast<uint8_t>(DataSource::Count))
    {
        return null_item_;
    }

    const auto& descriptor =
        source_descriptors_[source];

    if (field >= descriptor.count)
    {
        return null_item_;
    }

    return descriptor.items[field];
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
        get_item(
            static_cast<uint8_t>(DataSource::Furnace),
            static_cast<uint8_t>(FurnaceItem::ProfileElapsed));

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

const DataAggregator::Event&
DataAggregator::event_from_newest(
    std::size_t index) const noexcept
{
    return events_.from_newest(index);
}

void DataAggregator::collect_sample() noexcept
{
    const uint16_t elapsed_s =
        get_item(
            static_cast<uint8_t>(DataSource::Furnace),
            static_cast<uint8_t>(FurnaceItem::ProfileElapsed));

    if (elapsed_s < next_sample_s_)
        return;

    const int16_t temperature =
        static_cast<int16_t>(
            get_item(
                static_cast<uint8_t>(DataSource::Furnace),
                static_cast<uint8_t>(FurnaceItem::Temperature)));

    const uint8_t output =
        static_cast<uint8_t>(
            get_item(
                static_cast<uint8_t>(DataSource::Furnace),
                static_cast<uint8_t>(FurnaceItem::Power)));

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