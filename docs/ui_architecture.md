# UI Architecture

Status: Work in progress
Date: 2026-08-15

## Purpose

Define the architecture of the new Furnace UI before implementation.

The UI is the boundary between user interaction and the application.
Renderers provide presentation and translate backend input into semantic
UI actions.

The architecture is designed to keep application behavior independent
from UI presentation and renderer-specific implementation.

## Architecture Overview

    APPLICATION
    ┌─────────────────────────────────────────────────────┐
    │                                                     │
    │  Furnace                                            │
    │  AlarmDispatcher                                    │
    │  ProfileManager                                     │
    │  SettingManager                                     │
    │  TcParser                                           │
    │  PID                                                │
    │  DataAggregator                                     │
    │  Scheduler                                          │
    │  ...                                                │
    │                                                     │
    └──────────────────────┬──────────────────────────────┘
                           │
                    data / commands
                           │
                           ▼
    ┌─────────────────────────────────────────────────────┐
    │                         UI                          │
    │                                                     │
    │  current page                                       │
    │  page descriptors                                   │
    │  fields                                             │
    │  global actions                                     │
    │  page actions                                       │
    │  action handlers                                    │
    │                                                     │
    └──────────────────────┬──────────────────────────────┘
                           │
                    page + field data
                    semantic actions
                           │
              ┌────────────┴────────────┐
              ▼                         ▼
       LCD Renderer               TUI Renderer
              │                         │
              ▼                         ▼
       LCD Backend                 TUI Backend


## Responsibilities

### Application

The application owns system behavior and application state.

It owns application data sources and the DataAggregator.

The application does not know about:

- UI pages;
- page descriptors;
- renderers;
- renderer-specific presentation;
- physical UI input.

### DataAggregator

DataAggregator is an application-owned, UI-independent component.

It collects and stores data from application sources.

It may store:

- current values;
- data versions;
- change information;
- event queue;
- temperature history.

It does not know about pages, fields as defined by page descriptors,
renderers, or presentation.

### UI

The UI owns:

- current UI state/page;
- page descriptors;
- fields required by pages;
- global actions;
- page-specific actions;
- action handlers;
- UI interaction workflows.

The UI consumes data from DataAggregator.

The UI may issue commands to application modules through action handlers.

### Renderer

A renderer is responsible for:

- presenting the current UI page;
- requesting the data required by the page;
- translating values into the renderer's representation;
- tracking what it has already rendered;
- translating backend input into semantic UI actions.

The renderer does not know where application data comes from.

The renderer does not execute application behavior.

### Backend

The backend provides renderer-specific I/O.

Examples:

- DWIN/LCD communication;
- terminal output;
- keyboard input.


## Non-Responsibilities

### UI does not

- know physical button/key details;
- know LCD addresses or TUI line positions;
- define fonts, colors, or visual layout;
- own application data;
- collect data directly from every application module.

### DataAggregator does not

- know UI pages;
- know renderers;
- know presentation;
- process UI actions;
- control application behavior.

### Renderer does not

- access Furnace, PID, ProfileManager, etc. directly;
- decide application behavior;
- implement UI workflows;
- store an authoritative copy of application data.


## Fields

A page descriptor contains a set of fields and possible actions.

A field describes data required by the page.

A field does not describe its visual representation.

A field therefore does not contain:

- color;
- font;
- position;
- LCD address;
- TUI line;
- widget type.

The term "field" is preferred over "widget".

A field is conceptually identified by:

    Source + Field

The exact C++ representation is not yet decided.

Application data sources may define their own field enumeration, for example:

    Furnace:
        Temperature
        State
        Setpoint

The renderer does not need to know the source's internal field definition.


## Data Sources

Several application components may provide data to DataAggregator.

Possible sources include:

- Furnace;
- PID;
- TcParser;
- ProfileManager;
- SettingManager;
- other application modules.

A data source is defined by its ability to provide data, not necessarily by
its position in the application class hierarchy.

For example, Furnace and TcParser currently provide two semantically
different temperature values:

    TcParser.Temperature
        measured sensor temperature

    Furnace.Temperature
        temperature used by furnace/process logic

They should not be merged merely because their current values may be equal.


## Data Values

The initial value categories are intended to be small:

- byte;
- word;
- string.

Values remain native data through the application and UI data path.

Conversion to text is the responsibility of the renderer.

For example:

- TUI may convert an enum/integer to readable text;
- DWIN may receive the integer directly.

The aggregator should not convert values into presentation text.


## Versions and Change Tracking

DataAggregator stores the current value together with its version.

When new data is received, the aggregator compares it with the stored
value.

If the value changed, the corresponding field is marked as changed and
its version is updated.

Conceptually:

    source
       |
       | new value
       v
    aggregator
       |
       +-- compare with stored value
       |
       +-- update value if changed
       |
       +-- update version
       |
       +-- mark field changed


A renderer may store the last version it rendered instead of storing its
own copy of the value.

This is especially useful for string values.


## Events and Temperature History

DataAggregator may be the central bounded storage location for event and
temperature-history data.

A source does not need to transfer the complete queue repeatedly.

For example:

    new event
        |
        v
    DataAggregator
        |
        +-- append one item to event queue

    new temperature sample
        |
        v
    DataAggregator
        |
        +-- append one item to sample queue

The UI may request historical items individually.

Fixed-capacity storage should be used. core::RingBuffer is suitable for
these queues.


## Pages

A page descriptor contains:

- ordered fields;
- possible page-specific actions.

