#include <iostream>
// #include <iomanip>
// #include "furnace.hpp"
// #include "ui.hpp"
// #include "tui.hpp"
// #include "hal.hpp"
// #include "thermal_model.hpp"
#include "app.hpp"

int main()
{
    app::App app;

    while (true)
    {
        app.process();
    }
    
    return 0;  
};