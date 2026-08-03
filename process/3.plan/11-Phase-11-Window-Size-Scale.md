# PLAN-SCL-001: Window Size, Scale Menu and VFD Device-Pixel Grid

## Overview

Three related pieces of work:

1. make the main window's launch size deterministic and equal to a stated
   **1x = 1440×922** (RQ-SCL-001) instead of the implicit `1260×810` it falls
   into today;
2. add a **View** menu with the presets 1x / 1.25x / 1.5x / 1.75x / 2x plus
   Full Screen (RQ-SCL-002, RQ-SCL-003);
3. snap the **VFD glyph grid to whole device pixels** so the display renders
   identically at every scale (RQ-SCL-004).

Items 1–2 implement ADR-JUC-025; item 3 implements ADR-JUC-026. They are
independent: either could ship without the other. Item 3 is what actually
delivers the "better VFD rendering" the View menu was originally expected to
provide — the presets turned out not to guarantee it (only 1.75x lands on an
exact canvas scale), which is why it became its own requirement and ADR.

## References

- **Requirements**: RQ-SCL-001, RQ-SCL-002, RQ-SCL-003, RQ-SCL-004;
  RQ-GUI-005, RQ-GUI-008, RQ-GUI-020, RQ-GUI-033, RQ-GUI-037, RQ-GUI-049
- **ADRs**: ADR-JUC-025 *(tasks 001–003, 006)*, ADR-JUC-026 *(tasks 004)*;
  ADR-JUC-006 (control-table geometry), ADR-JUC-013 (vector background),
  ADR-JUC-023 (the renderer being amended), ADR-JUC-024 (bezel geometry,
  untouched)

Session state: `unit_tests = true`, `platform = windows`,
`chat_mode = chat-eco`.

**DoR — presented for owner approval before any task starts.**

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-SCL-001 | `windowSizeForScale()` + deterministic 1x launch size | M | RQ-SCL-001, DEC-JUC-063/064/068 |
| TASK-SCL-002 | View menu: five scale presets with active-size checkmark | M | RQ-SCL-002, DEC-JUC-065/066 |
| TASK-SCL-003 | View menu: Full Screen toggle | S | RQ-SCL-003, DEC-JUC-067 |
| TASK-SCL-004 | Snap the VFD glyph grid to whole device pixels | M | RQ-SCL-004, DEC-JUC-069/070/071 |
| TASK-SCL-005 | Verify live: launch size, five presets, full screen, VFD | S | RQ-SCL-001/002/003/004 |

---

### TASK-SCL-001: `windowSizeForScale()` + deterministic 1x launch size
- **Tier**: M
- **Status**: **Done** — suite 110/110, build warning-clean. `MENU_BAR_HEIGHT`
  now has one definition instead of the two literal `24`s; `WindowGeometryTests`
  pins the five preset sizes as numbers rather than as a re-derivation of the
  formula, so a change to the arithmetic has to face the sizes RQ-SCL-002
  promises the user.
- **Description**: Add `WINDOW_WIDTH_AT_1X = 1440`, `MENU_BAR_HEIGHT = 24` and
  a pure `windowSizeForScale(float scale)` to `xplorer/app/ControlTable.hpp`
  (headless `xpl_app_core`), returning
  `{round(scale × 1440), round(width × 786/1260) + 24}`. `Main.cpp`'s
  `MainWindow` constructor calls it with `1.0F` and applies the result via
  `centreWithSize`, with `setContentOwned(..., false)`; drop the `setSize` in
  `ScaledCanvasComponent`'s constructor and the two hard-coded `24`s in
  `MainComponent.cpp`.
- **Requirement refs**: RQ-SCL-001
- **ADR refs**: ADR-JUC-025 (DEC-JUC-063, DEC-JUC-064, DEC-JUC-068)
- **Acceptance Criteria**:
  - *Given* `windowSizeForScale(1.0F)`, *When* called, *Then* it returns
    `{1440, 922}`.
  - *Given* the other four presets, *When* called, *Then* they return
    `{1800, 1147}`, `{2160, 1371}`, `{2520, 1596}`, `{2880, 1821}`.
  - *Given* the function, *When* unit-tested, *Then* no JUCE window or
    component is instantiated — it is pure arithmetic in the headless layer.
  - *Given* no prior window geometry, *When* the application launches, *Then*
    the main window's content size is 1440×922.
  - *Given* the sources, *When* grepped, *Then* no literal `24` menu-bar
    height and no second statement of the launch size remain.
- **Dependencies**: None
- **Assignee**: AI

