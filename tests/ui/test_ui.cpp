#include "ui.hpp"
#include "furnace.hpp"
#include "profiles.hpp"
#include "settings.hpp"
#include "tc_parser.hpp"
#include "alarm.hpp"
#include "data_aggregator.hpp"
#include "pid.hpp"

#include <cassert>
#include <iostream>

namespace app
{

namespace
{

void test_initial_position(
    Ui& ui)
{
    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Main);
    assert(position.mode == Ui::Mode::Brief);

    std::cout << "test_initial_position: PASS\n";
}

void test_profile_page_initial(Ui& ui)
{
    assert(ui.profile_page() == 0);

    std::cout << "test_profile_page_initial: PASS\n";
}


void test_idle_start(
    Ui& ui,
    Furnace& furnace)
{
    assert(furnace.state() == Furnace::State::Idle);

    ui.execute({
        Ui::ActionType::Start,
        0
    });

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::ProfileSelection);
    assert(position.mode == Ui::Mode::Start);

    std::cout << "test_idle_start: PASS\n";
}

void test_invalid_action(
    Ui& ui,
    Furnace& furnace)
{
    assert(furnace.state() == Furnace::State::Idle);

    ui.execute({
        Ui::ActionType::Start,
        0
    });

    assert(
        ui.position().context ==
        Ui::Context::ProfileSelection);

    ui.execute({
        Ui::ActionType::Start,
        0
    });

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::ProfileSelection);
    assert(position.mode == Ui::Mode::Start);

    std::cout << "test_invalid_action: PASS\n";
}

void test_idle_settings(
    Ui& ui,
    Furnace& furnace)
{
    assert(furnace.state() == Furnace::State::Idle);

    ui.execute({
        Ui::ActionType::Settings,
        0
    });

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Settings);
    assert(position.mode == Ui::Mode::Pid);

    std::cout << "test_idle_settings: PASS\n";
}

void test_idle_events(
    Ui& ui,
    Furnace& furnace)
{
    assert(furnace.state() == Furnace::State::Idle);

    ui.execute({
        Ui::ActionType::Events,
        0
    });

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Events);
    assert(position.mode == Ui::Mode::None);

    std::cout << "test_idle_events: PASS\n";
}

void ensure_furnace_running(
    Furnace& furnace)
{
    if (furnace.state() == Furnace::State::Idle)
        furnace.start();

    assert(furnace.state() == Furnace::State::Running);
}

void test_running_next(
    Ui& ui,
    Furnace& furnace)
{
    ensure_furnace_running(furnace);

    ui.execute({
        Ui::ActionType::Next,
        0
    });

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Main);
    assert(position.mode == Ui::Mode::Detailed);

    std::cout << "test_running_next: PASS\n";
}

void test_running_stop(
    Ui& ui,
    Furnace& furnace)
{
    ensure_furnace_running(furnace);

    ui.execute({
        Ui::ActionType::Stop,
        0
    });

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Question);
    assert(position.mode == Ui::Mode::Stop);

    // Stop only opens the confirmation.
    assert(furnace.state() == Furnace::State::Running);

    std::cout << "test_running_stop: PASS\n";
}

void test_running_stop_cancel(
    Ui& ui,
    Furnace& furnace)
{
    ensure_furnace_running(furnace);

    ui.execute({
        Ui::ActionType::Stop,
        0
    });

    assert(
        ui.position().context ==
        Ui::Context::Question);

    assert(
        ui.position().mode ==
        Ui::Mode::Stop);

    ui.execute({
        Ui::ActionType::Cancel,
        0
    });

    assert(furnace.state() == Furnace::State::Running);

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Main);
    assert(position.mode == Ui::Mode::Brief);

    std::cout << "test_running_stop_cancel: PASS\n";
}

void test_running_stop_confirm(
    Ui& ui,
    Furnace& furnace)
{
    ensure_furnace_running(furnace);

    ui.execute({
        Ui::ActionType::Stop,
        0
    });

    assert(furnace.state() == Furnace::State::Running);

    ui.execute({
        Ui::ActionType::Confirm,
        0
    });

    assert(furnace.state() == Furnace::State::Stopped);

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Main);
    assert(position.mode == Ui::Mode::Brief);

    std::cout << "test_running_stop_confirm: PASS\n";
}

