#include "tc_parser.hpp"
#include "config.hpp"


namespace app
{

namespace
{

constexpr std::size_t sensor_count = 1;

} // anonymous namespace


void TC_Parser::init()
{
}


void TC_Parser::update()
{
}


std::size_t TC_Parser::count() const noexcept
{
    return app::config::tc_count;
}


TC_Parser::Sample TC_Parser::sample(uint8_t id) const noexcept
{
    return 0;
}


TC_Parser::Sample TC_Parser::average() const noexcept
{
    return 0;
}


TC_Parser::SensorStatus TC_Parser::status(uint8_t id) const noexcept
{
    return SensorStatus::Fault;
}


bool TC_Parser::is_overheated(uint8_t id) const noexcept
{
    return false;
}


} // namespace app
