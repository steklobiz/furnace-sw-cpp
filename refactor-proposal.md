# Refactor Proposal: DataAggregator / Ui / Tui Simplification

| | |
|---|---|
| **Status** | Proposal only — no code changed yet |
| **Date** | 2026-08-24 |
| **Scope** | `app/aggregator/data_aggregator.{hpp,cpp}`, `app/ui/ui.{hpp,cpp}`, `app/ui/tui/tui.{hpp,cpp}`, minor cleanup in `app/app.hpp` |
| **Out of scope** | Furnace FSM internals, PID, HAL, scheduler core, alarm/history subsystems (bugs noted in §6) |

---

## 1. Summary

The UI stack works, but it carries three kinds of avoidable weight:

1. **Redundant indirection** — every displayed value passes through three hand-synced
   tables (`Tui::Label.field` → `Ui::FieldMapping[]` position → item enum) where two of
   the three steps are pure plumbing.
2. **Copy-paste proliferation points** — adding one data source or one edited field
   requires touching 5–7 places with near-identical code.
3. **Overlapping caching mechanisms** — the TUI keeps four separate "did it change"
   mechanisms for what is conceptually one question.

Four refactors (**A–D**) plus a cleanup list remove ~20 % of the LOC (~290 of ~1470 lines)
in this layer and, more importantly, turn whole bug classes into compile errors.

| Refactor | One-liner | Net LOC | Kills |
|---|---|---|---|
| **A** | Labels bind directly to typed `(DataSource, item)` pairs | ≈ −40 | Table-sync drift bugs |
| **D** | Numeric edit = button flag; editor renders through the generic path | ≈ −90 | Editor special cases |
| **C** | Compare drawn *values* instead of version counters | ≈ −15 | Version-wraparound repaint skip |
| **B** | One shared refresh callback (or plain polling) instead of 3 trampolines | −27 … −80 | Source-boilerplate on every new source |

Recommended order: **A → D → C → B**. Each step compiles independently; see §7.

---

## 2. Current architecture (as-is)

### 2.1 Components

| File | LOC | Responsibility |
|---|---|---|
| `app/aggregator/data_aggregator.{hpp,cpp}` | 130 / 247 | Change-tracked cache `{value, version}` per `(source, field)`; push-triggered full re-poll |
| `app/ui/ui.{hpp,cpp}` | 203 / 272 | Page state machine, semantic actions, page→field mapping |
| `app/ui/tui/tui.{hpp,cpp}` | 95 / 522 | ANSI renderer + keyboard driver + numeric input mode |

Threading model is a single-threaded cooperative superloop (`App::run`, app.hpp:127).
Scheduler heartbeat: `Furnace::process` @1000 ms, `TcParser::update` @500 ms,
`Tui::process` @100 ms (registration order = execution order, app.hpp:88).

### 2.2 Data flow

```
TcParser.update() ──DataReady──────────────┐
Furnace.process() ──DataReady──────────────┼──> DataAggregator.update_<source>()
ProfileManager setters ──*ProfileChanged───┘        (re-polls ALL getters of that source,
                                                     bumps version if value changed)
                                                     │
              Ui::get_field(page, field) ──> DataAggregator.get_item(source, field)
                                                     │
              Tui::process() ── renders only cells whose cached version differs
```

Notifications carry no payload of interest — they are pure triggers. Any single change
causes a re-read of all getters of that source (e.g., one furnace value → 8 getter calls),
which is cheap and fine; the point here is only that the notification mechanism exists
solely to trigger those polls.

### 2.3 Command flow

```
_getch() ─> Tui.process_input() ─> Ui.execute({ActionType, arg})
                │                        ├─> ProfileManager.set_edit_*() ──EditProfileChanged──> aggregator
                │                        └─> Furnace.start()/stop()/reset()
                └─ (Edit* actions intercepted first → numeric input mode, tui.cpp:418-432)
```

---

## 3. Problems

### P1 — Triple hand-synced tables (highest risk)

A single Monitor-page row exists in three places that must stay aligned by *position*:

