# UI Improvement Plan — Implementation Tasks

> **Parent document:** [`ui_improvement_analysis.md`](./ui_improvement_analysis.md)  
> **Scope:** `Xplorer` project — `MainForm.cs`, `MainForm.Overrides.cs`, `XpanderConstants.cs`  
> **Constraint:** Zero behaviour change (REQ-UI-004). No new dependencies (REQ-UI-005).

---

## Summary of Work

| Task | Title | ADR | REQ | Effort |
|---|---|---|---|---|
| TASK-UI-001 | Centralise tag prefix resolution in `XpanderConstants` | ADR-UI-003 | REQ-UI-003 | Small |
| TASK-UI-002 | Create `TriggerRuleManager` | ADR-UI-002 | REQ-UI-002, REQ-UI-006 | Small |
| TASK-UI-003 | Create `PageRefreshManager` | ADR-UI-001 | REQ-UI-001, REQ-UI-006 | Medium |
| TASK-UI-004 | Wire new managers into `MainForm` | ADR-UI-001, ADR-UI-002 | REQ-UI-004, REQ-UI-006 | Small |
| TASK-UI-005 | Update architecture documentation | — | REQ-UI-007 | Small |

**Recommended execution order:** TASK-UI-001 → TASK-UI-002 → TASK-UI-003 → TASK-UI-004 → TASK-UI-005

Rationale: TASK-UI-001 provides the data structure that TASK-UI-003 consumes.
TASK-UI-002 and TASK-UI-003 can proceed independently after TASK-UI-001.
TASK-UI-004 wires everything together and is the final integration step.

---

## TASK-UI-001 — Centralise Tag Prefix Resolution in `XpanderConstants`

**ID:** TASK-UI-001  
**File:** `Xplorer\Common\XpanderConstants.cs`  
**ADR:** ADR-UI-003  
**REQ:** REQ-UI-003  
**Depends on:** nothing  
**Effort:** Small (~30 lines added, 0 lines removed from existing code)

### What to do

Add a new `internal readonly struct PageFamilyDescriptor` and a static
`PageFamilies` list to `XpanderConstants`. This replaces the four hard-coded
`StartsWith` chains that currently exist in `MainForm.Overrides.cs`.

### Struct to add inside `XpanderConstants`

```csharp
/// <summary>
/// Describes a family of multi-page controls sharing the same tag prefix (e.g. ENV_X, LFO_X).
/// Used for data-driven tag-to-parameter-name resolution.
/// </summary>
internal readonly struct PageFamilyDescriptor
{
    /// <summary>Tag prefix as it appears on the WinForms control (e.g. "ENV_X", "TRACK_X").</summary>
    internal string ControlTagPrefix { get; init; }

    /// <summary>Parameter name prefix used in the tone's ParameterMap (e.g. "ENV_", "TRACK_").</summary>
    internal string ParameterNamePrefix { get; init; }

    /// <summary>
    /// Zero-based index of the digit character within a concrete parameter name
    /// (e.g. in "ENV_1_ATTACK", the digit '1' is at index 4).
    /// </summary>
    internal int DigitIndex { get; init; }

    /// <summary>Number of instances of this page family (e.g. 5 for ENV, 4 for RAMP).</summary>
    internal int Count { get; init; }
}
```

### Static list to add inside `XpanderConstants`

```csharp
/// <summary>
/// Describes all multi-page control families, keyed by their control tag prefix.
/// Drives tag-to-parameter-name resolution without hard-coded string literals.
/// </summary>
internal static readonly IReadOnlyList<PageFamilyDescriptor> PageFamilies =
    new List<PageFamilyDescriptor>
    {
        new PageFamilyDescriptor { ControlTagPrefix = "ENV_X", ParameterNamePrefix = "ENV_", DigitIndex = 4, Count = ENV_COUNT },
        new PageFamilyDescriptor { ControlTagPrefix = "LFO_X", ParameterNamePrefix = "LFO_", DigitIndex = 4, Count = LFO_COUNT },
        new PageFamilyDescriptor { ControlTagPrefix = "RAMP_X", ParameterNamePrefix = "RAMP_", DigitIndex = 5, Count = RAMP_COUNT },
        new PageFamilyDescriptor { ControlTagPrefix = "TRACK_X", ParameterNamePrefix = "TRACK_", DigitIndex = 6, Count = TRACK_COUNT },
    };
```

