// data_aggregator.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "furnace.hpp"
#include "notification.hpp"
#include "profiles.hpp"
#include "tc_parser.hpp"
#include "settings.hpp"
#include "alarm.hpp"
#include "ring_buffer.hpp"
#include "config.hpp"

// Data aggregator between application data sources and the UI.
// Collects current snapshots from Furnace, ProfileManager, and other sources.
// Provides a stable, UI-oriented view of application data.
// Owns no UI logic; it only publishes current data and retains events and samples.

namespace app
{

enum class DataSource : uint8_t
{
    TcParser,
    Furnace,
    Profile,
    Setting,
    Alarm,

    Count
};


enum class FurnaceItem : uint8_t
{
    State,
    Step,
    StepType,
    Temperature,
    Setpoint,
    StepElapsed,
    ProfileElapsed,
    Power,
    Outputs,

    Count
};


enum class TcParserItem : uint8_t
{
    Temperature,

    Count
};


enum class ProfileItem : uint8_t
{
    StartProfileId,
    EditProfileId,

    Count
};


enum class SettingItem : uint8_t
{
    Buzzer,
    PidKp,
    PidKi,
    PidKd,
    MaxTemperature,
    PrestepOuts,

    Count
};


class DataAggregator
{
public:

    struct Event
    {
        uint32_t elapsed_s;
        DataSource source;
        NotificationType type;
        uint16_t argument;
    };

    struct FurnaceSample
    {
        uint32_t elapsed_s;
        int16_t temperature;
        uint8_t output;
    };
    
    // Connects data sources and registers notification callbacks.
    void init(
        TcParser& tc_parser,
        Furnace& furnace,
        ProfileManager& profiles,
        SettingManager& settings,
        AlarmDispatcher& alarms) noexcept;

    // Returns the current value of a data item.    
    const uint16_t& get_item(
        uint8_t source,
        uint8_t field) const noexcept;

    // Returns the current profile snapshot.    
    const Profile& profile() const noexcept;

     // Returns the number of retained events.
    std::size_t event_count() const noexcept;
    // Returns an event by position, starting with the newest event.
    const Event& event_from_newest(
        std::size_t index) const noexcept;

    // Returns the number of retained furnace samples.    
    std::size_t sample_count() const noexcept;
    // Returns a furnace sample by position, starting with the newest sample.
    const FurnaceSample& sample_from_newest(
        std::size_t index) const noexcept; 
               
    void collect_sample() noexcept;
        
    // Clears all retained events and furnace samples.
    void clear_history() noexcept;        
                
private:

    struct SourceDescriptor
    {
        uint16_t* items;
        std::size_t count;
    };

    
    static void tc_parser_callback(
        void* context,
        const Notification& notification) noexcept;

    static void furnace_callback(
        void* context,
        const Notification& notification) noexcept;
        
    static void settings_callback(
        void* context,
        const Notification& notification) noexcept;
    
    static void profile_callback(
        void* context,
        const Notification& notification) noexcept;

    static void alarm_callback(
        void* context,
        const Notification& notification) noexcept;
        
    void update_tc_parser() noexcept;
    void update_furnace() noexcept;
    void update_settings() noexcept;
    void update_profile() noexcept;

    void add_event(
        DataSource source,
        const Notification& notification) noexcept;
    
    TcParser* tc_parser_ = nullptr;
    Furnace* furnace_ = nullptr;
    ProfileManager* profiles_ = nullptr;
    SettingManager* settings_ = nullptr;
    AlarmDispatcher* alarms_{nullptr};
    
    uint16_t tc_parser_items_[
        static_cast<std::size_t>(TcParserItem::Count)]{};

    uint16_t furnace_items_[
        static_cast<std::size_t>(FurnaceItem::Count)]{};

    uint16_t profile_items_[
        static_cast<std::size_t>(ProfileItem::Count)]{};
        
    uint16_t setting_items_[
        static_cast<std::size_t>(SettingItem::Count)]{};
        
    Profile profile_{};

    uint16_t null_item_{};

    uint32_t next_sample_s_{0};
    
    core::RingBuffer<Event, config::history::event_capacity> events_;
        
    core::RingBuffer<FurnaceSample,config::history::sample_capacity> samples_;    
    
    SourceDescriptor source_descriptors_[
        static_cast<std::size_t>(DataSource::Count)]{};
};

} // namespace app