#include <cassert>
#include <cstdint>
#include <cstdio>

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

// Process the input flow:
//
// DWIN packet
//     -> DwinTransport
//     -> DwinRenderer
//     -> Ui::Action
//     -> Ui::Page::Settings


namespace
{
constexpr uint16_t ButtonStart      = 0x2000U;
constexpr uint16_t ButtonBack       = 0x2003U;
constexpr uint16_t ButtonSettings   = 0x2004U;


void feed_settings_touch(uint16_t button)
{
    // DWIN touch packet example:
    //
    // 5A A5 06 83 20 04 01 00 01
    // 0x2004 = button
    // value  = 1
    const uint8_t packet[] =
    {
        0x5A,
        0xA5,
        0x06,
        0x83,
        static_cast<uint8_t>(button >> 8U),
        static_cast<uint8_t>(button & 0xFFU),
        0x01,
        0x00,
        0x01
    };

    hal::test_feed_dwin_bytes(packet, sizeof(packet));
}

} // namespace


int main()
{
    app::SettingManager settings;
    app::ProfileManager profiles;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;
    app::AlarmDispatcher alarm;
    app::DataAggregator data_aggregator;
    app::Ui ui;

    pid.init({
        settings.view().pid_kp,
        settings.view().pid_ki,
        settings.view().pid_kd
    });

    tc_parser.init();

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    alarm.init(
        tc_parser,
        furnace,
        settings);

    data_aggregator.init(
        tc_parser,
        furnace,
        profiles,
        settings,
        alarm);

    ui.init(
        data_aggregator,
        furnace,
        profiles,
        settings);

    app::DwinTransport transport;
    app::DwinRenderer renderer;

    hal::init();

    transport.init();

    renderer.init(
        ui,
        transport);

    // Initial renderer cycle.
    renderer.process();

    assert(ui.page() == app::Ui::Page::Main);

    // Press Run on Main
    std::printf("\nMain->Profile selection\n");

    feed_settings_touch(ButtonStart);
    renderer.process();

    assert(ui.page() == app::Ui::Page::ProfileSelection);

    // Select profile 0 on Profile selection page

    // Press Back to return to the Main page

    // Press Settings on Main page
    std::printf("\nMain->Settings\n");

    feed_settings_touch(ButtonSettings);
    renderer.process();

    assert(ui.page() == app::Ui::Page::Settings);


    // Press Back on the DWIN display
    std::printf("\nSettings->Main\n");

    feed_settings_touch(ButtonBack);
    renderer.process();

    assert(ui.page() == app::Ui::Page::Main);


    std::printf("DWIN renderer test: PASS\n");
    
    return 0;
}