It does not contain visual layout information.

Example:

    Monitor page

    Fields:
        Furnace.Temperature
        Furnace.State
        Furnace.Setpoint

The renderer decides how those fields are represented.

Field order should remain consistent through the logical data path where
practical.

A renderer may introduce its own final mapping, for example:

    Field -> DWIN variable address

or:

    Field -> TUI line


## Actions

Actions are semantic user operations.

Examples:

    Home()
    Select(id)
    Stop()

Actions are independent from fields.

### Global actions

The UI may define global actions available from any page.

Example:

    Home()

### Page actions

A page may define additional actions that are meaningful on that page.

### Action arguments

Actions may have typed arguments.

Example:

    Select(profile_id)

The renderer converts backend input into the semantic action.

Different renderers may therefore produce the same action from different
physical inputs.

### Action handlers

Action handlers belong inside the UI.

They may:

- change the current page;
- update UI state;
- implement multi-step workflows;
- request confirmation;
- issue commands to the application.

Example:

    Stop()
       |
       v
    UI action handler
       |
       v
    ConfirmStop page
       |
       v
    Confirm()
       |
       v
    application stop command

The renderer does not need to know about this workflow.


## Renderer Responsibilities

The renderer knows how to represent values.

For example, if a field contains an integer representing an enum:

    value = 1

the TUI renderer may display:

    Running

while a DWIN renderer may transfer:

    1

The renderer may maintain local presentation state such as:

- current rendered page;
- last rendered versions;
- renderer-specific field mapping.

It should not maintain authoritative application data.


## UI State and FSM

The UI is stateful and may be modeled as a state machine.

However, a generic core::Fsm dependency is not currently an architectural
requirement.

The UI may use core::Fsm internally if it proves useful during
implementation.

The architecture should not depend on the existence of core::Fsm.

Simple page/state transitions may be implemented directly if that is
clearer.


## Core Containers

core::RingBuffer remains useful.

It can provide fixed-capacity storage for:

- events;
- temperature samples;
- other bounded sequences.

No dynamic allocation is required.


## Data Flow

Application data flow:

    Data Source
        |
        v
    DataAggregator
        |
        v
    UI
        |
        v
    Renderer
        |
        v
    Backend


User input flow:

    Backend
        |
        v
    Renderer
        |
        | semantic action
        v
    UI
        |
        v
    Action Handler
        |
        +----> UI state/page change
        |
        +----> application command


## Ownership

Application owns:

- application modules;
- DataAggregator;
- application state.

UI owns:

- UI state;
- pages;
- fields;
- actions;
- action handlers.

Renderer owns:

- presentation state;
- renderer-specific mappings;
- backend interaction.

Each layer should have one clear source of authoritative state.


## Design Principles

1. Application owns system behavior.
2. Application owns DataAggregator.
3. DataAggregator knows nothing about UI or presentation.
4. UI owns interaction workflows.
5. Renderer knows how to represent values.
6. Renderer translates backend input into semantic UI actions.
7. Renderer does not access application sources directly.
8. Fields describe data, not appearance.
9. Actions describe user operations, not backend inputs.
10. Field and action concepts are independent.
11. No widget abstraction is required.
12. Native values remain native until presentation.
13. Fixed-capacity storage is preferred.
14. Avoid unnecessary abstractions.
15. Do not create a generic data/reflection framework without a concrete
    requirement.
16. Keep the renderer/application boundary strict.
17. Keep DataAggregator independent of UI.
18. Preserve logical field order where practical.


## Implementation Strategy

The new UI should be implemented incrementally rather than all at once.

Preferred path:

    Architecture
        |
        v
    minimal DataAggregator
        |
        v
    one data source
        |
        v
    one field
        |
        v
    one page
        |
        v
    one renderer
        |
        v
    run on PC
        |
        v
    verify complete path
        |
        v
    expand incrementally

The first vertical slice should be deliberately small.

A possible first slice is:

    TcParser
        |
        | Temperature
        v
    DataAggregator
        |
        v
    Monitor page
        |
        v
    TUI renderer
        |
        v
    terminal

After the complete path works, additional fields, sources, pages, actions,
history, and renderers can be added incrementally.


## Dependencies

The intended high-level dependency direction is:

    Application
        |
        +-- DataAggregator
        |
        v
       UI
        |
        v
     Renderer
        |
        v
     Backend

The exact C++ dependency graph is not yet finalized.

UI must not introduce dependencies from renderers back into application
modules.

DataAggregator must not depend on UI.


## Open Questions

- Exact DataAggregator public API.
- How sources provide data to DataAggregator.
- Whether sources notify the aggregator or the aggregator polls them.
- How source/data versions are generated.
- Exact FieldId representation.
- Exact field-value representation.
- Exact changed-field API.
- Exact event/history API.
- Exact page descriptor representation.
- Exact action definition representation.
- Exact action argument representation.
- Exact page transition implementation.
- Whether core::Fsm will be used internally by UI.
- Renderer/UI synchronization protocol.
- Renderer version tracking details.
- Whether all application data sources participate in DataAggregator.


## Future Extensions

Possible future extensions include:

- additional data sources;
- additional value types;
- additional pages;
- additional action types;
- additional renderers;
- additional bounded data queues;
- other consumers of DataAggregator.


## Important Note

This document describes the architecture currently agreed upon.

It is intentionally not a complete implementation specification.

Unresolved implementation details must be decided only when they are
needed by the implementation.

The architecture should remain simple and avoid introducing abstractions
that do not solve a concrete problem.
