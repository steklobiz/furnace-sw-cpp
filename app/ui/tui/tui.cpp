// tui.cpp

#include "tui.hpp"

#include <cstdio>
#include <conio.h>
#include <limits>

namespace app
{

namespace
{
    
// Layout constants
constexpr std::size_t profile_editor_content_row = 3;
constexpr std::size_t profile_editor_input_row   = 8;
constexpr std::size_t profile_editor_button_row  = 10;

constexpr std::size_t settings_content_row  = 3;
constexpr std::size_t settings_input_row   = 9;
constexpr std::size_t settings_button_row   = 10;

constexpr std::size_t question_button_row = 6;

// Page title row
constexpr std::size_t title_row = 1;

// Starting row for fields.
constexpr std::size_t first_field_row = 3;
// Starting row for events and samples.
constexpr std::size_t first_data_row = 5;
constexpr std::size_t column_header_row = 3;
constexpr std::size_t divider_row = 4;

constexpr std::size_t page_count =
    static_cast<std::size_t>(Ui::Page::Count);

constexpr Tui::Label main_labels[] =
{
    {"State:",   0},
    {"Profile:", 1},
    {"Temperature, C:", 2}
};


constexpr Tui::Button main_buttons[] =
{
    {'s', "Start profile", Ui::ActionType::StartProfileSelection,0},
    {'e', "Edit profile",  Ui::ActionType::EditProfileSelection, 0},
    {'t', "Settings",      Ui::ActionType::Settings,             0},
    {'v', "Events",        Ui::ActionType::ShowEvents,           0},
    {'a', "Samples",       Ui::ActionType::ShowSamples,          0}
};


constexpr Tui::Label monitor_labels[] =
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


constexpr Tui::Button monitor_buttons[] =
{
    {'s', "Stop", Ui::ActionType::AskStopProfile, 0},
    {'q', "Back", Ui::ActionType::Back, 0}

};


constexpr Tui::Button profile_selection_buttons[] =
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

constexpr Tui::Label settings_labels[] =
{
    {"Buzzer:",              0},
    {"PID Kp:",              1},
    {"PID Ki:",              2},
    {"PID Kd:",              3},
    {"Max temperature, C:",  4},
    {"Prestep outputs:",  5}
};

constexpr Tui::Button settings_buttons[] =
{
    {'b', "Edit buzzer",
        Ui::ActionType::EditBuzzer, 0, true},

    {'p', "Edit PID Kp",
        Ui::ActionType::EditPidKp, 0, true},

    {'i', "Edit PID Ki",
        Ui::ActionType::EditPidKi, 0, true},

    {'d', "Edit PID Kd",
        Ui::ActionType::EditPidKd, 0, true},

    {'m', "Edit max temperature",
        Ui::ActionType::EditMaxTemperature, 0, true},

    {'o', "Edit prestep outputs",
        Ui::ActionType::EditPrestepOuts, 0, true},
        
    {'s', "Save",
        Ui::ActionType::SaveSettings, 0},
    
    {'c', "Cancel",
        Ui::ActionType::CancelSettings, 0}
};

constexpr Tui::Button profile_editor_buttons[] =
{
    {'e', "Edit setpoint", Ui::ActionType::EditSetpoint, 0, true},
    {'d', "Edit duration", Ui::ActionType::EditDuration, 0, true},
    {'f', "Edit outputs",    Ui::ActionType::EditOuts,    0, true},
    {'n', "Next",          Ui::ActionType::NextStep,     0},
    {'p', "Previous",      Ui::ActionType::PreviousStep, 0},
    {'s', "Save",          Ui::ActionType::SaveProfile,  0},
    {'c', "Cancel",        Ui::ActionType::CancelProfile, 0}
};

constexpr Tui::Label result_labels[] =
{
    {"State:", 0},
    {"Temperature, C:", 1},
    {"Outputs:",           9}

};

constexpr Tui::Button result_buttons[] =
{
    {'r', "Reset", Ui::ActionType::ResetFurnace, 0}

};

// Common buttons set for Events and Samples
constexpr Tui::Button history_buttons[] =
{
    {'q', "Back", Ui::ActionType::Back, 0}
};

constexpr Tui::Button question_buttons[] =
{
    {'o', "OK",     Ui::ActionType::ConfirmQuestion, 0},
    {'c', "Cancel", Ui::ActionType::CancelQuestion,  0}
};


} // namespace

const Tui::PageDescriptor Tui::page_descriptors[page_count] =
{
    // Main
    {
        "Main",
        main_labels,
        std::size(main_labels),
        main_buttons,
        std::size(main_buttons),
        nullptr
    },

    // ProfileSelection
    {
        "Profile Selection",
        nullptr,
        0,
        profile_selection_buttons,
        std::size(profile_selection_buttons),
        nullptr
    },

    // Settings
    {
        "Settings",
        settings_labels,
        std::size(settings_labels),
        settings_buttons,
        std::size(settings_buttons),
        &Tui::render_settings_page
    },

    // ProfileEditor
    {
        "Profile Editor",
        nullptr,
        0,
        profile_editor_buttons,
        std::size(profile_editor_buttons),
        &Tui::render_profile_editor_page
    },

    // Monitor
    {
        "Monitor",
        monitor_labels,
        std::size(monitor_labels),
        monitor_buttons,
        std::size(monitor_buttons),
        nullptr
    },

    // Result
    {
        "Result",
        result_labels,
        std::size(result_labels),
        result_buttons,
        std::size(result_buttons),
        nullptr
    },

    // Events
    {
        "Events",
        nullptr,
        0,
        history_buttons,
        std::size(history_buttons),
        &Tui::render_events_page
    },

    // Samples
    {
        "Samples",
        nullptr,
        0,
        history_buttons,
        std::size(history_buttons),
        &Tui::render_samples_page
    },

    // Question
    {
        "Question",
        nullptr,
        0,
        question_buttons,
        std::size(question_buttons),
        &Tui::render_question_page
    }
};


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

