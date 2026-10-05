// ui.cpp

#include "ui.hpp"

namespace app
{

const Ui::Transition Ui::transitions_[] =
{
    // -------------------------------------------------------------------------
    // Main / Brief
    // -------------------------------------------------------------------------

    // Idle
    {
        {Context::Main, Mode::Brief},
        Furnace::State::Idle,
        ActionType::Start,
        nullptr,
        {Context::ProfileSelection, Mode::Start}
    },

    {
        {Context::Main, Mode::Brief},
        Furnace::State::Idle,
        ActionType::Edit,
        nullptr,
        {Context::ProfileSelection, Mode::Edit}
    },

    {
        {Context::Main, Mode::Brief},
        Furnace::State::Idle,
        ActionType::Settings,
        nullptr,
        {Context::Settings, Mode::Pid}
    },

    {
        {Context::Main, Mode::Brief},
        Furnace::State::Idle,
        ActionType::Events,
        nullptr,
        {Context::Events, Mode::None}
    },

    // Running
    {
        {Context::Main, Mode::Brief},
        Furnace::State::Running,
        ActionType::Next,
        nullptr,
        {Context::Main, Mode::Detailed}
    },

    {
        {Context::Main, Mode::Brief},
        Furnace::State::Running,
        ActionType::Stop,
        nullptr,
        {Context::Question, Mode::Stop}
    },

    // Finished
    {
        {Context::Main, Mode::Brief},
        Furnace::State::Finished,
        ActionType::Reset,
        &Ui::reset_furnace,
        {Context::Main, Mode::Brief}
    },

    // Stopped
    {
        {Context::Main, Mode::Brief},
        Furnace::State::Stopped,
        ActionType::Reset,
        &Ui::reset_furnace,
        {Context::Main, Mode::Brief}
    },

    // Error
    {
        {Context::Main, Mode::Brief},
        Furnace::State::Error,
        ActionType::Reset,
        &Ui::reset_furnace,
        {Context::Main, Mode::Brief}
    },


    // -------------------------------------------------------------------------
    // Main / Detailed
    // -------------------------------------------------------------------------

    // Running
    {
        {Context::Main, Mode::Detailed},
        Furnace::State::Running,
        ActionType::Previous,
        nullptr,
        {Context::Main, Mode::Brief}
    },


    // -------------------------------------------------------------------------
    // ProfileSelection / Start
    // -------------------------------------------------------------------------

    // Idle
    {
        {Context::ProfileSelection, Mode::Start},
        Furnace::State::Idle,
        ActionType::Select,
        &Ui::select_profile_to_start,
        {Context::Main, Mode::Brief}
    },

    {
        {Context::ProfileSelection, Mode::Start},
        Furnace::State::Idle,
        ActionType::Next,
        &Ui::next_profile_page,
        {Context::None, Mode::None}
    },

    {
        {Context::ProfileSelection, Mode::Start},
        Furnace::State::Idle,
        ActionType::Previous,
        &Ui::previous_profile_page,
        {Context::None, Mode::None}
    },


    // -------------------------------------------------------------------------
    // ProfileSelection / Edit
    // -------------------------------------------------------------------------

    // Idle
    {
        {Context::ProfileSelection, Mode::Edit},
        Furnace::State::Idle,
        ActionType::Select,
        &Ui::select_profile_to_edit,
        {Context::Edit, Mode::None}
    },

    {
        {Context::ProfileSelection, Mode::Edit},
        Furnace::State::Idle,
        ActionType::Next,
        &Ui::next_profile_page,
        {Context::None, Mode::None}
    },

    {
        {Context::ProfileSelection, Mode::Edit},
        Furnace::State::Idle,
        ActionType::Previous,
        &Ui::previous_profile_page,
        {Context::None, Mode::None}
    },

    // -------------------------------------------------------------------------
    // Edit / None
    // -------------------------------------------------------------------------

    {
        {Context::Edit, Mode::None},
        Furnace::State::Idle,
        ActionType::Next,
        &Ui::next_edit_step,
        {Context::None, Mode::None}
    },

    {
        {Context::Edit, Mode::None},
        Furnace::State::Idle,
        ActionType::Previous,
        &Ui::previous_edit_step,
        {Context::None, Mode::None}
    },

    {
        {Context::Edit, Mode::None},
        Furnace::State::Idle,
        ActionType::SetSetpoint,
        &Ui::edit_setpoint,
        {Context::None, Mode::None}
    },

    {
        {Context::Edit, Mode::None},
        Furnace::State::Idle,
        ActionType::SetDuration,
        &Ui::edit_duration,
        {Context::None, Mode::None}
    },

    {
        {Context::Edit, Mode::None},
        Furnace::State::Idle,
        ActionType::SetOutputs,
        &Ui::edit_outs,
        {Context::None, Mode::None}
    },

    {
        {Context::Edit, Mode::None},
        Furnace::State::Idle,
        ActionType::Confirm,
        &Ui::save_edit,
        {Context::Main, Mode::Brief}
    },

    {
        {Context::Edit, Mode::None},
        Furnace::State::Idle,
        ActionType::Cancel,
        &Ui::cancel_edit,
        {Context::Main, Mode::Brief}
    },

    // -------------------------------------------------------------------------
    // Question / Stop
    // -------------------------------------------------------------------------

    // Running
    {
        {Context::Question, Mode::Stop},
        Furnace::State::Running,
        ActionType::Confirm,
        &Ui::stop_furnace,
        {Context::Main, Mode::Brief}
    },

    {
        {Context::Question, Mode::Stop},
        Furnace::State::Running,
        ActionType::Cancel,
        nullptr,
        {Context::Main, Mode::Brief}
    },
};

// -----------------------------------------------------------------
// Public API
// -----------------------------------------------------------------

void
Ui::init(
    DataAggregator& data,
    Furnace& furnace,
    ProfileManager& profiles,
    SettingManager& settings) noexcept
{
    data_ = &data;
    furnace_ = &furnace;
    profiles_ = &profiles;
    settings_ = &settings;

    position_ = {
        Context::Main,
        Mode::Brief
    };
}

void
Ui::execute(const Action& action) noexcept
{
    const Transition* transition = find_transition(action);

    if (transition == nullptr)
        return;

    bool success = true;

    if (transition->handler != nullptr)
        success = (this->*transition->handler)(action);

    if (success && transition->to.context != Context::None)
        position_ = transition->to;
}

// ------------------------- Getters -------------------------------

Ui::Position Ui::position() const noexcept
{
    return position_;
}


Furnace::State
Ui::state() const noexcept
{
    return furnace_->state();
}

uint8_t
Ui::profile_page() const noexcept
{
    return profile_page_;
}

uint8_t Ui::edit_step() const noexcept
{
    return edit_step_;
}

// -----------------------------------------------------------------
// Private helpers
// -----------------------------------------------------------------


const Ui::Transition*
Ui::find_transition(const Action& action) const noexcept
{
    const Furnace::State state = furnace_->state();

    for (const Transition& transition : transitions_)
    {
        if (transition.from.context != position_.context)
            continue;

        if (transition.from.mode != position_.mode)
            continue;

        if (transition.state != state)
            continue;

        if (transition.action != action.type)
            continue;

        return &transition;
    }

    return nullptr;
}

// --------------------- Transition handlers -----------------------

bool Ui::stop_furnace(const Action&) noexcept
{
    furnace_->stop();
    return true;
}

bool Ui::reset_furnace(const Action&) noexcept
{
    furnace_->reset();
    return true;
}
bool Ui::select_profile_to_start(const Action& action) noexcept
{
    const uint8_t profile_id = static_cast<uint8_t>(
        profile_page_ * ProfilesPerPage + action.argument);

    if (!profiles_->select_for_start(profile_id))
        return false;

    furnace_->start();
    return true;
}

bool Ui::select_profile_to_edit(const Action& action) noexcept
{
    const uint8_t profile_id = static_cast<uint8_t>(
        profile_page_ * ProfilesPerPage + action.argument);

    if (!profiles_->select_for_edit(profile_id))
        return false;

    edit_step_ = 0;
    return true;
}

bool Ui::next_profile_page(const Action&) noexcept
{
    ++profile_page_;

    if (profile_page_ >= profile_page_count())
        profile_page_ = 0;

    return true;
}

bool Ui::previous_profile_page(const Action&) noexcept
{
    if (profile_page_ == 0)
        profile_page_ = static_cast<uint8_t>(profile_page_count() - 1);
    else
        --profile_page_;

    return true;
}


bool Ui::next_edit_step(const Action&) noexcept
{
    const uint8_t count = edit_step_count();

    if (count == 0)
        return false;

    ++edit_step_;

    if (edit_step_ >= count)
        edit_step_ = 0;

    return true;
}

bool Ui::previous_edit_step(const Action&) noexcept
{
    const uint8_t count = edit_step_count();

    if (count == 0)
        return false;

    if (edit_step_ == 0)
        edit_step_ = static_cast<uint8_t>(count - 1);
    else
        --edit_step_;

    return true;
}

bool Ui::edit_setpoint(const Action& action) noexcept
{
    return profiles_->set_edit_setpoint(
        edit_step_,
        action.argument);
}

bool Ui::edit_duration(const Action& action) noexcept
{
    return profiles_->set_edit_duration(
        edit_step_,
        action.argument);
}

bool Ui::edit_outs(const Action& action) noexcept
{
    return profiles_->set_edit_outs(
        edit_step_,
        action.argument);
}

bool Ui::save_edit(const Action&) noexcept
{
    return profiles_->save_edit();
}

bool Ui::cancel_edit(const Action&) noexcept
{
    return profiles_->cancel_edit();
}

uint8_t Ui::profile_page_count() const noexcept
{
    const uint8_t count = profiles_->profile_count();

    return static_cast<uint8_t>(
        (count + ProfilesPerPage - 1) / ProfilesPerPage);
}

uint8_t Ui::edit_step_count() const noexcept
{
    const auto& profile = profiles_->edit_profile();

    for (uint8_t i = 0;
         i < config::profiles::max_steps;
         ++i)
    {
        const auto& step = profile.steps[i];

        if (step.setpoint_c == 0 &&
            step.duration == 0)
        {
            return static_cast<uint8_t>(i + 1);
        }
    }

    return config::profiles::max_steps;
}



} // namespace app