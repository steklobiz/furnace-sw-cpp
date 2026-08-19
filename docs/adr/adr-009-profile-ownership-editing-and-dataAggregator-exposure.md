# ADR: Profile Ownership, Editing, and Data Exposure

## Status

Accepted

## Context

The application needs to expose profile information to the UI.

The current requirements include:

- displaying the selected profile;
- selecting a profile before starting the furnace;
- displaying profile data;
- editing profile data through the UI;
- detecting profile changes efficiently;
- keeping the UI independent from the internal profile storage and implementation.

`ProfileManager` currently owns the active profile and the selected profile ID:

```
cpp
class ProfileManager
{
public:
    const Profile& view() const noexcept;
    Profile& edit() noexcept;

    uint8_t selected_id() const noexcept;

    bool open(uint8_t profile_id) noexcept;
    bool save() noexcept;

private:
    uint8_t selected_profile_id_ = 0;
    Profile current_profile_;
};
```
The UI must not become the owner of profile data, and DataAggregator must not become responsible for modifying application state.

DataAggregator is intended to provide a stable, UI-independent view of current application data. It stores data together with a version that changes when the stored data changes.

A profile is a structured object containing multiple steps and fields:
```    
struct Profile
{
    static constexpr uint8_t MaxSteps = 16;

    std::array<Step, MaxSteps> steps{};
};
```
Therefore, the architecture must define the appropriate granularity for profile data and versioning.

## Decision
## 1. Profile ownership

ProfileManager owns the authoritative Profile.

The authoritative profile is stored and modified by ProfileManager.

Neither Ui nor DataAggregator owns the authoritative profile.
```
ProfileManager
      │
      │ owns
      ▼
   Profile
```
## 2. Profile modification

The UI controls the profile editing workflow, but it does not directly modify the Profile.

The UI sends semantic editing commands to ProfileManager.

ProfileManager performs the actual modification and is responsible for enforcing profile-related rules and validation.

The intended flow is:
```    
User
  │
  ▼
Renderer
  │
  │ input
  ▼
UI
  │
  │ semantic edit command
  ▼
ProfileManager
  │
  │ validate + modify
  ▼
Profile
```
The UI must not directly manipulate the profile through a writable Profile&.

The existing ProfileManager::edit() API may be used during implementation where necessary, but the long-term interface should provide explicit profile modification operations instead of exposing unrestricted mutable access to the profile.

Examples:
```    
bool set_step_setpoint(
    uint8_t step,
    uint16_t value) noexcept;

bool set_step_duration(
    uint8_t step,
    uint16_t value) noexcept;

bool set_step_flags(
    uint8_t step,
    uint8_t flags) noexcept;
```
## 3. DataAggregator exposes profile data for reading

DataAggregator provides the UI and other consumers with a snapshot of the current profile.

The profile is exposed as one structured data item:
```
DataItem<Profile> profile_;
```
The preferred access is conceptually:
```    
const DataItem<Profile>& profile() const noexcept;        
```
## 4. Profile versioning

The profile has one version for the complete profile.

The version is incremented whenever any part of the profile changes.

For example, if:    
```
step[3].duration
```
changes, the profile version changes:
```
Profile version: 17 → 18
```
The following are deliberately not maintained:
```
step 0 version
step 1 version
step 2 version
...
```
or:
```
step[3].setpoint version
step[3].duration version
step[3].flags version
```
DataAggregator is a snapshot and publication layer, not a fine-grained change-tracking database.

## 5. Selected profile ID

The selected profile ID is exposed separately from the profile itself.

Conceptually:
```
DataItem<uint8_t> profile_id_;
DataItem<Profile> profile_;
```
The selected profile ID therefore has its own version.

This distinguishes two different changes:
```
Selected profile changes
        ↓
profile_id version changes
```
versus:
```
Profile contents change
        ↓
profile version changes
```
For example:
```
User selects profile 3
    ↓
selected ID: 2 → 3
selected ID version: 5 → 6

Profile 3 is loaded
    ↓
profile snapshot changes
profile version: 12 → 13
```
## 6. DataAggregator does not modify profiles

DataAggregator is read-oriented with respect to application data.

It must not provide an API through which the UI can modify the authoritative profile.

