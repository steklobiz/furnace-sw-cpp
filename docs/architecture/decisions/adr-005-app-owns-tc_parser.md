# Problem:
Who owns the TC_Parser and how do consumers access Samples?

# Chosen direction:
Root App owns TC_Parser — Consumers receive a pointer via init() and pull 
Samples via get_last_sample()

# Rationale:
TC_Parser does not need to know its consumers. Pull mode is simpler, avoids callbacks, and matches the periodic nature of temperature data

# TC_Parser Internal Flow

update() called at 4 Hz
    ↓
read raw ADC value
    ↓
parse to engineering units (°C)
    ↓
validate against limits
    ↓
store in latest_sample
    ↓
update sensor_status (Valid, Disconnected, Overheated, Fault)

# TC_Parser Class Draft (Conceptual)

// TC_Parser.h

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