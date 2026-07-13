# ADR-007: VFD Bitmap Glyph Rendering — Direct Sprite-Sheet Paint, JUCE-Cached

## Status
Accepted (owner, 2026-07-13 — option (b) of the open point)

## Requirements
RQ-GUI-020, RQ-GUI-031, RQ-GUI-033

## Context
The reference `VacuumFluoDisplayControl` (MidiApp.UIControls) renders the VFD
as bitmap glyphs, not font text: a single sprite sheet (`MATRIXTINY`, a 384×48
24-bpp BMP embedded in `Properties/Resources.resx`) holds 96 glyphs of
**12×16 px** in a 32×3 grid covering ASCII 32–126; a character `c` maps to
column `(c-32) % 32`, row `(c-32) / 32`, and anything outside 32–126 renders
as a space. The owner reports having recovered and then optimized this
implementation for performance; its mechanics are:

- an **offscreen buffer bitmap** (grid width × grid height) plus a kept-alive
  `Graphics` on it;
- `SetText` early-outs on identical text, pads to the full grid, invalidates;
- `OnPaint` **diffs the new text against the previous one** and blits only the
  changed character cells from the sheet into the buffer (GDI+ `DrawImage` is
  expensive per call, so minimizing calls mattered), then draws the buffer
  **unscaled, centered** in the client rectangle;
- the grid is derived from the control size: `cols = width / 12`,
  `lines = height / 16` — at the designer bounds (267×75, logical canvas)
  that is **22 columns × 4 lines**; the buffer is rebuilt on resize.

The JUCE display today is `DisplayPanel`, a dumb multiline text renderer fed
by `VfdDisplayHelper` (ADR-006 §4); this ADR replaces its *rendering* only —
the content pipeline is untouched.

## Decision
1. **Asset.** `extract_control_table.py` gains an extraction step that decodes
   the `MATRIXTINY` base64 BMP out of
   `MidiApp/MidiApp.UIControls/Properties/Resources.resx` and commits it as
   `juce/app/assets/vfd-matrix.png` (pure-python BMP→PNG conversion — stock
   JUCE does not read BMP), added to the existing `juce_add_binary_data`
   target. Same mechanically-regenerable pipeline as the background bitmap
   and button GIFs (ADR-006).
2. **Direct sprite-sheet paint, no manual buffer, no dirty-char diff.**
   `DisplayPanel::paint` walks the padded text grid and issues one
   `Graphics::drawImage` per cell with the integer source rectangle computed
   by the reference formula. The reference's two optimizations are dropped as
   obsolete under JUCE, not out of neglect:
   - *buffer bitmap* → `setBufferedToImage(true)`: JUCE itself caches the
     rendered component into an image and only re-runs `paint()` after a
     `repaint()`, which `setLines` triggers on real text changes (same
     early-out as the reference `SetText`). Zero hand-managed resources.
   - *changed-cell diffing* → unnecessary: a full repaint is ≤ 110 small
     12×16 blits into the cached image, negligible for JUCE's renderer even
     at knob-drag rates, and it only happens when the text actually changed.
3. **Grid & centering as the reference.** `cols = floor(w/12)`,
   `lines = floor(h/16)` from the logical-canvas bounds of the extracted
   `_vfdDisplay` control (267×75 → 22×4), glyph block centered in the bounds,
   black background. `VfdDisplayHelper`'s wrap threshold (`MAX_CHARS_PER_LINE`,
   currently a hard-coded 24) becomes this computed column count so content
   wrapping and rendering share one source of truth.
4. **Scaling.** The reference draws unscaled; our canvas is uniformly scaled
   (RQ-GUI-005), so scaling is unavoidable. The glyph pass sets
   **low (nearest-neighbour) resampling quality** to keep the dot-matrix
   pixels crisp; owner judges visually on Windows and may flip to medium if
   nearest looks too harsh at his scale factors.

## Consequences
- Hardware-look VFD with byte-identical glyph artwork; content logic
  (`VfdDisplayHelper`) and tests untouched.
- ~40 lines of paint code replace the buffer/diff machinery; nothing to
  dispose, no resize rebuild (grid is recomputed from bounds in `paint`).
- The sheet lives in BinaryData like every other reference asset and is
  regenerable from the .NET resources with one script run.

## Open point — resolved
At the reference designer bounds (267×75, 100 % DPI) the grid is **4 lines**,
so `VfdDisplayHelper`'s 5th line — the `MIDI CC:` info the owner added — does
not fit (5×16 = 80 px > 75 px); on the reference it only appears at higher
Windows DPI scale factors, where WinForms autoscaling enlarges the control.
**Owner picked option (b)** (2026-07-13): the display's logical height grows
to **82 px** (5×16 + 2) so all five reference content lines are always
visible. The growth is **upward** (y 74 → 67, bottom edge unchanged at 149):
the area above is free background artwork, while the LED panel (y = 150) and
the shortcut-button row (y = 155) sit directly below the original bottom
edge. Option (a) — strict 22×4 parity — was rejected.
