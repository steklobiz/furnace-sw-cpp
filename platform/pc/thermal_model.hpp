#pragma once

#include <cstdint>

namespace simulator
{

// A struct describes what the furnace is.    
struct ThermalParams
{
    // Amount of energy required to change the furnace temperature by 1°C.
    //
    // Larger value  -> slower heating and cooling.
    // Smaller value -> faster temperature changes.
    uint16_t heat_capacity = 500;

    // Heater output at 100% power.
    //
    // Units: energy units per second.
    uint16_t heater_power = 80;

    // Heat lost every second for each degree above ambient.
    //
    // Example:
    //
    // temperature = 200°C
    // ambient     = 25°C
    //
    // ΔT = 175°C
    //
    // cooling_rate = 2
    //
    // Heat loss = 175 × 2 = 350 energy units.
    uint16_t cooling_rate = 2;
};

// A struct describes where the furnace's state (where is it now).
struct ThermalState
{
    constexpr explicit ThermalState(
        int16_t initial_temperature) noexcept
        :
        temperature(initial_temperature),
        stored_energy(0),
        heater_remainder(0)
    {
    }
    
    // Current temperature.

    int16_t temperature;

    // Energy accumulated since the last whole-degree change.
    //
    // Range:
    //
    // -(heat_capacity-1) ... +(heat_capacity-1)
    //
    // Most of the time this value represents a fraction of one degree.
    int32_t stored_energy;

    // Preserves fractional heater output.
    //
    // Without this variable, low heater percentages slowly lose energy
    // because of integer division.
    //
    // Range:
    //
    // 0...99
    int16_t heater_remainder;
};

class ThermalModel
{
public:

    constexpr explicit ThermalModel(
        const ThermalParams& params,
        int16_t initial_temperature,
        int16_t ambient_temperature) noexcept
        :
        params_(params),
        state_{initial_temperature},
        ambient_temperature_(ambient_temperature)
    {
    }

    //----------------------------------------------------------------------
    // Simulate one second.
    //
    // heater_percent : 0...100
    // ambient        : ambient temperature in °C
    //----------------------------------------------------------------------
    void update(
        uint8_t heater_percent) noexcept
    {
        //--------------------------------------------------------------
        // 1. Heater adds thermal energy.
        //
        // Integer division would lose fractional energy.
        //
        // Example:
        //
        // heater_power = 255
        // heater = 50%
        //
        // Exact energy = 127.5
        //
        // Instead of discarding 0.5 every second we keep it in
        // heater_remainder_ and add it during the next update.
        //--------------------------------------------------------------

        int32_t heater_energy =
            static_cast<int32_t>(heater_percent) *
            params_.heater_power +
            state_.heater_remainder;

        state_.stored_energy += heater_energy / 100;

        state_.heater_remainder = heater_energy % 100;


        //--------------------------------------------------------------
        // 2. Heat naturally escapes into the environment.
        //
        // Cooling is proportional to the temperature difference.
        //
        // If furnace == ambient
        //     no cooling occurs.
        //--------------------------------------------------------------

        int16_t delta =
            state_.temperature - ambient_temperature_;

        state_.stored_energy -=
            static_cast<int32_t>(delta) *
            params_.cooling_rate;


        //--------------------------------------------------------------
        // 3. Convert stored energy into whole degrees.
        //
        // heat_capacity defines how many energy units correspond
        // to one degree.
        //
        // Example:
        //
        // heat_capacity = 500
        //
        // stored_energy = 630
        //
        // -> increase temperature by 1°C
        // -> keep remaining 130 units
        //
        // The remaining energy continues accumulating during the
        // following updates.
        //--------------------------------------------------------------

        while (state_.stored_energy >= params_.heat_capacity)
        {
            ++state_.temperature;
            state_.stored_energy -= params_.heat_capacity;
        }


        //--------------------------------------------------------------
        // 4. Cooling works exactly the same.
        //
        // If enough energy is lost, temperature decreases while the
        // remaining deficit is preserved.
        //--------------------------------------------------------------

        while (state_.stored_energy <=
               -static_cast<int32_t>(params_.heat_capacity))
        {
            --state_.temperature;
            state_.stored_energy += params_.heat_capacity;
        }
    }

    // Set ambient temperature
    void set_ambient(
        int16_t temperature);

    // Get current temperature
    int16_t temperature() const noexcept
    {
        return state_.temperature;
    }

    // temporary getter
    int32_t stored_energy() const noexcept
    {
        return state_.stored_energy;
    }
    
private:

    ThermalParams params_;
    ThermalState state_;
    

    // Room temperature.
    int16_t ambient_temperature_ = 25;


};

} // namespace simulator