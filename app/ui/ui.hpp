// ui.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "data_aggregator.hpp"
#include "profiles.hpp"
#include "settings.hpp"

// Represents the application UI state and navigation.
// Owns pages, actions, and user interaction logic.
// Reads application data through the DataAggregator.
// Independent of the concrete display implementation.

namespace app
{

class Ui
{
public:

    enum class ActionType : uint8_t
    {
        None,
    
        // Main / profile selection
        StartProfileSelection,
        EditProfileSelection,
        SelectProfile,
    
        StartProfileConfirm,
        EditProfileConfirm,
    
        // Settings
        Settings,
        EditBuzzer,
        EditPidKp,
        EditPidKi,
        EditPidKd,
        EditMaxTemperature,
        EditPrestepOuts,
        SaveSettings,
        CancelSettings,
    
        // Profile editor
        NextStep,
        PreviousStep,
        EditSetpoint,
        EditDuration,
        EditOuts,
        SaveProfile,
        CancelProfile,
    
        // Furnace control
        StopFurnace,
        ResetFurnace,
        ContinueFurnace,
    
        // Events
        ShowEvents,
    
        // Question
        AskStopProfile,
        ConfirmQuestion,
        CancelQuestion,

        // Samples
        ShowSamples,
                
        // Navigation
        Back
    };


    struct Action
    {
        ActionType type = ActionType::None;
        uint16_t argument = 0;
    };


    using CommandCallback =
        void (*)(void* context, Action action) noexcept;

    enum class Page : uint8_t
    {
        Main,
        ProfileSelection,
        Settings,
        ProfileEditor,
        Monitor,
        Result,
        Events,
        Samples,
        Question,

        Count
    };
    
    
    enum class ProfileSelectionMode : uint8_t
    {
        Start,
        Edit
    };

    enum class ProfileEditField : uint8_t
    {
        Setpoint,
        Duration,
        Flags
    };
    
    enum class SettingsField : uint8_t
    {
        Buzzer,
        PidKp,
        PidKi,
        PidKd,
        MaxTemperature,
    };


    struct FieldMapping
    {
        DataSource source;
        uint8_t field;
    };


    struct PageDescriptor
    {
        const FieldMapping* fields;
        std::size_t field_count;
    };

    // Initializes the UI with its data sources and application modules
    void init(
        DataAggregator& data,
        Furnace& furnace,
        ProfileManager& profiles,
        SettingManager& settings) noexcept;

    // Processes periodic UI logic, including page transitions.    
    void process() noexcept;     
    
    // Executes an action received from the renderer/input layer.
    void execute(Action action) noexcept;

    // Returns the currently active page.
    [[nodiscard]] Page page() const noexcept;
    
    // Returns the current value of a field on the specified page.
    [[nodiscard]] uint16_t get_field(
        Ui::Page page,
        uint8_t field) const noexcept;
        
    // Returns the current profile being edited.
    [[nodiscard]] const Profile& get_edit_profile() const noexcept;
    // Returns the current settings being edited.
    [[nodiscard]] const Settings& get_edit_settings() const noexcept;
    
    // Returns an event by its position relative to the newest event.
    [[nodiscard]] const DataAggregator::Event&
        event_from_newest(std::size_t index) const noexcept;

    // Returns the number of stored events.    
    [[nodiscard]] std::size_t event_count() const noexcept;

    // Returns an sample by its position relative to the newest event.
    [[nodiscard]] const DataAggregator::FurnaceSample&
        sample_from_newest(std::size_t index) const noexcept;

    // Returns the number of stored samples.
    [[nodiscard]] std::size_t sample_count() const noexcept;

    // Returns the currently selected profile step.
    [[nodiscard]] uint8_t current_step() const noexcept;
    
    // Registers the callback used to send application-level commands.        
    void set_command_callback(
        CommandCallback callback,
        void* context) noexcept;

private:

    struct ActionMapping
    {
        Ui::ActionType type;
        void (Ui::*callback)(uint16_t) noexcept;
    };    

    // Opens profile selection for starting a profile.
    void start_profile_selection(uint16_t) noexcept;
    // Opens profile selection for editing a profile.
    void edit_profile_selection(uint16_t) noexcept;
    // Selects a profile according to the current selection mode.
    void select_profile(uint16_t) noexcept;

    // Selects a profile and starts the furnace process.
    void confirm_start_profile(uint16_t) noexcept;
    // Selects a profile and opens it for editing.
    void confirm_edit_profile(uint16_t) noexcept;

    // Starts a settings editing session.
    void open_settings(uint16_t) noexcept;
    // Saves the edited settings and leaves the settings page.
    void save_settings(uint16_t) noexcept;
    // Discards the current settings edits.
    void cancel_settings(uint16_t) noexcept;

    // Updates the edited buzzer state.
    void edit_buzzer(uint16_t value) noexcept;
    // Updates the edited PID proportional coefficient.
    void edit_pid_kp(uint16_t value) noexcept;
    // Updates the edited PID integral coefficient.
    void edit_pid_ki(uint16_t value) noexcept;
    // Updates the edited PID derivative coefficient.
    void edit_pid_kd(uint16_t value) noexcept;
    // Updates the edited maximum temperature limit.
    void edit_max_temperature(uint16_t value) noexcept;
    // Updates the edited pre-step output configuration.
    void edit_prestep_outs(uint16_t value) noexcept;

