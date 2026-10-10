
// test_ui.cpp

#include <cassert>
#include <cstdint>
#include <iostream>

#include "ui.hpp"
#include "furnace.hpp"
#include "profiles.hpp"
#include "settings.hpp"
#include "tc_parser.hpp"
#include "alarm.hpp"
#include "data_aggregator.hpp"
#include "pid.hpp"


namespace
{

// -----------------------------------------------------------------------------
// Test application
// -----------------------------------------------------------------------------
//
// Owns one complete application object graph.
// A new TestApp is created for every test, so no mutable state is shared.
//
struct TestApp
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::AlarmDispatcher alarm;
    app::DataAggregator data;
    app::Furnace furnace;
    app::Ui ui;

    TestApp() noexcept
    {
        furnace.init(
            profiles,
            settings,
            tc_parser,
            pid);

        alarm.init(
            tc_parser,
            furnace,
            settings);

        data.init(
            tc_parser,
            furnace,
            profiles,
            settings,
            alarm);

        ui.init(
            data,
            furnace,
            profiles,
            settings);
    }
};


// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

// Tests profile selection page navigation independently of profile selection.
// Verifies circular forward and backward pagination across all pages.
void test_profile_page_navigation()
{
    TestApp app;

    assert(app.ui.profile_page() == 0);

    // Main / Brief -> ProfileSelection / Start.
    app.ui.execute({
        app::Ui::ActionType::Start
    });

    assert(
        app.ui.position().context ==
        app::Ui::Context::ProfileSelection);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Start);

    // Forward: 0 -> 1 -> 2 -> 0.
    app.ui.execute({
        app::Ui::ActionType::Next
    });

    assert(app.ui.profile_page() == 1);

    app.ui.execute({
        app::Ui::ActionType::Next
    });

    assert(app.ui.profile_page() == 2);

    app.ui.execute({
        app::Ui::ActionType::Next
    });

    assert(app.ui.profile_page() == 0);

    // Backward: 0 -> 2 -> 1 -> 0.
    app.ui.execute({
        app::Ui::ActionType::Previous
    });

    assert(app.ui.profile_page() == 2);

    app.ui.execute({
        app::Ui::ActionType::Previous
    });

    assert(app.ui.profile_page() == 1);

    app.ui.execute({
        app::Ui::ActionType::Previous
    });

    assert(app.ui.profile_page() == 0);

    std::cout
        << "test_profile_page_navigation: PASS\n";
}


// Tests starting a real profile from the first profile selection page.
// Verifies that selecting profile 0 starts the furnace and enters Main / Detailed.
void test_profile_start()
{
    TestApp app;

    assert(app.furnace.state() == app::Furnace::State::Idle);
    assert(app.ui.profile_page() == 0);

    // Main / Brief -> ProfileSelection / Start.
    app.ui.execute({
        app::Ui::ActionType::Start
    });

    assert(
        app.ui.position().context ==
        app::Ui::Context::ProfileSelection);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Start);

    // Select real profile 0.
    app.ui.execute({
        app::Ui::ActionType::Select,
        0
    });

    assert(app.profiles.start_profile_id() == 0);
    assert(app.furnace.state() == app::Furnace::State::Running);

    assert(
        app.ui.position().context ==
        app::Ui::Context::Main);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Detailed);

    // Furnace::process() publishes fresh Furnace data
    // and triggers DataAggregator::refresh().
    app.furnace.process();

    assert(
        app.data.furnace_item(app::FurnaceItem::Step) ==
        app.furnace.current_step());

    assert(
        app.data.furnace_item(app::FurnaceItem::StepType) ==
        app.furnace.step_type());

    assert(
        app.data.furnace_item(app::FurnaceItem::Temperature) ==
        app.furnace.current_temperature());

    assert(
        app.data.furnace_item(app::FurnaceItem::Setpoint) ==
        app.furnace.setpoint());

    assert(
        app.data.furnace_item(app::FurnaceItem::StepElapsed) ==
        app.furnace.step_elapsed());

    assert(
        app.data.furnace_item(app::FurnaceItem::ProfileElapsed) ==
        app.furnace.profile_elapsed());

    assert(
        app.data.furnace_item(app::FurnaceItem::Power) ==
        app.furnace.power());

    assert(
        app.data.furnace_item(app::FurnaceItem::Outputs) ==
        app.furnace.outputs());



    std::cout
        << "test_profile_start: PASS\n";
}


