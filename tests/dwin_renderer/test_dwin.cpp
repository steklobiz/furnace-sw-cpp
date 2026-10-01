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
constexpr uint16_t ProfileSlot0     = 0x2010U;
constexpr uint16_t ProfileSlot1     = 0x2011U;
constexpr uint16_t ButtonEdit       = 0x2002U;;
constexpr uint16_t ButtonPrevious   = 0x2007U;;
constexpr uint16_t ButtonNext       = 0x2008U;;

void feed_touch(uint16_t button)
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

    assert(ui.context() == app::Ui::Context::Main);

    // Main → ProfileSelection
    std::printf("\nMain->Profile selection\n");
    feed_touch(ButtonStart);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileSelection);
    assert(ui.profile_selection_page() == 0);

    // Next: page 0 -> page 1.
    std::printf("\nNext: page 0 -> page 1\n");
    feed_touch(ButtonNext);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileSelection);
    assert(ui.profile_selection_page() == 1U);

    // Next: page 1 -> page 2.
    std::printf("\nNext: page 1 -> page 2\n");
    feed_touch(ButtonNext);
    renderer.process();

    assert(ui.profile_selection_page() == 2U);

    // Next: page 2 -> page 0.
    std::printf("\nNext: page 2 -> page 0\n");
    feed_touch(ButtonNext);
    renderer.process();

    assert(ui.profile_selection_page() == 0U);

    // ProfileSelection → Monitor
    std::printf("\nProfile selection->Monitor\n");
    feed_touch(ProfileSlot0);
    renderer.process();

    assert(ui.context() == app::Ui::Context::Monitor);

    // Monitor → Main
    std::printf("\nMonitor->Main\n");
    feed_touch(ButtonBack);
    renderer.process();

    assert(ui.context() == app::Ui::Context::Main);

    // Main → ProfileSelection
    std::printf("\nMain->Profile selection\n");
    feed_touch(ButtonEdit);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileSelection);
    assert(ui.profile_selection_page() == 0);

    // ProfileSelection → ProfileEditor
    std::printf("\nProfile selection->Profile editor\n");
    feed_touch(ProfileSlot0);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileEditor);
    assert(ui.current_step() == 0);

    // Next: page 0 -> page 1.
    std::printf("\nNext: step 0 -> step 1\n");
    feed_touch(ButtonNext);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileEditor);
    assert(ui.current_step() == 1U);

    // Previous: step 1 -> step 0.
    std::printf("\nPrevious: step 1 -> step 0\n");
    feed_touch(ButtonPrevious);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileEditor);
    assert(ui.current_step() == 0U);

    // Previous: step 0 -> step 9.
    std::printf("\nPrevious: step 0 -> last step\n");
    feed_touch(ButtonPrevious);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileEditor);
    assert(ui.current_step() == app::config::profiles::max_steps - 1);

    // Next: step 15 -> step 0.
    std::printf("\nNext: last step -> step 0\n");
    feed_touch(ButtonNext);
    renderer.process();

    assert(ui.context() == app::Ui::Context::ProfileEditor);
    assert(ui.current_step() == 0U);

    // Profile edit → Main
    std::printf("\nProfile edit->Main\n");
    feed_touch(ButtonBack);
    renderer.process();

    assert(ui.context() == app::Ui::Context::Main);


    std::printf("DWIN renderer test: PASS\n");
    
    return 0;
}