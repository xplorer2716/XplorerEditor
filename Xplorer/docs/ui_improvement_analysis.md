# UI Improvement Analysis — SMART Requirements & Architecture Decision Record

> **Scope:** `Xplorer` project only — specifically `MainForm.cs`, `MainForm.Overrides.cs`
> and their relationship to `AbstractControllerMainForm` (in `MidiApp.MidiController`).
> No changes to the MIDI engine, model layer, or `Sanford.Multimedia.Midi`.

---

## Part 1 — SMART Requirements in EARS Notation

EARS (Easy Approach to Requirements Syntax) template forms used:

| Template | When used |
|---|---|
| **Ubiquitous** `The system shall …` | Always-active system properties |
| **Event-driven** `When <trigger>, the system shall …` | Reactive behaviour |
| **State-driven** `While <state>, the system shall …` | Conditions that persist |
| **Optional** `Where <feature included>, the system shall …` | Feature-dependent |
| **Unwanted** `The system shall not …` | Prohibited behaviour |

---

### REQ-UI-001 — Single Responsibility: Page Refresh Logic

**ID:** REQ-UI-001  
**Title:** Page refresh logic must not reside in the form class  
**Priority:** High  
**Type:** Event-driven

> When the user activates a multi-page panel (ENV, LFO, RAMP, TRACK),
> the system shall delegate all per-control value refresh operations
> to a dedicated class that is separate from `MainForm`.

**Rationale:**  
`Radio_ENV_X_Click`, `Radio_LFO_X_Click`, `Radio_RAMP_X_Click`, and `Radio_TRACK_X_Click`
in `MainForm.cs` each contain an identical structural pattern: call
`GetParameterNameForValuedControlTag`, then manually set `.Value` on every
control of that page. This is duplicated four times and is not UI rendering — it
is parameter-to-control mapping logic. Adding a new page-controlled parameter
today requires editing `MainForm.cs` directly.

**Measurable acceptance criteria:**
- `MainForm.cs` contains zero direct `.Value =` assignments triggered by a page radio-button click.
- A dedicated class owns all four page-refresh sequences.
- Adding a new page-controlled parameter requires only editing that dedicated class.

---

### REQ-UI-002 — Single Responsibility: Trigger Mutual-Exclusion Rules

**ID:** REQ-UI-002  
**Title:** Synth trigger mutual-exclusion rules must not reside in the form class  
**Priority:** High  
**Type:** Event-driven

> When the user changes `ENV_X_TRIG_EXTRIG` or `ENV_X_TRIG_LFOTRIG`
> (and their RAMP equivalents), the system shall enforce the mutual-exclusion
> rule (EXTRIG and LFOTRIG cannot both be active simultaneously) through logic
> that is separate from `MainForm`.

**Rationale:**  
`ENV_X_TRIG_EXTRIG_CheckedChanged`, `ENV_X_TRIG_LFOTRIG_CheckedChanged`,
`RAMP_X_TRIG_EXTRIG_CheckedChanged`, and `RAMP_X_TRIG_LFOTRIG_CheckedChanged`
in `MainForm.cs` encode synth behaviour rules (which controls to enable/disable
and which checkboxes to uncheck) directly inside WinForms event handlers.
A behaviour change in the synth protocol today requires editing form-level code.

**Measurable acceptance criteria:**
- The four `_CheckedChanged` handlers in `MainForm.cs` contain no `if/else` branches; they delegate to a single dedicated class.
- The mutual-exclusion and enable/disable rules live exclusively in that class.
- The class can be read and reasoned about without knowledge of WinForms controls.

---

### REQ-UI-003 — Eliminate Literal Tag Prefixes from View Layer

**ID:** REQ-UI-003  
**Title:** Tag-to-parameter-name resolution must not use hard-coded string literals in the form  
**Priority:** Medium  
**Type:** Ubiquitous

> The system shall resolve multi-page control tag prefixes
> (`"ENV_X"`, `"LFO_X"`, `"TRACK_X"`, `"RAMP_X"`) using a
> data-driven mechanism defined in the constants/model layer,
> not through `string.StartsWith` chains with literals embedded in the view.

**Rationale:**  
`GetParameterNameForValuedControlTag` and `GetValuedControlAndButtonNameForParameterName`
in `MainForm.Overrides.cs` contain four `StartsWith` chains with hard-coded
string literals and a `#warning TODO` acknowledging the issue. The page-type
classification logic is already partially present in `XpanderConstants`
(via `LFO_COUNT`, `RAMP_COUNT`, `TRACK_COUNT`). Unifying it removes the warning
and eliminates a maintenance inconsistency.

