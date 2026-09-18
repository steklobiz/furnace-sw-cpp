// pid.hpp
#pragma once

#include <cstdint>

namespace core
{

class Pid
{
public:

    struct Config
    {
        // Coefficients are stored with config::pid::scale factor.
        //
        // Example:
        // Kp = 2.5  -> 2500

        int32_t kp;
        int32_t ki;
        int32_t kd;
    };

    Pid() noexcept = default;

    void init(const Config& config) noexcept;
 
    void reset() noexcept;


    int32_t update(
        int32_t setpoint,
        int32_t measurement,
        uint32_t dt_ms) noexcept;


private:

    Config config_;

    int32_t integral_{0};
    int32_t previous_error_{0};

    bool first_update_{true};

};

}