```
tui.cpp:35   monitor_labels[]      {"Setpoint, C:", 4}          ← field index 4
ui.cpp:19    monitor_fields[4]     {Furnace, FurnaceItem::Setpoint}
data_aggr.hpp:34  enum FurnaceItem { ..., Setpoint }             ← enum order must match
```

Nothing enforces the alignment. Inserting a label, reordering an enum, or adding a getter
compiles fine and silently displays wrong data in the wrong row. This is the most dangerous
property of the current design because it fails at runtime, invisibly.

### P2 — Per-source boilerplate in the aggregator

Adding source #4 today touches **seven** places:

1. `enum class NewSourceItem` (data_aggregator.hpp)
2. items array member `DataItem<uint16_t> new_items_[]` (hpp:113-120 pattern)
3. mapping struct `NewSourceMapping` (data_aggregator.cpp:11-29 pattern)
4. constexpr mapping table (:57-80 pattern)
5. static callback trampoline (:133-175 pattern — three near-identical copies already exist)
6. `update_new_source()` method (:178-215 pattern)
7. `init()` wiring: pointer, descriptor, `set_notify_callback`, initial poll (:85-130)

The three existing callbacks differ only in accepted `NotificationType` and target method;
the three `update_*` methods differ only in which table/pointer they iterate.

### P3 — Four overlapping change-detection mechanisms in Tui

* `rendered_versions_[Page::Count][16]` (tui.hpp:80) — magic capacity `16`, per-field version cache
* `page_rendered_` (tui.hpp:78) — "statics drawn once" flag
* `rendered_profile_version_` + `rendered_profile_step_` (tui.hpp:83-84) — separate cache for the editor path

Plus the underlying version counters themselves (`uint8_t`, wraps at 256 updates — since the
comparison is equality, one repaint per wrap cycle is silently skipped). Each mechanism has
its own reset conditions scattered across `init()` / page-switch handling / first-render branches.

### P4 — ProfileEditor special-cased everywhere

* `page_descriptors[ProfileEditor] = {nullptr, 0}` in both ui.cpp:42-43 and tui.cpp:122-128
* dedicated render path `render_profile_editor_page()` + `render_profile_content()` (tui.cpp:277-374)
  with its own three-phase first-render/change-detect logic
* `process_input()` sniffs `ActionType::EditSetpoint/Duration/Flags` by value to enter numeric
  mode (tui.cpp:418-425) instead of the button declaring its own behavior
* the `label_count == 0` branch in button-row layout (tui.cpp:213-216, 228-232) exists only
  because some pages have no labels

Consequence: any new editing feature multiplies special cases instead of extending tables.

### P5 — Type erasure + triple-duplicated fallbacks

The chain Tui→Ui→aggregator erases everything to raw `uint8_t(source), uint8_t(field)`,
so every layer needs a bounds-check fallback:

* `DataAggregator::null_item_` (data_aggregator.hpp:124)
* `Ui::null_item_` (ui.cpp:51)
* `Tui::null_item_` (tui.cpp:150)

Three identical definitions, and every read pays two runtime range checks for a relationship
that is fully known at compile time.

### P6 — Dead / stale code in the touched files

