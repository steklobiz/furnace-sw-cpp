// history.hpp
#pragma once

#include <cstdint>

#include "config.hpp"
#include "ring_buffer.hpp"

namespace app
{

enum class EventId : uint8_t
{
    ProfileStarted,
    StepStarted,
    PauseStarted,
    ProfileStopped,
    ProfileFinished,
    ErrorOccurred
};

struct Event
{
    uint32_t elapsed_s;
    EventId  id;
    uint8_t  argument;
};

struct FurnaceSample
{
    uint32_t elapsed_s;
    int16_t  temperature;
    uint8_t  output;
};

class History
{
public:
    void init() noexcept;
    void clear() noexcept;

    void process(
        uint32_t elapsed_s,
        int16_t temperature,
        uint8_t output) noexcept;

    void add_event(
        uint32_t elapsed_s,
        EventId id,
        uint8_t argument = 0) noexcept;

    const core::RingBuffer<
        Event,
        config::history::event_capacity>&
    events() const noexcept
    {
        return events_;
    }

    const core::RingBuffer<
        FurnaceSample,
        config::history::sample_capacity>&
    samples() const noexcept
    {
        return samples_;
    }

private:
    core::RingBuffer<
        Event,
        config::history::event_capacity> events_;

    core::RingBuffer<
        FurnaceSample,
        config::history::sample_capacity> samples_;

    uint32_t next_sample_s_ = 0;
};

} // namespace app

