#include <iostream>
#include <iomanip>
#include "furnace.hpp"
#include "ui.hpp"
#include "tui.hpp"
#include "hal.hpp"
#include "thermal_model.hpp"


int main()
{
    app::Furnace furnace;

    app::Ui ui(furnace);

    app::Tui tui(ui);


    while(true)
    {
        furnace.process();

        ui.process();

        tui.process();
    }    

    /*      
    app::Furnace furnace;
    
    furnace.start();
    
    hal::set_heater_power(100);
    
    while(1){
        
//        hal::update();
        furnace.process();
//        std::cout << hal::tick_s() 
//            << " | " << furnace.get_current_step() 
//            << " | " << furnace.get_current_temp() 
//            << std::endl;
             
//        std::cout << hal::tick_s() << " | " << hal::get_temperature() << std::endl;         
        hal::delay_ms(1000);
    }
*/    
    return 0;  
};