void test_stopped_reset(
    Ui& ui,
    Furnace& furnace)
{
    ensure_furnace_running(furnace);

    ui.execute({
        Ui::ActionType::Stop,
        0
    });

    ui.execute({
        Ui::ActionType::Confirm,
        0
    });

    assert(furnace.state() == Furnace::State::Stopped);

    ui.execute({
        Ui::ActionType::Reset,
        0
    });

    assert(furnace.state() == Furnace::State::Idle);

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Main);
    assert(position.mode == Ui::Mode::Brief);

    std::cout << "test_stopped_reset: PASS\n";
}

void test_profile_start(
    Ui& ui,
    Furnace& furnace)
{
    assert(furnace.state() == Furnace::State::Idle);

    ui.execute({
        Ui::ActionType::Start,
        0
    });

    assert(
        ui.position().context ==
        Ui::Context::ProfileSelection);

    assert(
        ui.position().mode ==
        Ui::Mode::Start);

    ui.execute({
        Ui::ActionType::Select,
        3
    });

    assert(furnace.state() == Furnace::State::Running);

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::Main);
    assert(position.mode == Ui::Mode::Brief);

    std::cout << "test_profile_start: PASS\n";
}

    void test_profile_page_navigation(Ui& ui)
{
    assert(ui.profile_page() == 0);

    // Main / Brief -> ProfileSelection / Start
    ui.execute({Ui::ActionType::Start});

    assert(ui.position().context == Ui::Context::ProfileSelection);
    assert(ui.position().mode == Ui::Mode::Start);

    ui.execute({Ui::ActionType::Next});
    assert(ui.profile_page() == 1);

    ui.execute({Ui::ActionType::Next});
    assert(ui.profile_page() == 2);

    ui.execute({Ui::ActionType::Next});
    assert(ui.profile_page() == 0);

    ui.execute({Ui::ActionType::Previous});
    assert(ui.profile_page() == 2);

    ui.execute({Ui::ActionType::Previous});
    assert(ui.profile_page() == 1);

    std::cout << "test_profile_page_navigation: PASS\n";
}

    void test_profile_selection_on_page(Ui& ui, Furnace& furnace, ProfileManager& profiles)
{
    assert(ui.profile_page() == 0);

    // Main / Brief -> ProfileSelection / Start
    ui.execute({Ui::ActionType::Start});

    // Page 0 -> Page 1
    ui.execute({Ui::ActionType::Next});
    assert(ui.profile_page() == 1);

    // Visible item 3 on page 1 is profile 13.
    ui.execute({Ui::ActionType::Select, 3});

    assert(profiles.start_profile_id() == 13);
    assert(furnace.state() == Furnace::State::Running);

    std::cout << "test_profile_selection_on_page: PASS\n";
}

    void test_profile_selection_last_page(
        Ui& ui,
        Furnace& furnace,
        ProfileManager& profiles)
{
    // Main / Brief -> ProfileSelection / Start
    ui.execute({Ui::ActionType::Start});

    // Page 0 -> Page 1 -> Page 2
    ui.execute({Ui::ActionType::Next});
    ui.execute({Ui::ActionType::Next});

    assert(ui.profile_page() == 2);

    // First profile on last page: profile 20.
    ui.execute({Ui::ActionType::Select, 0});

    assert(profiles.start_profile_id() == 20);
    assert(furnace.state() == Furnace::State::Running);

    // Stop and return to Idle.
    furnace.stop();
    furnace.reset();

    // Main / Brief -> ProfileSelection / Start again.
    ui.execute({Ui::ActionType::Start});

    // profile_page_ is still 2.
    assert(ui.profile_page() == 2);

    // Last valid profile on page: profile 24.
    ui.execute({Ui::ActionType::Select, 4});

    assert(profiles.start_profile_id() == 24);
    assert(furnace.state() == Furnace::State::Running);

    std::cout << "test_profile_selection_last_page: PASS\n";
}

