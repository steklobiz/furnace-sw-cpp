# TC_Parser module

## Purpose

Acquire and provide temperature information from the furnace thermocouples.

## Responsibilities

- Acquire temperature measurements from all configured thermocouples.
- Validate measurements and determine sensor status.
- Provide individual thermocouple values.
- Provide an aggregate temperature value when requested.
- Provide sensor status to consumers.

## Data model


The parser manages temperature information for all configured thermocouples.

For each thermocouple:

- temperature measurement
- sensor status

A thermocouple is identified by its logical ID.

The parser can also provide an aggregate temperature calculated from
the available thermocouple measurements.

Temperature and sensor status are separate pieces of information.
Consumers may request either independently.

## Inputs

- Scheduler tick (4 Hz)
- MAX6675 driver
- Configuration (number of thermocouples, temperature limits, sensor configuration)

## Outputs

- temperature value of specific TC
- calculated temperature value
- TC status information

## Public Interface

init()
update()
temperature(id)
average_temperature()
status(id)



## Data types

enum class SensorStatus : uint8_t {
    Valid,
    Stale,
    Disconnected,
    Overheated,
    Fault
};

struct Sample {
    uint_16_t  value_c_scaled; // Temperature in °C x 4
    uint32_t   timestamp_ms; // Time when sample was taken. do we need it ???
    SensorStatus status;     // Current sensor health
    uint8_t    sensor_id;    // Which sensor (if multiple)
};

## Inputs

## Outputs

## Internal Flow (Data pipeline)

update() called at 4 Hz
    ↓
read raw TC IC value
    ↓
parse to engineering units (°C)
    ↓
validate against limits
    ↓
store in latest_sample
    ↓
update sensor_status (Valid, Disconnected, Overheated, Fault)

## Class Draft (Conceptual)


// TC_Parser.hpp

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
