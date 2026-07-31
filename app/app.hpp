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
        furnace_(profiles_, settings_),
        ui_(furnace_, profiles_),
        tui_(ui_)
    {
        app::Profile& p = profiles_.edit();

        p.steps[0] = {50, 10, 0x01};
        p.steps[1] = {50, 10, 0x02};
        p.steps[2] = {100, 10, 0x00};
        p.steps[3] = {100, 10, 0x00};
        p.steps[4] = {50, 10, 0x00};
        p.steps[5] = {000, 00, 0x00};
    }

    void process() noexcept
    {
        furnace_.process();
        ui_.process();
        tui_.process();
    }

private:
    ProfileManager profiles_;
    SettingManager settings_;
    Furnace furnace_;
    Ui ui_;
    Tui tui_;
};

} // namespace app