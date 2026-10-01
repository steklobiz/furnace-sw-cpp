
#include "ui.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>

namespace
{

void execute(
    app::Ui& ui,
    app::Ui::ActionType type,
    uint16_t argument = 0U) noexcept
{
    ui.execute({type, argument});
}

void test_initial_state(
    app::Ui& ui) noexcept
{
    assert(ui.context() == app::Ui::Context::Main);
    assert(ui.profile_selection_page() == 0);
    assert(ui.current_step() == 0);
}

void test_profile_selection(
    app::Ui& ui,
    app::ProfileManager& profiles) noexcept
{
    execute(
        ui,
        app::Ui::ActionType::StartProfileSelection);

    assert(ui.context() == app::Ui::Context::ProfileSelection);
    assert(ui.profile_selection_page() == 0);
    assert(ui.profile_selection_page_count() == 3);

    // Page 0: profiles 0..9
    assert(ui.profile_at_slot(0) == &profiles.profile(0));
    assert(ui.profile_at_slot(9) == &profiles.profile(9));
    assert(ui.profile_at_slot(10) == nullptr);

    execute(ui, app::Ui::ActionType::Next);

    // Page 1: profiles 10..19
    assert(ui.profile_selection_page() == 1);
    assert(ui.profile_at_slot(0) == &profiles.profile(10));
    assert(ui.profile_at_slot(9) == &profiles.profile(19));

    execute(ui, app::Ui::ActionType::Next);

    // Page 2: profiles 20..24
    assert(ui.profile_selection_page() == 2);
    assert(ui.profile_at_slot(0) == &profiles.profile(20));
    assert(ui.profile_at_slot(4) == &profiles.profile(24));

    // Remaining slots on the last page are empty.
    assert(ui.profile_at_slot(5) == nullptr);
    assert(ui.profile_at_slot(9) == nullptr);

    // Circular navigation: last page -> first page.
    execute(ui, app::Ui::ActionType::Next);
    assert(ui.profile_selection_page() == 0);

    // Circular navigation: first page -> last page.
    execute(ui, app::Ui::ActionType::Previous);
    assert(ui.profile_selection_page() == 2);

    // Back to page 1.
    execute(ui, app::Ui::ActionType::Previous);
    assert(ui.profile_selection_page() == 1);

    // Back to page 0.
    execute(ui, app::Ui::ActionType::Previous);
    assert(ui.profile_selection_page() == 0);
}

void test_profile_selection_reset(
    app::Ui& ui) noexcept
{
    // Starting profile selection always begins at page zero.
    execute(
        ui,
        app::Ui::ActionType::StartProfileSelection);

    execute(ui, app::Ui::ActionType::Next);
    execute(ui, app::Ui::ActionType::Previous);

    execute(
        ui,
        app::Ui::ActionType::EditProfileSelection);

    assert(ui.context() == app::Ui::Context::ProfileSelection);
    assert(ui.profile_selection_page() == 0);
}

void test_profile_editor_navigation(
    app::Ui& ui) noexcept
{
    // Select profile 0 for editing.
    execute(
        ui,
        app::Ui::ActionType::EditProfileSelection);

    execute(
        ui,
        app::Ui::ActionType::SelectProfile,
        0U);

    assert(ui.context() == app::Ui::Context::ProfileEditor);
    assert(ui.current_step() == 0);

    // Next is interpreted as "next profile step".
    execute(ui, app::Ui::ActionType::Next);
    assert(ui.current_step() == 1);

    execute(ui, app::Ui::ActionType::Next);
    assert(ui.current_step() == 2);

    // Previous is interpreted as "previous profile step".
    execute(ui, app::Ui::ActionType::Previous);
    assert(ui.current_step() == 1);

    execute(ui, app::Ui::ActionType::Previous);
    assert(ui.current_step() == 0);

    // Cannot move before the first step.
    execute(ui, app::Ui::ActionType::Previous);
    assert(ui.current_step() == app::config::profiles::max_steps - 1);
}

void test_profile_editor_last_step(
    app::Ui& ui) noexcept
{
    // Profile::MaxSteps is 16, so the valid step indices are 0..15.
    execute(
        ui,
        app::Ui::ActionType::EditProfileSelection);

    execute(
        ui,
        app::Ui::ActionType::SelectProfile,
        0U);

    for (std::size_t i = 0; i < app::config::profiles::max_steps - 1; ++i)
    {
        execute(ui, app::Ui::ActionType::Next);
    }

    assert(
        ui.current_step() ==
        app::config::profiles::max_steps - 1);

    // Cannot move beyond the last step.
    execute(ui, app::Ui::ActionType::Next);

    assert(
        ui.current_step() == 0);
}

void test_invalid_profile_slot(
    app::Ui& ui) noexcept
{
    execute(
        ui,
        app::Ui::ActionType::StartProfileSelection);

    // Move to the last page.
    execute(ui, app::Ui::ActionType::Next);
    execute(ui, app::Ui::ActionType::Next);

    assert(ui.profile_selection_page() == 2);

    // Profiles 20..24 exist, so slot 5 is empty.
    execute(
        ui,
        app::Ui::ActionType::SelectProfile,
        5);

    // Selecting an empty slot must have no effect.
    assert(ui.context() == app::Ui::Context::ProfileSelection);
    assert(ui.profile_selection_page() == 2);
}

} // namespace

int main()
{
    app::DataAggregator data;
    app::Furnace furnace;
    app::ProfileManager profiles;
    app::SettingManager settings;

    app::Ui ui;

    // Only profile-selection/navigation behavior is tested here.
    // These objects do not need application-level initialization for
    // the tested operations.
    ui.init(
        data,
        furnace,
        profiles,
        settings);


    std::cout << "test_initial_state\n";
    test_initial_state(ui);

    std::cout << "test_profile_selection\n";
    test_profile_selection(ui, profiles);

    std::cout << "test_profile_selection_reset\n";
    test_profile_selection_reset(ui);

    std::cout << "test_profile_editor_navigation\n";
    test_profile_editor_navigation(ui);

    std::cout << "test_profile_editor_last_step\n";
    test_profile_editor_last_step(ui);

    std::cout << "test_invalid_profile_slot\n";
    test_invalid_profile_slot(ui);

    std::cout << "Ui tests: PASS\n";

    return 0;
}