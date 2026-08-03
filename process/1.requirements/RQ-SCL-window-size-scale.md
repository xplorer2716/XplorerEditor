# RQ-SCL — Window Size & Scale Management

Scope: the application's reference (1x) window size, user-driven scaling
through a **View** menu (preset ratios + full screen), and the device-pixel
alignment the VFD display needs to stay identical to itself at any of those
scales. Sits on top of the free-resize behaviour already specified by
RQ-GUI-005/RQ-GUI-037.

---

## Functional Requirements

### RQ-SCL-001: Reference (1x) window size

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The application's **1x display scale SHALL be a main-window
  content width of 1440 logical px**, and on launch, with no window geometry
  previously persisted, the main window SHALL open at that 1x size. The
  height SHALL be derived from the width so the logical canvas keeps its
  aspect ratio (RQ-GUI-037) — the width is the authority — giving **1440×922**
  (898 px of canvas plus the 24 px menu-bar strip).
- **Rationale**: the JUCE build launches at 1260 px wide, which the owner
  reports as markedly narrower than the .NET editor they are used to; that
  editor was measured launching at ≈1473 px (2026-08-02). 1440 was chosen
  over that measurement because it clears a 1920×1080 display with room to
  spare while still reading as the familiar size.
- **Priority**: Must
- **Notes on provenance — why 1440 is not "1260 scaled up":** the reference's
  nominal `MainForm.ClientSize` width is 1260 (the value `LOGICAL_CANVAS_WIDTH`
  was derived from), but WinForms DPI-autoscaled the form at launch
  (`docs/architecture-analysis.md` row 17), so what the user actually saw was
  wider and **display-dependent**. There was therefore no display-independent
  default to recover. The application defines its own 1x directly as a width:
  the canvas's 1260×786 grid stays what it has always been — the *coordinate*
  space the extracted control table is expressed in (ADR-JUC-006) — while 1x
  names a *window* size. The two meet only in the uniform render transform
  RQ-GUI-005 already performs, and no ratio between them is computed or named
  anywhere else.
- **Acceptance Criteria** (Gherkin):
  - *Given* no prior window geometry is stored, *When* the application
    launches, *Then* the main window's content size is exactly 1440×922
    logical px.
  - *Given* that launch, *When* the View menu is opened, *Then* the **1x**
    entry is the checked one.
  - *Given* that launch, *When* the canvas is measured, *Then* it fills the
    content area below the menu bar with no perceptible letterbox band.
- **Dependencies**: RQ-GUI-001, RQ-GUI-005, RQ-GUI-037; ADR-JUC-006,
  ADR-JUC-013, ADR-JUC-025

---

### RQ-SCL-002: View menu — scale presets

- **Category**: Functional
- **EARS Type**: Event-driven
- **Statement**: WHEN the user selects a scale entry (**1x, 1.25x, 1.5x,
  1.75x or 2x**) from a **View** menu — placed in the menu bar between
  **Patch** and **Tools** (owner decision, 2026-08-02) — the application
  SHALL resize the main window to that multiple of the 1x size of
  RQ-SCL-001, preserving the canvas aspect ratio:

  | Preset | Content size | Canvas render scale |
  |---|---|---|
  | 1x | 1440 × 922 | ≈1.143 |
  | 1.25x | 1800 × 1147 | ≈1.429 |
  | 1.5x | 2160 × 1371 | ≈1.714 |
  | 1.75x | 2520 × 1596 | **exactly 2.000** |
  | 2x | 2880 × 1821 | ≈2.286 |

- **Rationale**: a mouse-dragged resize gives no repeatable geometry; a menu
  preset gives a stated one on demand, and lets the user step through sizes
  without hunting for a window edge.
- **Priority**: Must
- **Note — the presets are about *size*, not about pixel-grid alignment.**
  Only 1.75x happens to land on an exact canvas scale (2520 = 2 × 1260). The
  VFD's crispness at the other four is delivered by RQ-SCL-004, in the
  renderer, **not** by choosing convenient window sizes — which is why the
  ratios could be chosen for ergonomics alone.
