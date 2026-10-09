#pragma once

#include <cstdint>
#include "furnace.hpp"
#include "data_aggregator.hpp"
#include "profiles.hpp"
#include "settings.hpp"

namespace app {

class Ui
{
public:
    enum class Context : uint8_t
    {
        None,
        Main,
        ProfileSelection,
        Edit,
        Settings,
        Question,
        Events,
        Count
    };

    enum class Mode : uint8_t
    {
        None,
        Brief,
        Detailed,
        Start,
        Edit,
        Pid,
        Other,
        Stop,
        Count
    };

    enum class ActionType : uint8_t
    {

        // Context entry
        Start,
        Stop,
        Reset,
        Settings,
        Events,
        Edit,

        // Navigation
        Next,
        Previous,
        Select,
        Confirm,
        Cancel,

        // Numeric field editors
        SetSetpoint,
        SetDuration,
        SetOutputs,
        SetPidKp,
        SetPidKi,
        SetPidKd,
        SetMaxTemperature,
        SetBuzzer,
        SetPrestepOuts,

        None, // do we need it??
        Count
    };

    struct Action
    {
        ActionType type = ActionType::None;
        uint16_t argument = 0;
    };

    struct Position
    {
        Context context = Context::None;
        Mode mode = Mode::None;

        bool operator==(const Position& other) const noexcept
        {
            return context == other.context &&
                   mode == other.mode;
        }

        bool operator!=(const Position& other) const noexcept
        {
            return !(*this == other);
        }
    };

    static constexpr Position StayPosition{Context::None, Mode::None};

    using Handler = bool (Ui::*)(const Action&) noexcept;

    struct Transition
    {
        Position from;
        Furnace::State state;
        ActionType action;
        Handler handler;
        Position to;
    };

    void init(
        DataAggregator& data,
        Furnace& furnace,
        ProfileManager& profiles,
        SettingManager& settings) noexcept;

    void execute(const Action& action) noexcept;

    [[nodiscard]] Position position() const noexcept;
    [[nodiscard]] Furnace::State state() const noexcept;
    [[nodiscard]] uint8_t profile_page() const noexcept;
    [[nodiscard]] std::size_t profile_count() const noexcept;
    [[nodiscard]] uint8_t edit_step() const noexcept;

    [[nodiscard]] uint8_t event_page() const noexcept;
    [[nodiscard]] std::size_t event_count() const noexcept;

    [[nodiscard]] const DataAggregator::Event&
    event_from_newest(std::size_t index) const noexcept;

private:
    // Profile selection state.
    static constexpr uint8_t ProfilesPerPage = 10;

    // Event history paging.
    static constexpr uint8_t EventsPerPage = 4;

    uint8_t profile_page_ = 0;
    uint8_t edit_step_ = 0;
    uint8_t event_page_ = 0;

    // Transition handling.
    [[nodiscard]] const Transition*
    find_transition(const Action& action) const noexcept;

    // Transition handlers.
    bool begin_settings(const Action& action) noexcept;
    bool set_pid_kp(const Action& action) noexcept;
    bool set_pid_ki(const Action& action) noexcept;
    bool set_pid_kd(const Action& action) noexcept;
    bool set_max_temperature(const Action& action) noexcept;
    bool set_buzzer(const Action& action) noexcept;
    bool set_prestep_outs(const Action& action) noexcept;
    bool save_settings(const Action& action) noexcept;
    bool cancel_settings(const Action& action) noexcept;

    bool stop_furnace(const Action& action) noexcept;
    bool reset_furnace(const Action& action) noexcept;

    bool select_profile_to_start(const Action& action) noexcept;
    bool select_profile_to_edit(const Action& action) noexcept;

    bool next_profile_page(const Action& action) noexcept;
    bool previous_profile_page(const Action& action) noexcept;

    // Selects the next profile step for editing.
    bool next_edit_step(const Action& action) noexcept;
    // Selects the previous profile step for editing.
    bool previous_edit_step(const Action& action) noexcept;

    // Updates the setpoint of the selected profile step.
    bool edit_setpoint(const Action& action) noexcept;
    // Updates the duration of the selected profile step.
    bool edit_duration(const Action& action) noexcept;
    // Updates the outputs of the selected profile step.
    bool edit_outs(const Action& action) noexcept;

    bool save_edit(const Action& action) noexcept;
    bool cancel_edit(const Action& action) noexcept;

    // Select the next or previous event-history page.
    bool next_event_page(const Action& action) noexcept;
    bool previous_event_page(const Action& action) noexcept;

    // Number of event-history pages, including one empty page.
    [[nodiscard]] uint8_t event_page_count() const noexcept;
    // Profile selection helpers.
    [[nodiscard]] uint8_t profile_page_count() const noexcept;

    // Profile edit helpers.
    [[nodiscard]] uint8_t edit_step_count() const noexcept;

    // Dependencies.
    DataAggregator* data_ = nullptr;
    Furnace* furnace_ = nullptr;
    ProfileManager* profiles_ = nullptr;
    SettingManager* settings_ = nullptr;

    // Current UI position.
    Position position_{};

    // Transition table.
    static const Transition transitions_[];
};

}