| Item | Location | Note |
|---|---|---|
| Commented-out `trace_pid`/`trace_furnace`/`Ui::process` registrations | app.hpp:102-118 | Decide: delete or restore |
| `Furnace::state_name`, `step_type_name` marked "only for TUI" | app/furnace/furnace.hpp:59-60 | Not referenced anywhere by Tui |
| `MAX_RPOFILE_STEPS` (typo, unused, conflicts with config) | app/furnace/furnace.hpp:81 | Dead constant |
| `History` populated by Furnace but consumed by nothing | app/history/* | Park or wire up — decision needed, not part of this refactor |

---

## 4. Proposed refactors

### A — Typed label bindings (delete the middle table)

**Goal:** a label states *what it displays*; no positional coupling to any other table.

**Change:** `Tui::Label` carries the full address of its datum:

```cpp
// tui.hpp — before
struct Label { const char* caption; uint8_t field; };

// tui.hpp — after
struct Label
{
    const char* caption;
    DataSource  source;   // from data_aggregator.hpp
    uint8_t     item;
};
```

```cpp
// tui.cpp — before (index must match ui.cpp row order!)
static constexpr Tui::Label monitor_labels[] =
{
    {"State:",           0},
    {"Profile:",         1},
    {"Temperature, C:",  2},
    ...
};

// tui.cpp — after (self-contained, reorder-safe)
static constexpr Tui::Label monitor_labels[] =
{
    {"State:",           DataSource::Furnace, uint8_t(FurnaceItem::State)},
    {"Profile:",         DataSource::Profile, uint8_t(ProfileItem::StartProfileId)},
    {"Temperature, C:",  DataSource::Furnace, uint8_t(FurnaceItem::Temperature)},
    ...
};
```

`Ui` loses `FieldMapping`, `PageDescriptor`, the four field tables and `get_field(page, field)`
(ui.hpp:77-103, ui.cpp:11-46, 73-102) and gains one forwarding accessor:

```cpp
// ui.hpp
const DataItem<uint16_t>& item(DataSource source, uint8_t id) const noexcept;

// ui.cpp
const DataItem<uint16_t>&
Ui::item(DataSource source, uint8_t id) const noexcept
{
    return data_->get_item(uint8_t(source), id);
}
```

Layering is preserved: Renderer still talks only to `Ui`; `Ui` stays the sole doorway to the
aggregator. The optional compile-time guard below turns the old failure mode (bad index) into
a build error:

```cpp
constexpr bool labels_ok(const Tui::Label* l, std::size_t n)
{
    for (std::size_t i = 0; i < n; ++i)
    {
        if (uint8_t(l[i].source) >= uint8_t(DataSource::Count)) return false;
        // per-source Count check elided for brevity; same shape
    }
    return true;
}
static_assert(labels_ok(monitor_labels, std::size(monitor_labels)), "bad label binding");
```

**Deleted:** `Ui::FieldMapping`, `Ui::PageDescriptor`, `main_fields`/`monitor_fields`/
`result_fields`/`page_descriptors` (ui.cpp:11-46), `Ui::get_field` (73-102),
`Ui::null_item_`, `Tui::null_item_`.
**Added:** `item()` accessor (~8 lines), one extra column per label row.
**Net:** ≈ −40 lines. **Risk:** none — behavior identical; misalignment becomes impossible.
Adding a displayed value goes from *3 synced places* to *1 label row*.

---

### D — De-special-case the profile editor (do before C)

Two sub-changes.

#### D1 — Numeric input becomes a button property

```cpp
// tui.hpp — before
struct Button { char key; const char* caption; Ui::ActionType action; uint16_t argument; };

// tui.hpp — after
struct Button
{
    char key;
    const char* caption;
    Ui::ActionType action;
    uint16_t argument;
    bool numeric = false;   // true → enter numeric-input mode instead of executing
};
```

```cpp
// tui.cpp — profile_editor_buttons
{'e', "Edit setpoint", Ui::ActionType::EditSetpoint, 0, true},
{'d', "Edit duration", Ui::ActionType::EditDuration, 0, true},
{'f', "Edit flags",    Ui::ActionType::EditFlags,    0, true},
```

`process_input()` drops the action-type sniffing block (tui.cpp:418-425) and just checks
`button.numeric`. Future numeric fields on any page cost zero new logic.

#### D2 — Editor content through the generic label path

Introduce a pseudo-source owned by `Ui` (not the aggregator), because the editor projects
*view state* (`current_step_`) which lives in `Ui` per adr-009's ownership split:

```cpp
// data_aggregator.hpp
enum class DataSource : uint8_t
{
    TcParser, Furnace, Profile,
    Editor,     // resolved by Ui: current step of the profile being edited
    Count
};
```

```cpp
// ui.hpp — Editor item ids
enum class EditorItem : uint8_t { ProfileId, StepIndex, Setpoint, Duration, Flags, Count };
```

```cpp
// ui.cpp — resolve pseudo-source; scratch_ is a mutable member DataItem<uint16_t>
const DataItem<uint16_t>&
Ui::item(DataSource source, uint8_t id) const noexcept
{
    if (source == DataSource::Editor)
    {
        const auto& p  = data_->profile().value;
        const auto  i  = current_step_;
        switch (EditorItem(id))
        {
            case EditorItem::ProfileId: scratch_ = {p.id, 0};                 break;
            case EditorItem::StepIndex: scratch_ = {i, 0};                    break;
            case EditorItem::Setpoint:  scratch_ = {step_at(p, i).setpoint_c, 0}; break;
            /* Duration, Flags likewise */
            default:                    scratch_ = {};
        }
        return scratch_;
    }
    return data_->get_item(uint8_t(source), id);
}
```

(`scratch_` is safe as a `mutable` member under the documented single-thread superloop.
Note: with refactor C these items carry no versions at all — see below.)

The editor then becomes an ordinary page descriptor and its dedicated render path disappears:

```cpp
// tui.cpp — after
static constexpr Tui::Label editor_labels[] =
{
    {"Profile:",     DataSource::Editor, uint8_t(EditorItem::ProfileId)},
    {"Step:",        DataSource::Editor, uint8_t(EditorItem::StepIndex)},
    {"Setpoint, C:", DataSource::Editor, uint8_t(EditorItem::Setpoint)},
    {"Duration, s:", DataSource::Editor, uint8_t(EditorItem::Duration)},
    {"Flags:",       DataSource::Editor, uint8_t(EditorItem::Flags)},
};
```

`render_profile_editor_page()` (tui.cpp:314-374) and `render_profile_content()`
(:277-312) are deleted; `process()`'s `if (page == ProfileEditor)` branch (:191-195)
disappears; the `{nullptr, 0}` descriptors become real entries; the
`label_count == 0` layout special case (tui.cpp:213-216) disappears.

**Deleted:** ≈ 130 lines of editor-specific render/input logic.
**Added:** ≈ 40 lines (EditorItem resolution, button flags, editor_labels).
**Net:** ≈ −90 lines. **Risk:** low. Design note to record in an ADR addendum:
*transient view-state projections live in `Ui`; `DataAggregator` caches only source-owned state.*

---

### C — Snapshot rendering (values, not versions)

**Goal:** one uniform rule — *redraw a row iff its last drawn value differs* — replacing
all four cache mechanisms, and incidentally fixing the `uint8_t` version-wraparound repaint
skip (P3) because values are compared directly.

```cpp
// tui.hpp — before
uint8_t  rendered_versions_[Page::Count][16];   // magic 16
bool     page_rendered_;
uint8_t  rendered_profile_version_;
uint8_t  rendered_profile_step_;

