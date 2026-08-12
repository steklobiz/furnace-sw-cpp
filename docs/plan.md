# Furnace tasks list

## Milestones

- [ ] - M1: Architecture complete
- [ ] - M2: Complete PC furnace simulation
- [ ] - M3: Functional STM32 version
    - [ ] - M3.1  MCU boots    
    - [ ] - M3.2  GPIO works
    - [ ] - M3.3  timer works
    - [ ] - M3.4  UART works
    - [ ] - M3.5  SPI works
    - [ ] - M3.6  MAX6675 works
    - [ ] - M3.7  heater output works
    - [ ] - M3.8  scheduler works
    - [ ] - M3.9  application runs
    - [ ] - M3.10 complete thermal loop
- [ ] - M4: Real furnace validation
- [ ] - M5: Production validation
- [ ] - M6: Production-ready release

### Current milestone

M2 — Complete PC furnace simulation

> Status:
> - `[x]` implemented
> - `[~]` partially implemented / architecture decided but incomplete
> - `[ ]` not implemented
>
> This is the master development plan for the furnace controller.

## Core

- [x] - RingBuffer
- [x] - FSM
- [x] - Scheduler
- [x] - PID
- [~] - Generic formatting utilities
- [~] - Event stream / event storage
- [~] - Debug message stream

## Configuration

- [x] - Profile
- [x] - Profile management
- [x] - SettingManager
- [x] - Application configuration
- [~] - Configuration validation
- [ ] - Persistent settings storage
- [ ] - EEPROM / flash storage
- [ ] - Settings migration / versioning

## Thermal

- [x] - Furnace main algorithm
- [x] - Furnace FSM
- [x] - Furnace model
- [x] - PID integration
- [x] - TC Parser
- [~] - Furnace initialization / dependency wiring
- [~] - Furnace output control
- [~] - Alarm Dispatcher
- [ ] - Furnace auto tuning
- [ ] - Thermal safety handling
- [ ] - Thermal limits
- [~] - Temperature history
- [~] - Thermal events

## Application

- [~] - Application class / application structure
- [~] - Object creation order
- [~] - Dependency wiring
- [~] - Application initialization
- [~] - Scheduler integration
- [~] - Configuration → module initialization
- [~] - Furnace / TC / Alarm integration
- [ ] - Complete application startup sequence
- [ ] - Complete application runtime loop

## UI

- [~] - UI FSM — current implementation
- [ ] - Rewrite UI FSM using transition table
- [ ] - Define UI states
- [ ] - Define UI events
- [ ] - Define UI transition table
- [ ] - UI state handlers
- [ ] - UI enter/exit handlers
- [~] - Main page/state
- [~] - Monitor page/state
- [~] - Profile selection state
- [~] - UI state/event API
- [~] - Profile loading
- [~] - Furnace monitor data getters
- [~] - UI command handling
- [~] - UI ↔ Furnace interaction
- [ ] - Alarm presentation
- [ ] - Event presentation
- [ ] - Complete UI

## TUI

- [x] - TUI structure
- [x] - Main screen
- [x] - Monitor screen
- [x] - Profile selection screen
- [x] - UI state-based rendering
- [x] - Basic keyboard input
- [~] - Non-blocking keyboard input
- [~] - Continuous monitor update
- [~] - Event/history display
- [ ] - Complete TUI

## DWIN

- [~] - DWIN communication architecture
- [~] - DWIN protocol handling
- [~] - DWIN display integration architecture
- [ ] - DWIN driver
- [ ] - DWIN command parser
- [ ] - DWIN page implementation
- [ ] - DWIN UI integration

## Devices / Drivers

- [~] - MAX6675 integration
- [~] - Thermocouple acquisition
- [~] - SPI abstraction
- [~] - GPIO abstraction
- [ ] - Complete MAX6675 driver
- [ ] - Heater output driver
- [ ] - Complete hardware integration

## MCU Drivers

- [~] - GPIO driver
- [~] - SPI driver
- [~] - UART driver
- [~] - Timer driver
- [~] - PWM driver
- [ ] - ADC driver
- [ ] - Interrupt handling
- [ ] - Clock driver

## Platform

- [x] - PC platform
- [x] - MCU platform structure
- [x] - HAL abstraction concept
- [~] - PC HAL implementation
- [~] - MCU HAL implementation
- [~] - Common application code for PC and MCU
- [~] - Identical `main.cpp` for PC and MCU
- [ ] - Complete MCU hardware integration
- [ ] - Complete PC simulation platform

## Diagnostics

### Debug messages

- [x] - Logging architecture explored
- [x] - Log levels
- [x] - Tags
- [x] - PC logging backend
- [x] - MCU logging backend
- [~] - Debug monitor
- [ ] - Release-build logging policy

### Events

- [~] - Event/message distinction
- [~] - Event generation concept
- [~] - Event storage architecture
- [~] - Event history
- [~] - Furnace value history
- [ ] - Complete event API
- [ ] - Complete event storage
- [ ] - UI event history
- [ ] - Persistent event history

## Alarms

- [~] - Alarm concept
- [~] - AlarmDispatcher architecture
- [~] - Alarm state tracking
- [~] - Alarm → Furnace error/stop behavior
- [ ] - Complete alarm implementation
- [ ] - Alarm definitions
- [ ] - Alarm priority/severity
- [ ] - Alarm acknowledgement
- [ ] - Alarm UI
- [ ] - Alarm history

## Testing

### Unit tests

- [ ] RingBuffer
- [ ] PID
- [ ] FSM
- [ ] Profile
- [ ] Furnace model
- [ ] TC Parser
- [ ] Furnace
- [ ] Alarm Dispatcher
- [ ] Event storage

### Integration tests

- [ ] Profile → Furnace
- [ ] TC Parser → Furnace
- [ ] Furnace → PID → Output
- [ ] Alarm → Furnace Error
- [ ] Furnace → Events
- [ ] UI → Furnace

### System tests

- [ ] Complete PC furnace simulation
- [ ] Profile execution simulation
- [ ] Alarm simulation
- [ ] Long-running simulation
- [ ] MCU integration testing

## Integration

- [~] - Core ↔ Application integration
- [~] - Settings ↔ Furnace integration
- [~] - Profile ↔ Furnace integration
- [~] - TC Parser ↔ Furnace integration
- [~] - Furnace ↔ UI integration
- [~] - AlarmDispatcher ↔ Furnace integration
- [~] - Scheduler ↔ Application integration
- [ ] - Complete system integration

## Resource usage

- [ ] - Measure Flash usage
- [ ] - Measure RAM usage
- [ ] - Review linker map
- [ ] - Identify largest Flash consumers
- [ ] - Identify largest RAM consumers
- [ ] - Define Flash/RAM safety margins
- [ ] - Verify release build fits STM32G030F6P6

## Finalization

- [ ] - Safety behavior review
- [ ] - Error handling review
- [ ] - Resource usage review
- [ ] - RAM/flash usage review
- [ ] - Timing/determinism review
- [ ] - Release configuration
- [ ] - PC simulation verification
- [ ] - STM32 verification
- [ ] - Hardware testing
- [ ] - Final documentation
- [ ] - Production-ready build