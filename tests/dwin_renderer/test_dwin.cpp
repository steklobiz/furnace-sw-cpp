#include <set>
// test_dwin.cpp

#include "alarm.hpp"
#include "data_aggregator.hpp"
#include "dwin_renderer.hpp"
#include "dwin_transport.hpp"
#include "furnace.hpp"
#include "hal.hpp"
#include "pid.hpp"
#include "profiles.hpp"
#include "settings.hpp"
#include "tc_parser.hpp"
#include "ui.hpp"
#include "dwin_protocol.hpp" // temporary

#include <cassert>
#include <cstdint>
#include <cstdio>

namespace
{

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

    app::DwinTransport transport;
    app::DwinRenderer renderer;

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

        renderer.init(
            ui,
            data,
            transport);
    }
};

void dwin_variable_change(
    const uint16_t address,
    const uint16_t value)
{
    const uint8_t packet[]
    {
        0x5A,
        0xA5,
        0x06,
        0x83,
        static_cast<uint8_t>(address >> 8U),
        static_cast<uint8_t>(address & 0xFFU),
        0x01,
        static_cast<uint8_t>(value >> 8U),
        static_cast<uint8_t>(value & 0xFFU)
    };

    hal::test_feed_dwin_bytes(
        packet,
        sizeof(packet));
}

void test_profile_page_navigation()
{
    TestApp app;

    assert(app.ui.profile_page() == 0U);

    // Main / Brief -> ProfileSelection / Start.
    dwin_variable_change(0x3000U, 0x0000U);
    app.renderer.update();

    assert(
        app.ui.position().context ==
        app::Ui::Context::ProfileSelection);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Start);

    // Forward: 0 -> 1 -> 2 -> 0.
    dwin_variable_change(0x3000U, 0x0012U);
    app.renderer.update();

    assert(app.ui.profile_page() == 1U);

    dwin_variable_change(0x3000U, 0x0012U);
    app.renderer.update();

    assert(app.ui.profile_page() == 2U);

    dwin_variable_change(0x3000U, 0x0012U);
    app.renderer.update();

    assert(app.ui.profile_page() == 0U);

    // Backward: 0 -> 2 -> 1 -> 0.
    dwin_variable_change(0x3000U, 0x0010U);
    app.renderer.update();

    assert(app.ui.profile_page() == 2U);

    dwin_variable_change(0x3000U, 0x0010U);
    app.renderer.update();

    assert(app.ui.profile_page() == 1U);

    dwin_variable_change(0x3000U, 0x0010U);
    app.renderer.update();

    assert(app.ui.profile_page() == 0U);


    std::printf(
        "test_profile_page_navigation: PASS\n");
}

void test_profile_start()
{
    TestApp app;

    assert(
        app.furnace.state() ==
        app::Furnace::State::Idle);

    assert(app.ui.profile_page() == 0U);

    // Main / Brief / Idle + Start -> ProfileSelection / Start.
    dwin_variable_change(0x3000U, 0x0000U);
    app.renderer.update();

    assert(
        app.ui.position().context ==
        app::Ui::Context::ProfileSelection);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Start);

    // Select real profile 0.
    dwin_variable_change(0x3000U, 0x1000U);
    app.renderer.update();

    assert(
        app.profiles.start_profile_id() == 0U);

    assert(
        app.furnace.state() ==
        app::Furnace::State::Running);

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

    assert(app.furnace.state() == app::Furnace::State::Running);

    assert(
        app.ui.position().context ==
        app::Ui::Context::Main);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Detailed);

    assert(
        app.furnace.state() ==
        app::Furnace::State::Running);


    // Main / Detailed + Running -> Question / Stop
    dwin_variable_change(0x3000U, 0x0006U); // Press stop
    app.renderer.update();

    assert(
        app.ui.position().context ==
        app::Ui::Context::Question);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Stop);

    assert(
        app.furnace.state() ==
        app::Furnace::State::Running);

    // Question / Stop / Running + Confirm -> Main / Brief / Stopped
    dwin_variable_change(0x3000U, 0x0020U); // Confirm
    app.renderer.update();

    assert(
        app.ui.position().context ==
        app::Ui::Context::Main);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Brief);

    assert(
        app.furnace.state() ==
        app::Furnace::State::Stopped);

    // Main / Brief / Stopped + Reset -> Main / Brief
    dwin_variable_change(0x3000U, 0x0008U);
    app.renderer.update();

    assert(
        app.ui.position().context ==
        app::Ui::Context::Main);

    assert(
        app.ui.position().mode ==
        app::Ui::Mode::Brief);

    assert(
        app.furnace.state() ==
        app::Furnace::State::Idle);

    std::printf(
        "test_profile_start: PASS\n");
}

