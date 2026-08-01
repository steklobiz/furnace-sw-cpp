#pragma once

#include <cstdint>

namespace core
{

class Pid
{
public:

    struct Config
    {
        // Coefficients are stored with PID_SCALE factor.
        //
        // Example:
        // Kp = 2.5  -> 2500

        int32_t kp;
        int32_t ki;
        int32_t kd;

        int32_t output_min; // should be 0
//        int32_t output_max;
    };


    static constexpr int32_t SCALE = 1000;


    explicit Pid(const Config& config) noexcept;


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