### TASK-SCL-002: View menu — five scale presets with active-size checkmark
- **Tier**: M
- **Status**: **Done** — suite 110/110, build warning-clean. Inserting `View`
  mid-bar shifted the `getMenuForIndex` cases (Tools 2→3, Help 3→4) but left
  `menuItemSelected` untouched, which is the property DEC-JUC-065 relied on:
  dispatch is by item id, not by menu index.
  - **Added mid-task, owner-caught:** the original `applyWindowScale` called
    `centreWithSize` unconditionally, with nothing stopping a preset from
    producing a window larger than the display — `centreWithSize` does not
    clamp to the screen, and an earlier plan/ADR draft had assumed the OS
    would. Fixed per DEC-JUC-073: `applyWindowScale` now checks the preset
    against `Desktop::getDisplays().getDisplayForRect(...)->userArea` and
    calls `setFullScreen(true)` instead of resizing when the preset would
    overflow either axis; it also clears an existing full-screen state before
    resizing to a preset that does fit.
- **Description**: Insert `"View"` into `MainComponent::getMenuBarNames()`
  between `"Patch"` and `"Tools"` (order: File, Patch, View, Tools, Help);
  build its `PopupMenu` in `getMenuForIndex()` with the five scale items;
  handle them in `menuItemSelected()` via `applyWindowScale()`, which calls
  `windowSizeForScale()` and either re-centres the top-level window at that
  size or, if it would not fit the display, enters full screen instead
  (DEC-JUC-073). Tick the item whose `windowSizeForScale(preset).width`
  equals the window's current content width — an integer comparison, no
  stored "current scale" state.
- **Requirement refs**: RQ-SCL-002
- **ADR refs**: ADR-JUC-025 (DEC-JUC-065, DEC-JUC-066, DEC-JUC-073)
- **Acceptance Criteria**:
  - *Given* the main window, *When* the menu bar is read left to right,
    *Then* the order is File, Patch, View, Tools, Help.
  - *Given* the View menu is opened, *Then* five scale entries (1x, 1.25x,
    1.5x, 1.75x, 2x) are listed.
  - *Given* the user selects 1.5x, *When* the window resizes, *Then* its
    content size is exactly 2160×1371.
  - *Given* the window is at a preset size, *When* the View menu is opened,
    *Then* that entry is checked and no other is.
  - *Given* the window at a non-preset width, *When* the View menu is opened,
    *Then* no entry is checked.
  - *Given* a preset larger than the display's usable area, *When* it is
    selected, *Then* the application enters full screen instead of resizing
    off-screen, and the View menu subsequently shows Full Screen checked.
  - *Given* the window full screen for that reason, *When* a preset that DOES
    fit is selected, *Then* full screen is exited and the window resizes to
    exactly that preset.
  - *Given* the preset-matching predicate, *When* unit-tested headlessly at
    each preset width and one pixel either side, *Then* it matches only at
    the exact width.
- **Dependencies**: TASK-SCL-001
- **Assignee**: AI

### TASK-SCL-003: View menu — Full Screen toggle
- **Tier**: S
- **Status**: **Done** — suite 110/110, build warning-clean. No bounds are
  saved by this code; `setFullScreen(!isFullScreen())` delegates entirely to
  `ResizableWindow`, per DEC-JUC-067.
- **Description**: Add a "Full Screen" item to the View menu calling
  `juce::ResizableWindow::setFullScreen()` on the top-level window, toggled
  from its current state.
- **Requirement refs**: RQ-SCL-003
- **ADR refs**: ADR-JUC-025 (DEC-JUC-067)
- **Acceptance Criteria**:
  - *Given* a windowed main window, *When* Full Screen is selected, *Then*
    the window covers the display and the canvas scales uniformly to fill it.
  - *Given* a full-screen window, *When* Full Screen is selected again,
    *Then* it returns to exactly its prior size and position.
- **Dependencies**: TASK-SCL-002
- **Assignee**: AI

### TASK-SCL-004: Snap the VFD glyph grid to whole device pixels
- **Tier**: M
- **Status**: **Done** — suite 110/110, build warning-clean, owner visually
  accepted 2026-08-03.
  - **The plan's own formula was wrong and a failing test caught it.** This
    task originally specified `cellWidth = max(3, round(12 × scale / 3) × 3)`
    with the height derived as `4/3` of it, to keep the cell in exact 3:4.
    Two defects, both found on the first run rather than by inspection:
    - *the constraint was unnecessary* — `paintGlyph` receives only the cell
      **width** and draws in width-normalised units with `y ∈ 0..4/3`, so the
      glyph's shape is 3:4 whatever the cell height. The height is a row
      pitch. The two axes can be snapped independently with no distortion;
    - *rounding to nearest overflows the glass* — at the 1x canvas scale
      (1440/1260 ≈ 1.143) the quarter-step rule rounded **up** to 1.25,
      giving a 330 px block inside a 305 px glass, so the outermost glyphs
      would have been drawn under the bezel band. The glass carries only ~3
      logical px of slack over its grid.
  - **Delivered instead:** `cellWidth = max(1, floor(12 × scale))` and
    `cellHeight = max(1, floor(16 × scale))`, independent per axis. Floor, so
    the block can only ever be *smaller* than nominal — by under one device
    pixel per cell — and can never overflow. Finer than the withdrawn rule,
    which quantised the scale to multiples of 1/4 (a 25 % relative jump at
    the low end).
  - **One pre-existing test was amended**, and it is the kind of change the
    process is right to be suspicious of, so it is recorded here explicitly:
    `"A fractional render scale produces a correctly sized block"` required
    the block to stay within half a pixel of `columns × 12 × scale` —
    precisely the *absence* of this snap, so it could not survive the change
    either way. It now asserts the new one-sided contract: never larger than
    nominal, and within one device pixel per cell of it. Stricter in the
    direction that now matters (overflow), not loosened to pass.