**Measurable acceptance criteria:**
- The `#warning TODO` comment in `GetValuedControlAndButtonNameForParameterName` is removed.
- Both resolution methods are driven by a data structure in `XpanderConstants` or a new constants class.
- Zero naked `"ENV_X"`, `"LFO_X"`, `"TRACK_X"`, `"RAMP_X"` string literals remain in `MainForm.Overrides.cs`.

---

### REQ-UI-004 — Preserve Existing Behaviour Exactly

**ID:** REQ-UI-004  
**Title:** All refactoring must produce identical runtime behaviour  
**Priority:** Mandatory / Non-negotiable  
**Type:** Unwanted

> The system shall not alter the observable behaviour of the application
> (MIDI output, UI state, event order, display update) as a result of
> any structural refactoring described in this document.

**Rationale:**  
The Xplorer application targets real hardware (Oberheim Xpander / Matrix-12).
Any deviation in event order or MIDI message sequencing can corrupt the synth's
state. Correctness is the highest constraint.

**Measurable acceptance criteria:**
- The application builds without new warnings or errors.
- All existing manual test scenarios (patch load/save, automation, page switching,
  modulation matrix, tone morphing) pass unchanged.
- No existing call site in `MainForm.Designer.cs` is modified.

---

### REQ-UI-005 — No New External Dependencies

**ID:** REQ-UI-005  
**Title:** Refactoring must not introduce new NuGet packages or framework dependencies  
**Priority:** Mandatory  
**Type:** Unwanted

> The system shall not require any new NuGet package, DI container, or
> framework namespace not already referenced by the `Xplorer` project.

**Rationale:**  
The existing Manager pattern (`FileOperationsManager`, `SettingsManager`,
`ModulationMatrixManager`) achieves clean separation without any dependency
injection infrastructure. The same pattern is sufficient here. Introducing a DI
container or MVVM framework would require updating all three Git repositories
and is out of scope.

**Measurable acceptance criteria:**
- `Xplorer.csproj` `<ItemGroup>` package references are identical before and after.
- No `using` directives for new namespaces appear in modified files.

---

### REQ-UI-006 — Consistency with Existing Manager Pattern

**ID:** REQ-UI-006  
**Title:** New classes must follow the established Manager pattern  
**Priority:** Medium  
**Type:** Ubiquitous

> The system shall implement new separation classes using the same
> structural pattern as `FileOperationsManager`, `SettingsManager`,
> and `ModulationMatrixManager`: a `sealed` class receiving `MainForm`
> as a constructor parameter, with `internal` methods.

**Rationale:**  
The existing Manager classes are the de-facto architectural pattern for
view-logic separation in this codebase. Consistency reduces cognitive load
for future contributors and avoids introducing a second pattern.

