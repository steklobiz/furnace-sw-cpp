#pragma once
#include "profiles.hpp"
#include "furnace.hpp"
#include "ui.hpp"
#include "tui.hpp"

namespace app
{

class App
{
public:

    App()
        :
        furnace_(profiles_),
        ui_(furnace_, profiles_),
        tui_(ui_)
    {
    }

    void process() noexcept
    {
        furnace_.process();
        ui_.process();
        tui_.process();
    }

private:
    Profiles profiles_;
    Furnace furnace_;
    Ui ui_;
    Tui tui_;
};

} // namespace app