// tc_parser.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <array>
#include "config.hpp"

namespace app
{

    
class TcParser
{
public:
    using Sample = uint16_t;
    using DataReadyCallback = void (*)(void*);

    enum class SensorStatus : uint8_t
    {
        Valid,
        Disconnected,
        Fault
    };

    
    // Initializes the temperature parser.
    void init();
    
    // Registers a callback invoked when new temperature data is ready.
    void set_data_ready_callback(
        DataReadyCallback callback,
        void* context) noexcept;
    
    // Acquires and processes the latest samples. 
    // Don't rename it to process(). This method updates internal data
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

private:

    DataReadyCallback data_ready_callback_ = nullptr;
    void* data_ready_context_ = nullptr;

    std::array<Sample, app::config::tc_count> samples_{};
    std::array<SensorStatus, app::config::tc_count> statuses_{};
    std::array<bool, app::config::tc_count> overheated_{};
};

} // namespace app
