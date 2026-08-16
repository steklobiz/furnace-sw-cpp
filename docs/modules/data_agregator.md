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
- event producers
- temperature sample producers

A source may provide only data that has changed since the previous
aggregation.

## Outputs

DataAggregator provides:

- current data values;
- data versions;
- information about changed fields;
- event items;
- temperature-history items.

The exact query/API mechanism is not yet defined.

## Information model

Data is organized by source and field.

Conceptually:

    Source
        |
        +-- Field
        +-- Field
        +-- Field
        ...

A field has:

- source identity;
- field identity;
- value;
- version.

The exact C++ representation of field values is not yet defined.

Initially, the supported value categories are expected to be:

- byte;
- word;
- string.

Historical/queued data is stored as sequences of items rather than as
ordinary current-value fields.

## Dependencies

DataAggregator depends on the application data sources from which it
collects data.

It may use:

- core::RingBuffer for bounded queues/history.

It must not depend on UI or renderer modules.

## Public Interface

The public interface is not yet finalized.

It will provide mechanisms to:

- initialize/configure the aggregator;
- refresh/receive data from sources;
- obtain current field values;
- obtain field versions;
- determine changed fields;
- access queued event/history items.

## Internal State

DataAggregator is expected to contain:

- current field values;
- versions associated with stored values;
- change information;
- event queue;
- temperature-history queue.

Storage must be statically bounded.

No dynamic allocation is required.

## Processing Flow

Conceptually:

    Application source
          |
          | new/updated data
          v
    DataAggregator
          |
          +-- compare with stored value
          |
          +-- update value
          |
          +-- update version
          |
          +-- mark field changed
          |
          +-- append new event/sample if applicable
          |
          v
       consumers

The exact update mechanism is not yet defined.

## Timing

DataAggregator does not define application timing.

It may be refreshed periodically by the application scheduler or updated
when a source reports new data.

The exact mechanism is an open question.

## Error Handling

DataAggregator should not use exceptions or dynamic error objects.

Invalid source/field identifiers and other error conditions should be
handled according to the final API design.

Exact error handling is not yet defined.

## Configuration

Storage capacities should be compile-time constants.

Examples:

- event capacity;
- temperature-history capacity;
- maximum number of data fields.

Exact configuration values are not yet defined.

## Interactions with Other Modules

### Application data sources

Sources provide data to DataAggregator.

DataAggregator does not control the behavior of the sources.

### UI

UI consumes data from DataAggregator.

DataAggregator has no knowledge of UI pages, actions, or renderers.

### Renderers

Renderers do not access application sources directly.

They obtain UI-required data through the UI/data path.

### Core

DataAggregator may use core::RingBuffer for fixed-capacity queues.

## Design Notes

DataAggregator is an application-level component, not a UI component.

Its purpose is data aggregation and retention, not presentation.

The aggregator should preserve native data types. Conversion to text or
other presentation-specific formats belongs to the renderer.

The aggregator may retain data that is not currently displayed. This allows
a page to be changed without requiring immediate re-collection of all data.

Field order should remain stable through the data path where practical.

The aggregator may receive only the latest changed item for queue-like
data. It does not need to receive the complete queue on every update.

## Open Questions

- How are data sources registered?
- How does a source report that data has changed?
- Does the aggregator poll sources, receive notifications, or support both?
- How are versions generated?
- What exactly constitutes a data field?
- Exact FieldId representation.
- Exact field-value representation.
- Exact API for obtaining changed fields.
- Exact API for reading event/history items.
- How does the aggregator distinguish current fields from queued data?
- Does every application data source need to participate in the aggregator?
- How is initialization performed?

## Future Extensions

Possible future extensions include:

- additional data sources;
- additional value types;
- additional bounded queues;
- multiple consumers;
- persistence of selected data;
- more sophisticated change/version tracking.

## Important note:

DataAggregator must remain independent of UI and presentation.

It aggregates application data for consumers; it does not define how that
data is displayed or interacted with.