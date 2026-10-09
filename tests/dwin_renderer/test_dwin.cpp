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

    // Main / Brief -> ProfileSelection / Start.
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

    // Main / Brief / Stopped + Reset -> Main / Brief / Idle
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

} // namespace

int main()
{
    test_profile_page_navigation();
    std::printf("--------------------\n");
    test_profile_start();

    std::printf(
        "All DWIN tests passed.\n");

    return 0;
}

