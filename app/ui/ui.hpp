// ui.hpp
#pragma once

#include <cstdint>

#include "data_aggregator.hpp"
#include "furnace.hpp"
#include "profiles.hpp"
#include "settings.hpp"

// UI defines the application-facing user interface state.
// It owns pages, page fields, actions, and interaction workflows.
// It consumes data through DataAggregator and is independent of
// renderer-specific presentation and backend I/O.

namespace app
{

class Ui
{
public:

    enum class Page : uint8_t
    {
        Main,
        Monitor
    };

    // Fields displayed by the Main page.
    enum class MainField : uint8_t
    {
        Temperature,
        
        Count
    };

    // Fields displayed by the Monitor page.
    enum class MonitorField : uint8_t
    {
        State,
        CurrentStep,
        Temperature,

        Count
    };

    
    // Initializes the UI and connects it to the application modules
    // required for UI operations.
    void init(
        Furnace& furnace,
        ProfileManager& profiles,
        SettingManager& settings,
        DataAggregator& data) noexcept;

    // Updates UI state and processes pending UI actions.
    void process() noexcept;

    // Returns the current data item associated with a Main page field.
    // The returned item contains both the current value and its version.
    const DataItem<uint16_t>& get_field(
        MainField field) const noexcept;

    // Returns the current data item associated with a Monitor page field.
    // The returned item contains both the current value and its version.            
    const DataItem<uint16_t>& get_field(
        MonitorField field) const noexcept;
    
    Page page() const noexcept;    
        
private:
    struct FieldMapping
    {
        uint8_t source;
        uint8_t field;
    };

    static constexpr FieldMapping main_fields_[] =
    {
        {
            static_cast<uint8_t>(DataSource::TcParser),
            static_cast<uint8_t>(TcParserItem::Temperature)
        }
    };
    
    static constexpr FieldMapping monitor_fields_[] =
    {
        {
            static_cast<uint8_t>(DataSource::Furnace),
            static_cast<uint8_t>(FurnaceItem::State)
        },
        {
            static_cast<uint8_t>(DataSource::Furnace),
            static_cast<uint8_t>(FurnaceItem::Step)
        },
        {
            static_cast<uint8_t>(DataSource::TcParser),
            static_cast<uint8_t>(TcParserItem::Temperature)
        }
    };

    
    Page page_ = Page::Main;
    
    Furnace* furnace_ = nullptr;
    ProfileManager* profiles_ = nullptr;
    SettingManager* settings_ = nullptr;
    DataAggregator* data_ = nullptr;
};

} // namespace app