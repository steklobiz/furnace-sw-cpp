// data_aggregator.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "furnace.hpp"
#include "notification.hpp"
#include "profiles.hpp"
#include "tc_parser.hpp"

namespace app
{

template<typename T>
struct DataItem
{
    T value{};
    uint8_t version = 0;
};


enum class DataSource : uint8_t
{
    TcParser,
    Furnace,
    Profile,

    Count
};


enum class FurnaceItem : uint8_t
{
    State,
    Step,
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


class DataAggregator
{
public:

    void init(
        TcParser& tc_parser,
        Furnace& furnace,
        ProfileManager& profiles) noexcept;

    const DataItem<uint16_t>& get_item(
        uint8_t source,
        uint8_t field) const noexcept;

    const DataItem<Profile>& profile() const noexcept;


private:

    struct SourceDescriptor
    {
        DataItem<uint16_t>* items;
        std::size_t count;
    };


    static void tc_parser_callback(
        void* context,
        const Notification& notification) noexcept;

    static void furnace_callback(
        void* context,
        const Notification& notification) noexcept;

    static void profile_callback(
        void* context,
        const Notification& notification) noexcept;


    void update_tc_parser() noexcept;
    void update_furnace() noexcept;
    void update_profile() noexcept;


    TcParser* tc_parser_ = nullptr;
    Furnace* furnace_ = nullptr;
    ProfileManager* profiles_ = nullptr;

    DataItem<uint16_t> tc_parser_items_[
        static_cast<std::size_t>(TcParserItem::Count)]{};

    DataItem<uint16_t> furnace_items_[
        static_cast<std::size_t>(FurnaceItem::Count)]{};

    DataItem<uint16_t> profile_items_[
        static_cast<std::size_t>(ProfileItem::Count)]{};

    DataItem<Profile> profile_{};

    DataItem<uint16_t> null_item_{};

    SourceDescriptor source_descriptors_[
        static_cast<std::size_t>(DataSource::Count)]{};
};

} // namespace app