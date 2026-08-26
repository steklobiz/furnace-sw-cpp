// ui.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "data_aggregator.hpp"
#include "profiles.hpp"

namespace app
{

class Ui
{
public:

    enum class Page : uint8_t
    {
        Main,
        ProfileSelection,
        ProfileEditor,
        Monitor,
        Result,

        Count
    };


    enum class ActionType : uint8_t
    {
        None,

        StartProfileSelection,
        EditProfileSelection,
        SelectProfile,

        StartProfileConfirm,
        EditProfileConfirm,

        NextStep,
        PreviousStep,

        EditSetpoint,
        EditDuration,
        EditFlags,

        SaveProfile,
        CancelProfile,

        StopFurnace,
        ResetFurnace,

        Back
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
    
    struct Action
    {
        ActionType type = ActionType::None;
        uint16_t argument = 0;
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



    void init(
        DataAggregator& data,
        ProfileManager& profiles,
        Furnace& furnace) noexcept;

    void execute(Action action) noexcept;

    Page page() const noexcept;

    uint16_t get_field(
        Ui::Page page,
        uint8_t field) const noexcept;
    
    const Profile& get_edit_profile() const noexcept;

    uint8_t current_step() const noexcept;


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
    
    void next_step(uint16_t) noexcept;
    void previous_step(uint16_t) noexcept;

    void edit_setpoint(uint16_t  value) noexcept;
    void edit_duration(uint16_t  value) noexcept;
    void edit_flags(uint16_t  value) noexcept;
        
    void save_profile(uint16_t) noexcept;
    void cancel_profile(uint16_t) noexcept;
    
    void stop_furnace(uint16_t) noexcept;
    void reset_furnace(uint16_t) noexcept;
    
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
    
        {Ui::ActionType::NextStep,
            &Ui::next_step},
    
        {Ui::ActionType::PreviousStep,
            &Ui::previous_step},
    
        {Ui::ActionType::EditSetpoint,
            &Ui::edit_setpoint},
        
        {Ui::ActionType::EditDuration,
            &Ui::edit_duration},
        
        {Ui::ActionType::EditFlags,
            &Ui::edit_flags},            
            
        {Ui::ActionType::SaveProfile,
            &Ui::save_profile},
    
        {Ui::ActionType::CancelProfile,
            &Ui::cancel_profile},
    
        {Ui::ActionType::StopFurnace,
            &Ui::stop_furnace},
    
        {Ui::ActionType::ResetFurnace,
            &Ui::reset_furnace},
    
        {Ui::ActionType::Back,
            &Ui::back}
    };
        
    DataAggregator* data_;
    ProfileManager* profiles_;
    Furnace* furnace_;

    Page page_ = Page::Main;

    uint8_t current_step_ = 0;
    
    ProfileSelectionMode profile_selection_mode_ =
    ProfileSelectionMode::Start;
};

} // namespace app