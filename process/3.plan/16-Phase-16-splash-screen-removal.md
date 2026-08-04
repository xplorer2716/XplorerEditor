# PLAN-GFX-002: Removal of the Splash Screen

## Overview

Remove the startup splash screen. Implements RQ-GUI-055 and ADR-JUC-030
(DEC-JUC-092/093).

Owner report (2026-08-04, session GFX, Windows): for about one second at launch,
two panels are visible superimposed, at different sizes and colours. Root cause
(confirmed by reading `Main.cpp`): the `juce::SplashScreen` (1260×786, default
palette) and the `MainWindow` (1440×922, user palette) are both constructed in
`initialise()` and both stay on screen for the length of the splash's fixed 2 s
`deleteAfterDelay`. The mismatch cannot be fixed by re-timing or resizing the
splash: the splash paints the same function as the main panel
(`paintVectorBackground`, ADR-JUC-013), and that panel has been resizable
(RQ-SCL-001/002) and user-themeable (RQ-GUI-046, RQ-DSN-095) since two later
ADRs — a panel-mirroring splash cannot match the window that follows it at any
size or any delay. Full reasoning in ADR-JUC-030.

**Decision taken: remove the splash outright**, not re-time or realign it. See
ADR-JUC-030's Alternatives Considered for why the two lighter options (dismiss on
ready; align size/palette) were rejected.

## References

- **Requirements**: RQ-GUI-055 *(new — the removal)*; RQ-GUI-025 *(splash item
  struck)*; RQ-GUI-005, RQ-GUI-037, RQ-GUI-046, RQ-SCL-001, RQ-SCL-002,
  RQ-DSN-095, RQ-FMW-072, RQ-GUI-035 *(must survive untouched)*
- **ADRs**: ADR-JUC-030 *(this plan implements it — DEC-JUC-092, DEC-JUC-093)*;
  ADR-JUC-013 *(partially superseded: the splash paragraph)*, ADR-JUC-020
  *(Consequences note corrected)*, ADR-JUC-025 *(window sizing — untouched)*

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = chat-eco`.
Branch: `feature/GFX`.

## What must NOT change

- **The main window itself.** Launch size (`applyWindowScale(*this, 1.0F)`),
  resizability, native title bar, `closeButtonPressed`, single-instance guard
  (`moreThanOneInstanceAllowed`, RQ-FMW-072), `unhandledException` handling
  (RQ-GUI-035) — none of this is touched.
- **`paintVectorBackground`'s signature.** It keeps its `palette` parameter even
  though `MainComponent::paint` becomes its only caller — that parameter is the
  headless-test seam ADR-JUC-020 (DEC-JUC-036) established, reused by
  `BackgroundRendererTests`. See ADR-JUC-030, DEC-JUC-093.
- **`defaultBlockPalette()` and `BlockPalette.hpp`.** Still used elsewhere
  (e.g. `PageFamilyBlock.cpp`'s fallback when no `XplorerLookAndFeel` is found).
  Only the splash's *call* to `defaultBlockPalette()` goes.

## Verification strategy — why no new unit test

Tier M, `session.unit_tests = true`: the rule is one unit test per generated
function or method. This task deletes code inside `JUCEApplication::initialise()`,
which has no headless entry point (`Main.cpp` is excluded from every test target —
confirmed: no test file references `XplorerApplication` or `MainWindow`). There is
nothing new to unit-test. Verification is:

- **The compiler**, for the two includes/calls this removal makes unused.
- **A grep**, per the RQ-DSN-071 DoD pattern: no `SplashScreen` reference left in
  `juce/app/src`.
- **Manual launch**, against RQ-GUI-055's Gherkin — only one window ever appears.
- **The existing suite stays green**, unmodified — it does not touch `Main.cpp`,
  so an unrelated regression here would be a signal the removal reached further
  than intended.

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GFX-004 | Remove the `SplashScreen` construction, its image and its `paintVectorBackground` call from `Main.cpp` | M | RQ-GUI-055, DEC-JUC-092 |

Single task: the change is confined to one function in one file.

---

### TASK-GFX-004: Remove the splash screen from `XplorerApplication::initialise()`
- **Tier**: M
- **Status**: **Done** — commit 3d69077. Splash image, paint call and
  `SplashScreen` deleted; `BackgroundRenderer.hpp` and
  `xplorer/app/ControlTable.hpp` includes removed from `Main.cpp` (both had
  become unused). App launched (Debug) and closed cleanly. Full rebuild clean;
  all 6 suites green, 0 test modified: `xpl_tests_app_juce` 314/314,
  `xpl_tests_app` 2014/2014, `xpl_tests_framework` 123/123, `xpl_tests_model`
  333/333, `xpl_tests_midi` 68/68, `xpl_tests_settings` 31/31,
  `xpl_tests_controller` 99/99.
- **Description**: In `Main.cpp`'s `initialise()`, delete the splash `juce::Image`
  construction, its `paintVectorBackground(splashGraphics, defaultBlockPalette())`
  paint block, the `new juce::SplashScreen(...)->deleteAfterDelay(...)` call and
  the two comments framing them, leaving only
  `_window = std::make_unique<MainWindow>();`. Remove the `#include`s that become
  unused as a result (`BackgroundRenderer.hpp` for `paintVectorBackground`,
  `BlockPalette.hpp` for `defaultBlockPalette` — verify against what
  `MainComponent.hpp`, already included, re-exposes before deleting either).
  No change to `MainWindow`, `XplorerApplication`'s other overrides, or any file
  outside `Main.cpp`.
- **Requirement refs**: RQ-GUI-055, RQ-GUI-005, RQ-GUI-035, RQ-FMW-072
- **ADR refs**: ADR-JUC-030 (DEC-JUC-092); supersedes the splash paragraph of
  ADR-JUC-013
- **Acceptance Criteria** (Gherkin):
  - *Given* the application is launched, *When* the first window appears,
    *Then* it is the main window at `windowSizeForScale(1.0)` (1440×922) and no
    other window was shown before or alongside it.
  - *Given* a user with customised block colours, *When* the application starts,
    *Then* no panel is ever painted in the default palette.
  - *Given* `juce/app/src`, *When* searched for `SplashScreen`, *Then* there is
    no match.
  - *Given* the build, *When* it runs, *Then* it compiles with no warning
    (no unused-include diagnostic) and the full suite passes with no test
    modified.
- **Dependencies**: None
- **Assignee**: AI

---

## Definition of Ready

- [x] Task has a description, Gherkin acceptance criteria and a tier.
- [x] Task references its requirement and ADR IDs.
- [x] No visual literal introduced — this task only removes code.
- [x] **Owner approval** — granted 2026-08-04, session GFX.
- [x] Task Done; full test suite green; ADR-JUC-030 ready to move from Proposed
      to Accepted.
