// tc_parser.hpp
#pragma once
#include <cstdint>

enum class SensorStatus : uint8_t {
    Valid,
    Stale,
    Disconnected,
    Overheated,
    Fault
};

struct Sample {
    uint16_t  value_c_scaled; // Temperature in °C x 4
    uint32_t   timestamp_ms; // Time when sample was taken
    SensorStatus status;     // Current sensor health
    uint8_t    sensor_id;    // Which sensor (if multiple)
};


class TC_Parser {
public:
    // Called by Root App during initialization
    void init();

    // Called by scheduler at 4 Hz
    // Reads raw ADC, parses into Sample, stores latest
    void update();

    // Called by consumers (Furnace, UI, History) when they need data
    Sample get_last_sample() const;

private:
    Sample latest_sample;
    bool has_valid_sample;
    SensorStatus sensor_status;

    Sample read_and_parse_sensor();
    void update_sensor_status();
};