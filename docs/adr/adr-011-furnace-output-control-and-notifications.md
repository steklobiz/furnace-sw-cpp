# ADR: Furnace Output Control and Notifications

## Status

Accepted

## Context

`Furnace` determines the required output state and currently applies it through the HAL:

```cpp
hal::set_outputs(uint8_t outputs);
```

The HAL is responsible only for applying the output state to the hardware or simulator. It does not know why an output was changed.

We also want output changes to appear as events in `DataAggregator` and on the TUI Events page.

## Decision

`Furnace` owns the application-level output state and is responsible for output-change notifications.

Output interaction is centralized in an internal function:

```cpp
void set_outputs(uint8_t outputs) noexcept;
```

The function:

1. Compares the requested state with the current state.
2. Determines which output bits were set or reset.
3. Updates the Furnace output state.
4. Applies the complete output byte through `hal::set_outputs()`.
5. Generates `OutputSet` / `OutputReset` notifications for changed outputs.

The notification argument identifies the output number, not the bit mask.

For example:

```text
old: 0001
new: 0101

OutputSet, ID=2
```

## Responsibilities

```text
Furnace
  ├── decides required output state
  ├── owns output state
  ├── applies state through HAL
  └── generates output-change events

HAL
  └── applies output state to hardware/simulator

DataAggregator
  └── records output-change notifications as events
```

Output notifications represent **application-level commanded state changes**, since the current HAL interface does not report hardware-level failures.

## Consequences

* Output changes have a single point of control in `Furnace`.
* HAL remains independent of application events.
* Repeatedly requesting the same output state does not generate duplicate events.
* The same behavior works for both PC simulation and STM32 hardware.
* The Events page can show individual output changes.