    const auto page =
        ui_->page();

    if (page != rendered_page_)
    {
        rendered_page_ = page;
        page_rendered_ = false;

        clear_screen();
    }

    const auto page_index =
        static_cast<std::size_t>(page);

    if (page_index >= std::size(page_descriptors))
    {
        return;
    }

    const auto& descriptor =
        page_descriptors[page_index];

    if (descriptor.render != nullptr)
    {
        (this->*descriptor.render)();
    }
    else
    {
        render_page(
            descriptor,
            page);
    }
}

    void Tui::render_page(
        const PageDescriptor& descriptor,
        Ui::Page page) noexcept
{
    const auto page_index =
        static_cast<std::size_t>(page);

    if (!page_rendered_)
    {
        clear_line(title_row);
        std::printf("%s", descriptor.name);

        const std::size_t first_button_row =
            descriptor.label_count == 0
                ? first_field_row
                : first_field_row +
                    descriptor.label_count + 1;

        render_buttons(
            descriptor,
            first_button_row);
    }

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

        clear_line(first_field_row + row);

        std::printf(
            "%s %u",
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
            descriptor.button_count == 1)
        {
            ++row;
        }

        clear_line(row);

        std::printf(
            "[%c] %s",
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

    clear_line(profile_editor_content_row);
    std::printf(
        "Step: %u",
        static_cast<unsigned>(step_index));

    clear_line(profile_editor_content_row + 1);
    std::printf(
        "Setpoint, C: %u",
        static_cast<unsigned>(step.setpoint_c));

    clear_line(profile_editor_content_row + 2);
    std::printf(
        "Duration, s: %u",
        static_cast<unsigned>(step.duration));

    clear_line(profile_editor_content_row + 3);
    std::printf(
        "Flags: %u",
        static_cast<unsigned>(step.outs));
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

    const auto& descriptor =
        page_descriptors[
            static_cast<std::size_t>(
                Ui::Page::ProfileEditor)];

    if (!page_rendered_)
    {
        clear_line(title_row);
        std::printf("%s", descriptor.name);

        // Clear the reserved numeric-input line.
        clear_line(profile_editor_input_row);

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
        render_buttons(
            descriptor,
            profile_editor_button_row);
    }

    page_rendered_ = true;
}

    void Tui::render_settings_page() noexcept
{
    const auto& settings =
        ui_->get_edit_settings();

    constexpr auto page_index =
        static_cast<std::size_t>(Ui::Page::Settings);

    const auto& descriptor =
        page_descriptors[page_index];

    if (!page_rendered_)
    {
        clear_line(title_row);
        std::printf("%s", descriptor.name);

        render_buttons(
            descriptor,
            settings_button_row);
    }

    const uint16_t values[] =
    {
        settings.buzzer_state,
        settings.pid_kp,
        settings.pid_ki,
        settings.pid_kd,
        settings.max_temperature_c,
        settings.prestep_outs
    };

    for (std::size_t i = 0;
         i < std::size(values);
         ++i)
    {
        if (page_rendered_ &&
            rendered_values_[page_index][i] == values[i])
        {
            continue;
        }

        rendered_values_[page_index][i] = values[i];

        const auto& label =
            descriptor.labels[i];

        clear_line(settings_content_row + i);

        std::printf(
            "%s %u",
            label.caption,
            static_cast<unsigned>(values[i]));
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
    const auto page = ui_->page();

    const auto input_row =
        page == Ui::Page::Settings
            ? settings_input_row
            : profile_editor_input_row;

    if (!_kbhit())
    {
        return;
    }

    const int key = _getch();

    if (key >= '0' && key <= '9')
    {
        const auto digit =
            static_cast<uint16_t>(key - '0');

        constexpr auto max =
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
        clear_line(input_row);
    }
    else if (key == '\x1b')
    {
        input_mode_ = InputMode::Normal;
        input_action_ = Ui::ActionType::None;
        input_value_ = 0;
        input_has_value_ = false;

        // Clear numeric input prompt.
        clear_line(input_row);
    }

    if (input_mode_ == InputMode::Numeric)
    {
        clear_line(input_row);

        std::printf(
            "Enter value: %u",
            static_cast<unsigned>(input_value_));
    }
}

static const char* data_source_name(
    const DataSource source) noexcept
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

static const char* notification_type_name(
    const NotificationType type) noexcept
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

    case NotificationType::ProfileStopped:
        return "ProfileStopped";        
        
    case NotificationType::EditProfileChanged:
        return "EditProfileChanged";

    case NotificationType::StartProfileChanged:
        return "StartProfileChanged";

    case NotificationType::SettingsChanged:
        return "SettingsChanged";

    case NotificationType::OutputSet:
        return "OutputSet";
        
    case NotificationType::OutputReset:
        return "OutputReset";
    }
    
    return "Unknown";
}

    void Tui::render_events_page() noexcept
{
    constexpr auto page_index =
        static_cast<std::size_t>(Ui::Page::Events);

    const auto& descriptor =
        page_descriptors[page_index];

    if (!page_rendered_)
    {
        clear_line(title_row);
        std::printf("%s", descriptor.name);

        clear_line(column_header_row);
        std::printf(
            "Time       Source     Event              ID");

        clear_line(divider_row);
        std::printf(
            "----------------------------------------------------");

        render_buttons(
            descriptor,
            first_data_row + MaxEventsPerPage + 1);
    }

    const auto count =
        ui_->event_count();

    for (std::size_t i = 0;
         i < MaxEventsPerPage;
         ++i)
    {
        const std::size_t row =
            first_data_row + i;

        clear_line(row);

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


    void Tui::render_samples_page() noexcept
{
    constexpr auto page_index =
        static_cast<std::size_t>(Ui::Page::Samples);

    const auto& descriptor =
        page_descriptors[page_index];

    if (!page_rendered_)
    {
        clear_line(title_row);
        std::printf("%s", descriptor.name);

        clear_line(column_header_row);
        std::printf(
            "Time       Temperature    Power");

        clear_line(divider_row);
        std::printf(
            "--------------------------------------");

        render_buttons(
            descriptor,
            first_data_row + MaxSamplesPerPage + 1);
    }

    const auto count =
        ui_->sample_count();

    for (std::size_t i = 0;
         i < MaxSamplesPerPage;
         ++i)
    {
        const std::size_t row =
            first_data_row + i;

        clear_line(row);

        if (i >= count)
        {
            continue;
        }

        const auto& sample =
            ui_->sample_from_newest(i);

        std::printf(
            "%02u:%02u:%02u  %10d C    %3u %%",
            static_cast<unsigned>(
                sample.elapsed_s / 3600),
            static_cast<unsigned>(
                (sample.elapsed_s / 60) % 60),
            static_cast<unsigned>(
                sample.elapsed_s % 60),
            static_cast<int>(
                sample.temperature),
            static_cast<unsigned>(
                sample.output));
    }

    page_rendered_ = true;
}

    void Tui::render_question_page() noexcept
{
    if (page_rendered_)
        return;

    const auto& descriptor =
        page_descriptors[
            static_cast<std::size_t>(Ui::Page::Question)];

    clear_line(title_row);
    std::printf("%s", descriptor.name);

    clear_line(first_field_row);
    std::printf("Stop current profile?");

    render_buttons(
        descriptor,
        question_button_row);

    page_rendered_ = true;
}

void Tui::move(
    const std::size_t row,
    const std::size_t col) noexcept
{
    std::printf("\033[%zu;%zuH", row, col);
}

void Tui::clear_line(const std::size_t row) noexcept
{
    move(row, 1);
    std::printf("\033[2K");
}

void Tui::clear_screen() noexcept
{
    std::printf("\033[2J\033[H");
}

} // namespace app