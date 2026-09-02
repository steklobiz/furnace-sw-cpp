// ui.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "data_aggregator.hpp"
#include "profiles.hpp"
#include "settings.hpp"

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
        ProfileEditorOuts,
        ProfileEditorVals,        
        Monitor,
        Result,
        Events,

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
    Page page() const noexcept;
    
    // Returns the current value of a field on the specified page.
    uint16_t get_field(
        Ui::Page page,
        uint8_t field) const noexcept;
        
    // Returns the current profile being edited.
    const Profile& get_edit_profile() const noexcept;
    // Returns the current settings being edited.
    const Settings& get_edit_settings() const noexcept;
    
    // Returns an event by its position relative to the newest event.
    const DataAggregator::Event&
        event_from_newest(std::size_t index) const noexcept;

    // Returns the number of stored events.    
    std::size_t event_count() const noexcept;

    // Returns the currently selected profile step.
    uint8_t current_step() const noexcept;
    
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


    void start_profile_selection(uint16_t) noexcept;
    void edit_profile_selection(uint16_t) noexcept;
    void select_profile(uint16_t) noexcept;
    
    void confirm_start_profile(uint16_t) noexcept;
    void confirm_edit_profile(uint16_t) noexcept;
    
    void open_settings(uint16_t) noexcept;
    void save_settings(uint16_t) noexcept;
    void cancel_settings(uint16_t) noexcept;

    void edit_buzzer(uint16_t value) noexcept;
    void edit_pid_kp(uint16_t value) noexcept;
    void edit_pid_ki(uint16_t value) noexcept;
    void edit_pid_kd(uint16_t value) noexcept;
    void edit_max_temperature(uint16_t value) noexcept;
        
    void next_step(uint16_t) noexcept;
    void previous_step(uint16_t) noexcept;

    void edit_setpoint(uint16_t  value) noexcept;
    void edit_duration(uint16_t  value) noexcept;
    void edit_outs(uint16_t  value) noexcept;
        
    void save_profile(uint16_t) noexcept;
    void cancel_profile(uint16_t) noexcept;
    
    void stop_furnace(uint16_t) noexcept;
    // Requests the application to reset the furnace.    
    void request_reset_furnace(uint16_t) noexcept;
    // Requests the application to continue the furnace process.
    void request_continue_furnace(uint16_t argument) noexcept;
    void show_events(uint16_t) noexcept;
    
    void back(uint16_t) noexcept;

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
            
        {Ui::ActionType::Back,
            &Ui::back}
    };
        
    DataAggregator* data_;
    ProfileManager* profiles_;
    SettingManager* settings_;
    Furnace* furnace_;

    Page page_ = Page::Main;

    uint8_t current_step_ = 0;
    
    ProfileSelectionMode profile_selection_mode_ =
    ProfileSelectionMode::Start;
    
    CommandCallback command_callback_{nullptr};
    void* command_context_{nullptr};
};

} // namespace app