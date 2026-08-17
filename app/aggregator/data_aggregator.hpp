// data_aggregator.hpp

// DataAggregator collects and stores current application data from
// registered data sources. It is independent of UI and presentation.
// Values remain in their native form and are tracked with a version
// that changes when the stored value changes. It may also own bounded
// event and history queues.

#pragma once

#include <cstdint>

#include "furnace.hpp"
#include "tc_parser.hpp"

namespace app
{
    
// Stores the current value of a data item and its change version.
template<typename T>
struct DataItem
{
    T value{};
    uint8_t version = 0;
};

// Identifies the application data source.
enum class DataSource : uint8_t
{
    TcParser,
    Furnace
};

// Identifies data items provided by Furnace.
enum class FurnaceItem : uint8_t
{
    State,
    Step,
    StepElapsed,
    
    Count
};

// Identifies data items provided by TcParser.
enum class TcParserItem : uint8_t
{
    Temperature,
    Error,
    
    Count
};

class DataAggregator
{
public:

    // Connects the aggregator to application data sources
    // and registers for their notifications.
    void init(
        TcParser& tc_parser,
        Furnace& furnace) noexcept;
        
    // Returns specified data item.    
    DataItem<int16_t> get_item(
        uint8_t source_id, 
        uint8_t field_id) noexcept;
     
private:

    // Receives notifications from registered data sources.
    static void on_notification(
        void* context,
        const Notification& notification) noexcept;
        
    // Identifies the notification source and updates the
    // corresponding aggregated data.
    void handle_notification(
        const Notification& notification) noexcept;

    // Updates a Furnace data item.    
    void update_item_value(
        FurnaceItem id, 
        uint16_t value) noexcept;
    
    // Updates a TcParser data item.    
    void update_item_value(
        TcParserItem id, 
        uint16_t value) noexcept;
            
    TcParser* tc_parser_ = nullptr;
    Furnace* furnace_ = nullptr;

    DataItem<int16_t> furnace_items_[
        static_cast<std::size_t>(FurnaceItem::Count)]{};
        
    DataItem<int16_t> tc_parser_items_[
        static_cast<std::size_t>(TcParserItem::Count)]{};
};

} // namespace app