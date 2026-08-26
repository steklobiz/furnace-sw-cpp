// data_aggregator.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "furnace.hpp"
#include "notification.hpp"
#include "profiles.hpp"
#include "tc_parser.hpp"
#include "settings.hpp"

// Data aggregator between application data sources and the UI.
// Collects current snapshots from Furnace, ProfileManager, and other sources.
// Provides a stable, UI-oriented view of application data.
// Owns no UI logic; it only publishes current data and forwards notifications.

namespace app
{

enum class DataSource : uint8_t
{
    TcParser,
    Furnace,
    Profile,
    Setting,

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

    Count
};


class DataAggregator
{
public:

    void init(
        TcParser& tc_parser,
        Furnace& furnace,
        ProfileManager& profiles,
        SettingManager& settings) noexcept;

    const uint16_t& get_item(
        uint8_t source,
        uint8_t field) const noexcept;

    const Profile& profile() const noexcept;


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


    void update_tc_parser() noexcept;
    void update_furnace() noexcept;
    void update_settings() noexcept;
    void update_profile() noexcept;


    TcParser* tc_parser_ = nullptr;
    Furnace* furnace_ = nullptr;
    ProfileManager* profiles_ = nullptr;
    SettingManager* settings_ = nullptr;

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

    SourceDescriptor source_descriptors_[
        static_cast<std::size_t>(DataSource::Count)]{};
};

} // namespace app