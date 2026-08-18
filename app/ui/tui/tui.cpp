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
    {
        std::printf("\033[2J\033[H");

        rendered_page_ = page;
        page_initialized_ = true;

        switch (page)
        {
            case Ui::Page::Main:
                main_initialized_ = false;
                break;

            case Ui::Page::Monitor:
                monitor_initialized_ = false;
                break;
        }
    }

    switch (page)
    {
        case Ui::Page::Main:
            render_main();
            break;

        case Ui::Page::Monitor:
            render_monitor();
            break;
    }
}

//------------------------------------------------------
// Helper functions
//------------------------------------------------------


void Tui::process_input() noexcept
{
#ifdef PLATFORM_PC

    if (!_kbhit())
    {
        return;
    }

    const int key = _getch();

    switch (key)
    {
        case 's':
            ui_->execute(Ui::Action::StartProfile);
            break;

        default:
            break;
    }

#endif
}

// Renders all fields of the Main page, then marks the page as
// initialized so subsequent renders only print changed values.
void Tui::render_main() noexcept
{
    render_main_field(Ui::MainField::Temperature);
    
    main_initialized_ = true;
}


// Renders all fields of the Monitor page, then marks the page as
// initialized so subsequent renders only print changed values.
void Tui::render_monitor() noexcept
{
    render_monitor_field(Ui::MonitorField::State);
    render_monitor_field(Ui::MonitorField::CurrentStep);
    render_monitor_field(Ui::MonitorField::Temperature);
    
    monitor_initialized_ = true;
}


// Renders a single Main-page field, but only when its value changed.
// The enum value doubles as the index into main_versions_; the same
// value selects the item via get_field(), keeping index and switch
// cases in lockstep.
void Tui::render_main_field(
    Ui::MainField field) noexcept
{
    // Fetch the field's current data item from Ui. Ui resolves the
    // field to the correct DataAggregator item internally.
    const auto& item = ui_->get_field(field);

    const std::size_t index =
        static_cast<std::size_t>(field);

    // Change detection: skip unless the item version differs from the
    // last printed one. The initialized flag disables the check on the
    // very first render so every field is printed at least once.
    if (main_initialized_ &&
        main_versions_[index] == item.version)
    {
        return;
    }

    // Remember the printed version before emitting output.
    main_versions_[index] = item.version;

    switch (field)
    {
        case Ui::MainField::Temperature:
            std::printf(
                "Temperature: %u C\n",
                static_cast<unsigned>(item.value));
            break;

        // Placeholder for future Main-page fields.
        case Ui::MainField::Count:
            break;
    }
}

// Renders a single Monitor-page field using the same version-based
// change detection as render_main_field().
void Tui::render_monitor_field(
    Ui::MonitorField field) noexcept
{
    const auto& item = ui_->get_field(field);

    const std::size_t index =
        static_cast<std::size_t>(field);

    if (monitor_initialized_ &&
        monitor_versions_[index] == item.version)
    {
        return;
    }

    monitor_versions_[index] = item.version;

    switch (field)
    {
        case Ui::MonitorField::State:
            std::printf(
                "State: %u\n",
                static_cast<unsigned>(item.value));
            break;

        case Ui::MonitorField::CurrentStep:
            std::printf(
                "Step: %u\n",
                static_cast<unsigned>(item.value));
            break;

        case Ui::MonitorField::Temperature:
            std::printf(
                "Temperature: %u C\n",
                static_cast<unsigned>(item.value));
            break;

        // Placeholder for future Monitor-page fields.
        case Ui::MonitorField::Count:
            break;
    }
}

} // namespace app