    // Selects the next profile step for editing.
    void next_step(uint16_t) noexcept;
    // Selects the previous profile step for editing.
    void previous_step(uint16_t) noexcept;

    // Updates the setpoint of the selected profile step.
    void edit_setpoint(uint16_t  value) noexcept;
    // Updates the duration of the selected profile step.
    void edit_duration(uint16_t  value) noexcept;
    // Updates the outputs of the selected profile step.
    void edit_outs(uint16_t  value) noexcept;

    // Saves the edited profile.
    void save_profile(uint16_t) noexcept;
    // Cancels profile editing without saving.
    void cancel_profile(uint16_t) noexcept;
    
    void stop_furnace(uint16_t) noexcept;
    // Requests the application to reset the furnace.    
    void request_reset_furnace(uint16_t) noexcept;
    // Requests the application to continue the furnace process.
    void request_continue_furnace(uint16_t argument) noexcept;

    // Opens the event history page.
    void show_events(uint16_t) noexcept;

    // Opens a confirmation question before stopping the profile.
    void ask_stop_profile(uint16_t) noexcept;
    // Confirms the currently displayed question.
    void confirm_question(uint16_t) noexcept;
    // Cancels the currently displayed question.
    void cancel_question(uint16_t) noexcept;

    // Opens the samples history page.
    void show_samples(uint16_t) noexcept;

    // Returns to the previous page according to the current navigation context.
    void back(uint16_t) noexcept;

    // Maps UI action types to their corresponding action handler functions.
    static constexpr ActionMapping action_mapping[] =
    {
        {Ui::ActionType::StartProfileSelection,
            &Ui::start_profile_selection},
    
        {Ui::ActionType::EditProfileSelection,
            &Ui::edit_profile_selection},
    
        {Ui::ActionType::SelectProfile,
            &Ui::select_profile},
    
        {Ui::ActionType::StartProfileConfirm,
            &Ui::confirm_start_profile},
    
        {Ui::ActionType::EditProfileConfirm,
            &Ui::confirm_edit_profile},
    
        {Ui::ActionType::Settings,
            &Ui::open_settings},    
       
        {Ui::ActionType::SaveSettings,
            &Ui::save_settings},

        {Ui::ActionType::CancelSettings,
            &Ui::cancel_settings},
            
        {Ui::ActionType::EditBuzzer,
            &Ui::edit_buzzer},
    
        {Ui::ActionType::EditPidKp,
            &Ui::edit_pid_kp},
    
        {Ui::ActionType::EditPidKi,
            &Ui::edit_pid_ki},
    
        {Ui::ActionType::EditPidKd,
            &Ui::edit_pid_kd},
    
        {Ui::ActionType::EditMaxTemperature,
            &Ui::edit_max_temperature},     
            
        {Ui::ActionType::EditPrestepOuts,
            &Ui::edit_prestep_outs},    
            
        {Ui::ActionType::NextStep,
            &Ui::next_step},
    
        {Ui::ActionType::PreviousStep,
            &Ui::previous_step},
    
        {Ui::ActionType::EditSetpoint,
            &Ui::edit_setpoint},
        
        {Ui::ActionType::EditDuration,
            &Ui::edit_duration},
        
        {Ui::ActionType::EditOuts,
            &Ui::edit_outs},            
            
        {Ui::ActionType::SaveProfile,
            &Ui::save_profile},
    
        {Ui::ActionType::CancelProfile,
            &Ui::cancel_profile},
    
        {Ui::ActionType::StopFurnace,
            &Ui::stop_furnace},
    
        {Ui::ActionType::ResetFurnace,
            &Ui::request_reset_furnace},

        {Ui::ActionType::ContinueFurnace,
            &Ui::request_continue_furnace},
            
        {Ui::ActionType::ShowEvents,
            &Ui::show_events},   

        {Ui::ActionType::AskStopProfile,
            &Ui::ask_stop_profile},
        
        {Ui::ActionType::ConfirmQuestion,
            &Ui::confirm_question},
        
        {Ui::ActionType::CancelQuestion,
            &Ui::cancel_question},

    {Ui::ActionType::ShowSamples,
        &Ui::show_samples},

        {Ui::ActionType::Back,
            &Ui::back}
    };
        
    DataAggregator*data_ = nullptr;
    ProfileManager*profiles_ = nullptr;
    SettingManager*settings_ = nullptr;
    Furnace*furnace_ = nullptr;

    Page page_ = Page::Main;

    uint8_t current_step_ = 0;

    // Determines whether profile selection is used for starting or editing a profile.
    ProfileSelectionMode profile_selection_mode_ =
        ProfileSelectionMode::Start;

    // Callback used to send commands from the UI to the application.
    CommandCallback command_callback_{nullptr};
    // Context passed to the application command callback.
    void* command_context_{nullptr};
};

} // namespace app