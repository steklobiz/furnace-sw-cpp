// tui.cpp

#include "tui.hpp"

#include <cstdio>
#include <conio.h>

namespace app
{

namespace
{

static constexpr Tui::Label main_labels[] =
{
    {"State:",   0},
    {"Profile:", 1},
    {"Temperature, C:", 2}
};


static constexpr Tui::Button main_buttons[] =
{
    {'s', "Start profile", Ui::ActionType::StartProfileSelection, 0},
    {'e', "Edit profile",  Ui::ActionType::EditProfileSelection, 0}
};


static constexpr Tui::Label monitor_labels[] =
{
    {"State:",             0},
    {"Profile:",           1},
    {"Step:",              2},
    {"Temperature, C:",    3},
    {"Setpoint, C:",       4},
    {"Step elapsed, s:",   5},
    {"Profile elapsed, s:",6},
    {"Power, %:",          7},
    {"Outputs:",           8}
};


static constexpr Tui::Button monitor_buttons[] =
{
    {'s', "Stop", Ui::ActionType::StopFurnace, 0},
    {'r', "Reset", Ui::ActionType::ResetFurnace, 0}
};


static constexpr Tui::Button profile_selection_buttons[] =
{
    {'0', "Profile 0", Ui::ActionType::SelectProfile, 0},
    {'1', "Profile 1", Ui::ActionType::SelectProfile, 0},
    {'2', "Profile 2", Ui::ActionType::SelectProfile, 0},
    {'3', "Profile 3", Ui::ActionType::SelectProfile, 0},
    {'4', "Profile 4", Ui::ActionType::SelectProfile, 0},
    {'5', "Profile 5", Ui::ActionType::SelectProfile, 0},
    {'6', "Profile 6", Ui::ActionType::SelectProfile, 0},
    {'7', "Profile 7", Ui::ActionType::SelectProfile, 0},
    {'8', "Profile 8", Ui::ActionType::SelectProfile,0},
    {'9', "Profile 9", Ui::ActionType::SelectProfile, 0},
    {'b', "Back",    Ui::ActionType::Back, 0}
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

    // ProfileEditor
    {
        nullptr,
        0,
        nullptr,
        0
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
        nullptr,
        0,
        nullptr,
        0
    }
};

} // namespace


const DataItem<uint16_t> Tui::null_item_{};


void Tui::init(Ui& ui) noexcept
{
    ui_ = &ui;

    rendered_page_ = Ui::Page::Count;
    page_rendered_ = false;
}


void Tui::process() noexcept
{
    process_input();

    const auto page = ui_->page();

    if (page != rendered_page_)
    {
        rendered_page_ = page;
        page_rendered_ = false;

        std::printf("\033[2J\033[H");
    }

    const auto page_index =
        static_cast<std::size_t>(page);

    if (page_index >= std::size(page_descriptors))
        return;

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
            rendered_versions_[page_index][label.field] ==
                item.version)
        {
            continue;
        }

        rendered_versions_[page_index][label.field] =
            item.version;

        std::printf(
            "\033[%zu;1H\033[2K%s %u",
            row + 3,
            label.caption,
            static_cast<unsigned>(item.value));
    }

    page_rendered_ = true;
}

void Tui::process_input() noexcept
{
    if (!_kbhit())
    {
        return;
    }

    const char key =
        static_cast<char>(_getch());

    const auto page = ui_->page();

    const std::size_t page_index =
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

        if (button.key == key)
        {
            ui_->execute({
                button.action,
                0
            });

            return;
        }
    }
}

} // namespace app