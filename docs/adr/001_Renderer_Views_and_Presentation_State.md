# ADR: Renderer Views and Presentation State

## Status

Accepted

## Context

The common `Ui` defines semantic pages such as `Main`, `Monitor`, and `Settings`.

Different renderers may need different presentation states within the same page. For example, the DWIN renderer divides `Settings` into `SettingsPid` and `SettingsOther`.

## Decision

A **renderer view** is a renderer-specific presentation state within a UI page.

A view determines which UI fields and controls are presented and how they are mapped to the renderer's output.

The distinction is:

* **UI page** — semantic UI state.
* **Renderer view** — renderer-specific presentation state.
* **UI fields/data** — values provided by `Ui`.
* **Renderer descriptors** — mappings used to present a view.

A view does not own or duplicate application data.

For example:

```text
Main
  ├── MainIdle
  ├── MainRunning
  └──...
Events
  ├── EventsPage1
  ├── EventsPage2
  └──...
Settings
  ├── SettingsPid
  └── SettingsOther
```

The renderer owns the view state. The common `Ui` remains independent of renderer-specific presentation details.

Runtime conditions such as furnace state, temperature, or alarms are data/state, not views.