### Verification

- `XpanderConstants.cs` builds without errors.
- Existing consumers of `XpanderConstants` (e.g. `MainForm.Overrides.cs`) are
  not yet changed at this step — the list is additive.

---

## TASK-UI-002 — Create `TriggerRuleManager`

**ID:** TASK-UI-002  
**File:** `Xplorer\View\TriggerRuleManager.cs` *(new file)*  
**ADR:** ADR-UI-002  
**REQ:** REQ-UI-002, REQ-UI-006  
**Depends on:** nothing (does not use TASK-UI-001 output)  
**Effort:** Small (~80 lines)

### What to do

Extract the four `_CheckedChanged` handler bodies from `MainForm.cs` into a new
`TriggerRuleManager`. The ENV and RAMP variants share the same two methods;
the correct controls are passed as arguments.

### New file skeleton

```csharp
using MidiApp.UIControls;
using System.Windows.Forms;

namespace Xplorer.View
{
    /// <summary>
    /// Manages Xpander trigger mutual-exclusion UI rules for ENV and RAMP pages.
    /// Enforces the rule that EXTRIG and LFOTRIG cannot be simultaneously active,
    /// and controls the enabled state of dependent controls accordingly.
    /// </summary>
    internal sealed class TriggerRuleManager
    {
        private readonly MainForm _form;

        internal TriggerRuleManager(MainForm form)
        {
            System.Diagnostics.Debug.Assert(form != null);
            _form = form;
        }

        /// <summary>
        /// Applies enable/disable and mutual-exclusion rules when EXTRIG is changed.
        /// Call from ENV_X_TRIG_EXTRIG_CheckedChanged and RAMP_X_TRIG_EXTRIG_CheckedChanged.
        /// </summary>
        internal void ApplyExTrigRule(
            CheckBoxValuedControl exTrigControl,
            CheckBoxValuedControl lfoTrigControl,
            Control lfoSourceControl,
            Control gatedControl)
        {
            if (exTrigControl.Checked)
            {
                lfoTrigControl.Checked = false;
                lfoSourceControl.Enabled = false;
                gatedControl.Enabled = true;
            }
            else
            {
                lfoSourceControl.Enabled = false;
                if (!lfoTrigControl.Checked)
                {
                    gatedControl.Enabled = false;
                }
            }
        }

        /// <summary>
        /// Applies enable/disable and mutual-exclusion rules when LFOTRIG is changed.
        /// Call from ENV_X_TRIG_LFOTRIG_CheckedChanged and RAMP_X_TRIG_LFOTRIG_CheckedChanged.
        /// </summary>
        internal void ApplyLfoTrigRule(
            CheckBoxValuedControl exTrigControl,
            CheckBoxValuedControl lfoTrigControl,
            Control lfoSourceControl,
            Control gatedControl)
        {
            if (lfoTrigControl.Checked)
            {
                exTrigControl.Checked = false;
                lfoSourceControl.Enabled = true;
                gatedControl.Enabled = true;
            }
            else
            {
                lfoSourceControl.Enabled = false;
                if (!exTrigControl.Checked)
                {
                    gatedControl.Enabled = false;
                }
            }
        }
    }
}
```

### Changes to `MainForm.cs`

The four `_CheckedChanged` handlers become one-line delegations.

