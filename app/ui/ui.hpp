// ui.hpp

// UI defines the application-facing user interface state.
// It owns pages, page fields, actions, and interaction workflows.
// It consumes data through DataAggregator and is independent of
// renderer-specific presentation and backend I/O.

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
        Monitor
    };

    enum class MonitorField : uint8_t
    {
        Temperature,
        State,
        Step,

        Count
    };
    
    struct Field
    {
        uint8_t source;
        uint8_t id;
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

    Page page() const noexcept;
    
    const Field* fields() const noexcept;
    std::size_t field_count() const noexcept;
    
private:

    Furnace* furnace_ = nullptr;
    ProfileManager* profiles_ = nullptr;
    SettingManager* settings_ = nullptr;
    DataAggregator* data_ = nullptr;
    
    Page page_ = Page::Monitor;
};

} // namespace app