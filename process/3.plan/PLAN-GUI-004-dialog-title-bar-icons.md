# PLAN-GUI-004: Dialog Title-Bar Icons

## Overview
Replace the OS-generic title-bar icon on the Settings, About and Dependencies
windows with hand-authored vector glyphs, in the same technique the app's
other icons already use.

## References
- **Requirements**: RQ-GUI-070
- **ADRs**: ADR-GUI-001 (referenced, not amended — no new decision)
- **Design system**: RQ-DSN-061 (LookAndFeel reads tokens)

---

## Tasks

### TASK-GUI-026: Gear and info-circle icons, wired to the three windows
- **Tier**: M
- **Status**: Done (2026-08-15)
- **Description**: New `DialogIcons.hpp/.cpp` (`DialogIcon::Settings/About`,
  `dialogTitleBarIcon()`), two new tokens (`dialogIconAccent`,
  `dialogIconSize`), and a `setIcon()` call at each of the three
  `DialogWindow::LaunchOptions::launchAsync()` sites
  (`SettingsDialog.cpp::showSettingsDialog`, `Dialogs.cpp::showAboutDialog`,
  `Dialogs.cpp::showDependenciesWindow` — the last reusing the Settings gear).
- **Requirement refs**: RQ-GUI-070
- **ADR refs**: ADR-GUI-001 (referenced)
- **Acceptance Criteria** (Gherkin):
  - **Given** each of the three windows, **When** opened, **Then** its title
    bar shows the intended glyph (gear / gear / circled "i") rather than the
    OS default
  - **Given** the rendered icon image, **When** a pixel outside the glyph is
    sampled, **Then** it is exactly `component.dialogIconAccent`
  - **Given** Settings and About rendered at the same size, **When**
    compared pixel-by-pixel, **Then** at least one pixel differs
  - **Given** the source tree, **When** searched for a raw colour/size
    literal on these icons, **Then** none exists outside `DesignTokens.hpp`
- **Dependencies**: None
- **Assignee**: AI

---

## Verification note
Rendered geometry verified two ways: `DialogIconTests.cpp` (compiled clean
under `xpl_tests_app_juce`, `-Werror`; the target itself does not link in
this container — two pre-existing, unrelated failures, `BoundRadioGroupTests`
and JUCE's vendored SheenBidi, per PLAN-GUI-001's verification note — so it
runs in CI on macOS/Windows, not here); and a throwaway console program
(written, run, deleted — not part of the deliverable) that called
`dialogTitleBarIcon()` directly and wrote both icons to PNG for visual
review, since this container has no window manager and a native title bar
never renders under bare Xvfb, unlike the on-canvas icons verified in earlier
GUI-session tasks by screenshotting the running app.

**Correction, 2026-08-15 (TASK-GUI-028, PLAN-GUI-005).** "Compiled clean" was not
enough, and the gap showed the same day. `DialogIconTests` ran for the first time
during TASK-GUI-027 — which had reason to exclude the unrelated
`BoundRadioGroupTests.cpp` and so got the target to link — and one assertion
failed immediately: it sampled pixel (2, 2), a CORNER of the square image, and
required the badge accent there. The badge is a circle inscribed in that square,
so the corner is the one place that colour can never be. The icon was right and
the test was wrong; the test is corrected, not relaxed. Recorded here rather than
edited into the note above, because the note's claim — that compilation was the
only verification available at the time — was true, and this is what it cost.

## Out of scope
Every other window's icon (there are none — the main window and every other
dialog already inherit whatever the OS resolves, unreported by the owner).
The three windows' content, size, and behaviour.