- **Description**: In `VfdSegmentRenderer::renderBlock`, replace the float
  cell metrics with the whole-device-pixel `cellWidthForScale()` /
  `cellHeightForScale()` (floored, clamped to 1), so cell origins and the
  image size are integers. In `DisplayPanel::paint`, size the destination
  rectangle from the returned image's pixel dimensions and round the centring
  offset in device space before converting back to logical, so the blit is
  1:1 with no resample. The glow radius and the normalised segment paths
  follow automatically — both are already expressed relative to `cellWidth`.
- **Requirement refs**: RQ-SCL-004, RQ-GUI-033, RQ-GUI-049
- **ADR refs**: ADR-JUC-026 (DEC-JUC-069, DEC-JUC-070, DEC-JUC-071)
- **Acceptance Criteria**:
  - *Given* any scale, *When* the cell metrics are computed, *Then* both are
    whole numbers of device pixels.
  - *Given* any scale, *When* the cell is compared with its nominal size,
    *Then* it is never larger, and smaller by less than one device pixel.
  - *Given* a deliberately fractional scale (e.g. 1.8015), *When* a line
    containing the same character several times is rendered, *Then* the
    rasters of those cells are pixel-identical to one another.
  - *Given* the rendered block, *When* it is drawn, *Then* the destination
    device rectangle equals the image's pixel dimensions, so no resampling
    filter is applied.
  - *Given* the launch geometry, *When* the display is measured, *Then* the
    grid is still 22×5 and `SegmentFontTests` passes unmodified.
  - *Given* a scale small enough to floor to zero, *When* the cell is
    computed, *Then* it clamps to 1×1 rather than collapsing.
- **Dependencies**: None *(independent of TASK-SCL-001..003)*
- **Assignee**: AI

### TASK-SCL-005: Verify live
- **Tier**: S
- **Status**: **Done** — owner built and ran the application, inspected the
  VFD at several window sizes, and accepted the result as visually clean
  (2026-08-03) — including the DEC-JUC-072 residual (the display's own
  fractional device position), inspected and accepted knowingly rather than
  unnoticed.
  - **What this covers, precisely, since "verified live" can mean different
    things and this plan has been burned by that gap before (TASK-DSP-005 /
    007's own post-implementation notes).** Automated: build is
    warning-clean, suite is 110/110, `WindowGeometryTests` pins the five
    preset sizes as numbers. Owner-observed: the VFD reads as acceptably
    sharp across the sizes tried. **Not separately done:** a pixel-probe
    measurement of the default 1440×922 launch size, of each preset's exact
    on-screen dimensions, or of full-screen behaviour on a specific 16:9
    panel — the owner's pass was a direct visual judgement, not an
    instrumented one. If a future session needs the exact numbers (e.g. to
    debug a regression), that measurement still has to be taken; it was not
    substituted here.
- **Description**: Build, launch, and measure: the default window size, each
- **Description**: Build, launch, and measure: the default window size, each
  of the five presets, Full Screen on the owner's display, and the VFD at a
  fractional scale (full screen 16:9) before and after TASK-SCL-004. Confirm
  the launch width reads as equivalent to the .NET editor the owner
  remembers, and record the residual whole-block offset DEC-JUC-072 leaves in
  place, so a later reader knows it was looked at rather than missed.
- **Requirement refs**: RQ-SCL-001, RQ-SCL-002, RQ-SCL-003, RQ-SCL-004
- **ADR refs**: ADR-JUC-025, ADR-JUC-026
- **Acceptance Criteria**:
  - *Given* the built application, *When* launched with no prior geometry,
    *Then* the measured content size is 1440×922 and the View menu shows 1x
    checked.
  - *Given* each preset in turn, *When* selected, *Then* the measured content
    size matches the table in RQ-SCL-002, or the entry is unchecked because
    the OS clamped it.
  - *Given* full screen on a 16:9 display, *When* the VFD is inspected at
    pixel level, *Then* repeated characters are identical.
  - *Given* the build, *When* it completes, *Then* it is warning-clean and
    the existing suite still passes.
- **Dependencies**: TASK-SCL-001, TASK-SCL-002, TASK-SCL-003, TASK-SCL-004
- **Assignee**: AI
