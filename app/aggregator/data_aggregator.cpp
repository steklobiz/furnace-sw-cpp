#include "data_aggregator.hpp"

namespace app
{

void DataAggregator::init(
    TcParser& tc_parser,
    Furnace& furnace) noexcept
{
    tc_parser_ = &tc_parser;
    furnace_ = &furnace;

    tc_parser_->set_data_ready_callback(
        tc_data_is_ready,
        this);

    furnace_->set_data_ready_callback(
        furnace_data_is_ready,
        this);
}

void DataAggregator::tc_data_is_ready(void* context) noexcept
{
    auto* aggregator = static_cast<DataAggregator*>(context);

    aggregator->update_temperature();
}

void DataAggregator::furnace_data_is_ready(void* context) noexcept
{
    auto* aggregator = static_cast<DataAggregator*>(context);

    aggregator->update_furnace();
}

void DataAggregator::update_temperature() noexcept
{
    const int16_t value = tc_parser_->average();

    if (!initialized_)
    {
        temperature_.value = value;
        initialized_ = true;
        return;
    }

    if (temperature_.value != value)
    {
        temperature_.value = value;
        ++temperature_.version;
    }
}

void DataAggregator::update_furnace() noexcept
{
    // Example only:
    // const int16_t value = furnace_->current_temperature();

    // update the corresponding DataItem here.
}

const DataItem<int16_t>&
DataAggregator::temperature() const noexcept
{
    return temperature_;
}

} // namespace app