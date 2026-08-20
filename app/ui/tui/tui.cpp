// tui.cpp
#include "tui.hpp"

#include <cstdio>
#include <conio.h>

// Terminal (TUI) renderer.
//
// Rendering model:
//   - The application drives this renderer by calling Tui::process()
//     periodically (typically every frame / control-loop tick).
//   - Each call re-reads the currently active page from Ui and renders
//     only the fields of that page; fields of inactive pages are ignored.
//   - Fields are rendered using a "publish only on change" scheme:
//     Ui items carry a monotonic version counter that DataAggregator
//     increments whenever a value changes. Tui remembers the last
//     version it printed for each field, so a field is printed only
//     when its version differs from the last printed one. This keeps
//     the terminal output stable instead of reprinting identical
//     values on every tick.
//   - The *_initialized_ flags force the first render of a page to
//     print every field regardless of its version, ensuring the whole
//     page content appears once when the page is first shown.


namespace app
{

// Binds the renderer to a UI instance.
// Must be called once before process(); the pointer stays valid for
// the lifetime of both objects.
void Tui::init(Ui& ui) noexcept
{
    ui_ = &ui;
}


// Renders the currently active page.
// Called repeatedly by the application. Queries Ui for the current
// page and dispatches to the matching renderer. A page switch in Ui
// takes effect on the next call to process().
void Tui::process() noexcept
{
    process_input();

    const Ui::Page page = ui_->page();

    if (!page_initialized_ ||
        page != rendered_page_)
        
    // Initial page rendering    
    {
        std::printf("\033[2J\033[H");

        rendered_page_ = page;
        page_initialized_ = true;

        switch (page)
        {
            case Ui::Page::Main:
                main_initialized_ = false;
                break;

            case Ui::Page::ProfileSelection:
                render_profile_selection();
                break;

            case Ui::Page::ProfileEditor:
                render_profile_editor();
                break;

            case Ui::Page::Monitor:
                monitor_initialized_ = false;
                break;

            case Ui::Page::Result:
                result_initialized_ = false;
                break;
        }
    }
    
    // Continuous page update  
    switch (page)
    {
        case Ui::Page::Main:
            render_main();
            break;

        case Ui::Page::ProfileSelection:
            // no updates for static page
            break;

        case Ui::Page::ProfileEditor:
            // no updates for static page
            break;

        case Ui::Page::Monitor:
            render_monitor();
            break;

        case Ui::Page::Result:
            render_result();
            break;
    }
}

//------------------------------------------------------
// Helper functions
//------------------------------------------------------


void Tui::process_input() noexcept
{
    if (!_kbhit())
    {
        return;
    }

    const char key = static_cast<char>(_getch());

    switch (ui_->page())
    {
        case Ui::Page::Main:
            for (const auto& button : main_buttons_)
            {
                if (button.key == key)
                {
                    ui_->execute(button.action);
                    return;
                }
            }
            break;
            
        case Ui::Page::ProfileSelection:
        {
            if (key >= '0' && key <= '9')
            {
                const uint8_t selected_profile_id =
                    static_cast<uint8_t>(key - '0');
        
                const Ui::ActionType action =
                    ui_->profile_selection_action();
        
                ui_->execute({
                    action,
                    selected_profile_id
                });
            }
            else if (key == 'b')
            {
                ui_->execute({
                    Ui::ActionType::Back
                });
            }
        
            break;
        }
         
        case Ui::Page::ProfileEditor:
            render_profile_editor();
            break;   
            
        case Ui::Page::Monitor:
            for (const auto& button : monitor_buttons_)
            {
                if (button.key == key)
                {
                    ui_->execute(button.action);
                    return;
                }
            }
            break;
            
        case Ui::Page::Result:
            for (const auto& button : result_buttons_)
            {
                if (button.key == key)
                {
                    ui_->execute(button.action);
                    return;
                }
            }
            break;
    
    }
}


// Renders all fields of the Main page, then marks the page as
// initialized so subsequent renders only print changed values.
void Tui::render_main() noexcept
{
    if (!main_initialized_)
    {
        std::printf("\033[1;1HMain");
    }

    for (std::size_t row = 0;
         row < std::size(main_labels_);
         ++row)
    {
        const auto& label = main_labels_[row];

        const auto field =
            static_cast<Ui::MainField>(label.field);

        const auto& item = ui_->get_field(field);

        const std::size_t index =
            static_cast<std::size_t>(field);

        if (main_initialized_ &&
            main_versions_[index] == item.version)
        {
            continue;
        }

        main_versions_[index] = item.version;

        std::printf(
            "\033[%zu;1H\033[2K%s %u",
            row + 3,
            label.caption,
            static_cast<unsigned>(item.value));
    }

    // Render Main page buttins
    if (!main_initialized_)
    {
        for (std::size_t i = 0;
             i < std::size(main_buttons_);
             ++i)
        {
            const auto& button = main_buttons_[i];

            std::printf(
                "\033[%zu;1H[%c] %s",
                std::size(main_labels_) + i + 4,
                button.key,
                button.caption);
        }
    }

    main_initialized_ = true;
}

void Tui::render_profile_editor() noexcept
{
    std::printf("PROFILE EDITOR\n");
}

// Renders all fields of the Monitor page, then marks the page as
// initialized so subsequent renders only print changed values.
void Tui::render_monitor() noexcept
{
    if (!monitor_initialized_)
    {
        std::printf("\033[1;1HMonitor");
    }

    for (std::size_t row = 0;
         row < std::size(monitor_labels_);
         ++row)
    {
        const auto& label = monitor_labels_[row];

        const auto field =
            static_cast<Ui::MonitorField>(label.field);

        const auto& item = ui_->get_field(field);

        const std::size_t index =
            static_cast<std::size_t>(field);

        if (monitor_initialized_ &&
            monitor_versions_[index] == item.version)
        {
            continue;
        }

        monitor_versions_[index] = item.version;

        std::printf(
            "\033[%zu;1H\033[2K%s %u",
            row + 3,
            label.caption,
            static_cast<unsigned>(item.value));
    }

    // Render Monitor page buttins
    if (!monitor_initialized_)
    {
        for (std::size_t i = 0;
             i < std::size(monitor_buttons_);
             ++i)
        {
            const auto& button = monitor_buttons_[i];
    
            std::printf(
                "\033[%zu;1H[%c] %s",
                std::size(monitor_labels_) + i + 4,
                button.key,
                button.caption);
        }
    }
    
    monitor_initialized_ = true;
}

void Tui::render_result() noexcept
{
    if (!result_initialized_)
    {
        std::printf("\033[1;1HResult");
    }

    for (std::size_t row = 0;
         row < std::size(result_labels_);
         ++row)
    {
        const auto& label = result_labels_[row];

        const auto field =
            static_cast<Ui::ResultField>(label.field);

        const auto& item = ui_->get_field(field);

        const std::size_t index =
            static_cast<std::size_t>(field);

        if (result_initialized_ &&
            result_versions_[index] == item.version)
        {
            continue;
        }

        result_versions_[index] = item.version;

        std::printf(
            "\033[%zu;1H\033[2K%s %u",
            row + 3,
            label.caption,
            static_cast<unsigned>(item.value));
    }

    // Render Result page buttins
    if (!result_initialized_)
    {
        for (std::size_t i = 0;
             i < std::size(result_buttons_);
             ++i)
        {
            const auto& button = result_buttons_[i];
    
            std::printf(
                "\033[%zu;1H[%c] %s",
                std::size(result_labels_) + i + 4,
                button.key,
                button.caption);
        }
    }
    
    result_initialized_ = true;
}
        
void Tui::render_profile_selection() noexcept
{
    std::printf("SELECT PROFILE\n\n");

    for (uint8_t id = 0; id < 10; ++id)
    {
        std::printf(
            "%u  Profile %u\n",
            static_cast<unsigned>(id),
            static_cast<unsigned>(id));
    }

    std::printf("\nb  Back\n");
}

} // namespace app