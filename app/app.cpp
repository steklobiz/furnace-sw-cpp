#include "app.hpp"

namespace app {

void Application::reset_furnace() noexcept
{
    alarm_dispatcher_.clear_all();
    furnace_.reset();
}

void Application::ui_command_callback(
    void* context,
    Ui::Action action) noexcept
{
    auto& app =
        *static_cast<Application*>(context);

    switch (action.type)
    {
    case Ui::ActionType::ResetFurnace:
        app.reset_furnace();
        break;

    default:
        break;
    }
}

} // namespace app