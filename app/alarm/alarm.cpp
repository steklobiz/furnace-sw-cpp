// alarm.cpp
#include "alarm.hpp"

#include "tc_parser.hpp"
#include "furnace.hpp"


namespace app
{
void AlarmDispatcher::init(TcParser& tc_parser, Furnace& furnace)
{
    tc_parser_ = &tc_parser;
    furnace_ = &furnace;
}    

void AlarmDispatcher::process()
{
    for (uint8_t id = 0; id < tc_parser_->count(); ++id)
    {
        if (tc_parser_->status(id) != TcParser::SensorStatus::Valid)
            raise(AlarmId::TcError);
    
        if (tc_parser_->is_overheated(id))
            raise(AlarmId::OverTemperature);
    }

    if (has_active())
        furnace_->error();

}


bool AlarmDispatcher::is_active(AlarmId id) const noexcept
{
    return alarms_[static_cast<std::size_t>(id)];
}


bool AlarmDispatcher::has_active() const noexcept
{
    for (bool active : alarms_)
    {
        if (active)
            return true;
    }

    return false;    
}

void AlarmDispatcher::raise(AlarmId id) noexcept
{
    auto& active =
        alarms_[static_cast<std::size_t>(id)];

    if (active)
        return;

    active = true;

    if (notify_callback_ != nullptr)
    {
        notify_callback_(
            notify_context_,
            {
                this,
                NotificationType::Error,
                static_cast<uint16_t>(id)
            });
    }
}

void AlarmDispatcher::set_notify_callback(
    NotificationCallback callback,
    void* context) noexcept
{
    notify_callback_ = callback;
    notify_context_ = context;
}

} // namespace app
