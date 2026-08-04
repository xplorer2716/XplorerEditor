# PLAN-GFX-005: Reference-Aligned Menu Bar

## Overview

Bring File/Patch/Tools/Help into exact alignment with
`XplorerEditor-dotnet-archive`'s `MainForm` — order, labels, icons and keyboard
shortcuts — leaving View (RQ-SCL-002/003) and Piano keyboard (RQ-GUI-028) as the
two sanctioned JUCE-only departures. Implements the corrected RQ-GUI-008 and
ADR-JUC-032 (DEC-JUC-098/099/100/101).

Owner report (2026-08-04, session GFX): *"Les menus dans la version JUCE ne sont
pas alignés avec ceux de la version de référence [...] Ils ne sont pas dans le
même ordre, n'ont pas de raccourcis affichés, et manquent les icônes associées."*

Two research corrections made during specification, recorded because they
changed the plan's shape:
- **Shortcuts do exist in the reference** (sixteen of them) — an initial grep of
  `ShortcutKeyDisplayString` (always blank) wrongly concluded otherwise; the real
  source is the separate `ShortcutKeys` property, confirmed against the owner's
  own list and the `.resx`.
- **"New" and "Save as" are not simple menu wiring.** "New" loads a bundled
  default patch (not a blank one); "Save as" is dropped entirely, owner-confirmed
  redundant with "Save" (which already always prompts for a file).

## References

- **Requirements**: RQ-GUI-008 *(corrected — this plan implements it)*, RQ-GUI-007,
  RQ-GUI-025, RQ-GUI-027, RQ-GUI-028, RQ-SCL-002, RQ-SCL-003
