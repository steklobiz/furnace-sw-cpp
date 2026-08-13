// pid.cpp
#include "pid.hpp"
#include "config.hpp"

namespace core
{

namespace
{


} // anonymous namespace


void Pid::init(const Config& config) noexcept
{
    config_ = config;
    reset();
}

void Pid::reset() noexcept
{
    integral_ = 0;
    previous_error_ = 0;
    first_update_ = true;
}


int32_t Pid::update(
    int32_t setpoint,
    int32_t measurement,
    uint32_t dt_ms) noexcept
{
    const int32_t error = setpoint - measurement;

    //----------------------------------------------------------
    // P term
    //----------------------------------------------------------

    const int32_t p =
        error * config_.kp;

    //----------------------------------------------------------
    // I term
    //
    // integral_ is stored with scale precision.
    //----------------------------------------------------------

    integral_ +=
        error *
        config_.ki *
        static_cast<int32_t>(dt_ms) /
        1000;

    const int32_t i = integral_;

    //----------------------------------------------------------
    // D term
    //
    // d is stored with scale precision.
    //----------------------------------------------------------

    int32_t d = 0;

    if (!first_update_)
    {
        d =
            (error - previous_error_) *
            config_.kd *
            1000 /
            static_cast<int32_t>(dt_ms);
    }

    //----------------------------------------------------------
    // Convert the total PID result to output units.
    //----------------------------------------------------------

    int32_t output =
        (p + i + d) /
        app::config::pid::scale;

    //----------------------------------------------------------
    // Output limiting + anti-windup.
    //----------------------------------------------------------

    if (output > app::config::pid::output_max_power)
    {
        output = app::config::pid::output_max_power;

        integral_ =
            output * app::config::pid::scale - p - d;
    }
    else if (output < app::config::pid::output_min_power)
    {
        output = app::config::pid::output_min_power;

        integral_ =
            output * app::config::pid::scale - p - d;
    }

    previous_error_ = error;
    first_update_ = false;

#ifdef PLATFORM_PC

    debug_ =
    {
        error,
        p,
        i,
        d,
        output
    };

#endif

    return output;
}

}