#include <cstdio>
#include <cstdlib>

#include "furnace.hpp"
#include "profiles.hpp"
#include "settings.hpp"
#include "tc_parser.hpp"
#include "pid.hpp"

namespace
{

int tests_failed = 0;

void check(
    bool condition,
    const char* expression,
    const char* test)
{
    if (!condition)
    {
        std::printf(
            "FAIL: %s: %s\n",
            test,
            expression);

        ++tests_failed;
    }
}

#define CHECK(expr) \
    check((expr), #expr, __func__)


void test_initial_state()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Idle));
}

void test_start_without_preparation()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));
}

void test_start_with_prestep()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    CHECK(
        furnace.outputs() == 0x01);

    CHECK(
        furnace.power() == 0);

    CHECK(
        furnace.current_step() == 0);
}

void test_continue_from_prestep()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();
        
    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    furnace.reset();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    CHECK(
        furnace.current_step() == 0);

    CHECK(
        furnace.outputs() == 0x02);    
}

void test_start_without_prestep()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(1);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    CHECK(
        furnace.outputs() == 0x01);

    CHECK(
        furnace.power() == 0);

    CHECK(
        furnace.current_step() == 0);
}

void test_stop_during_prestep()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    CHECK(
        furnace.outputs() == 0x01);

    furnace.stop();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Stopped));

    CHECK(
        furnace.outputs() == 0);
}

void test_error_during_prestep()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    CHECK(
        furnace.outputs() == 0x01);

    furnace.error();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Error));

    CHECK(
        furnace.outputs() == 0);
}

void test_process_during_prestep()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    CHECK(
        furnace.outputs() == 0x01);

    CHECK(
        furnace.power() == 0);

    furnace.process();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    CHECK(
        furnace.outputs() == 0x01);

    CHECK(
        furnace.power() == 0);

    CHECK(
        furnace.current_step() == 0);
}

void test_process_after_prestep()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    furnace.reset();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    CHECK(
        furnace.outputs() == 0x02);

    furnace.process();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    CHECK(
        furnace.current_step() == 0);
}

void test_prestep_does_not_advance_time()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    for (int i = 0; i < 10; ++i)
        furnace.process();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    CHECK(furnace.current_step() == 0);
    CHECK(furnace.step_elapsed() == 0);
    CHECK(furnace.profile_elapsed() == 0);
    CHECK(furnace.outputs() == 0x01);
    CHECK(furnace.power() == 0);
}

void test_first_step_time()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    furnace.reset();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    CHECK(furnace.current_step() == 0);
    CHECK(furnace.outputs() == 0x02);
    CHECK(furnace.step_elapsed() == 0);
    CHECK(furnace.profile_elapsed() == 0);

    furnace.process();

    CHECK(furnace.step_elapsed() == 1);
    CHECK(furnace.profile_elapsed() == 1);
}

void test_step_transition()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(0);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Waiting));

    furnace.reset();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    CHECK(furnace.current_step() == 0);
    CHECK(furnace.outputs() == 0x02);
    CHECK(furnace.step_elapsed() == 0);
    CHECK(furnace.profile_elapsed() == 0);

    for (int i = 0; i < 9; ++i)
    {
        furnace.process();

        CHECK(
            furnace.state() ==
            static_cast<uint16_t>(
                app::Furnace::State::Running));

        CHECK(furnace.current_step() == 0);
        CHECK(furnace.step_elapsed() == i + 1);
        CHECK(furnace.profile_elapsed() == i + 1);
        CHECK(furnace.outputs() == 0x02);
    }

    // 10th second finishes step 0
    furnace.process();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    CHECK(furnace.current_step() == 1);
    CHECK(furnace.step_elapsed() == 0);
    CHECK(furnace.profile_elapsed() == 10);
    CHECK(furnace.outputs() == 0x03);
}

void test_profile_finished()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(1);

    furnace.start();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Running));

    // Total profile duration:
    // 60 + 20 + 60 + 20 = 160 seconds.
    for (int i = 0; i < 160; ++i)
        furnace.process();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Finished));

    CHECK(furnace.current_step() == 4);
    CHECK(furnace.profile_elapsed() == 160);
    CHECK(furnace.outputs() == 0);
    CHECK(furnace.power() == 0);
}

void test_reset_from_finished()
{
    app::ProfileManager profiles;
    app::SettingManager settings;
    app::TcParser tc_parser;
    core::Pid pid;
    app::Furnace furnace;

    furnace.init(
        profiles,
        settings,
        tc_parser,
        pid);

    profiles.select_for_start(1);

    furnace.start();

    // Profile 1 duration = 160 s.
    for (int i = 0; i < 160; ++i)
        furnace.process();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Finished));

    CHECK(furnace.outputs() == 0);
    CHECK(furnace.power() == 0);

    furnace.reset();

    CHECK(
        furnace.state() ==
        static_cast<uint16_t>(
            app::Furnace::State::Idle));

    CHECK(furnace.outputs() == 0);
    CHECK(furnace.power() == 0);
}

} // namespace


int main()
{
    test_initial_state();
    test_start_without_preparation();
    test_start_with_prestep();
    test_continue_from_prestep();
    test_start_without_prestep();
    test_stop_during_prestep();
    test_error_during_prestep();
    test_process_during_prestep();
    test_process_after_prestep();
    test_prestep_does_not_advance_time();
    test_first_step_time();
    test_step_transition();
    test_profile_finished();
    test_reset_from_finished();
    
    if (tests_failed != 0)
    {
        std::printf(
            "\n%d test(s) failed\n",
            tests_failed);

        return EXIT_FAILURE;
    }

    std::printf("All tests passed\n");

    return EXIT_SUCCESS;
}