**Measurable acceptance criteria:**
- New classes are `internal sealed`.
- Constructor parameter type is `MainForm`.
- No `public` API surface is added.
- File location follows the existing `Xplorer\View\` convention.

---

### REQ-UI-007 — Documentation Must Be Updated

**ID:** REQ-UI-007  
**Title:** Architecture documents must reflect structural changes  
**Priority:** Medium  
**Type:** Ubiquitous

> The system shall update `Xplorer\docs\architecture-static.md` and
> `Xplorer\docs\architecture-dynamic.md` to include the new classes and
> their relationships whenever a structural refactoring task is completed.

**Measurable acceptance criteria:**
- `architecture-static.md` Mermaid diagram includes new Manager classes.
- `architecture-dynamic.md` sequence diagram for page-change flow reflects the new delegation path.

---

## Part 2 — Architecture Decision Record

---

### ADR-UI-001 — Apply Manager Pattern to Page Refresh Logic

**ID:** ADR-UI-001  
**Status:** Proposed  
**Date:** 2026  
**Addresses:** REQ-UI-001, REQ-UI-004, REQ-UI-005, REQ-UI-006  
**Deciders:** Pascal Schmitt

#### Context

`MainForm.cs` contains four structurally identical page-refresh methods:
`Radio_ENV_X_Click`, `Radio_LFO_X_Click`, `Radio_RAMP_X_Click`,
`Radio_TRACK_X_Click`. Each method:

1. Notifies the controller of the page change (via `XController.SendPageUpdate`).
2. Reads the current value of every parameter on that page from the controller.
3. Assigns each value to the corresponding control's `.Value` property.
4. Calls `VfdDisplayHelper.UpdateState` on the first control of the page.

Step 1 is already a controller concern. Steps 2–4 are parameter-binding logic,
not display logic. The same four-step pattern is also triggered indirectly from
`OnPageChange` in `MainForm.Overrides.cs` when the synth changes the active page.

#### Decision

**Create `PageRefreshManager`** in `Xplorer\View\PageRefreshManager.cs`, following
the exact structural pattern of `FileOperationsManager`:

- `internal sealed class PageRefreshManager`
- Constructor: `PageRefreshManager(MainForm form)`
- Four public methods: `RefreshEnvPage(RadioButton button, bool fromUser)`,
  `RefreshLfoPage(RadioButton button, bool fromUser)`,
  `RefreshRampPage(RadioButton button, bool fromUser)`,
  `RefreshTrackPage(RadioButton button, bool fromUser)`
- `MainForm` event handlers become one-line delegations.
- `OnPageChange` in `MainForm.Overrides.cs` delegates to the same methods.

#### Alternatives Considered

| Alternative | Reason rejected |
|---|---|
| Keep logic in `MainForm.cs`, extract to a private base method | Does not move the logic out of the form; still untestable without a form instance |
| Generic data-driven page refresh using reflection | Over-engineering; the control names are fixed by the Designer; hardcoding per-page is faster and clearer |
| MVVM with data binding | Requires a full WinForms MVVM framework or manual `INotifyPropertyChanged`; violates REQ-UI-005 |

#### Consequences

- **Positive:** `MainForm.cs` loses ~120 lines of repetitive parameter-fetch code.
- **Positive:** Page refresh logic is in one place; adding a new page parameter requires editing only `PageRefreshManager`.
- **Positive:** Consistent with existing `ModulationMatrixManager` precedent.
- **Negative:** `PageRefreshManager` still holds direct references to named WinForms controls via `MainForm`; it is not fully isolated from the form. This is accepted as a limitation of the WinForms Designer contract.

---

### ADR-UI-002 — Apply Manager Pattern to Trigger Mutual-Exclusion Rules

**ID:** ADR-UI-002  
**Status:** Proposed  
**Date:** 2026  
**Addresses:** REQ-UI-002, REQ-UI-004, REQ-UI-005, REQ-UI-006  
**Deciders:** Pascal Schmitt

#### Context

`MainForm.cs` contains four `_CheckedChanged` event handlers that encode
Xpander synth behaviour rules:

- `ENV_X_TRIG_EXTRIG_CheckedChanged` — when EXTRIG is checked, uncheck LFOTRIG, disable LFO_SOURCE, enable GATED.
- `ENV_X_TRIG_LFOTRIG_CheckedChanged` — when LFOTRIG is checked, uncheck EXTRIG, enable LFO_SOURCE and GATED.
- `RAMP_X_TRIG_EXTRIG_CheckedChanged` — identical structure for RAMP.
- `RAMP_X_TRIG_LFOTRIG_CheckedChanged` — identical structure for RAMP.

Each handler ends with `CheckBoxValuedControl_CheckedChanged(sender, e)` to
propagate the value to the controller. The conditional enable/disable logic
(which controls are enabled given which checkboxes are active) is a synth
protocol rule, not a WinForms display rule. The ENV and RAMP variants are
structurally identical.

#### Decision

**Create `TriggerRuleManager`** in `Xplorer\View\TriggerRuleManager.cs`:

- `internal sealed class TriggerRuleManager`
- Constructor: `TriggerRuleManager(MainForm form)`
- Two public methods:
  - `ApplyExTrigCheckedRule(IValuedControl exTrigControl, IValuedControl lfoTrigControl, IValuedControl lfoSourceControl, IValuedControl gatedControl, bool isChecked)`
  - `ApplyLfoTrigCheckedRule(IValuedControl exTrigControl, IValuedControl lfoTrigControl, IValuedControl lfoSourceControl, IValuedControl gatedControl, bool isChecked)`
- Both ENV and RAMP handlers delegate to the same methods, passing the
  relevant controls as arguments — eliminating the ENV/RAMP duplication.
- After applying rules, each handler still calls `CheckBoxValuedControl_CheckedChanged`
  to propagate to the controller (no change to controller interaction).

#### Alternatives Considered

| Alternative | Reason rejected |
|---|---|
| Move rules into `XpanderController` and fire `AutomationParameterChangeEvent` back | Would require the controller to know about control enable/disable state — controller-to-view coupling in the wrong direction |
| Encode rules in `XpanderConstants` as a lookup table | Over-engineering for four known rules; adds indirection without simplifying |
| Leave as-is | The existing `#warning TODO` in adjacent code confirms the debt is acknowledged |

#### Consequences

- **Positive:** `MainForm.cs` four handlers become one-line delegations.
- **Positive:** ENV and RAMP trigger rules share the same implementation.
- **Positive:** The rule is readable in isolation, without a WinForms context.
- **Negative:** The manager must accept `Control` or `IValuedControl` references as parameters, keeping a light coupling to WinForms types. This is unavoidable with the Designer contract and is consistent with how `ModulationMatrixManager` operates.

---

### ADR-UI-003 — Centralise Tag Prefix Resolution in XpanderConstants

**ID:** ADR-UI-003  
**Status:** Proposed  
**Date:** 2026  
**Addresses:** REQ-UI-003, REQ-UI-004  
**Deciders:** Pascal Schmitt

