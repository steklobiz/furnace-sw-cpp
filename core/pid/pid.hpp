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

#ifdef PLATFORM_PC

    struct DebugState
    {
        int32_t error;
        int32_t p;
        int32_t i;
        int32_t d;
        int32_t output;
    };

    const DebugState& debug() const noexcept
    {
        return debug_;
    }
    
#endif

    static constexpr int32_t SCALE = 1000;


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
    
#ifdef PLATFORM_PC

    DebugState debug_{};

#endif
    
};

}