**Before (example — `ENV_X_TRIG_EXTRIG_CheckedChanged`):**
```csharp
private void ENV_X_TRIG_EXTRIG_CheckedChanged(object sender, EventArgs e)
{
    if (ENV_X_TRIG_EXTRIG.Checked)
    {
        this.ENV_X_TRIG_LFOTRIG.Checked = false;
        this.ENV_X_TRIG_LFO_SOURCE.Enabled = false;
        this.ENV_X_TRIG_GATED.Enabled = true;
    }
    else
    {
        this.ENV_X_TRIG_LFO_SOURCE.Enabled = false;
        if (!this.ENV_X_TRIG_LFOTRIG.Checked)
        {
            this.ENV_X_TRIG_GATED.Enabled = false;
        }
    }
    CheckBoxValuedControl_CheckedChanged(sender, e);
}
```

**After:**
```csharp
private void ENV_X_TRIG_EXTRIG_CheckedChanged(object sender, EventArgs e)
{
    _triggerRuleManager.ApplyExTrigRule(ENV_X_TRIG_EXTRIG, ENV_X_TRIG_LFOTRIG, ENV_X_TRIG_LFO_SOURCE, ENV_X_TRIG_GATED);
    CheckBoxValuedControl_CheckedChanged(sender, e);
}
```

Apply the same transformation to:
- `ENV_X_TRIG_LFOTRIG_CheckedChanged` → `ApplyLfoTrigRule`
- `RAMP_X_TRIG_EXTRIG_CheckedChanged` → `ApplyExTrigRule`
- `RAMP_X_TRIG_LFOTRIG_CheckedChanged` → `ApplyLfoTrigRule`

### Field to add in `MainForm.Overrides.cs`

```csharp
private TriggerRuleManager _triggerRuleManager;
```

### Verification

- Application builds without errors.
- ENV trigger interaction: checking EXTRIG unchecks LFOTRIG, disables LFO_SOURCE, enables GATED — identical to before.
- RAMP trigger interaction: identical to before.
- MIDI output is unaffected (controller interaction path via `CheckBoxValuedControl_CheckedChanged` is unchanged).

---

## TASK-UI-003 — Create `PageRefreshManager`

**ID:** TASK-UI-003  
**File:** `Xplorer\View\PageRefreshManager.cs` *(new file)*  
**ADR:** ADR-UI-001  
**REQ:** REQ-UI-001, REQ-UI-006  
**Depends on:** TASK-UI-001 (uses `XpanderConstants.PageFamilies`)  
**Effort:** Medium (~180 lines)

### What to do

Extract the four `Radio_X_Click` handler bodies from `MainForm.cs` and the
`OnPageChange` routing from `MainForm.Overrides.cs` into a new `PageRefreshManager`.
Also rewrite `GetParameterNameForValuedControlTag` and
`GetValuedControlAndButtonNameForParameterName` in `MainForm.Overrides.cs`
to use `XpanderConstants.PageFamilies` (removing the `#warning TODO`).

### New file skeleton

