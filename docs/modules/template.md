# Module Name

## Purpose

*Why does this module exist?*

Example:
Reads thermocouple data and converts it into validated temperature samples for the application.

	
## Responsibilities

*Things this module MUST do.*

Example:

- Read sensor.
- Parse raw data.
- Validate sample.
- Report sensor status.
- Check if TC is connected
-

## Non-Responsibilities

This prevents future feature creep.

TC_Parser does not:

- control the heater;
- store temperature history;
- decide alarms;
- update UI.



## Inputs

*Everything entering the module.*

Example:

- Scheduler tick (4 Hz)
- MAX6675 driver
- Configuration


## Outputs

*Everything leaving the module.*

Example:

Latest Sample
SensorStatus

## Information model

*Data Types* (One of the most important section)

Sample:
- temperature
- status
- timestamp

SensorStatus

- Valid
- Disconnected
- ShortCircuit
- Fault

## Dependencies

*External modules.*

Example:

MAX6675 driver


## Public Interface

Only conceptual.

Example:

init()
update()
get_last_sample()
status()


## Internal State

*What the module remembers.*

Example:

latest_sample

sensor_status

has_valid_sample


## Processing Flow

Add diagram.

update()
↓
read sensor
↓
parse
↓
validate
↓
store latest sample
↓
update status


## Timing

Example:

update()

called every 250 ms

maximum latency:
250 ms

Execution time:
<100 µs


## Error Handling

Example:

Disconnected sensor
↓
status = Disconnected
↓
sample becomes invalid


## Configuration

*Things that can change.*

Example:

- sampling frequency
- sensor type
- validation limits


## Interactions with Other Modules

Example:

Providers:
- MAX6675

Consumers:
- Furnace
- UI
- TemperatureHistory


## Design Notes

*Interesting decisions.*

Example:
- Stores only the latest sample.
- History belongs elsewhere.


## Open Questions

*Things intentionally undecided.*


## Future Extensions

*Ideas that are NOT part of today's implementation.*


## Important note:

While writing this specification, we are forbidden to write C++.