// tui.hpp — after
static constexpr std::size_t max_row_count = /* constexpr max over label tables */;
uint16_t drawn_values_[std::size_t(Ui::Page::Count)][max_row_count];
bool     drawn_valid_[std::size_t(Ui::Page::Count)] = {};
```

```cpp
// tui.cpp — one loop for every page, editor included
void Tui::render_page(const PageDescriptor& d, Ui::Page page) noexcept
{
    const auto p = std::size_t(page);

    if (!drawn_valid_[p])                      // page entry: statics once
        draw_statics(d);                       // title + buttons (today's !page_rendered_ block)

    for (std::size_t r = 0; r < d.label_count; ++r)
    {
        const auto& item = ui_->item(d.labels[r].source, d.labels[r].item);

        if (drawn_valid_[p] && drawn_values_[p][r] == item.value)
            continue;                          // unchanged → skip

        drawn_values_[p][r] = item.value;
        draw_row(r, d.labels[r], item.value);  // "\033[<row>;1H\033[2K<caption> <value>"
    }

    drawn_valid_[p] = true;
}
```

Notes:

* Row *position* is the natural key now (a label owns its row); the `[page][field-index]`
  addressing disappears together with the magic `16`.
* `max_row_count` is computed at compile time from the (now constexpr) label tables in the
  same TU — no hand-maintained capacity.
* Version counters remain useful inside `DataAggregator` itself (cheap dirty tracking);
  they simply stop being part of the renderer contract. If desired later, `version` can be
  widened or dropped entirely — the UI no longer depends on either choice.
* Alternative **C2**: drop the arrays entirely and unconditionally redraw all rows each
  100 ms tick (overwrite-in-place via `\033[2K`, as done today — no flicker). Fewest lines of
  state; costs ~15 printfs/tick, negligible on PC console. Choose C2 if you prefer minimalism,
  C1 if you want the renderer ready for a bandwidth-limited LCD backend.

**Deleted:** `rendered_versions_`, `page_rendered_`, `rendered_profile_version_`,
`rendered_profile_step_`, plus their reset/update logic in `init()`/`process()`.
**Net:** ≈ −15 lines and, more importantly, exactly one change-detection rule left.

---

### B — Simplify aggregator refresh (two variants)

#### B-min — one shared trampoline (recommended)

All notifications are payload-less triggers (verified: no consumer reads `Notification`
fields), so filtering by type adds nothing. Replace the three trampolines and three
`update_*` methods with:

```cpp
void DataAggregator::notification_callback(
    void* context, const Notification&) noexcept
{
    static_cast<DataAggregator*>(context)->refresh();
}

void DataAggregator::refresh() noexcept
{
    for (const auto& m : tc_parser_mapping) update(m.item, (tc_parser_->*m.get)(), tc_parser_items_);
    for (const auto& m : furnace_mapping)   update(m.item, (furnace_->*m.get)(),   furnace_items_);
    for (const auto& m : profile_mapping)   update(m.item, (profiles_->*m.get)(),  profile_items_);
    update(profile_, profiles_->edit_profile());
}
```

Cost of the "over-refresh": ~11 getter reads + comparisons per notification — nanoseconds
on target hardware, and version-gated writes mean nothing downstream notices.
`init()` registers the same callback for all three sources.

**Deleted:** 3 trampolines + type filters + 3 methods (≈ 65 lines) → +18 lines. **Net ≈ −27.**

#### B-full — drop push entirely, poll on tick

Register `refresh()` next to the TUI tick in `App::init()`:

```cpp
scheduler_.every<DataAggregator, &DataAggregator::refresh>(100, &data_aggregator_); // before Tui!
scheduler_.every<Tui, &Tui::process>(100, tui_);                                    // order = priority
```

Deletes the notify wiring from `init()` altogether. Sources keep their (untouched)
`set_notify_callback` hooks for future consumers.

Tradeoff vs. B-min: up to one tick (≤100 ms) display latency — invisible for this UI;
and refresh runs even when nothing notified. In exchange the aggregator stops depending
on the notification mechanism completely, and `NotificationType` usage shrinks to
`ProfileManager` alone.

**Recommendation:** start with B-min; move to B-full when convenient. Go B-full directly
if you expect ≥2 new sources soon — each new source then costs exactly
*one enum + one array + one table entry* (three places, down from seven).

---

## 5. Cleanup list (small, independent, low-risk)

| # | Item | Location | Action |
|---|---|---|---|
| K1 | Commented-out scheduler blocks (`trace_pid`, `Ui::process`) | app.hpp:102-118 | Delete (git keeps history) or restore deliberately |
| K2 | Unused `state_name`/`step_type_name` ("only for TUI") | app/furnace/furnace.hpp:59-60 | Delete |
| K3 | Unused typo constant `MAX_RPOFILE_STEPS` | app/furnace/furnace.hpp:81 | Delete |
| K4 | Duplicate `null_item_` ×3 | da hpp:124, ui.cpp:51, tui.cpp:150 | Removed by refactor A (one guarded copy remains in aggregator) |
| K5 | Magic capacity `16` | tui.hpp:80 | Removed by refactor C |
| K6 | `<conio.h>` included directly in application-layer tui.cpp | tui.cpp:6 | Note for future backend split (`InputBackend` interface alongside future DWIN renderer); do not block refactor on it |
| K7 | Scheduler dual API (template + fn-pointer `every`/`once`) | core/scheduler | Audit call sites; keep one form |
| K8 | Layering leak: core includes platform | core/scheduler/scheduler.cpp:2 | Invert dependency (inject delay callable); optional |

---

## 6. Out-of-scope bug register (documented, NOT fixed by this refactor)

| # | Severity | Where | Issue | Sketch fix |
|---|---|---|---|---|
| G1 | **High** | app/furnace/furnace.cpp:356-359 | `next_step()` increments `current_step_` then indexes `steps[current_step_]` **before** validating against `max_steps` (:362) — out-of-bounds read with a fully-populated profile | Validate before indexing |
| G2 | Low | data_aggregator.hpp:20 (+ tui caching) | `uint8_t version` wraps at 256 → one skipped repaint per wrap cycle | Neutralized by refactor C (value comparison) |
| G3 | **Safety-relevant** | app/app.hpp:68-71, app/alarm/* | `AlarmDispatcher` initialized but never scheduled — over-temperature protection currently dead; `alarms_` never cleared; `has_active()` would re-fire `error()` every pass | Schedule it + clear-on-ack semantics; own task |
| G4 | Info | app/history/* | History written by Furnace, read by nobody | Wire into Monitor/Result or park explicitly |
| G5 | Medium | `Furnace::current_temperature` (furnace.hpp:65) | `int16_t` returned as `uint16_t` — negative temperatures display as huge positives | Sign-aware formatting or store signed |
| G6 | Medium | profiles.hpp:46 vs config.hpp:15 vs furnace.hpp:81 | Conflicting step limits: `MaxSteps=16` / `config max_steps=10` / `MAX_RPOFILE_STEPS=5` | Single source of truth in config |
| G7 | Info | tests/furnace_test/main.cpp | Uses removed APIs (`app::Profiles`, old ctor) — does not compile | Rewrite against current headers |
| G8 | Cosmetic | Makefile:4-5,12,22,33 | References nonexistent dirs (`app/ui/dwin`, `core/format`, `core/log/backends`) | Harmless wildcards; prune when convenient |

---

## 7. Implementation order & verification

Order chosen so every step leaves the tree compilable and behaviorally identical:

```
A (typed bindings)      — foundation; touches labels + Ui accessor
D (editor de-special)   — builds on A's accessor; adds Editor pseudo-source + Button.numeric
C (snapshot rendering)  — benefits from D (single generic path covers editor too)
B (refresh simplification) — independent; slot in anytime
K1..K8                  — sprinkle anywhere
```

Per-step verification:

1. `make clean && make` — zero warnings (`-Wall -Wextra -Wpedantic` already on).
2. Manual smoke script (`make run`):
   - Main → `s` → select profile 0 → Monitor shows State/Step/Temperature/Setpoint/elapseds/Power/Outputs updating live
   - Monitor `b` back; Main → `e` → select profile → editor shows ProfileId/Step/Setpoint/Duration/Flags
   - Editor: `n`/`p` step nav; `e`→digits→Enter commits setpoint (verify row updates); `\b` edits; Esc cancels; overflow past 65535 clamps
   - Editor `s` saves → back at Main; `c` cancels → selection page
   - Start profile → Monitor `s` stop → Result shows State/Temperature → `r` resets → Main
   - Every `Back` path lands where §ui.cpp:246-270 says
3. After C: leave one value changing rapidly >256 updates (e.g., Power during ramp) and confirm continuous repainting (wrap regression test).
4. Diff review checklist: no behavioral keys pressed twice, no row drawn at stale coordinates, `null_item_` reachable only via aggregator.

---

## 8. Estimated impact

| Metric | Before | After (A+D+C+B-min+cleanups) |
|---|---|---|
| LOC in the six files | ≈ 1470 | ≈ 1180 (−20 %) |
| Places to add a displayed value | 3 (synced by discipline) | 1 (label row) |
| Places to add a data source | 7 | 3 (B-min) / 3 total (B-full) |
| Change-detection mechanisms in renderer | 4 | 1 |
| Editor-specific code paths | 3 (render ×2 + input sniff) | 0 |
| Fallback `DataItem` definitions | 3 | 1 |
| Bug classes eliminated structurally | — | table drift, version wrap, editor-only regressions |
