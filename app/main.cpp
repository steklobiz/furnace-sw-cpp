// main.cpp
#include "logger.hpp"

// Define tags
constexpr Tag app_tag{"Application", Level::Off};
constexpr Tag network_tag{"Network", Level::Off};

int main()
{
    Log::info(app_tag, "Application started");
    Log::warning(network_tag, "Network timeout");
    Log::error(app_tag, "Critical error: ", -1);
    Log::info(app_tag, "Value: ", 42, " and flag: ", true);
    
    return 0;
}