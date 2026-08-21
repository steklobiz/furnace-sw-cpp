// data_aggregator.cpp
#include "data_aggregator.hpp"
#include <cassert> // temporary

namespace app
{

void DataAggregator::init(
    TcParser& tc_parser,
    Furnace& furnace,
    ProfileManager& profiles) noexcept
{
    tc_parser_ = &tc_parser;
    furnace_ = &furnace;
    profiles_= &profiles;
    
    // Register the aggregator as a notification receiver.
    tc_parser_->set_notify_callback(
        on_notification,
        this);

    furnace_->set_notify_callback(
        on_notification,
        this);

    profiles_->set_notify_callback(
        on_notification,
        this);    
    
    // Initialize the profile snapshot immediately.
    update_edit_profile();        
}

void DataAggregator::on_notification(
    void* context,
    const Notification& notification) noexcept
{
    // Recover the DataAggregator instance that registered the callback.
    auto* aggregator =
        static_cast<DataAggregator*>(context);

    aggregator->handle_notification(notification);
}

void DataAggregator::handle_notification(
    const Notification& notification) noexcept
{
    // The notification context identifies the source
    // that generated the notification.
    if (notification.context == tc_parser_)
    {
        // TcParser currently provides temperature data.
        
        // Attention!!! temperature sended via notification. no need to read data 
        
        return;
    }
    
    
    if (notification.context == profiles_)
    {
        switch (notification.type)
        {
            case NotificationType::StartProfileChanged:
                update_item_value(
                    ProfileItem::StartProfileId,
                    profiles_->start_profile_id());
                break;

            case NotificationType::EditProfileChanged:
                update_item_value(
                    ProfileItem::EditProfileId,
                    profiles_->edit_profile_id());

                update_edit_profile();
                break;

            default:
                break;
        }

        return;
    }
    
    // Furnace notifications are handled according
    // to their notification type.
    if (notification.context == furnace_)
    {
        // Notification came from Furnace.
        switch (notification.type)
        {
            case NotificationType::DataReady:

                update_item_value(
                    FurnaceItem::State,
                    static_cast<uint16_t>(furnace_->state()));
                    
                update_item_value(
                    FurnaceItem::Step,
                    static_cast<uint16_t>(furnace_->current_step()));            
            
                update_item_value(
                    FurnaceItem::Temperature,
                    static_cast<uint16_t>(furnace_->current_temperature()));
                    
                update_item_value(
                    FurnaceItem::Setpoint,
                    static_cast<uint16_t>(furnace_->setpoint()));
                    
                update_item_value(
                    FurnaceItem::StepElapsed,
                    static_cast<uint16_t>(furnace_->step_elapsed()));

                update_item_value(
                    FurnaceItem::ProfileElapsed,
                    static_cast<uint16_t>(furnace_->profile_elapsed()));

                update_item_value(
                    FurnaceItem::Power,
                    static_cast<uint16_t>(furnace_->power()));
                
                update_item_value(
                    FurnaceItem::Outputs,
                    static_cast<uint16_t>(furnace_->outputs()));        
                    
                break;
            case NotificationType::Error:
                break;
            default:
                break;
        }
    }
    
}

const DataItem<uint16_t>& DataAggregator::get_item(uint8_t source_id, uint8_t field_id) noexcept
{
    // TODO: check bounds
    switch (static_cast<DataSource>(source_id))
    {
        case DataSource::TcParser:
            return tc_parser_items_[
                static_cast<std::size_t>(field_id)];

        case DataSource::Furnace:
            return furnace_items_[
                static_cast<std::size_t>(field_id)];
                
        case DataSource::Profile: 
            return profile_items_[ 
                static_cast<std::size_t>(field_id)]; 
                       
        default:
            // Invalid source_id — this is a programmer error.
            // Depending on your project conventions, one of:
            assert(false && "Invalid DataSource");
            // TODO: insert halt here
            break;        
    }
    return null_item_;
}

const DataItem<Profile>&
DataAggregator::profile() const noexcept
{
    return edit_profile_;
}

/*
Functions below may be chaned by template. Need to be checked 

template<typename T>
void update_item(
    DataItem<T>& item,
    const T& value) noexcept
{
    if (item.value != value)
    {
        item.value = value;
        ++item.version;
    }
}
*/

void DataAggregator::update_item_value(FurnaceItem id, uint16_t value) noexcept
{
    auto& item = furnace_items_[
        static_cast<std::size_t>(id)
        ];

    
    if (item.value != value)
    {
        item.value = value;
        ++item.version;
    }
}



void DataAggregator::update_item_value(TcParserItem id, uint16_t value) noexcept
{
    auto& item = tc_parser_items_[
        static_cast<std::size_t>(id)
        ];

    
    if (item.value != value)
    {
        item.value = value;
        ++item.version;
    }
}

void DataAggregator::update_item_value( ProfileItem id, uint16_t value) noexcept 
{ 
    auto& item = profile_items_[
        static_cast<std::size_t>(id)
        ]; 

            
    if (item.value != value) 
    {
        item.value = value;
        ++item.version;
    }
}


void DataAggregator::update_edit_profile() noexcept
{
    const Profile& profile = profiles_->edit_profile();

    if (edit_profile_.value != profile)
    {
        edit_profile_.value = profile;
        ++edit_profile_.version;
    }
}

} // namespace app