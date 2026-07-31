// main.cpp
#include "app.hpp"
#include "logger.hpp"
#include "hal.hpp"

// Define tags
namespace{
    
constexpr Tag tag {
    "MAIN", 
    Level::Off
};

}

app::App myapp;

int main()
{
    Log::info(tag, "Application started");

    while(1)
    {
        myapp.process();

        hal::delay_ms(1000);
    }
    
    return 0;
}