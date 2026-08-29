// tui.cpp

#include "tui.hpp"

#include <cstdio>
#include <conio.h>
#include <limits>

namespace app
{

namespace
{
    
// Lfayout constants
static constexpr std::size_t profile_editor_content_row = 3;
static constexpr std::size_t profile_editor_input_row   = 8;
static constexpr std::size_t profile_editor_button_row  = 10;
    
static constexpr Tui::Label main_labels[] =
{
    {"State:",   0},
    {"Profile:", 1},
    {"Temperature, C:", 2}
};


static constexpr Tui::Button main_buttons[] =
{
    {'s', "Start profile", Ui::ActionType::StartProfileSelection,0},
    {'e', "Edit profile",  Ui::ActionType::EditProfileSelection, 0},
    {'t', "Settings",      Ui::ActionType::Settings,             0},     {'v', "Events",        Ui::ActionType::ShowEvents,           0}
};


static constexpr Tui::Label monitor_labels[] =
{
    {"State:",             0},
    {"Profile:",           1},
    {"Step:",              2},
    {"Step type:",         3},
    {"Temperature, C:",    4},
    {"Setpoint, C:",       5},
    {"Step elapsed, s:",   6},
    {"Profile elapsed, s:",7},
    {"Power, %:",          8},
    {"Outputs:",           9}
};


static constexpr Tui::Button monitor_buttons[] =
{
    {'s', "Stop", Ui::ActionType::StopFurnace, 0},
    {'q', "Back", Ui::ActionType::Back, 0}

};


static constexpr Tui::Button profile_selection_buttons[] =
{
    {'0', "Profile 0", Ui::ActionType::SelectProfile, 0},
    {'1', "Profile 1", Ui::ActionType::SelectProfile, 1},
    {'2', "Profile 2", Ui::ActionType::SelectProfile, 2},
    {'3', "Profile 3", Ui::ActionType::SelectProfile, 3},
    {'4', "Profile 4", Ui::ActionType::SelectProfile, 4},
    {'5', "Profile 5", Ui::ActionType::SelectProfile, 5},
    {'6', "Profile 6", Ui::ActionType::SelectProfile, 6},
    {'7', "Profile 7", Ui::ActionType::SelectProfile, 7},
    {'8', "Profile 8", Ui::ActionType::SelectProfile,8},
    {'9', "Profile 9", Ui::ActionType::SelectProfile, 9},
    {'q', "Back",    Ui::ActionType::Back, 0}
};
/*
static constexpr Tui::Label profile_editor_labels[] =
{
    {"Profile:", 0},
    {"Step:", 1},
    {"Setpoint, C:", 2},
    {"Duration, s:", 3},
    {"Flags:", 4}
};
*/
static constexpr Tui::Label settings_labels[] =
{
    {"Buzzer:",              0},
    {"PID Kp:",              1},
    {"PID Ki:",              2},
    {"PID Kd:",              3},
    {"Max temperature, C:",  4}
};

static constexpr Tui::Button settings_buttons[] =
{
{'b', "Edit buzzer", Ui::ActionType::EditBuzzer,
     0, true},
{'p', "Edit PID Kp", Ui::ActionType::EditPidKp,
     0, true},
{'i', "Edit PID Ki", Ui::ActionType::EditPidKi,
     0, true},
{'d', "Edit PID Kd", Ui::ActionType::EditPidKd,
     0, true},
{'m', "Edit max temperature", Ui::ActionType::EditMaxTemperature,
     0, true},
{'q', "Back", Ui::ActionType::Back, 0},
};

static constexpr Tui::Button profile_editor_buttons[] =
{
    {'e', "Edit setpoint", Ui::ActionType::EditSetpoint, 0, true},
    {'d', "Edit duration", Ui::ActionType::EditDuration, 0, true},
    {'f', "Edit flags",    Ui::ActionType::EditFlags,    0, true},
    {'n', "Next",          Ui::ActionType::NextStep,     0},
    {'p', "Previous",      Ui::ActionType::PreviousStep, 0},
    {'s', "Save",          Ui::ActionType::SaveProfile,  0},
    {'c', "Cancel",        Ui::ActionType::CancelProfile, 0}
};

static constexpr Tui::Label result_labels[] =
{
    {"State:", 0},
    {"Temperature, C:", 1}
};

static constexpr Tui::Button result_buttons[] =
{
    {'r', "Reset", Ui::ActionType::ResetFurnace, 0}

};

static constexpr Tui::Button events_buttons[] =
{
    {'q', "Back", Ui::ActionType::Back, 0}
};

static constexpr Tui::PageDescriptor page_descriptors[] =
{
    // Main
    {
        main_labels,
        std::size(main_labels),
        main_buttons,
        std::size(main_buttons)
    },

    // ProfileSelection
    {
        nullptr,
        0,
        profile_selection_buttons,
        std::size(profile_selection_buttons)
    },

    {
    settings_labels,
    std::size(settings_labels),
    settings_buttons,
    std::size(settings_buttons)
    },
    
    // ProfileEditor
    {
        nullptr,
        0,
        profile_editor_buttons,
        std::size(profile_editor_buttons)
    },

    // Monitor
    {
        monitor_labels,
        std::size(monitor_labels),
        monitor_buttons,
        std::size(monitor_buttons)
    },

    // Result
    {
        result_labels,
        std::size(result_labels),
        result_buttons,
        std::size(result_buttons)
    },
    // Events
{
        nullptr,
        0,
        events_buttons,
        std::size(events_buttons)
    }
};

} // namespace

void Tui::init(Ui& ui) noexcept
{
    ui_ = &ui;

    rendered_page_ = Ui::Page::Count;
    page_rendered_ = false;

    input_mode_ = InputMode::Normal;
    input_action_ = Ui::ActionType::None;
    input_value_ = 0;
    input_has_value_ = false;

    rendered_step_index_ = 0xff;
}

void Tui::process() noexcept
{
    process_input();

    const auto page = ui_->page();

    if (page != rendered_page_)
    {
        rendered_page_ = page;
        page_rendered_ = false;

        // Clear the terminal when switching pages.
        std::printf("\033[2J\033[H");
    }

    const auto page_index =
        static_cast<std::size_t>(page);

    if (page_index >= std::size(page_descriptors))
        return;

    if (page == Ui::Page::ProfileEditor)
    {
        render_profile_editor_page();
        return;
    }

    if (page == Ui::Page::Events)
    {
        render_events_page();
        return;
    }
    
    render_page(
        page_descriptors[page_index],
        page);
}    
    

void Tui::render_page(
    const PageDescriptor& descriptor,
    Ui::Page page) noexcept
{
    if (!page_rendered_)
    {
        std::printf(
            "\033[1;1HPage %u",
            static_cast<unsigned>(page));

        const std::size_t first_button_row =
            descriptor.label_count == 0
                ? 3
                : descriptor.label_count + 4;

        render_buttons(
            descriptor,
            first_button_row);
    }

    const auto page_index =
        static_cast<std::size_t>(page);

    for (std::size_t row = 0;
         row < descriptor.label_count;
         ++row)
    {
        const auto& label =
            descriptor.labels[row];

        const auto& item =
            ui_->get_field(
                page,
                label.field);

        if (page_rendered_ &&
            rendered_values_[page_index][label.field] ==
                item)
        {
            continue;
        }

        rendered_values_[page_index][label.field] =
            item;

        std::printf(
            "\033[%zu;1H\033[2K%s %u",
            row + 3,
            label.caption,
            static_cast<unsigned>(item));
    }

    page_rendered_ = true;
}

void Tui::render_buttons(
    const PageDescriptor& descriptor,
    std::size_t first_button_row) noexcept
{
    for (std::size_t i = 0;
         i < descriptor.button_count;
         ++i)
    {
        const auto& button =
            descriptor.buttons[i];

        std::size_t row =
            first_button_row + i;

        if (descriptor.label_count == 0 &&
            i == descriptor.button_count - 1)
        {
            ++row;
        }

        std::printf(
            "\033[%zu;1H[%c] %s",
            row,
            button.key,
            button.caption);
    }
}

void Tui::render_profile_content() noexcept
{
    const auto& profile =
        ui_->get_edit_profile();

    const auto step_index =
        static_cast<std::size_t>(ui_->current_step());

    if (step_index >= profile.steps.size())
    {
        return;
    }

    const auto& step =
        profile.steps[step_index];

    std::printf(
        "\033[%zu;1H\033[2KStep: %u",
        profile_editor_content_row,
        static_cast<unsigned>(step_index));

    std::printf(
        "\033[%zu;1H\033[2KSetpoint, C: %u",
        profile_editor_content_row + 1,
        static_cast<unsigned>(step.setpoint_c));

    std::printf(
        "\033[%zu;1H\033[2KDuration, s: %u",
        profile_editor_content_row + 2,
        static_cast<unsigned>(step.duration));

    std::printf(
        "\033[%zu;1H\033[2KFlags: %u",
        profile_editor_content_row + 3,
        static_cast<unsigned>(step.flags));
}

void Tui::render_profile_editor_page() noexcept
{
    const auto& profile =
        ui_->get_edit_profile();

    const auto step_index =
        ui_->current_step();

    if (step_index >= profile.steps.size())
    {
        return;
    }

    const auto& step =
        profile.steps[step_index];

    if (!page_rendered_)
    {
        std::printf(
            "\033[1;1H\033[2KProfile Editor");

        // Clear the reserved numeric-input line.
        std::printf(
            "\033[%zu;1H\033[2K",
            profile_editor_input_row);

        rendered_step_index_ = 0xff;
    }

    if (!page_rendered_ ||
        step_index != rendered_step_index_ ||
        step != rendered_step_)
    {
        render_profile_content();

        rendered_step_index_ = step_index;
        rendered_step_ = step;
    }

    if (!page_rendered_)
    {
        const auto& descriptor =
            page_descriptors[
                static_cast<std::size_t>(
                    Ui::Page::ProfileEditor)];
    
        render_buttons(
            descriptor,
            profile_editor_button_row);
    }

    page_rendered_ = true;
}

void Tui::process_input() noexcept
{
    if (input_mode_ == InputMode::Numeric)
    {
        process_numeric_input();
        return;
    }

    if (!_kbhit())
    {
        return;
    }

    const char key =
        static_cast<char>(_getch());

    const auto page =
        ui_->page();

    const auto page_index =
        static_cast<std::size_t>(page);

    if (page_index >= std::size(page_descriptors))
    {
        return;
    }

    const auto& descriptor =
        page_descriptors[page_index];

    for (std::size_t i = 0;
         i < descriptor.button_count;
         ++i)
    {
        const auto& button =
            descriptor.buttons[i];

        if (button.key != key)
        {
            continue;
        }

        if (button.numeric_input)
        {
            input_mode_ = InputMode::Numeric;
            input_action_ = button.action;
            input_value_ = 0;
            input_has_value_ = false;

        std::printf(
            "\033[%zu;1H\033[2KEnter value: ",
            profile_editor_input_row);
    
            return;
        }

        ui_->execute({
            button.action,
            button.argument
        });

        return;
    }
}


void Tui::process_numeric_input() noexcept
{
    if (!_kbhit())
    {
        return;
    }

    const int key = _getch();

    if (key >= '0' && key <= '9')
    {
        const auto digit =
            static_cast<uint16_t>(key - '0');

        const auto max =
            std::numeric_limits<uint16_t>::max();

        if (input_value_ <=
            static_cast<uint16_t>((max - digit) / 10))
        {
            input_value_ =
                static_cast<uint16_t>(
                    input_value_ * 10 + digit);

            input_has_value_ = true;
        }
    }
    else if (key == '\b')
    {
        input_value_ =
            static_cast<uint16_t>(
                input_value_ / 10);

        input_has_value_ =
            input_value_ != 0;
    }
    else if (key == '\r')
    {
        if (!input_has_value_)
        {
            return;
        }
    
        ui_->execute({
            input_action_,
            input_value_
        });
    
        input_mode_ = InputMode::Normal;
        input_action_ = Ui::ActionType::None;
        input_value_ = 0;
        input_has_value_ = false;
    
        // Clear numeric input prompt.
        std::printf(
            "\033[%zu;1H\033[2K",
            profile_editor_input_row);
    }
    else if (key == '\x1b')
    {
        input_mode_ = InputMode::Normal;
        input_action_ = Ui::ActionType::None;
        input_value_ = 0;
        input_has_value_ = false;
    
        // Clear numeric input prompt.
        std::printf(
            "\033[%zu;1H\033[2K",
            profile_editor_input_row);
    }
    if (input_mode_ == InputMode::Numeric)
    {
        std::printf(
            "\033[%zu;1H\033[2KEnter value: %u",
            profile_editor_input_row,
            static_cast<unsigned>(input_value_));
    }
}

const char* data_source_name(
    DataSource source) noexcept
{
    switch (source)
    {
    case DataSource::TcParser:
        return "TcParser";

    case DataSource::Furnace:
        return "Furnace";

    case DataSource::Profile:
        return "Profile";

    case DataSource::Setting:
        return "Setting";

    case DataSource::Alarm:
        return "Alarm";

    case DataSource::Count:
        break;
    }

    return "Unknown";
}

const char* notification_type_name(
    NotificationType type) noexcept
{
    switch (type)
    {
    case NotificationType::DataReady:
        return "DataReady";

    case NotificationType::Error:
        return "Error";

    case NotificationType::StepStarted:
        return "StepStarted";

    case NotificationType::ProfileStarted:
        return "ProfileStarted";

    case NotificationType::ProfileFinished:
        return "ProfileFinished";

    case NotificationType::EditProfileChanged:
        return "EditProfileChanged";

    case NotificationType::StartProfileChanged:
        return "StartProfileChanged";

    case NotificationType::SettingsChanged:
        return "SettingsChanged";
    }

    return "Unknown";
}


void Tui::render_events_page() noexcept
{
    if (!page_rendered_)
    {
        std::printf(
            "\033[1;1H\033[2KEvents");

        std::printf(
            "\033[3;1H\033[2KTime       Source     Event              ID");
        
        std::printf(
            "\033[4;1H\033[2K"
            "----------------------------------------------------");
    
        const auto& descriptor =
            page_descriptors[
                static_cast<std::size_t>(Ui::Page::Events)];

        render_buttons(
            descriptor,
            16);
    }

    const auto count =
        ui_->event_count();

    constexpr std::size_t first_row = 5;
    constexpr std::size_t max_rows = 10;

    for (std::size_t i = 0;
         i < max_rows;
         ++i)
    {
        const std::size_t row =
            first_row + i;

        std::printf(
            "\033[%zu;1H\033[2K",
            row);

        if (i >= count)
        {
            continue;
        }

        const auto& event =
            ui_->event_from_newest(i);

        std::printf(
            "%02u:%02u:%02u  %-10s %-18s %u",
            static_cast<unsigned>(
                event.elapsed_s / 3600),
            static_cast<unsigned>(
                (event.elapsed_s / 60) % 60),
            static_cast<unsigned>(
                event.elapsed_s % 60),
            data_source_name(event.source),
            notification_type_name(event.type),
            static_cast<unsigned>(event.argument));
    }

    page_rendered_ = true;
}


} // namespace app