- **ADRs**: ADR-JUC-032 *(this plan implements it — DEC-JUC-098/099/100/101)*

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = chat-eco`.
Branch: `feature/GFX`.

## Final menu content (the contract this plan builds toward)

| Menu | Items, in order | Shortcuts |
|---|---|---|
| File | New, Open, *sep*, Save, *sep*, Exit | Ctrl+N, Ctrl+O, —, Ctrl+S, —, — |
| Patch | Previous, Next, Go to patch..., *sep*, Randomize, Rename, Store, Synchronize | F5, F6, F7, —, F8, F9, F10, F12 |
| Tools | Settings, Tune Request, Piano keyboard *(exception)*, Single patches...▸[...], Backup/Restore...▸[...] | Ctrl+G, F4, —, —, — |
| Help | Xplorer help, *sep*, Check for new releases, Go to website, About... | F1, —, —, —, — |

Icons: New, Open, Save only (already extracted to
`juce/app/assets/menu_new.png`, `menu_open.png`, `menu_save.png` — verified valid
PNGs, decoded from `MainForm.resx`).

## What must NOT change

- **View menu** (RQ-SCL-002/003) and its scale-preset/full-screen logic —
  untouched, explicitly excluded from the alignment by the owner.
- **Piano keyboard** — kept, same position (after Tune Request), no shortcut.
- **Every existing action's underlying logic** — Open/Save file pickers,
  Goto/Store dialogs, Rename dialog, Randomize, Settings dialog, Tune request,
  Single-patches/Backup-Restore submenus. This plan changes menu *shape* and
  adds a few *new* dispatch cases; it does not touch working ones.
- **`menuItemSelected`'s existing switch cases** for ids already wired — new ids
  are added, not renumbered, so no existing shortcut/click path is disturbed
  mid-plan.

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GFX-008 | Embed the 3 menu icons; add `oberheim.syx` post-build copy | M | RQ-GUI-008, DEC-JUC-098, DEC-JUC-100 |
| TASK-GFX-009 | Rebuild File/Patch/Help menu content (order, labels, new items); wire Synchronize, Help URLs, New | M | RQ-GUI-008, DEC-JUC-100, DEC-JUC-101 |
| TASK-GFX-010 | Shortcut table + `keyPressed` override + icon/shortcut display in `getMenuForIndex` | M | RQ-GUI-008, DEC-JUC-099 |

Sequential: 009 needs 008's assets in place to attach icons; 010 needs 009's
final item ids to build its table against.

---

### TASK-GFX-008: Embed menu icons; wire the default-patch file copy
- **Tier**: M
- **Status**: Not Started
- **Description**: Add `assets/menu_new.png`, `assets/menu_open.png`,
  `assets/menu_save.png` (already extracted and committed) to
  `juce_add_binary_data(XplorerAssets SOURCES ...)` in `juce/app/CMakeLists.txt`.
  Add a `POST_BUILD` custom command on the `XplorerApp` target copying
  `${CMAKE_CURRENT_SOURCE_DIR}/../oberheim.syx` to `$<TARGET_FILE_DIR:XplorerApp>`
  (mirrors the reference's own build-time content copy). No C++ consumer yet —
  this task only makes the assets available.
- **Requirement refs**: RQ-GUI-008, RQ-GUI-007
- **ADR refs**: ADR-JUC-032 (DEC-JUC-098, DEC-JUC-100)
- **Acceptance Criteria** (Gherkin):
  - *Given* a clean configure and build, *When* `XplorerAssets` builds, *Then*
    `BinaryData::menu_new_png`, `menu_open_png` and `menu_save_png` (and their
    `*Size` companions) exist.
  - *Given* a build of `XplorerApp`, *When* it completes, *Then*
    `oberheim.syx` is present next to `Xplorer.exe`.
  - *Given* the build, *When* it runs, *Then* it compiles with no warning and
    the suite passes with no test modified.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GFX-009: File/Patch/Help menu content — order, labels, new items
- **Tier**: M
- **Status**: Not Started
- **Description**: In `MainComponent::getMenuForIndex`, rebuild the File,
  Patch and Help cases to the table in this plan's Overview (Tools already
  matches, aside from label casing "Tune request" → "Tune Request" — folded in
  here since it's a one-line change in the same function). Assign new
  contiguous ids for the four new dispatch targets — Synchronize, Xplorer help,
  Check for new releases, Go to website — and add their cases to
  `menuItemSelected`: Synchronize calls
  `_controller->sendProgramChangeAndGetSinglePatchFromSynth(_controller->currentProgramNumber())`
  (same call Goto/Store already make); the three Help items call
  `juce::URL(...).launchInDefaultBrowser()` with the reference's three URLs
  (user manual, GitHub releases, project website — named constants, not
  inline literals per DoD). Add a "New" case that resolves
  `juce::File::getSpecialLocation(juce::File::currentExecutableFile).getSiblingFile("oberheim.syx")`
  and passes it to the same controller entry point Open's file-chooser callback
  already uses. Remove "Save as" from nowhere (it was never present) — this
  task's File case simply never adds it, per the owner-confirmed deviation.
  Icon attachment and shortcut display are deliberately **not** part of this
  task (TASK-GFX-010) — this task only gets content, order and dispatch right.
- **Requirement refs**: RQ-GUI-008, RQ-GUI-027
- **ADR refs**: ADR-JUC-032 (DEC-JUC-100, DEC-JUC-101)
- **Acceptance Criteria** (Gherkin):
  - *Given* the File menu, *When* opened, *Then* it shows New, Open, *sep*,
    Save, *sep*, Exit — in that order, no "Save as".
  - *Given* the Patch menu, *When* opened, *Then* it shows Previous, Next, Go to
    patch..., *sep*, Randomize, Rename, Store, Synchronize — in that order.
  - *Given* the Help menu, *When* opened, *Then* it shows Xplorer help, *sep*,
    Check for new releases, Go to website, About... — in that order.
  - *Given* "New" is clicked, *When* it completes, *Then* the loaded tone matches
    `oberheim.syx`'s contents and the program number is whatever the file
    specifies.
  - *Given* "Synchronize" is clicked, *When* it completes, *Then* the current
    patch is re-fetched from the synth, identically to Goto/Store.
  - *Given* each of the three Help URL items is clicked, *When* invoked, *Then*
    the corresponding URL opens in the system default browser.
  - *Given* the build, *When* it runs, *Then* it compiles with no warning and
    the suite passes with no test modified.
- **Dependencies**: TASK-GFX-008
- **Assignee**: AI

---

### TASK-GFX-010: Shortcut table, `keyPressed` override, icon and shortcut display
- **Tier**: M
- **Status**: Not Started
- **Description**: Add a file-local `constexpr` array of
  `{menuItemId, juce::KeyPress, const char* displayText}` for the sixteen
  reference shortcuts (New/Open/Save/Previous/Next/Goto/Randomize/Rename/Store/
  Synchronize/Settings/TuneRequest/XplorerHelp — ids from TASK-GFX-009 plus the
  existing ones). `getMenuForIndex` builds each affected item via the explicit
  `PopupMenu::Item` form (not the `addItem(id, text)` convenience) so
  `shortcutKeyDescription` can be set from the table, and attaches
  `juce::Drawable::createFromImageData(BinaryData::menu_*_png, ...Size)` as
  `item.image` for New/Open/Save only. Add
  `bool MainComponent::keyPressed(const juce::KeyPress&) override`: look up the
  incoming key in the table; on a match, call `menuItemSelected(id, 0)` and
  return `true`; otherwise return `false` (so unmatched keys — e.g.
  `PageSelectorButton`'s own Ctrl+C/Ctrl+V — fall through unaffected).
  `setWantsKeyboardFocus` on `MainComponent`/`ScaledCanvasComponent` as needed
  so the override actually receives key events at the window level.
- **Requirement refs**: RQ-GUI-008
- **ADR refs**: ADR-JUC-032 (DEC-JUC-099)
- **Acceptance Criteria** (Gherkin):
  - *Given* each of the sixteen reference shortcuts, *When* its menu item is
    read, *Then* its displayed shortcut text matches the reference exactly.
  - *Given* an item with no reference shortcut (Exit, About, Piano keyboard,
    View, and everything under Single patches/Backup-Restore), *When* read,
    *Then* it shows none.
  - *Given* the application has focus, *When* any of the sixteen key
    combinations is pressed, *Then* the same action as its menu click fires.
  - *Given* a `PageSelectorButton` is focused, *When* Ctrl+C/Ctrl+V is pressed,
    *Then* page copy/paste (RQ-GUI-027) still works, unaffected by the new
    top-level handler.
  - *Given* New/Open/Save, *When* the File menu is opened, *Then* each shows its
    reference icon; *given* every other item in every menu, *Then* none shows
    an icon.
  - *Given* the build, *When* it runs, *Then* it compiles with no warning and
    the suite passes with no test modified.
- **Dependencies**: TASK-GFX-009
- **Assignee**: AI

---

## Verification

- **Headless guard** (`session.unit_tests = true`): the shortcut table (id ↔
  `KeyPress` ↔ display text) is a plain data structure the tokens/geometry
  pattern in this session already established as worth pinning — a test can
  assert its sixteen entries against the reference table above without a
  display. This is the one piece of this plan that is mechanically checkable
  beyond "it compiles."
- **Visual/interactive verification by launching the app**: menu content/order
  (screenshot each dropdown), icon presence, and actually pressing each of the
  sixteen shortcuts — there is no pixel baseline for `PopupMenu`, matching every
  other menu/dialog surface in this codebase.
- **Existing suite stays green**, unmodified, at every task.

## Definition of Ready

- [x] Each task has a description, Gherkin acceptance criteria and a tier.
- [x] Each task references its requirement and ADR IDs.
- [x] No UI literal introduced without a named constant (URLs, ids, the shortcut
      table) — DoD applies at Tier M.
- [ ] **Owner approval — pending.** Requirement, ADR and plan are written; no
      code has been touched beyond the already-extracted icon PNGs.
