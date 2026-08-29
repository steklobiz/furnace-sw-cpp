# ADR: Introduce DataAggregator

**Status:** Accepted  
**Date:** 2026-08-15

## Context

The UI needs data from several application sources:

- Furnace
- ProfileManager
- SettingManager
- TcParser
- AlarmDispatcher
- application events
- temperature samples

We considered letting the UI collect this data directly.

This would make the UI responsible for:

- knowing individual application data sources;
- requesting and refreshing their data;
- detecting data changes;
- storing data between page changes;
- maintaining event/history queues.

As the number of pages and data sources grows, this would tightly couple the
UI to the application structure and make the UI harder to maintain.

## Decision

Introduce a separate `DataAggregator` class owned by the application.

`DataAggregator` knows about application data sources but knows nothing about:

- UI pages;
- fields defined by page descriptors;
- renderers;
- LCD/TUI/Web presentation;
- UI actions.

`DataAggregator` stores:

- current application data;
- information about changed data;
- bounded event data;
- bounded temperature-history data.

The UI consumes data from `DataAggregator` instead of collecting it directly
from application sources.

Architecture:

    Application
         |
         +-----------------------------+
         |                             |
         v                             v
    Application sources         DataAggregator
         |                             |
         | notifications               |
         +---------------------------->|
                                       |
                                       v
                                      UI
                                       |
                                       v
                                   Renderers

The application sources remain responsible for their own domain behavior.
`DataAggregator` is a data collection and retention component, not a
controller.

## Update and Notification Model

Data sources provide information to `DataAggregator` through the project's
notification mechanism.

Notifications can represent:

- ordinary data availability (`DataReady`);
- application events such as `ProfileStarted`, `StepStarted`, and
  `ProfileFinished`;
- data-change notifications such as profile or settings changes;
- errors and other future event types.

For ordinary data updates, `DataAggregator` reads the current state from the
source and updates its snapshot.

For discrete events, `DataAggregator` may retain an event in its bounded
event queue.

The notification path is intentionally directed toward `DataAggregator`;
application sources do not depend on UI or renderer modules.

## Event and History Retention

`DataAggregator` owns bounded historical retention.

Events and temperature samples are stored using the existing
`core::RingBuffer` implementation.

An event contains:

- elapsed process time;
- source;
- notification type;
- optional argument.

The timestamp is obtained from the current Furnace elapsed-time value already
stored in the `DataAggregator` snapshot. `DataAggregator` does not maintain a
second application clock.

Temperature sampling is also a `DataAggregator` responsibility. Furnace
provides current temperature, output, and elapsed-time data through its normal
data update. `DataAggregator` decides when a sample is due and retains it.

When a new profile starts, `DataAggregator` clears the retained events and
temperature samples and then records the `ProfileStarted` event.

## Rationale

1. Separates application data collection from UI interaction.
2. Removes direct dependencies from UI to individual application sources.
3. Allows data to remain available when the user changes pages.
4. Centralizes data aggregation and change handling.
5. Provides a natural owner for bounded event and history queues.
6. Allows future consumers to use aggregated data without making the
   aggregator UI-specific.
7. Keeps application sources independent of presentation.
8. Keeps historical retention out of domain modules such as Furnace.

## Alternatives Considered

### UI collects data directly

**Rejected** because it couples UI to application data sources and makes UI
responsible for data management and history retention.

### Renderers collect data directly

**Rejected** because renderers would need to know application organization
and would duplicate data-access logic.

### Application sources notify UI directly

**Rejected** because this couples application modules to the UI.

### Furnace owns history collection

**Rejected** because periodic history sampling is a consumer/data-retention
concern rather than Furnace domain behavior. Furnace should provide its
current state and process events; `DataAggregator` decides what to retain.

### DataAggregator maintains its own process clock

**Rejected** because Furnace is the authoritative owner of process elapsed
time. The aggregator uses the current Furnace elapsed-time snapshot when it
needs a timestamp.

## Consequences

### Positive

- lower UI/application coupling;
- centralized data management;
- shared data for multiple consumers;
- simpler renderers;
- bounded, allocation-free event/history retention;
- application sources remain independent of presentation;
- historical data remains available independently of the current UI page.

### Negative

- additional abstraction;
- additional statically bounded storage;
- notification/update paths must be maintained;
- consumers depend on the DataAggregator interface.

## Open Questions

The following details remain implementation-level concerns rather than
reasons to change the architectural decision:

- exact data-field/value representation;
- exact change-detection API;
- exact event/history query API;
- exact UI query API;
- additional notification/event types;
- whether further application sources should participate in aggregation.

The notification mechanism and bounded event/history ownership are no longer
open architectural questions: sources notify `DataAggregator`, and
`DataAggregator` owns retained event/history data.

## Implementation Constraints

`DataAggregator` must remain independent of UI and presentation.

It must not:

- know about UI pages or page descriptors;
- access renderers;
- format data for presentation;
- process UI actions;
- control application behavior.

Storage must remain statically bounded and compatible with the project's
embedded constraints. No dynamic allocation, exceptions, or UI-specific
dependencies are required.
