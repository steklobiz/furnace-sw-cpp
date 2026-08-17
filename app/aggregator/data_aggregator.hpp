#pragma once

#include <cstdint>

#include "furnace.hpp"
#include "tc_parser.hpp"

namespace app
{

template<typename T>
struct DataItem
{
    T value{};
    uint8_t version = 0;
};

class DataAggregator
{
public:
    void init(
        TcParser& tc_parser,
        Furnace& furnace) noexcept;

    const DataItem<int16_t>& temperature() const noexcept;

private:
    static void tc_data_is_ready(void* context) noexcept;
    static void furnace_data_is_ready(void* context) noexcept;

    void update_temperature() noexcept;
    void update_furnace() noexcept;

    TcParser* tc_parser_ = nullptr;
    Furnace* furnace_ = nullptr;

    bool initialized_ = false;

    DataItem<int16_t> temperature_{};
};

} // namespace app