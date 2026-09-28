// main.cpp
#include <windows.h>
#include "app.hpp"

int main()
{
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