void test_profile_selection_invalid_last_page(
    Ui& ui,
    Furnace& furnace,
    ProfileManager& profiles)
{
    // Main / Brief -> ProfileSelection / Start
    ui.execute({Ui::ActionType::Start});

    // Page 0 -> Page 1 -> Page 2
    ui.execute({Ui::ActionType::Next});
    ui.execute({Ui::ActionType::Next});

    assert(ui.profile_page() == 2);

    const uint16_t previous_profile_id = profiles.start_profile_id();

    // Select an empty slot. The handler must fail without changing
    // the selected profile or applying the transition target.
    ui.execute({Ui::ActionType::Select, 5});

    assert(furnace.state() == Furnace::State::Idle);
    assert(ui.position().context == Ui::Context::ProfileSelection);
    assert(ui.position().mode == Ui::Mode::Start);
    assert(profiles.start_profile_id() == previous_profile_id);

    std::cout << "test_profile_selection_invalid_last_page: PASS\n";

    }

void test_idle_edit(
    Ui& ui,
    Furnace& furnace)
{
    assert(furnace.state() == Furnace::State::Idle);

    // Main / Brief -> ProfileSelection / Edit
    ui.execute({Ui::ActionType::Edit});

    const Ui::Position position = ui.position();

    assert(position.context == Ui::Context::ProfileSelection);
    assert(position.mode == Ui::Mode::Edit);

    std::cout << "test_idle_edit: PASS\n";

}

void test_profile_selection_for_edit(
    Ui& ui,
    Furnace& furnace,
    ProfileManager& profiles)
{
    assert(furnace.state() == Furnace::State::Idle);
    assert(ui.profile_page() == 0);

    // Main / Brief -> ProfileSelection / Edit
    ui.execute({Ui::ActionType::Edit});

    assert(ui.position().context == Ui::Context::ProfileSelection);
    assert(ui.position().mode == Ui::Mode::Edit);

    // Page 0 -> Page 1
    ui.execute({Ui::ActionType::Next});

    assert(ui.profile_page() == 1);

    // Visible item 3 on page 1 is profile 13.
    ui.execute({Ui::ActionType::Select, 3});

    assert(profiles.edit_profile_id() == 13);
    assert(furnace.state() == Furnace::State::Idle);

    assert(ui.position().context == Ui::Context::Edit);
    assert(ui.position().mode == Ui::Mode::None);

    std::cout << "test_profile_selection_for_edit: PASS\n";

}


} // namespace

} // namespace app

int main()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::AlarmDispatcher alarm;
    app::DataAggregator data;

    // These dependencies are shared by the tests.
    // Furnace and Ui are created separately for each test.

    // -----------------------------------------------------------------
    // Initial position
    // -----------------------------------------------------------------

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_initial_position(ui);
    }

    // -----------------------------------------------------------------
    // Idle
    // -----------------------------------------------------------------

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_profile_start(ui, furnace);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_idle_start(ui, furnace);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_idle_edit(ui, furnace);
    }



    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_invalid_action(ui, furnace);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_idle_settings(ui, furnace);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_idle_events(ui, furnace);
    }

    // -----------------------------------------------------------------
    // Running
    // -----------------------------------------------------------------

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_running_next(ui, furnace);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_running_stop(ui, furnace);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_running_stop_cancel(ui, furnace);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_running_stop_confirm(ui, furnace);
    }

    // -----------------------------------------------------------------
    // Stopped
    // -----------------------------------------------------------------

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_stopped_reset(ui, furnace);
    }

    // -----------------------------------------------------------------
    // Profile pages
    // -----------------------------------------------------------------

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_profile_page_navigation(ui);
    }

    // -----------------------------------------------------------------
    // Profile selection
    // -----------------------------------------------------------------

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_profile_selection_on_page(ui, furnace, profiles);
    }

    // -----------------------------------------------------------------
    // Last profile page
    // -----------------------------------------------------------------

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_profile_selection_last_page(ui, furnace, profiles);
    }

    {
        app::Furnace furnace;
        furnace.init(profiles, settings, tc_parser, pid);

        app::Ui ui;
        ui.init(data, furnace, profiles, settings);

        app::test_profile_selection_invalid_last_page(
            ui,
            furnace,
            profiles);
    }



    std::cout << "Ui tests: PASS\n";

    return 0;
}