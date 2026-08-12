# Furnace

## Responsibility

Controls the thermal process.

## States

- Idle
- Running
- Paused
- Finished
- Stopped
- Error

## Inputs

- Thermocouple data
- Profile
- PID settings
- Commands

## Outputs

- Heater output
- Furnace state
- Events

## Processing

Furnace::process() is called by the scheduler once per second.

## Dependencies

- Profile
- TcParser
- PID
- EventStream
- AlarmDispatcher
