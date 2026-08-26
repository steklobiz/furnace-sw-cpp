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
    {SettingItem::MaxTemperature,  &SettingManager::get_max_temperature_c}
};

} // namespace


void DataAggregator::init(
    TcParser& tc_parser,
    Furnace& furnace,
    ProfileManager& profiles,
    SettingManager& settings) noexcept
{
    tc_parser_ = &tc_parser;
    furnace_ = &furnace;
    profiles_ = &profiles;
    settings_ = &settings;

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
    if (notification.type != NotificationType::DataReady)
        return;

    auto& aggregator =
        *static_cast<DataAggregator*>(context);

    aggregator.update_furnace();
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

} // namespace app