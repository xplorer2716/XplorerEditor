# PLAN-GUI-011: Save Patch Dialog — Default the File Name to the Tone Name

## Overview
Add a small usability fix requested by the owner: the save-patch dialog
(File → Save / the "save patch" shortcut) currently opens with a blank file
name, forcing the user to type it by hand every time. It should default to
the edited tone's own name.

An OS-legal-filename sanitizer already exists — `midiapp::service::
makeUniqueFilenameFromString` (`FileUtils.hpp`), used today by the bank
single-patch extraction (RQ-CTL-003). This reuses it rather than adding a
second implementation of the same rule.

## References
- **Requirements**: RQ-GUI-077 (new), RQ-CTL-004
- **ADRs**: None. Reuses/extends an existing utility module (`FileUtils`)
  through its existing public signature style; no new architectural
  decision. TASK-GUI-038 is Tier S; TASK-GUI-041 is Tier M (adds one shared
  method, `trimTrailingSpaces`, used by two modules).

---

## Tasks

### TASK-GUI-038: Pre-fill the save-patch dialog with the tone name
- **Tier**: S
- **Status**: Done (2026-08-18)
- **Description**: In `MainComponent`'s `btPatchSave` action (shared by the
  File → Save menu item and the shortcut button, RQ-GUI-021), compute a
  sanitized, unique default file name from `_controller->toneName()` (the
  public `AbstractController` accessor — `xpanderTone()` is private) via
  `makeUniqueFilenameFromString`, checked against the user's Documents
  directory (the dialog's own starting directory, chosen because no
  directory is tracked anywhere else in this codebase), and pass it as the
  `FileChooser`'s initial file so its name field is pre-filled.
- **Requirement refs**: RQ-GUI-077
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** an edited tone named "LEAD 1", **When** File → Save (or the
    save-patch shortcut) is triggered, **Then** the save dialog's file name
    field reads "LEAD 1.syx"
  - **Given** a tone name containing characters illegal in a file name,
    **When** the dialog opens, **Then** the pre-filled name has already had
    them stripped by the same sanitizer the bank-extraction path uses
  - **Given** the pre-filled name, **When** the user still edits or replaces
    it before confirming, **Then** the file is saved under whatever name
    they confirm (unchanged existing behaviour — this task only changes the
    dialog's starting value)
- **Dependencies**: None
- **Assignee**: AI
  - *Amended 2026-08-18 (owner report, same session):* `toneName()` returns
    the model's fixed-width, space-padded storage form, so a short name
    reached the sanitizer with meaningless trailing spaces (space is a legal
    file-name character, so the sanitizer did not remove them). Fixed by
    trimming trailing whitespace (`juce::String::trimEnd()`) before the
    sanitizer call — at the call site, not inside the shared sanitizer, so
    RQ-CTL-003's bank extraction (which reads already-unpadded names) is
    untouched.

### TASK-GUI-041: Trim trailing spaces in bulk single-tone extraction file names too
- **Tier**: M
- **Status**: Done (2026-08-26)
- **Description**: `getSingleTonesFromSynth`'s reception handler
  (`XpanderControllerMidiEvents.cpp`, `handleAllDataDumpRequest`,
  `SinglePatch` mode) built each extracted patch's file name from
  `XpanderTone::getNameFromByteArray` — the same fixed-width, space-padded
  storage form TASK-GUI-038 already found and trimmed for the save-patch
  dialog — without trimming it, so short patch names produced file names
  with meaningless trailing spaces. The controller module has no JUCE
  dependency (ADR-JUC-004), so it cannot call `juce::String::trimEnd()` as
  the save-patch call site does; the trim step is instead extracted into a
  new shared helper, `midiapp::service::trimTrailingSpaces` (`FileUtils.hpp`/
  `.cpp`), called from both this call site and the save-patch dialog's — one
  implementation of the trim rule, not one per layer.
- **Requirement refs**: RQ-GUI-077, RQ-CTL-004
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** a synth patch named "LEAD 1" (shorter than the fixed on-device
    width, hence space-padded internally), **When** "Get all single patches
    from synth" extracts it, **Then** the written file is named "LEAD 1.syx"
    with no trailing spaces
  - **Given** the save-patch dialog's default file name (TASK-GUI-038),
    **When** it is computed, **Then** it uses the same
    `trimTrailingSpaces` helper as the bulk-extraction path, not a separate
    implementation
- **Dependencies**: TASK-GUI-038
- **Assignee**: AI

---

## Note on testing
Tier S, and the changed code is a lambda gluing an existing, already-tested
sanitizer (`FileUtilsTests`, if present, or exercised indirectly through
RQ-CTL-003) to a `juce::FileChooser` constructor argument — no new function
was introduced. Verified by reading the resulting `FileChooser`'s
`getStartingFile()`/constructor argument in isolation is impractical
(native, OS-driven dialog); verification is a full app build plus reasoning
through JUCE's own documented `initialFileOrDirectory` behaviour, not a new
automated test.

## Out of scope
Tracking a "last used directory" across saves/loads — no such mechanism
exists anywhere in this codebase today, and the owner did not ask for one;
introducing it here would be scope creep. The starting directory stays
whatever JUCE's own file-chooser default is, now carrying a pre-filled name
alongside it.
