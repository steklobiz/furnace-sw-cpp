# Furnace Architecture

## 1. Overview

Furnace is an embedded furnace controller implemented in C++17.

The application is designed to run on both:

- PC — simulation and development
- STM32 — target hardware

Application code must not depend directly on the MCU hardware.

The system is deterministic and does not use:

- dynamic memory allocation
- exceptions
- RTTI
- virtual functions
- OS dependencies

---

## 2. Main Architecture

```text
                         Application
                              │
              ┌───────────────┼───────────────┐
              │               │               │
           Furnace            UI         Diagnostics
              │               │               │
       ┌──────┼──────┐        │        ┌──────┴──────┐
       │      │      │        │        │             │
      PID    TC     Model    TUI     Events      Debug messages
            Parser
       │
       │
    Alarms
              │
              ↓
             Core
       ┌──────┼──────┬──────┐
       │      │      │      │
      FSM   PID  Scheduler RingBuffer

              │
              ↓
          Platform
        ┌──────┴──────┐
        │             │
       PC            MCU
                      │
                MCU Drivers
                
```
## 3. Layers

### Core

Generic, hardware-independent functionality.

Examples:

- `core::Fsm`
- `core::Pid`
- `core::RingBuffer`
- `core::Scheduler`

Core modules must not depend on:

- Furnace
- UI
- MCU
- PC platform
- hardware devices

### Application

Contains furnace-specific behavior and application coordination.

Examples:

- Furnace
- Profile
- ProfileManager
- SettingManager
- TcParser
- AlarmDispatcher
- EventStream
- UI

Application code may use Core but must not directly access MCU peripherals.

### Devices

Drivers for external hardware devices.

Examples:

- MAX6675
- DWIN
- MCP23S17
- MAX485

Device drivers use lower-level platform/MCU interfaces where necessary.

### MCU Drivers

Drivers for STM32 peripherals.

Examples:

- GPIO
- SPI
- UART
- Timer
- PWM

These drivers are MCU-specific.

Application code must not depend directly on them.

### Platform

Provides platform-specific services and implementations.

There are two platforms:

platform/
├── pc/
└── mcu/

The application should use the same interfaces regardless of platform.


## 4. Ownership

The application root owns major application objects.

For example:

App
├── Furnace
├── TcParser
├── AlarmDispatcher
├── ProfileManager
├── SettingManager
├── History
├── PID
├── UI
└── Scheduler

Objects are created with explicit ownership.

No singleton-based global ownership is used.

Dependencies are passed explicitly.


## 5. Furnace

Furnace owns the thermal process state.

It is responsible for:

- profile execution
- step progression
- setpoint calculation
- PID operation
- output decisions
- furnace state
- thermal process timing

Furnace does not own:

- UI
- persistent settings storage
- hardware drivers
- display communication


## 6. Thermocouple Parser

`TcParser` is responsible for thermocouple data.

It provides:

- current samples
- sensor status
- average temperature
- overtemperature information

Furnace consumes the parsed data.

The parser does not control Furnace.


## 7. Alarm Dispatcher

`AlarmDispatcher` is responsible for detecting and maintaining alarm conditions.

It may request Furnace to enter an error/safe state.

Alarm handling must result in safe heater behavior.

The exact alarm API is still under development.


## 8. UI

The UI is separate from Furnace.

The UI sends commands to the application/Furnace and reads application state.

The UI must not implement thermal process logic.

### UI States

The current UI states are:

- Main
- Monitor
- ProfileSelect
- Result


### UI FSM

The current UI FSM implementation is temporary.

The target implementation uses the table-driven `core::Fsm` architecture.

State + Event
      ↓
transition table
      ↓
next state
      ↓
enter / handler / exit


## 9. Diagnostics

Diagnostics are divided conceptually into:

### Debug messages

Developer-oriented information.

Used during development and debugging.

### Events

Operator/system history.

Examples:

- profile started
- profile finished
- step changed
- furnace stopped
- alarm raised
- alarm cleared

Events may contain important furnace values.

Events are more important to the production system than debug logging.


## 10. Storage

Fixed-size storage is preferred.

`core::RingBuffer` provides generic fixed-capacity storage.

Application-specific streams may use it for:

- events
- temperature history
- diagnostic history

No dynamic containers are used in the embedded application.


## 11. Scheduling

Periodic application processing is driven by the scheduler.

Typical flow:

Scheduler
    │
    ├── TC update
    │
    ├── Furnace process
    │
    ├── Alarm processing
    │
    └── UI processing

Processing functions should be non-blocking.


## 12. PC / MCU Strategy

The same application code must run on PC and MCU.

Only platform-specific implementation changes.

              Application
                   │
          ┌────────┴────────┐
          │                 │
        PC build          MCU build
          │                 │
      PC platform       MCU platform

PC provides simulation and development capabilities.

MCU provides the real hardware implementation.


## 13. Design Rules

1. Prefer simple designs over abstractions without a clear benefit.
2. Keep dependencies explicit.
3. Keep ownership explicit.
4. Avoid circular dependencies.
5. Keep hardware out of application logic.
6. Keep thermal logic out of UI.
7. Keep generic algorithms in Core.
8. Use fixed-size storage.
9. Avoid dynamic allocation.
10. Avoid unnecessary global state.
11. Keep PC and MCU application behavior equivalent.
12. Stabilize interfaces before building many dependencies on them.
13. Test stable behavioral modules on PC.
14. Prefer deterministic, non-blocking processing.

## 14. C++ Usage Policy

The project uses C++17, but intentionally uses a restricted
embedded-oriented subset of C++.

The code should remain understandable to an experienced
embedded C developer.

### Preferred

- classes
- namespaces
- `enum class`
- references
- `constexpr`
- templates where they provide simple static generic code
- fixed-size containers
- compile-time tables
- strong types
- RAII where useful and deterministic

### Avoid

- dynamic allocation
- exceptions
- RTTI
- virtual functions
- inheritance unless clearly justified
- `std::string`
- `std::vector`
- `std::function`
- iostreams
- complex template metaprogramming
- unnecessary operator overloading
- hidden global state

### Goal

C++ should provide better type safety, encapsulation and
compile-time facilities without making the code dependent
on advanced C++ knowledge.

A future developer experienced in embedded C should be able
to understand and maintain the project without becoming a
C++ language expert.
