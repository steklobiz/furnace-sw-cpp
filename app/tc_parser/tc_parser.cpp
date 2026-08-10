// tc_parser.cpp

#include "tc_parser.hpp"
#include "config.hpp"


namespace app
{

namespace
{

// constexpr std::size_t sensor_count = 1;

} // anonymous namespace


void TcParser::init()
{
    // Before the first successful update(), we have no valid measurement.
    for (std::size_t i = 0; i < count(); ++i)
    {
        samples_[i] = 0;
        statuses_[i] = SensorStatus::Fault;
        overheated_[i] = false;
    }
}


void TcParser::update()
{
    // Actual thermocouple acquisition and parsing
    // will be implemented here.
}


std::size_t TcParser::count() const noexcept
{
    return app::config::tc_count;
}


TcParser::Sample TcParser::sample(uint8_t id) const noexcept
{
    return samples_[id];
}


TcParser::Sample TcParser::average() const noexcept
{
    if (count() == 0)
        return 0;

    uint32_t sum = 0;

    for (std::size_t i = 0; i < count(); ++i)
        sum += samples_[i];

    return static_cast<Sample>(sum / count());
}


TcParser::SensorStatus TcParser::status(uint8_t id) const noexcept
{
    return statuses_[id];
}


bool TcParser::is_overheated(uint8_t id) const noexcept
{
 return overheated_[id];
}


} // namespace app
