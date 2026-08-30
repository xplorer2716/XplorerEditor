# PLAN-GUI-014: Rename Dialog Opens Without the Stored Name's Wire Padding

## Overview
Closes the owner-reported defect where the rename dialog accepted only as many
new characters as the previous patch name was long — six for a six-character
name, with two spaces surviving behind them. The cause was not the length limit
(`PatchNameInputFilter` caps at `TONE_NAME_LENGTH`, correctly) but the
pre-fill: the field was seeded with the model's fixed-width, space-padded
storage form, so part of the 8-character budget was already spent on characters
the user cannot see.

The fix is at the boundary the padding crosses — the UI strips exactly what the
model writes — and nothing about the wire representation moves.

## References
- **Requirements**: RQ-GUI-081, with RQ-GUI-025 (character set, length,
  dialog workflow), RQ-MOD-023 (fixed-width stored name) and RQ-TST-001
  constraining it
- **ADRs**: None. Tier M, contained to one module: a new pure function in the
  dialogs translation unit, its single call site, and a named constant moved
  beside the length it pads to. No cross-cutting choice, no new dependency, no
  new public surface beyond the tested helper.
- **Design system**: not applicable, and this is deliberate — the change is the
  textual content of a text field, not a colour, spacing, metric or typographic
  value, so the design system owns none of it (RQ-DSN §2). Recorded here so the
  absence reads as a decision, not an omission.

## Session variables
`unit_tests: true`, `platform: windows`, `chat_mode: chat-eco`.

---

## Tasks

### TASK-GUI-053: Strip the stored padding before pre-filling the rename field
- **Tier**: M
- **Status**: Done
- **Description**: Add `patchNameForEditing()` to the dialogs module — a pure
  right-trim of the model's padding character — and use it where
  `showRenameDialog` seeds its `AlertWindow` text editor. Name the padding
  character as `model::constants::TONE_NAME_PADDING_CHAR` beside
  `TONE_NAME_LENGTH` and consume it from both `XpanderTone::setToneName` (which
  writes it) and the new helper (which strips it), so the two cannot drift.
  `PatchNameInputFilter`, `isPatchNameValid` and the accept path are untouched.
- **Requirement refs**: RQ-GUI-081, RQ-GUI-025, RQ-MOD-023, RQ-TST-001
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - *Given* the stored name `BRASS   `, *When* the editable form is computed,
    *Then* it is `BRASS` and three characters of budget are recovered.
  - *Given* the stored name `STRING  ` — the reported case — *When* the editable
    form is computed, *Then* it is `STRING`, leaving room for exactly two more
    characters, eight in total.
  - *Given* a stored name filling the whole width, *When* the editable form is
    computed, *Then* it is returned unchanged.
  - *Given* a stored name with a leading or interior space, *When* the editable
    form is computed, *Then* only the trailing padding is removed.
  - *Given* a stored name that is nothing but padding, *When* the editable form
    is computed, *Then* it is empty.
  - *Given* any editable form, *When* it is validated, *Then* `isPatchNameValid`
    still accepts it.
- **Dependencies**: None
- **Assignee**: AI

---

## Verification record (2026-08-30)

| Check | Result |
|---|---|
| `cmake --build juce/build-win-local --config Release` (all targets) | clean — no error, no warning |
| `ctest -C Release` | **180/180 passed**, 0 failed (179 before, +1 new scenario) |
| New scenarios (`[RQ-GUI-081]`) | 1 case, 6 GIVENs, 12 assertions, all passed |
| Pre-existing tests modified | **none** |

## Open for owner verification
- The gesture end to end in the running application: open the rename dialog on
  a short-named patch, click in the field rather than typing straight away, and
  confirm eight characters can now be entered. No test drives a real
  `TextEditor`'s caret and mouse, so the click path itself is reasoned about
  (JUCE's `mouseDown` clearing the select-all) rather than executed.
