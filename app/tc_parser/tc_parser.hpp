#pragma once

#include <cstddef>
#include <cstdint>

namespace app
{

class TC_Parser
{
public:
    using Sample = uint16_t;

    enum class SensorStatus : uint8_t
    {
        Valid,
        Disconnected,
        Fault
    };

    
    // Initializes the temperature parser.
    void init();

    // Acquires and processes the latest samples.
    void update();

    // Returns the number of configured thermocouples.
    std::size_t count() const noexcept;

    // Returns the latest sample from the specified thermocouple.
    // Sample value is temperature in °C multiplied by 4.
    Sample sample(uint8_t id) const noexcept;

    // Returns the arithmetic mean of all configured thermocouple samples.
    Sample average() const noexcept;

    // Returns the current status of the specified thermocouple.
    SensorStatus status(uint8_t id) const noexcept;

    // Returns true when the specified thermocouple reports
    // a temperature above its configured limit.
    bool is_overheated(uint8_t id) const noexcept;
};

} // namespace app
