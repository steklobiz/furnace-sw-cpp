```cpp
#pragma once

#include <cstddef>
#include <cstdint>

#include "ring_buffer.hpp"

namespace platform::trace
{

struct FurnaceSample
{
    uint32_t time_s;
    int32_t temperature;
    int32_t setpoint;
    uint8_t output;
};

struct PidSample
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


template<
    std::size_t FurnaceCapacity,
    std::size_t PidCapacity>
class Trace
{
public:

    void add_furnace(
        const FurnaceSample& sample) noexcept
    {
        furnace_.push_overwrite(sample);
    }

    void add_pid(
        const PidSample& sample) noexcept
    {
        pid_.push_overwrite(sample);
    }

    const auto& furnace() const noexcept
    {
        return furnace_;
    }

    const auto& pid() const noexcept
    {
        return pid_;
    }

    void clear() noexcept
    {
        furnace_.clear();
        pid_.clear();
    }

private:

    core::RingBuffer<FurnaceSample, FurnaceCapacity> furnace_;
    core::RingBuffer<PidSample, PidCapacity> pid_;
};

} // namespace platform::trace
```
