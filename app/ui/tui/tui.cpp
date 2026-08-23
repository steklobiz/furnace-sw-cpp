// tui.cpp

#include "tui.hpp"

#include <cstdio>

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
    {'s', "Start profile", Ui::ActionType::StartProfileSelection},
    {'e', "Edit profile",  Ui::ActionType::EditProfileSelection}
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
    {'s', "Stop", Ui::ActionType::StopFurnace},
    {'r', "Reset", Ui::ActionType::ResetFurnace}
};


static constexpr Tui::PageDescriptor page_descriptors[] =
{
    {
        main_labels,
        std::size(main_labels),
        main_buttons,
        std::size(main_buttons)
    },

    {
        nullptr,
        0,
        nullptr,
        0
    },

    {
        nullptr,
        0,
        nullptr,
        0
    },

    {
        monitor_labels,
        std::size(monitor_labels),
        monitor_buttons,
        std::size(monitor_buttons)
    },

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

        for (std::size_t i = 0;
             i < descriptor.button_count;
             ++i)
        {
            const auto& button =
                descriptor.buttons[i];

            std::printf(
                "\033[%zu;1H[%c] %s",
                descriptor.label_count + i + 4,
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
    // Input handling will be moved here from the old TUI.
}

} // namespace app