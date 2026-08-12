
/*
struct FurnaceTraceSample
{
    uint32_t time_s;
    int32_t temperature;
    int32_t setpoint;
    uint8_t output;
};

struct PidTraceSample
{
    uint32_t time_s;
    int32_t setpoint;
    int32_t measurement;
    int32_t error;
    int32_t p;
    int32_t i;
    int32_t d;
    int32_t output;
};


void add_furnace(...);
void add_pid(...);

const auto& furnace() const noexcept;
const auto& pid() const noexcept;

void clear() noexcept;
*/