- **Note — reach of the upper presets.** 1x fits a 1920×1080 display; 1.25x
  and 1.5x need ≈1440 px of height; 1.75x needs more than a 2560×1440 panel
  offers (1596 px of content height) and 2x targets 4K. **WHEN the selected
  preset does not fit the display the window is on, the application SHALL
  switch to full screen instead** of opening an oversized window that spills
  off the visible area — full screen is the closest available equivalent to
  "as large as this preset asks for". This is a stated behaviour, not a
  fallback left to the OS: an oversized `centreWithSize` is not guaranteed to
  be clamped or kept fully on-screen.
- **Acceptance Criteria** (Gherkin):
  - *Given* the main window is open, *When* the menu bar is read left to
    right, *Then* the order is File, Patch, View, Tools, Help.
  - *Given* the View menu is opened, *Then* five scale entries (1x, 1.25x,
    1.5x, 1.75x, 2x) are listed alongside Full Screen (RQ-SCL-003).
  - *Given* the user selects the 1.5x entry, *When* the window resizes,
    *Then* the content size is exactly 2160×1371 logical px.
  - *Given* the window is at one of the five preset sizes, *When* the View
    menu is opened, *Then* that entry is shown as checked and no other is.
  - *Given* the window was resized by dragging, *When* the View menu is
    opened, *Then* no scale entry is shown as checked.
  - *Given* a display too small for the selected preset (content size larger
    than the display's usable area in either axis), *When* that preset is
    selected, *Then* the application enters full screen instead of resizing
    to the oversized preset.
  - *Given* the window already full screen because a preset did not fit,
    *When* the View menu is opened, *Then* **Full Screen** is checked and no
    scale entry is.
  - *Given* the window full screen for any reason, *When* a preset that DOES
    fit the display is selected, *Then* the application leaves full screen
    and resizes to exactly that preset.
- **Dependencies**: RQ-SCL-001, RQ-SCL-003, RQ-SCL-004, RQ-GUI-005,
  RQ-GUI-008, RQ-GUI-033, RQ-GUI-037; ADR-JUC-023, ADR-JUC-025

---

### RQ-SCL-003: View menu — Full Screen

- **Category**: Functional
- **EARS Type**: Event-driven
- **Statement**: WHEN the user selects **Full Screen** from the View menu,
  the application SHALL toggle the main window between full-screen display
  and its previous windowed size and position, preserving the uniform-scaling
  behaviour of RQ-GUI-005.
- **Rationale**: the largest available canvas without hand-matching a screen
  resolution to a preset.
- **Priority**: Must
- **Note**: the logical canvas is 1260:786 ≈ 1.603:1 and a 16:9 display is
  1.778:1, so full screen on 16:9 is **height-constrained** and leaves black
  bands at the left and right (≈145 px each side on 2560×1440). This is
  accepted by the owner and is **not** a defect: preserving the aspect ratio
  is what RQ-GUI-005 requires. The resulting canvas scales are fractional
  (≈1.34 / 1.80 / 2.72 at 1080p / 1440p / 2160p), which is exactly the case
  RQ-SCL-004 exists to handle.
- **Acceptance Criteria** (Gherkin):
  - *Given* the main window is windowed at any size, *When* Full Screen is
    selected, *Then* the window covers the full display and the canvas scales
    uniformly to fill it, aspect ratio preserved.
  - *Given* the window is full-screen, *When* Full Screen is selected again,
    *Then* the window returns to exactly the size and position it had before.
  - *Given* full screen on a 16:9 display, *When* the VFD is inspected,
    *Then* it satisfies RQ-SCL-004 — every occurrence of a character is
    rendered identically.
- **Dependencies**: RQ-SCL-002, RQ-SCL-004, RQ-GUI-005, RQ-GUI-008;
  ADR-JUC-025

---

### RQ-SCL-004: VFD glyph grid on whole device pixels

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The VFD display SHALL lay out its glyph cells on **whole
  device pixels** at every render scale, so that every occurrence of a given
  character rasterises identically — within a line, between lines, and
  between window sizes. The glyph block SHALL be drawn to a device rectangle
  whose size equals the rendered image's pixel size exactly, with no
  resampling.
- **Rationale**: at fractional canvas scales the same character currently
  renders slightly differently from column to column, which reads as an
  uneven, "dirty" display. Full screen on every common 16:9 resolution is
  such a case (RQ-SCL-003), so this is the normal situation, not an edge one.
- **Priority**: Must
- **The defect, precisely.** `VfdSegmentRenderer::renderBlock` places cell
  origins in floating point at `column × CELL_WIDTH × scale` and
  `row × CELL_HEIGHT × scale`. The cell is 12×16 logical px, so the grid
  falls on whole device pixels only when `12 × scale` **and** `16 × scale`
  are both integers — i.e. only when `scale` is a multiple of **0.25**
  (`pgcd(12,16) = 4`). Otherwise each column starts at a different sub-pixel
  phase and its glyph is rasterised with slightly different stroke placement
  and weight.
  - **This is not a resolution loss.** The renderer already rasterises at the
    true device scale via `getPhysicalPixelScaleFactor()` (ADR-JUC-023,
    DEC-JUC-053), so no detail is missing and nothing is magnified — the
    sprite-sheet problem RQ-GUI-033 was amended to remove stays removed. What
    remains is an **irregularity of phase**, and it is what the eye picks up
    as unevenness across a line of text.
  - Two secondary contributors: the block image's pixel size is rounded
    (`roundToInt`) while its contents were laid out unrounded, and the final
    `drawImage` targets a fractional device rectangle. The existing code notes
    the latter as "<0.1%, invisible side by side" — but that figure measures
    **size** error, not the softening of a 1 px stroke displaced by half a
    pixel. The two are different claims and only the first was measured.
- **The glyph block SHALL never be larger than its nominal size**, so it
  cannot overflow the glass onto the bezel: the glass carries about three
  logical pixels of slack over the grid it holds, so the snap SHALL round
  **down**, not to nearest. The resulting block is under one device pixel per
  cell smaller than nominal, absorbed as centred black margin.
- **Glyph proportion is not at risk and SHALL NOT constrain the snap.** The
  renderer derives glyph geometry from the cell **width** alone (segment paths
  are width-normalised with `y` running `0..4/3`), so the drawn shape is
  exactly 3:4 whatever the cell height is; the height is a row pitch, not a
  drawing dimension. The two axes may therefore be snapped independently.
- **Acceptance Criteria** (Gherkin):
  - *Given* any render scale, *When* the glyph block is laid out, *Then* every
    cell origin and every cell dimension is a whole number of device pixels.
  - *Given* a line containing the same character more than once, *When* it is
    rendered at a fractional canvas scale (e.g. full screen on 2560×1440),
    *Then* each occurrence is pixel-identical to the others.
  - *Given* the rendered block, *When* it is drawn, *Then* its destination
    device rectangle matches the image's pixel dimensions exactly, so no
    resampling filter is applied.
  - *Given* the snapped cell, *When* it is compared with its nominal size,
    *Then* it is never larger, and smaller by less than one device pixel, so
    the block always fits inside the glass.
  - *Given* the launch geometry, *When* the display is measured, *Then* the
    grid is still 22×5 and every existing VFD scenario (RQ-GUI-020,
    RQ-GUI-033, RQ-GUI-049) still passes.
- **Dependencies**: RQ-GUI-020, RQ-GUI-033, RQ-GUI-049, RQ-GUI-050,
  RQ-SCL-003, RQ-DSN-097; ADR-JUC-023, ADR-JUC-024, ADR-JUC-026 *(to be
  written — the renderer amendment)*
