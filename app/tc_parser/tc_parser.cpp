#include "tc_parser.hpp"
#include "config.hpp"


namespace app
{

namespace
{

constexpr std::size_t sensor_count = 1;

} // anonymous namespace


void TcParser::init()
{
}


void TcParser::update()
{
}


std::size_t TcParser::count() const noexcept
{
    return app::config::tc_count;
}


TcParser::Sample TcParser::sample(uint8_t id) const noexcept
{
    return 0;
}


TcParser::Sample TcParser::average() const noexcept
{
    return 0;
}


TcParser::SensorStatus TcParser::status(uint8_t id) const noexcept
{
    return SensorStatus::Fault;
}


bool TcParser::is_overheated(uint8_t id) const noexcept
{
    return false;
}


} // namespace app
