#include "alarm.hpp"
#include "data_aggregator.hpp"
#include "dwin_renderer.hpp"
#include "dwin_transport.hpp"
#include "furnace.hpp"
#include "pid.hpp"
#include "profiles.hpp"
#include "settings.hpp"
#include "tc_parser.hpp"
#include "ui.hpp"
#include "hal.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace
{

void dwin_variable_change(uint16_t address, uint16_t value)
{
    const uint8_t packet[] =
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

    hal::test_feed_dwin_bytes(packet, sizeof(packet));
}

void test_settings_switching()
{
    app::DataAggregator data;
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;

    app::Furnace furnace;
    app::Ui ui;
    app::DwinTransport transport;
    app::DwinRenderer renderer;

    tc_parser.init();

    core::Pid::Config pid_config{};
    pid_config.kp = 100;
    pid_config.ki = 20;
    pid_config.kd = 50;

    pid.init(pid_config);

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    ui.init(
        data,
        furnace,
        profiles,
        settings);

    renderer.init(
        ui,
        data,
        transport);

    // Main -> Settings PID
    renderer.update();

    dwin_variable_change(0x3000U, 0004U);
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Pid);

    // Change PID settings
    dwin_variable_change(0x0030U, 150U);
    renderer.update();
    assert(settings.get_edit_pid_kp() == 150U);

    dwin_variable_change(0x0032U, 30U);
    renderer.update();
    assert(settings.get_edit_pid_ki() == 30U);

    dwin_variable_change(0x0034U, 70U);
    renderer.update();
    assert(settings.get_edit_pid_kd() == 70U);

    // Settings PID -> Settings Other
    dwin_variable_change(0x3000U, 0x0012U); // Next
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Other);

    // Next from Settings Other has no transition and must be ignored
    dwin_variable_change(0x3000U, 0x0012U); // Next
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Other);

    // Settings Other -> Settings PID
    dwin_variable_change(0x3000U, 0x0010U); // Previous
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Pid);

    // Settings PID -> Main
    dwin_variable_change(0x3000U, 0x0022U);
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Main);
    assert(ui.position().mode == app::Ui::Mode::Brief);
}


void test_profile_selection()
{
    app::DataAggregator data;
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;

    app::Furnace furnace;
    app::Ui ui;
    app::DwinTransport transport;
    app::DwinRenderer renderer;

    tc_parser.init();

    core::Pid::Config pid_config{};
    pid_config.kp = 100;
    pid_config.ki = 20;
    pid_config.kd = 50;

    pid.init(pid_config);

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    ui.init(
        data,
        furnace,
        profiles,
        settings);

    renderer.init(
        ui,
        data,
        transport);

    // Main -> Profile selection.
    renderer.update();

    dwin_variable_change(0x3000U, 0x0000U); // Start
    renderer.update();

    assert(
        ui.position().context ==
        app::Ui::Context::ProfileSelection);

    assert(
        ui.position().mode ==
        app::Ui::Mode::Start);

    // Profile selection starts at page 0.
    assert(ui.profile_page() == 0U);

    // Profile pages: 0-9, 10-19, 20-24.
    dwin_variable_change(0x3000U, 0x0012U); // Next
    renderer.update();

    assert(ui.profile_page() == 1U);

    dwin_variable_change(0x3000U, 0x0012U); // Next
    renderer.update();

    assert(ui.profile_page() == 2U);

    dwin_variable_change(0x3000U, 0x0012U); // Next
    renderer.update();

    assert(ui.profile_page() == 0U);

    // Selecting profile 0 starts the profile and returns to Main Brief.
    dwin_variable_change(0x3000U, 0x1000U);
    renderer.update();

    assert(profiles.start_profile_id() == 0U);

    // Main Brief -> Main Detailed.
    dwin_variable_change(0x3000U, 0x0012U); // Next
    renderer.update();

}

} // namespace

int main()
{
    test_settings_switching();
    test_profile_selection();

    std::printf("DwinRenderer tests: PASS\n");
    return 0;
}