void test_settings_modify_cancel() noexcept
{
    TestApp app;

    // Main -> Settings PID
    app.renderer.update();

    dwin_variable_change(0x3000U, 0x0004U); // Settings
    app.renderer.update();

    assert(app.ui.position().context == app::Ui::Context::Settings);
    assert(app.ui.position().mode == app::Ui::Mode::Pid);

    // Modify PID settings
    dwin_variable_change(0x0030U, 150U); // Kp
    app.renderer.update();

    dwin_variable_change(0x0032U, 30U); // Ki
    app.renderer.update();

    dwin_variable_change(0x0034U, 70U); // Kd
    app.renderer.update();

    assert(app.settings.get_edit_pid_kp() == 150U);
    assert(app.settings.get_edit_pid_ki() == 30U);
    assert(app.settings.get_edit_pid_kd() == 70U);

    // Settings PID -> Settings Other
    dwin_variable_change(0x3000U, 0x0012U); // Next
    app.renderer.update();

    assert(app.ui.position().context == app::Ui::Context::Settings);
    assert(app.ui.position().mode == app::Ui::Mode::Other);

    // Modify other settings
    dwin_variable_change(0x0040U, 250U); // Max temperature
    app.renderer.update();

    dwin_variable_change(0x0042U, 0U); // Buzzer off
    app.renderer.update();

    dwin_variable_change(0x0044U, 5U); // Prestep outputs
    app.renderer.update();

    assert(app.settings.get_edit_max_temperature() == 250U);
    assert(app.settings.get_edit_buzzer_state() == 0U);
    assert(app.settings.get_edit_prestep_outs() == 5U);

    // Settings Other -> Settings PID
    dwin_variable_change(0x3000U, 0x0010U); // Previous
    app.renderer.update();

    // Return to Settings Other to refresh its displayed values
    dwin_variable_change(0x3000U, 0x0012U); // Next
    app.renderer.update();

    assert(app.ui.position().context == app::Ui::Context::Settings);
    assert(app.ui.position().mode == app::Ui::Mode::Other);


    // Cancel settings.
    dwin_variable_change(0x3000U, 0x0022U);
    app.renderer.update();

    // The UI returns to Main.
    assert(app.ui.position().context == app::Ui::Context::Main);

    // Committed values are unchanged.
    assert(app.settings.get_pid_kp() == 100U);
    assert(app.settings.get_pid_ki() == 20U);
    assert(app.settings.get_pid_kd() == 50U);
    assert(app.settings.get_buzzer_state() == 1U);
    assert(app.settings.get_max_temperature() == 1200U);
    assert(app.settings.get_prestep_outs() == 0U);

    // The edit copy is restored as well.
    assert(app.settings.get_edit_pid_kp() == 100U);
    assert(app.settings.get_edit_pid_ki() == 20U);
    assert(app.settings.get_edit_pid_kd() == 50U);
    assert(app.settings.get_edit_buzzer_state() == 1U);
    assert(app.settings.get_edit_max_temperature() == 1200U);
    assert(app.settings.get_edit_prestep_outs() == 0U);

    // Settings PID -> Main (Cancel)
    dwin_variable_change(0x3000U, 0x0022U);
    app.renderer.update();

    assert(app.ui.position().context == app::Ui::Context::Main);
    assert(app.ui.position().mode == app::Ui::Mode::Brief);

    // Main -> Settings PID
    dwin_variable_change(0x3000U, 0x0004U);
    app.renderer.update();

    assert(app.ui.position().context == app::Ui::Context::Settings);
    assert(app.ui.position().mode == app::Ui::Mode::Pid);

    // Settings PID -> Settings Other
    dwin_variable_change(0x3000U, 0x0012U);
    app.renderer.update();

    assert(app.ui.position().mode == app::Ui::Mode::Other);

}


