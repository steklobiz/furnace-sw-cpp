# DataAggregator

## Purpose

Provide a centralized, application-owned store of data collected from
multiple application data sources.

DataAggregator separates data collection and storage from the modules that
consume the data, such as UI.

## Responsibilities

- Collect data from registered application data sources.
- Store the latest value of collected data.
- Store the version of each data item.
- Detect changes between newly collected and stored values.
- Track changed fields.
- Store bounded event data.
- Store bounded temperature-history data.
- Provide collected data to consumers.
- Keep data available independently of the current UI page.

## Non-Responsibilities

DataAggregator does not:

- know about UI pages;
- know about page descriptors;
- know about renderers;
- know about LCD, TUI, or other presentation backends;
- convert values into presentation text;
- process UI actions;
- control application behavior;
- define application-domain behavior.

## Inputs

DataAggregator receives data from application data sources.

Examples:

- Furnace
- ProfileManager
- SettingManager
- TcParser
- AlarmDispatcher
- other event producers

A source may provide only data that has changed since the previous
aggregation.

Sources may also report discrete application events through notifications.

## Outputs

DataAggregator provides:

- current data values;
- information about changed fields;
- event items;
- temperature-history items.

The current implementation uses snapshots of application data and
notification-driven updates. Presentation-specific formatting remains
outside DataAggregator.

## Information model

Data is organized by source and field.

Conceptually:

    Source
        |
        +-- Field
        +-- Field
        +-- Field
        ...

A current field has:

- source identity;
- field identity;
- value.

Historical/queued data is stored as sequences of items rather than as
ordinary current-value fields.

The current implementation uses native application values in the
aggregator. Where a field is exposed through the current UI-oriented
interface, its value is represented according to that interface's type.

## Dependencies

DataAggregator depends on the application data sources from which it
collects data.

It uses:

- core::RingBuffer for bounded event and temperature-history storage.

It must not depend on UI or renderer modules.

## Public Interface

The current public interface provides mechanisms to:

- initialize/configure the aggregator;
- obtain current field values;
- obtain the current profile snapshot;
- obtain retained events;
- obtain retained temperature-history samples;
- clear retained history.

The interface is intentionally oriented toward consumers rather than
application-source internals.

## Internal State

DataAggregator contains:

- current field values;
- event queue;
- temperature-history queue;
- references to registered application data sources.

Storage is statically bounded.

No dynamic allocation is required.

## Processing Flow

Conceptually:

    Application source
          |
          | DataReady / event notification
          v
    DataAggregator
          |
          +-- update current snapshot
          |
          +-- detect changed fields
          |
          +-- retain event if applicable
          |
          +-- retain sample if sampling interval has elapsed
          |
          v
       consumers

For event timestamps, DataAggregator uses the current elapsed-time value
already present in its Furnace snapshot. It does not maintain a separate
application clock.

## Timing

DataAggregator does not define application timing.

It is updated when a registered source reports new data, typically through
the application's notification mechanism.

DataAggregator owns the decision of when to retain periodic temperature
samples, but the elapsed time used for sampling comes from Furnace.

## Notifications

Application sources can report both ordinary data updates and discrete
events.

Examples include:

- DataReady
- Error
- ProfileStarted
- StepStarted
- ProfileFinished
- EditProfileChanged
- StartProfileChanged
- SettingsChanged

DataAggregator interprets notifications according to their purpose.

For example:

- `DataReady` updates the corresponding current snapshot and may trigger
  periodic sample retention.
- `ProfileStarted` starts a new retained history and records the event.
- `StepStarted` records a retained event.
- `ProfileFinished` records a retained event.

The notification mechanism keeps application sources independent of
DataAggregator's storage implementation.

## History

DataAggregator owns bounded historical retention.

### Events

Events are stored in a fixed-capacity `core::RingBuffer`.

An event contains:

- elapsed time;
- source;
- notification type;
- optional argument.

The elapsed time is obtained from the current Furnace snapshot when the
event is retained.

### Temperature samples

Temperature samples are stored in a fixed-capacity `core::RingBuffer`.

A sample contains:

- elapsed time;
- temperature;
- heater output.

Sampling is a DataAggregator responsibility. Furnace only provides the
current values through its normal data notification.

Starting a new profile clears the retained events and samples.

## Error Handling

DataAggregator should not use exceptions or dynamic error objects.

Invalid source/field identifiers and other error conditions should be
handled according to the final API design.

The aggregator does not decide how application errors are handled. For
example, AlarmDispatcher remains responsible for alarm processing and
application-level safety actions.

## Configuration

Storage capacities are compile-time constants.

The current implementation uses the existing project configuration for
history capacities and sampling period.

## Interactions with Other Modules

### Application data sources

Sources provide current data and notifications to DataAggregator.

DataAggregator does not control the behavior of the sources.

### Furnace

Furnace is the authoritative source of furnace process state, including
elapsed process time.

Furnace reports its current data through `DataReady` and reports discrete
process events through notifications.

Furnace does not directly write DataAggregator history.

### AlarmDispatcher

AlarmDispatcher reports alarm-related notifications to DataAggregator.

DataAggregator may retain relevant alarm/error events but does not replace
AlarmDispatcher's responsibility for alarm processing or furnace control.

### ProfileManager

ProfileManager provides profile data and profile-change notifications.

DataAggregator stores the current profile snapshot for consumers.

### SettingManager

SettingManager provides settings data and settings-change notifications.

### TcParser

TcParser provides temperature-related data and notifications.

### UI

UI consumes data from DataAggregator.

DataAggregator has no knowledge of UI pages, actions, or renderers.

### Renderers

Renderers do not access application sources directly.

They obtain UI-required data through the UI/data path.

### Core

DataAggregator uses `core::RingBuffer` for fixed-capacity queues.

## Design Notes

DataAggregator is an application-level component, not a UI component.

Its purpose is data aggregation and retention, not presentation.

The aggregator preserves native application data where practical. Conversion
to text or other presentation-specific formats belongs to the renderer.

The aggregator may retain data that is not currently displayed. This allows
a page to be changed without requiring immediate re-collection of all data.

Data sources remain responsible for their own domain behavior. DataAggregator
does not become a controller merely because it stores their data or events.

The aggregator may receive only the latest changed item for current-value
data. It does not need to receive the complete application state on every
update.

## Future Extensions

Possible future extensions include:

- additional data sources;
- additional value types;
- additional bounded queues;
- multiple consumers;
- persistence of selected data;
- more sophisticated change tracking;
- additional event types.

## Important Note

DataAggregator must remain independent of UI and presentation.

It aggregates application data for consumers; it does not define how that
data is displayed or interacted with.
