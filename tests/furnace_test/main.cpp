#include "logger.hpp"
#include "furnace.hpp"
#include "profiles.hpp"

// Define tags
constexpr Tag app_tag{"Application", Level::Off};
constexpr Tag network_tag{"Network", Level::Off};

app::Profiles profiles;
app::Furnace furnace(profiles);

int main()
{
    Log::info(app_tag, "Application started");
    Log::warning(network_tag, "Network timeout");
    Log::error(app_tag, "Critical error: ", -1);
    Log::info(app_tag, "Value: ", 42, " and flag: ", true);
    
    // Get a reference (alias) to the internal profile
    app::Profile& p = profiles.edit();

    p.steps[0] = {50, 10, 0x01};  // setpoint 100°C, duration 30s, flags
    p.steps[1] = {50, 10,  0x02};
    p.steps[2] = {100, 10, 0x00};
    p.steps[3] = {100, 10, 0x00};
    p.steps[4] = {50,  10, 0x00};
    p.steps[5] = {000, 00, 0x00};
    
    furnace.start();
    
    while(1)
    {
        furnace.process();        
    }
    
    return 0;
}