The intended direction is:    
```
ProfileManager
      │
      │ profile changes
      ▼
DataAggregator
      │
      │ read-only snapshot
      ▼
UI
```
not:
```
UI
 │
 ▼
DataAggregator
 │
 │ modifies
 ▼
Profile
``` 
## 7. Profile changes must be propagated to DataAggregator

When the profile or selected profile changes, DataAggregator must update its corresponding data item and version.

The preferred mechanism is the application's existing notification/update mechanism rather than periodic polling.

The intended flow is: 
```
ProfileManager
      │
      │ change notification
      ▼
DataAggregator
      │
      ├── update profile ID if required
      │
      └── update profile snapshot
             │
             └── increment version
```
The exact notification type and API will be defined when ProfileManager notification support is implemented.

## 8. UI determines field-level presentation

Although the profile is exposed to the UI as one structured data item, the UI is free to work with individual profile fields.

For example, the profile editor may display:
```
Step 3
  Setpoint: 850 °C
  Duration: 120 s
  Flags:    0x03
```
The UI may determine that only the duration field needs to be redrawn.

This does not require DataAggregator to maintain a separate version for duration.

The responsibility is divided as follows:
```
DataAggregator
    "Has the profile changed?"

UI
    "Which profile fields are relevant to this page?"

Renderer
    "Which screen elements need to be redrawn?"
```
## 9. Renderer independence

The renderer must not access ProfileManager directly.

The renderer only receives UI data and user input from Ui.

The resulting architecture is:                
```
                         Application
                              │
                    ┌─────────┴─────────┐
                    │                   │
              ProfileManager       DataAggregator
                    │                   │
                    │ owns              │ snapshot
                    ▼                   ▼
                 Profile               UI
                                        │
                                  ┌─────┴─────┐
                                  │           │
                               display      edit command
                                  │           │
                                  ▼           ▼
                               Renderer  ProfileManager
```
## 10. Rationale for whole-profile versioning

A single profile version was selected because:

the profile is naturally a single structured application object;
the UI needs access to the profile as a coherent set of data;
fine-grained field versions would add bookkeeping and complexity;
DataAggregator should not depend on renderer-specific presentation requirements;
the UI can determine which individual fields require rendering;
profile editing does not require the aggregator to track which exact field changed;
the approach remains simple and deterministic for the embedded environment.

If a future requirement demonstrates that per-field change tracking is necessary, the data model can be extended later. It is intentionally not part of the current design.

## Consequences

### Positive
ProfileManager has clear ownership of profile state.
Profile modification has a single authoritative entry point.
UI can implement profile selection and profile editing without owning profile data.
DataAggregator provides a stable read-only representation for the UI.
The complete profile can be detected as changed with one version value.
The design avoids unnecessary per-field version bookkeeping.
Renderer implementations remain independent of application modules.
The architecture supports both the current profile-selection requirement and the future profile editor.
### Negative
DataAggregator stores a copy/snapshot of the Profile.
A profile change requires updating the complete profile snapshot.
The UI cannot determine from the profile version alone which exact field changed.
ProfileManager needs a notification/update mechanism so changes can be propagated to DataAggregator.

These costs are acceptable for the current application requirements.

## Resulting Responsibilities

Component         Responsibility
ProfileManager    Owns the authoritative profile
ProfileManager    Selects and loads profiles
ProfileManager    Validates and performs profile modifications
ProfileManager    Notifies consumers when profile data changes
DataAggregator    Stores the current profile snapshot
DataAggregator    Stores selected profile ID
DataAggregator    Maintains profile and profile-ID versions
DataAggregator    Provides read-only profile data to consumers
Ui                Controls profile selection/editing workflow
Ui                Sends semantic profile-edit commands
Ui                Determines which profile fields are relevant to the current page
Renderer          Displays profile data
Renderer          Converts user input into UI commands
Renderer          Has no direct dependency on ProfileManager

## Summary

The authoritative profile belongs to ProfileManager.

The UI controls the editing workflow but does not directly modify the profile.

DataAggregator publishes a read-only, versioned snapshot of the complete profile and separately publishes the selected profile ID.

The profile has one version for the whole object; individual steps and fields do not have independent versions.

This provides a simple data model that supports both profile selection now and profile editing later without coupling the UI to profile storage or implementation details.    
            
      