```csharp
using MidiApp.MidiController.Service;
using MidiApp.UIControls;
using System;
using System.Diagnostics;
using System.Windows.Forms;
using Xplorer.Common;
using Xplorer.Controller;

namespace Xplorer.View
{
    /// <summary>
    /// Manages multi-page panel refresh (ENV, LFO, RAMP, TRACK).
    /// Encapsulates the per-page control value update sequences that were
    /// previously duplicated across Radio_ENV_X_Click, Radio_LFO_X_Click,
    /// Radio_RAMP_X_Click, and Radio_TRACK_X_Click in MainForm.
    /// </summary>
    internal sealed class PageRefreshManager
    {
        private readonly MainForm _form;

        internal PageRefreshManager(MainForm form)
        {
            Debug.Assert(form != null);
            _form = form;
        }

        private XpanderController XController => _form.XController;

        // ---------------------------------------------------------------
        // ENV page
        // ---------------------------------------------------------------

        internal void RefreshEnvPage(RadioButton button, bool fromUser)
        {
            if (button == null) return;
            if (fromUser) XController.SendPageUpdate((string)button.Tag);

            string p(string tag) => _form.ResolveParameterNameForTag(tag);

            _form.ENV_X_ATTACK.Value          = XController.GetParameter(p((string)_form.ENV_X_ATTACK.Tag)).Value;
            _form.ENV_X_DECAY.Value           = XController.GetParameter(p((string)_form.ENV_X_DECAY.Tag)).Value;
            _form.ENV_X_DELAY.Value           = XController.GetParameter(p((string)_form.ENV_X_DELAY.Tag)).Value;
            _form.ENV_X_MODE_DADR.Value       = XController.GetParameter(p((string)_form.ENV_X_MODE_DADR.Tag)).Value;
            _form.ENV_X_MODE_FREERUN.Value    = XController.GetParameter(p((string)_form.ENV_X_MODE_FREERUN.Tag)).Value;
            _form.ENV_X_MODE_RESET.Value      = XController.GetParameter(p((string)_form.ENV_X_MODE_RESET.Tag)).Value;
            _form.ENV_X_RELEASE.Value         = XController.GetParameter(p((string)_form.ENV_X_RELEASE.Tag)).Value;
            _form.ENV_X_SUSTAIN.Value         = XController.GetParameter(p((string)_form.ENV_X_SUSTAIN.Tag)).Value;
            _form.ENV_X_TRIG_EXTRIG.Value     = XController.GetParameter(p((string)_form.ENV_X_TRIG_EXTRIG.Tag)).Value;
            _form.ENV_X_TRIG_GATED.Value      = XController.GetParameter(p((string)_form.ENV_X_TRIG_GATED.Tag)).Value;
            _form.ENV_X_TRIG_LFOTRIG.Value    = XController.GetParameter(p((string)_form.ENV_X_TRIG_LFOTRIG.Tag)).Value;
            _form.ENV_X_TRIG_LFO_SOURCE.Value = XController.GetParameter(p((string)_form.ENV_X_TRIG_LFO_SOURCE.Tag)).Value;
            _form.ENV_X_TRIG_SINGLE_MULTI.Value = XController.GetParameter(p((string)_form.ENV_X_TRIG_SINGLE_MULTI.Tag)).Value;
            _form.ENV_X_VOLUME.Value          = XController.GetParameter(p((string)_form.ENV_X_VOLUME.Tag)).Value;

            _form.VfdDisplayHelper.UpdateState(_form.ENV_X_VOLUME);
        }

        // ---------------------------------------------------------------
        // LFO page  (same structural pattern)
        // ---------------------------------------------------------------

        internal void RefreshLfoPage(RadioButton button, bool fromUser) { /* ... same pattern */ }

        // ---------------------------------------------------------------
        // RAMP page
        // ---------------------------------------------------------------

        internal void RefreshRampPage(RadioButton button, bool fromUser) { /* ... same pattern */ }

        // ---------------------------------------------------------------
        // TRACK page
        // ---------------------------------------------------------------

        internal void RefreshTrackPage(RadioButton button, bool fromUser) { /* ... same pattern */ }
    }
}
```

> **Note on `ResolveParameterNameForTag`:**
> `GetParameterNameForValuedControlTag` in `MainForm.Overrides.cs` is made
> `internal` (it currently is `private`) and renamed `ResolveParameterNameForTag`
> so `PageRefreshManager` can call it. The method body is simultaneously
> rewritten per TASK-UI-001.

### Changes to `MainForm.cs`

**Before (example — `Radio_ENV_X_Click`):**
```csharp
private void Radio_ENV_X_Click(object sender, EventArgs e)
{
    RadioButton button = sender as RadioButton;
    if (button == null) return;
    if (e != null) XController.SendPageUpdate((string)button.Tag);
    string sParameterName = GetParameterNameForValuedControlTag((string)this.ENV_X_ATTACK.Tag);
    this.ENV_X_ATTACK.Value = Controller.GetParameter(sParameterName).Value;
    // ... 13 more lines ...
    this.VfdDisplayHelper.UpdateState(this.ENV_X_VOLUME);
}
```

**After:**
```csharp
private void Radio_ENV_X_Click(object sender, EventArgs e)
    => _pageRefreshManager.RefreshEnvPage(sender as RadioButton, e != null);
```

