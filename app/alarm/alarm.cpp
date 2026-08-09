// alarm.cpp
#include "alarm.hpp"

#include "tc_parser.hpp"
#include "furnace.hpp"


namespace app
{


AlarmDispatcher(
    TcParser& tc_parser,
    Furnace& furnace)
{
}


void AlarmDispatcher::process()
{
}


bool AlarmDispatcher::is_active(AlarmId id) const noexcept
{
}


bool AlarmDispatcher::has_active() const noexcept
{
}


void AlarmDispatcher::raise(AlarmId id) noexcept
{
}


} // namespace app
