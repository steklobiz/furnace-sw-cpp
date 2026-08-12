# Problem:
What is tc_parer's sample format?

# Decision

```
enum class SensorStatus : uint8_t {
    Valid,
    Stale,
    Disconnected,
    Overheated,
    Fault
};

struct Sample {
    uint_16_t  value_c_scaled; // Temperature in °C x 4
    uint32_t   timestamp_ms; // Time when sample was taken
    SensorStatus status;     // Current sensor health
    uint8_t    sensor_id;    // Which sensor (if multiple)
};
```