Apply the same transformation to `Radio_LFO_X_Click`, `Radio_RAMP_X_Click`,
`Radio_TRACK_X_Click`.

### Changes to `MainForm.Overrides.cs`

#### `OnPageChange` — delegate to `PageRefreshManager`

**Before:**
```csharp
private void OnPageChange(object sender, PageChangeEventArgs arg)
{
    string sPageName = arg.PageName;
    RadioButton radio = null;
    if (_pagesRadioButtonsMap.TryGetValue(sPageName, out radio))
    {
        radio.Checked = true;
        if (sPageName.StartsWith("ENV"))       { this.Radio_ENV_X_Click(radio, null); }
        else if (sPageName.StartsWith("LFO"))  { Radio_LFO_X_Click(radio, null); }
        else if (sPageName.StartsWith("RAMP")) { Radio_RAMP_X_Click(radio, null); }
        else if (sPageName.StartsWith("TRACK")){ Radio_TRACK_X_Click(radio, null); }
    }
}
```

**After:**
```csharp
private void OnPageChange(object sender, PageChangeEventArgs arg)
{
    if (!_pagesRadioButtonsMap.TryGetValue(arg.PageName, out RadioButton radio)) return;
    radio.Checked = true;
    _pageRefreshManager.RefreshPage(radio, fromUser: false);
}
```

> `PageRefreshManager.RefreshPage(RadioButton button, bool fromUser)` is a
> dispatcher method that inspects `button.Tag` to determine the page family
> and routes to the correct `RefreshXxxPage` method.

#### `GetParameterNameForValuedControlTag` — rewrite using `PageFamilies`

**Before:**
```csharp
private string GetParameterNameForValuedControlTag(string valuedControlTag)
{
    string tag = valuedControlTag;
    if (tag.StartsWith("ENV_X") || tag.StartsWith("LFO_X"))
    {
        for (int iX = 1; iX < XpanderConstants.LFO_COUNT + 1; iX++)
        {
            string sRadioButtonName = tag.Substring(0, 5).Replace('X', iX.ToString()[0]);
            RadioButton rdButton = _pagesRadioButtonsMap[sRadioButtonName];
            if (rdButton.Checked) { tag = tag.Replace("_X_", ...); break; }
        }
    }
    else if (tag.StartsWith("TRACK_X")) { ... }
    else if (tag.StartsWith("RAMP_X"))  { ... }
    return tag;
}
```

**After:**
```csharp
internal string ResolveParameterNameForTag(string valuedControlTag)
{
    foreach (PageFamilyDescriptor family in XpanderConstants.PageFamilies)
    {
        if (!valuedControlTag.StartsWith(family.ControlTagPrefix)) continue;
        for (int i = 1; i <= family.Count; i++)
        {
            string radioName = family.ControlTagPrefix.Replace("X", i.ToString());
            if (_pagesRadioButtonsMap.TryGetValue(radioName, out RadioButton rd) && rd.Checked)
                return valuedControlTag.Replace("_X_", $"_{i}_");
        }
        break;
    }
    return valuedControlTag;
}
```

#### `GetValuedControlAndButtonNameForParameterName` — rewrite using `PageFamilies`, remove `#warning`

**After:**
```csharp
private bool GetValuedControlAndButtonNameForParameterName(
    string parameterName,
    out string valuedControlTag,
    out string radioButtonTag)
{
    foreach (PageFamilyDescriptor family in XpanderConstants.PageFamilies)
    {
        if (!parameterName.StartsWith(family.ParameterNamePrefix)) continue;
        string digit = parameterName.Substring(family.DigitIndex, 1);
        valuedControlTag = parameterName.Remove(family.DigitIndex, 1).Insert(family.DigitIndex, "X");
        radioButtonTag   = family.ControlTagPrefix.Replace("X", digit);
        return true;
    }
    valuedControlTag = null;
    radioButtonTag   = null;
    return false;
}
```

### Field to add in `MainForm.Overrides.cs`

