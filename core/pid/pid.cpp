#include "pid.hpp"


namespace core
{


Pid::Pid(const Config& config) noexcept
    :
    config_(config)
{
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


    //
    // Proportional term
    //
    const int32_t p =
        (error * config_.kp) / SCALE;


    //
    // Integral term
    //
    integral_ +=
        (error * config_.ki * static_cast<int32_t>(dt_ms))
        / (SCALE * 1000);


    //
    // Derivative term
    //
    int32_t d = 0;

    if (!first_update_)
    {
        const int32_t delta =
            error - previous_error_;

        d =
            (delta * config_.kd * 1000)
            / (SCALE * static_cast<int32_t>(dt_ms));
    }


    first_update_ = false;
    previous_error_ = error;


    int32_t output =
        p + integral_ + d;


    //
    // Output limiting
    //
    if (output > config_.output_max)
    {
        output = config_.output_max;

        // anti windup
        integral_ = output - p - d;
    }
    else if (output < config_.output_min)
    {
        output = config_.output_min;

        // anti windup
        integral_ = output - p - d;
    }


    return output;
}


}