#### Context

Two private methods in `MainForm.Overrides.cs` — `GetParameterNameForValuedControlTag`
and `GetValuedControlAndButtonNameForParameterName` — share the same four
`StartsWith` chains with hard-coded string literals (`"ENV_X"`, `"LFO_X"`,
`"TRACK_X"`, `"RAMP_X"`, `"ENV_"`, `"LFO_"`, `"TRACK_"`, `"RAMP_"`), each with
different substring indices. A `#warning TODO` explicitly notes these literals
should be unified.

`XpanderConstants` already defines the numeric counts for each page type
(`LFO_COUNT = 5`, `ENV_COUNT = 5`, `TRACK_COUNT = 3`, `RAMP_COUNT = 4`) but
does not define the corresponding tag prefixes.

#### Decision

**Add a static lookup structure to `XpanderConstants`** — a list or dictionary
mapping a page-family prefix (e.g. `"ENV_X"`) to its:

- Base prefix used in tag names (e.g. `"ENV_"`)
- Digit index in a parameter name string
- Page count

Both `GetParameterNameForValuedControlTag` and `GetValuedControlAndButtonNameForParameterName`
are then rewritten to iterate this lookup instead of using `StartsWith` chains,
and the `#warning` is removed.

The two methods remain in `MainForm.Overrides.cs` for now (they are form-level
helpers that use `_pagesRadioButtonsMap`). Moving them to a utility class is
deferred to a future task (not in scope here) to respect REQ-UI-004.

#### Alternatives Considered

| Alternative | Reason rejected |
|---|---|
| Move both methods to a new `TagResolver` static class | Increases scope; the methods still need access to `_pagesRadioButtonsMap` which is private to `MainForm.Overrides.cs` — requires additional API changes |
| Leave as-is | Explicitly contradicts the existing `#warning TODO` |

#### Consequences

- **Positive:** Removes the `#warning TODO`.
- **Positive:** Adding a new page family (hypothetical future) requires one entry in the lookup, not changes to two method bodies.
- **Positive:** No behaviour change (purely structural).
- **Negative:** Minor — the lookup structure adds a small amount of indirection that is slightly less obvious than the current chain at first glance.

---

## Part 3 — Technical Decisions

---

### TD-UI-001 — File Locations

**ID:** TD-UI-001  
**Related ADR:** ADR-UI-001, ADR-UI-002

| New File | Location |
|---|---|
| `PageRefreshManager.cs` | `Xplorer\View\PageRefreshManager.cs` |
| `TriggerRuleManager.cs` | `Xplorer\View\TriggerRuleManager.cs` |

Both files are in the same directory and project namespace as `FileOperationsManager.cs`
and `ModulationMatrixManager.cs`.

---

### TD-UI-002 — Constructor and Field Convention

**ID:** TD-UI-002  
**Related ADR:** ADR-UI-001, ADR-UI-002

All new Manager classes follow this exact pattern:

```csharp
internal sealed class XxxManager
{
    private readonly MainForm _form;

    internal XxxManager(MainForm form)
    {
        Debug.Assert(form != null);
        _form = form;
    }
}
```

Matches `FileOperationsManager` and `ModulationMatrixManager` exactly.

---

### TD-UI-003 — Instantiation in OnLoad

**ID:** TD-UI-003  
**Related ADR:** ADR-UI-001, ADR-UI-002

New Manager instances are created in `MainForm.Overrides.cs` `OnLoad`,
in the same block as the existing managers:

```csharp
_settingsManager = new SettingsManager(this);
_fileOperationsManager = new FileOperationsManager(this);
// NEW:
_pageRefreshManager = new PageRefreshManager(this);
_triggerRuleManager = new TriggerRuleManager(this);
```

---

### TD-UI-004 — XpanderConstants Page Family Descriptor

**ID:** TD-UI-004  
**Related ADR:** ADR-UI-003

A new `internal readonly struct PageFamilyDescriptor` is added to `XpanderConstants.cs`:

```csharp
internal readonly struct PageFamilyDescriptor
{
    internal string TagPrefix { get; init; }      // e.g. "ENV_X"
    internal string NamePrefix { get; init; }     // e.g. "ENV_"
    internal int DigitIndex { get; init; }        // index of the digit in parameter name
    internal int Count { get; init; }             // number of pages in this family
}
```

A static `IReadOnlyList<PageFamilyDescriptor> PageFamilies` property exposes the
four families. Both resolution methods iterate this list.

---

### TD-UI-005 — Impact on MidiApp.MidiController Repository

**ID:** TD-UI-005

**No changes to `MidiApp.MidiController` are required or planned.**

`AbstractControllerMainForm` is not modified. All changes are confined to the
`Xplorer` project (single Git repository: `XplorerEditor`).