```csharp
private PageRefreshManager _pageRefreshManager;
```

### Verification

- Application builds without errors or new warnings.
- The `#warning TODO` is gone.
- Switching ENV/LFO/RAMP/TRACK pages updates all controls identically to before.
- `OnPageChange` triggered by synth updates the correct page identically to before.
- `VfdDisplayHelper` receives the same `UpdateState` calls as before.

---

## TASK-UI-004 — Wire New Managers into `MainForm`

**ID:** TASK-UI-004  
**Files:** `Xplorer\View\MainForm.Overrides.cs`  
**ADR:** ADR-UI-001, ADR-UI-002  
**REQ:** REQ-UI-004, REQ-UI-006  
**Depends on:** TASK-UI-002, TASK-UI-003  
**Effort:** Small (field declarations + two lines in `OnLoad`)

### Changes to `MainForm.Overrides.cs`

#### Add private fields (alongside existing manager fields)

```csharp
private PageRefreshManager _pageRefreshManager;
private TriggerRuleManager _triggerRuleManager;
```

#### Instantiate in `OnLoad` (after existing manager instantiation)

```csharp
// existing
_settingsManager        = new SettingsManager(this);
_fileOperationsManager  = new FileOperationsManager(this);

// new
_pageRefreshManager     = new PageRefreshManager(this);
_triggerRuleManager     = new TriggerRuleManager(this);
```

### Verification

- `OnLoad` sequence is unchanged from the user's perspective (no observable difference).
- Both new fields are initialised before any event that could invoke them
  (`RegisterForControllerEvents` is called after this block).

---

## TASK-UI-005 — Update Architecture Documentation

**ID:** TASK-UI-005  
**Files:** `Xplorer\docs\architecture-static.md`, `Xplorer\docs\architecture-dynamic.md`  
**REQ:** REQ-UI-007  
**Depends on:** TASK-UI-004 (document the implemented state)  
**Effort:** Small

### Changes to `architecture-static.md`

- Add `PageRefreshManager` and `TriggerRuleManager` to the **View Layer** class
  diagram with `uses` associations to `MainForm`.
- Add `PageFamilyDescriptor` and `PageFamilies` to the **Constants** section.

### Changes to `architecture-dynamic.md`

- Update the **Page Change** sequence diagram to show the new delegation path:
  `Radio_X_Click → PageRefreshManager.RefreshXxxPage` and
  `OnPageChange → PageRefreshManager.RefreshPage`.
- Add a note on `TriggerRuleManager` under the **User Interaction** section.

---

## Acceptance Checklist (all tasks)

Before closing any task as complete, verify:

- [ ] Project builds: `dotnet build` — zero errors, zero new warnings
- [ ] `MainForm.cs` event handlers for pages are single-line delegations
- [ ] `MainForm.cs` trigger `_CheckedChanged` handlers are single-line delegations
- [ ] `#warning TODO` in `GetValuedControlAndButtonNameForParameterName` is removed
- [ ] Zero new NuGet packages in `Xplorer.csproj`
- [ ] No changes to `MidiApp.MidiController` project
- [ ] New classes are `internal sealed` with `private readonly MainForm _form` field
- [ ] Architecture docs updated

---

## Out of Scope (Deferred)

The following improvements were identified but deliberately excluded from this plan
to stay within the constraints of REQ-UI-004 and REQ-UI-005:

| Deferred Item | Reason |
|---|---|
| Moving `ResolveParameterNameForTag` to a standalone `TagResolver` static class | Requires making `_pagesRadioButtonsMap` accessible outside `MainForm.Overrides.cs`; increases scope |
| Unit tests for `TriggerRuleManager` and `PageRefreshManager` | No test project exists; creating one is a separate initiative |
| Moving binding engine (`RecursiveRegisterValuedUserControl`) out of `AbstractControllerMainForm` | Requires changes to `MidiApp.MidiController` repository; cross-repo change out of scope |
| Async/await modernisation of the worker thread | Separate concern; different ADR required |
