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

void test_initial_render()
{
    app::DataAggregator data;
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    app::AlarmDispatcher alarms;
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

    renderer.update();

    assert(ui.position().context == app::Ui::Context::Main);
    assert(ui.position().mode == app::Ui::Mode::Brief);

    feed_dwin_touch(0x2004U);
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Pid);
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

    dwin_variable_change(0x3000, 150U);
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Pid);

    // Change PID settings
    dwin_variable_change(0x2020U, 150U);
    renderer.update();

    feed_dwin_touch(0x2021U);
    renderer.update();

    feed_dwin_touch(0x2022U);
    renderer.update();

    // Change PID settings
    feed_dwin_touch(0x2020U);
    renderer.update();
    assert(settings.get_pid_kp() == 150U);

    feed_dwin_touch(0x2021U);
    renderer.update();
    assert(settings.get_pid_ki() == 30U);

    feed_dwin_touch(0x2022U);
    renderer.update();
    assert(settings.get_pid_kd() == 70U);

    // Settings PID -> Settings Other
    feed_dwin_touch(0x2008U);
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Other);

    // Settings Other -> Settings PID
    feed_dwin_touch(0x2007U);
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Settings);
    assert(ui.position().mode == app::Ui::Mode::Pid);

    // Settings PID -> Main
    feed_dwin_touch(0x2011U);
    renderer.update();

    assert(ui.position().context == app::Ui::Context::Main);
    assert(ui.position().mode == app::Ui::Mode::Brief);
}

} // namespace

int main()
{
    test_initial_render();
    test_settings_switching();

    std::printf("DwinRenderer tests: PASS\n");
    return 0;
}

