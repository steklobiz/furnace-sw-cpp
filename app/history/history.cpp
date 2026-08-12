// history.cpp
#include "history.hpp"

namespace app
{

void History::init() noexcept
{
    clear();
}

void History::clear() noexcept
{
    events_.clear();
    samples_.clear();

    next_sample_s_ = 0;
}

void History::process(
    uint32_t elapsed_s,
    int16_t temperature,
    uint8_t output) noexcept
{
    if (elapsed_s < next_sample_s_)
        return;

    samples_.push_overwrite({
        elapsed_s,
        temperature,
        output
    });

    next_sample_s_ += config::history::sample_period_s;
}

void History::add_event(
    uint32_t elapsed_s,
    EventId id,
    uint8_t argument) noexcept
{
    events_.push_overwrite({
        elapsed_s,
        id,
        argument
    });
}

} // namespace app