/*
 * Tests circular event-page navigation using events generated
 * by a real Furnace profile.
 */
void test_events_navigation()
{
    TestApp app;

    // Select Profile 1 and start it directly.
    assert(app.profiles.select_for_start(1));
    app.furnace.start();

    assert(app.furnace.state() == app::Furnace::State::Running);

    // Each test step lasts one second.
    // Ten calls are sufficient for a profile with at most
    // nine active steps and a terminating 0-0 step.
    for (uint8_t i = 0; i < 10; ++i)
    {
        app.furnace.process();
    }

    assert(app.furnace.state() == app::Furnace::State::Finished);
    assert(app.ui.event_count() >= 9);

    // Return the furnace to Idle so the Events action is accepted.
    // This does not clear the recorded event history.
    app.furnace.reset();

    assert(app.furnace.state() == app::Furnace::State::Idle);

    // Main / Brief -> Events / None.
    app.ui.execute({
        app::Ui::ActionType::Events
    });

    assert(app.ui.position().context == app::Ui::Context::Events);
    assert(app.ui.event_page() == 0);

    // Forward: 0 -> 1 -> 2 -> 0.
    app.ui.execute({app::Ui::ActionType::Next});
    assert(app.ui.event_page() == 1);

    app.ui.execute({app::Ui::ActionType::Next});
    assert(app.ui.event_page() == 2);

    app.ui.execute({app::Ui::ActionType::Next});
    assert(app.ui.event_page() == 0);

    // Backward: 0 -> 2 -> 1 -> 0.
    app.ui.execute({app::Ui::ActionType::Previous});
    assert(app.ui.event_page() == 2);

    app.ui.execute({app::Ui::ActionType::Previous});
    assert(app.ui.event_page() == 1);

    app.ui.execute({app::Ui::ActionType::Previous});
    assert(app.ui.event_page() == 0);

    // Cancel returns to Main / Brief.
    app.ui.execute({app::Ui::ActionType::Cancel});

    assert(app.ui.position().context == app::Ui::Context::Main);
    assert(app.ui.position().mode == app::Ui::Mode::Brief);

    std::cout << "test_events_navigation: PASS\n";
}

/*
 * Tests event-page boundaries when the final page is only partially filled.
 * Five events should produce two pages: four events, then one event.
 */
void test_event_page_boundaries()
{
    TestApp app;

    // Select Profile 1 and start it.
    assert(app.profiles.select_for_start(1));
    app.furnace.start();

    // Stop the profile to generate the final events.
    app.furnace.stop();

    // Return to Idle without clearing event history.
    app.furnace.reset();

    // Confirm that the test generated exactly five events.
    assert(app.furnace.state() == app::Furnace::State::Idle);
    assert(app.ui.event_count() == 5);

    // Main / Brief -> Events / None.
    app.ui.execute({
        app::Ui::ActionType::Events
    });

    assert(app.ui.position().context == app::Ui::Context::Events);
    assert(app.ui.event_page() == 0);

    // Two pages: four events on page 0, one on page 1.
    app.ui.execute({app::Ui::ActionType::Next});
    assert(app.ui.event_page() == 1);

    // Next wraps to the first page.
    app.ui.execute({app::Ui::ActionType::Next});
    assert(app.ui.event_page() == 0);

    // Previous wraps to the partially filled last page.
    app.ui.execute({app::Ui::ActionType::Previous});
    assert(app.ui.event_page() == 1);

    // Cancel returns to Main / Brief.
    app.ui.execute({app::Ui::ActionType::Cancel});

    assert(app.ui.position().context == app::Ui::Context::Main);
    assert(app.ui.position().mode == app::Ui::Mode::Brief);

    std::cout << "test_event_page_boundaries: PASS\n";
}

} // namespace


int main()
{
    test_profile_page_navigation();
    test_profile_start();
    test_events_navigation();
    test_event_page_boundaries();

    std::cout
        << "All UI tests passed.\n";

    return 0;
}
