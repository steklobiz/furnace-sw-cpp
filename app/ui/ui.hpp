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

    const DataItem<uint16_t>& get_field(
        Page page,
        uint8_t field) const noexcept;

    const DataItem<Profile>& get_edit_profile() const noexcept;

    uint8_t current_step() const noexcept;


private:

    void start_profile_selection() noexcept;
    void edit_profile_selection() noexcept;

    void confirm_start_profile(uint8_t profile_id) noexcept;
    void confirm_edit_profile(uint8_t profile_id) noexcept;

    void next_step() noexcept;
    void previous_step() noexcept;

    void save_profile() noexcept;
    void cancel_profile() noexcept;

    void back() noexcept;


    DataAggregator* data_;
    ProfileManager* profiles_;
    Furnace* furnace_;

    Page page_ = Page::Main;

    uint8_t current_step_ = 0;

    static const DataItem<uint16_t> null_item_;
};

} // namespace app