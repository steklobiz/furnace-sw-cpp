# ADR: UI Commands and Application Coordination

## Status

Accepted

## Context

Some UI actions affect more than one application module.

For example, resetting the Furnace requires both:

* clearing active alarms in `AlarmDispatcher`;
* resetting the Furnace state and process data.

The UI should not coordinate these modules directly.

## Decision

UI actions that require application-level coordination are sent to `Application`.

The command flow is:

```text
TUI input
    │
    ▼
UI action
    │
    ▼
Application
    │
    ├── AlarmDispatcher
    └── Furnace
```

The UI sends the command **when the action is executed**, not during rendering or periodic UI processing.

For `ResetFurnace`, Application performs:

```cpp
void Application::reset_furnace() noexcept
{
    alarm_dispatcher_.clear_all();
    furnace_.reset();
}
```

`Furnace` remains independent of `AlarmDispatcher`, and `Ui` does not coordinate application modules directly.

## Consequences

* UI remains responsible for presentation and user actions.
* Application coordinates operations involving multiple modules.
* Furnace owns only its own reset/state logic.
* AlarmDispatcher remains independent of Furnace reset implementation.
* The same command mechanism can be used for future application-level UI actions.
