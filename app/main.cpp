// main.cpp
#include "app.hpp"


int main()
{
    hal::init();
     
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