// alarm.hpp
#pragma once

#include <cstdint>
#include <array>
#include "tc_parser.hpp"

namespace app
{

class TC_Parser;
class Furnace;

enum class AlarmId : uint8_t
{
    TcError,
    OverTemperature,
    EmergencyStop,

    Count
};

enum class AlarmState : uint8_t
{
    Inactive,
    Active
};

class AlarmDispatcher
{
public:
    void init(TcParser& tc_parser, Furnace& furnace);

    void process();

    // Returns true if the specified alarm is currently active.
    bool is_active(AlarmId id) const noexcept;
    
    // Returns true if at least one alarm is currently active.
    bool has_active() const noexcept;
    
    // Activates the specified alarm.
    void raise(AlarmId id) noexcept;

private:
    TcParser* tc_parser_{nullptr};
    Furnace* furnace_{nullptr};
    
    std::array<bool,
               static_cast<std::size_t>(AlarmId::Count)> alarms_{};    
};

} // namespace app