void test_settings_modify_save() noexcept
{
    TestApp app;

    // Main -> Settings PID
    app.renderer.update();

    dwin_variable_change(0x3000U, 0x0004U); // Settings
    app.renderer.update();

    // Modify PID settings
    dwin_variable_change(0x0030U, 150U); // Kp
    app.renderer.update();

    dwin_variable_change(0x0032U, 30U); // Ki
    app.renderer.update();

    dwin_variable_change(0x0034U, 70U); // Kd
    app.renderer.update();

    // Settings PID -> Settings Other
    dwin_variable_change(0x3000U, 0x0012U); // Next
    app.renderer.update();

    // Modify other settings
    dwin_variable_change(0x0040U, 250U); // Max temperature
    app.renderer.update();

    dwin_variable_change(0x0042U, 0U); // Buzzer off
    app.renderer.update();

    dwin_variable_change(0x0044U, 5U); // Prestep outputs
    app.renderer.update();

    // Save settings.
    dwin_variable_change(0x3000U, 0x0020U); // Confirm / Save
    app.renderer.update();

    assert(app.ui.position().context == app::Ui::Context::Main);
    assert(app.ui.position().mode == app::Ui::Mode::Brief);

    // Verify committed values.
    assert(app.settings.get_pid_kp() == 150U);
    assert(app.settings.get_pid_ki() == 30U);
    assert(app.settings.get_pid_kd() == 70U);
    assert(app.settings.get_buzzer_state() == 0U);
    assert(app.settings.get_max_temperature() == 250U);
    assert(app.settings.get_prestep_outs() == 5U);

    // Reopen Settings and verify the values are retained.
    dwin_variable_change(0x3000U, 0x0004U); // Settings PID
    app.renderer.update();

    assert(app.settings.get_edit_pid_kp() == 150U);
    assert(app.settings.get_edit_pid_ki() == 30U);
    assert(app.settings.get_edit_pid_kd() == 70U);

    dwin_variable_change(0x3000U, 0x0012U); // Settings Other
    app.renderer.update();

    assert(app.settings.get_edit_buzzer_state() == 0U);
    assert(app.settings.get_edit_max_temperature() == 250U);
    assert(app.settings.get_edit_prestep_outs() == 5U);
}

void test_events_navigation()
{
    // Create application dependencies.
    app::SettingManager settings;
    app::ProfileManager profiles;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;
    app::AlarmDispatcher alarm;
    app::DataAggregator data;
    app::Ui ui;

    // Initialize PID and thermocouple parser.
    pid.init({
        settings.view().pid_kp,
        settings.view().pid_ki,
        settings.view().pid_kd
    });

    tc_parser.init();

    // Initialize the furnace.
    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    // Initialize alarms and data aggregation.
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

    // Initialize the UI.
    ui.init(
        data,
        furnace,
        profiles,
        settings);

    // Initial position must be Main / Brief.
    assert(ui.state() == app::Furnace::State::Idle);
    assert(ui.position().context == app::Ui::Context::Main);
    assert(ui.position().mode == app::Ui::Mode::Brief);

    // Main -> Events.
    ui.execute({app::Ui::ActionType::Events, 0U});

    assert(ui.position().context == app::Ui::Context::Events);
    assert(ui.position().mode == app::Ui::Mode::None);
    assert(ui.event_page() == 0U);

    // Empty history has one logical page.
    assert(ui.event_count() == 0U);

    // Next must keep the UI in Events.
    // With no events, the page index remains zero.
    ui.execute({app::Ui::ActionType::Next, 0U});

    assert(ui.position().context == app::Ui::Context::Events);
    assert(ui.position().mode == app::Ui::Mode::None);
    assert(ui.event_page() == 0U);

    // Previous must also keep the UI in Events.
    ui.execute({app::Ui::ActionType::Previous, 0U});

    assert(ui.position().context == app::Ui::Context::Events);
    assert(ui.position().mode == app::Ui::Mode::None);
    assert(ui.event_page() == 0U);

    // Cancel: Events -> Main / Brief.
    ui.execute({app::Ui::ActionType::Cancel, 0U});

    assert(ui.position().context == app::Ui::Context::Main);
    assert(ui.position().mode == app::Ui::Mode::Brief);
}


void test_dwin_string()
{
    app::DwinProtocol protocol;
    app::DwinTransport transport;

    transport.init();

    const auto packet =
        protocol.write_string(0x1400U, "Event 1", 8U);

    assert(packet.size == 14U);

    // Send the packet through HAL to print it in the PC terminal.
    transport.send(packet.data, packet.size);
}

} // namespace

int main()
{
    test_dwin_string();
    /*
    test_profile_page_navigation();
    std::printf("--------------------\n");
    test_profile_start();
    std::printf("--------------------\n");
    test_settings_modify_cancel();
    std::printf("--------------------\n");
    test_settings_modify_save();
    std::printf("--------------------\n");
    test_events_navigation();
    std::printf(
        "All DWIN tests passed.\n");
*/
    return 0;
}

