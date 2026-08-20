# ADR: Profile Manager Ownership and Editing Model

## Status

Accepted

## Context

Profiles are stored persistently in EEPROM, while the application needs RAM copies for runtime use and profile editing.

The UI supports two independent workflows:

- selecting a profile to start;
- selecting a profile to edit.

The selected profile IDs represent UI workflow state and are therefore owned by `Ui`.

The `ProfileManager` owns profile data and is responsible for loading and saving profiles.

The UI needs access to the profile being edited through `DataAggregator`.

## Decisions

### 1. ProfileManager owns two RAM profile copies

`ProfileManager` stores:

- `start_profile_` — the profile loaded for Furnace runtime use;
- `edit_profile_` — the profile currently being edited.

```text
ProfileManager
├── start_profile_ ──► Furnace
└── edit_profile_  ──► DataAggregator ──► UI
```
The two profiles are independent.

### 2. UI owns profile selection IDs

Ui stores:
```
uint8_t start_profile_id_;
uint8_t edit_profile_id_;
```
ProfileManager does not own these IDs.

The UI selects an ID and tells ProfileManager which profile to load.

The profile module provides:
```
inline constexpr uint8_t invalid_profile_id = 0xFF;
```    
Both UI selection IDs initially have this value.

### 3. ProfileManager loads profiles

The API distinguishes the two purposes:
```
bool load_for_start(uint8_t profile_id) noexcept;
bool load_for_edit(uint8_t profile_id) noexcept;
```
load_for_start() loads the selected EEPROM profile into start_profile_.

load_for_edit() loads the selected EEPROM profile into edit_profile_.

ProfileManager does not perform the selection itself.

### 4. Profile editing uses the RAM copy directly

The UI modifies edit_profile_ through the ProfileManager editing interface.

No additional temporary/original profile copy is required.

### 5. Cancel requires no operation

When the user cancels editing, no restore operation is performed.

The modified RAM edit_profile_ is simply discarded logically.

The next edit selection calls:    
```
load_for_edit(id);
```    
which reloads the profile from EEPROM.

### 6. Save commits the editing copy

Saving writes edit_profile_ to EEPROM:
```
bool save_edit() noexcept;
```    
No notification is generated after saving because the UI leaves the editor and no longer needs the edit-profile snapshot.

### 7. Notifications are consumer-driven

A notification is generated only when another component needs to react.

load_for_start() does not notify because only Furnace consumes start_profile_.

load_for_edit() notifies because DataAggregator needs to refresh its edit-profile snapshot.

Saving and cancelling do not generate notifications.

### 8. DataAggregator exposes only the edit profile

DataAggregator does not expose:

start_profile_;
start profile ID;
edit profile ID.

It exposes the profile data needed by the UI for editing.

The UI already owns the profile IDs because it generated them as part of the selection workflow.

### 9. Profile is a value type

Profile and Step provide == and != operators.

These operators are retained as normal value-semantic operations and are not required for the Cancel mechanism.

## Resulting API

The essential ProfileManager interface is:    
```
const Profile& start_profile() const noexcept;

Profile& edit_profile() noexcept;
const Profile& edit_profile() const noexcept;

bool load_for_start(uint8_t profile_id) noexcept;
bool load_for_edit(uint8_t profile_id) noexcept;

bool save_edit() noexcept;
```   
## Consequences
- Runtime and editing profiles cannot interfere with each other.
- The Furnace is isolated from profile editing.
- UI owns workflow state rather than duplicating it in ProfileManager.
- Cancel requires no rollback mechanism.
- DataAggregator receives only data that the UI actually needs.
- Notifications remain minimal and meaningful.
     