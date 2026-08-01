// main.cpp
#include "app.hpp"
#include "logger.hpp"

// Define tags
namespace{
    
constexpr Tag tag {
    "MAIN", 
    Level::Off
};

}

int main()
{
    hal::init();
     
    Log::info(tag, "Application started");

    app::App myapp;

    if (!myapp.init())
    {
        while(true)
        {
        }
    }
        
    myapp.run();
    
    return 0;
}