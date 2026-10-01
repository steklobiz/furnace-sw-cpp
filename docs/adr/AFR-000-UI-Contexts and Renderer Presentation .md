# ADR: UI Contexts and Renderer Presentation

## Status

Accepted

## Context

The common `Ui` currently contains concepts such as `Main`, `ProfileSelection`, `ProfileEditor`, `Settings`, `Monitor`, and `Events`.

These were previously represented as UI pages or views. This terminology became ambiguous because different concepts were being mixed together:

* logical areas of the application;
* navigation through collections;
* different portions of a physical display;
* renderer-specific presentation states.

Different renderers also have different presentation capabilities.

For example, a TUI can display many values simultaneously, while a small LCD may need several screens to present the same information.

Therefore, the common UI must not model renderer-specific presentation constraints.

## Decision

The common UI uses the concept of a **UI Context**.

A UI Context represents a renderer-independent area of user interaction.

Examples:

```text
Main
ProfileSelection
ProfileEditor
Settings
Monitor
Events
Samples
Question
Result
```

A Context answers:

> What part of the application is the user interacting with?

A Context does not describe a physical display screen.

### Context-specific navigation state

A Context may have state required for its interaction.

Examples:

```text
ProfileSelection
    selected profile
    collection page

ProfileEditor
    selected profile
    current step

Events
    collection page
```

This state belongs to the common UI when it is required for interaction semantics.

Collection paging is a navigation concept. It is not a renderer screen.

### Application data

Application data remains owned by the appropriate application modules.

For example:

```text
Furnace
    state
    temperature
    setpoint
    power

ProfileManager
    profiles
    steps

DataAggregator
    events
    samples
```

The UI consumes this data but does not duplicate its ownership.

### Renderer presentation

A renderer is responsible for deciding how a UI Context and its data are presented.

A renderer may introduce its own:

* Screens;
* presentation modes;
* layouts;
* display mappings;
* input mappings;
* visible-item capacities.

These concepts are renderer-specific and must not be added to the common UI merely because one renderer requires them.

For example:

```text
Common UI:

Context = Main
Furnace state = Running
Temperature = 120
Setpoint = 150
Power = 70
```

A TUI may present all of this on one screen.

A small LCD may present the same information using several renderer-specific screens or presentation modes.

The common UI does not need to know about those screens.

## Renderer capacity

When a renderer has limited capacity for a collection, it may provide the required capacity to the common UI.

For example:

```text
DWIN:
    12 selectable profiles

Small LCD:
    4 selectable profiles
```

The UI may use this capacity to implement collection navigation.

The UI must not receive renderer-specific details such as:

```text
4 rows
3 columns
480 × 320 pixels
DWIN page 4
```

Only the semantic capacity required for the interaction should cross the renderer/UI boundary.

## Terminology

The following terminology is used:

### UI Context

A renderer-independent area of user interaction.

Examples:

```text
Main
ProfileSelection
ProfileEditor
Settings
Monitor
Events
```

### Collection Page

A portion of a collection being browsed.

Examples:

```text
ProfileSelection:
    page 0
    page 1
    page 2

Events:
    page 0
    page 1
```

A collection page is not a UI Context and not a renderer screen.

### Screen

A renderer-specific presentation.

A Screen describes how a Context or collection page is presented by a particular renderer.

### Presentation Mode

An optional renderer-specific concept used when one renderer needs multiple presentations of the same Context.

A Presentation Mode is not part of the common UI architecture.

### View

The term `View` is not used as an architectural concept in the common UI.

## Consequences

### Positive

* The common UI remains renderer-independent.
* TUI and LCD renderers can present the same application state differently.
* Renderer limitations do not leak into application semantics.
* Collection paging remains separate from physical display screens.
* Renderer-specific presentation can evolve independently.
* The architecture does not require every renderer to implement concepts it does not need.

### Negative

* Renderer implementations have their own presentation state.
* The boundary between common navigation state and renderer presentation must be maintained carefully.
* A renderer may need to communicate limited capacity information to the common UI.

## Rejected alternatives

### UI Pages

Rejected because `Main`, `Settings`, `Monitor`, etc. are not necessarily physical or collection pages.

### Common UI Views

Rejected because the term became ambiguous with renderer presentation concepts.

### Common UI Modes

Rejected because different renderers may need different presentation modes. For example, a TUI may not need modes that are required by a small LCD.

### Renderer-specific concepts in Ui

Rejected because this couples the common UI to a particular display technology.

## Summary

The architecture follows:

```text
                 Common UI
                     │
                  Context
                     │
          context-specific state
                     │
          application data sources
                     │
                     ▼
                 Renderer
                     │
        ┌────────────┼────────────┐
        ▼            ▼            ▼
     Screen       Mode*        Layout
                                  │
                            Capacity/input
```

`*` Presentation Mode is optional and renderer-specific.

The fundamental rule is:

> **The common UI describes what the user is interacting with. The renderer decides how that interaction is presented.**
