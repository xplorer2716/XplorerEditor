# PLAN-GUI-015: VFD Glyph Rendering Alignment with Xpander Hardware

## Overview
Correct the 16-segment glyph masks in the vendored ASCII table to match the exact rendering of the Xpander hardware VFD display, and stop the application from ever displaying lowercase (the physical unit has no lowercase capability). Reference: hardware photos `letters.jpg`, `digits_and_symbols.jpg` and the app's own `vfdxplorer.jpg` in `docs/assets/`.

## References
- **Requirements**: RQ-GUI-033, RQ-GUI-020, RQ-GUI-049 (amended 2026-09-02)
- **ADRs**: ADR-JUC-023 (vector segment rendering, DEC-JUC-051/052), ADR-JUC-026 (glyph grid snapping)
- **Source file**: `juce/tools/vendor/16-Segment-ASCII_HEX-NDP.txt` (vendored upstream; 9 rows now diverge — see header note added in that file)
- **Regeneration**: `juce/tools/generate_segment_font.py` (run after edits to produce `GeneratedSegmentFont.inc`)

Hardware reference photos:
- `docs/assets/letters.jpg`: ABCDEFGHIJKLMNOPQRSTUVWXYZ
- `docs/assets/digits_and_symbols.jpg`: 0123456789<>()[]/+-*$'
- `docs/assets/vfdxplorer.jpg`: the app's own VFD rendering the same test string, for side-by-side comparison

---

## Tasks

### TASK-GUI-055: Correct '0' (zero) — remove centre bar
- **Tier**: S
- **Status**: Done
- **Description**: Xpander's '0' is the plain 8-segment ring, no diagonal — identical to 'O'.
- **Change**: `0x44FF` → `0x00FF` (A,B,C,D,E,F,G,H only; drops N,T)
- **File**: `16-Segment-ASCII_HEX-NDP.txt` line 44
- **Note**: '0' and 'O' are now the same mask by design — a deliberate collision (see Notes).

---

### TASK-GUI-056: Correct '1' (one) — plain vertical stroke
- **Tier**: S
- **Status**: Done — owner-confirmed against the running app
- **Description**: Three iterations, each rejected on visual review before the last:
  1. `0x00CC` (C,D,G,H — full left and right verticals): read as **"11"**, two full-height bars side by side rather than one glyph.
  2. `0x008C` (C,D,H — full right vertical plus a short top-left flag): still left a visible stray fragment top-left.
  3. `0x000C` (C,D only — the plain right vertical, no serif, no diagonal): **owner-confirmed correct.**
- **Change**: `0x040C` (C,D,N — diagonal serif) → `0x000C` (C,D)
- **File**: line 52 (line numbers shifted after the vendor-file header note was added)
- **Side effect**: `0x000C` is also the vendored table's existing mask for `'!'`, making `'1'`/`'!'` a new intentional collision — accepted (not overridden) because the owner confirmed `'!'` and `'?'` never reach this display in practice.

---

### TASK-GUI-057: Swap '5' and 'S' masks — they were inverted
- **Tier**: S
- **Status**: Done
- **Description**: Decoded both masks: the pre-existing 'S' mask (`0x88BB`) is all-straight-segment — the classic squared 7-segment shape — while the pre-existing '5' mask (`0x90B3`) uses a diagonal (R) — the oblique shape. Xpander puts the squared shape on '5' and the oblique one on 'S', i.e. exactly swapped from the vendored table.
- **Change**: '5' `0x90B3` → `0x88BB`; 'S' `0x88BB` → `0x90B3`
- **File**: lines 49 ('5') and 79 ('S')

---

### TASK-GUI-058: Correct '7' — make it oblique
- **Tier**: S
- **Status**: Done — visually validated by owner
- **Change**: `0x000F` (A,B,C,D — straight) → `0x4403` (A,B,N,T — top bar plus one continuous diagonal from top-right through centre to bottom-left)
- **File**: line 51

---

### TASK-GUI-059: Correct '9' — remove bottom horizontal bar
- **Tier**: S
- **Status**: Done
- **Change**: `0x88BF` (A,B,C,D,E,F,H,P,U) → `0x888F` (A,B,C,D,H,P,U — drops E,F)
- **File**: line 53

---

### TASK-GUI-060: '(' / ')' and '<' / '>' — Xpander renders both pairs identically
- **Tier**: S
- **Status**: Done
- **Description**: Owner clarified (not a shape defect): on the Xpander, `<` and `(` render as the exact same rounded-brace glyph, and likewise `>` and `)`. Decoding showed the vendored `<`/`>` masks were the `(`/`)` chevron **plus** one extra straight segment (U for `<`, P for `>`) — that extra stroke was the only difference. `(` and `)` themselves are unchanged.
- **Change**: '<' `0x9400` → `0x1400` (now = '('); '>' `0x4900` → `0x4100` (now = ')')
- **File**: lines 56 ('<') and 58 ('>')

---

### TASK-GUI-061: Correct 'Y' — make it a true oblique Y
- **Tier**: S
- **Status**: Done
- **Change**: `0x88BC` (C,D,E,F,H,P,U — squared/boxy) → `0x2500` (K,N,S — two upper diagonals meeting a centre-lower vertical stem)
- **File**: line 85

---

### TASK-GUI-062: Add '.' (period) as a true dot
- **Tier**: S
- **Status**: Done
- **Description**: The vendored '.' mask draws a stray single diagonal (R), not a dot. Added as a fourth off-model override (DEC-JUC-052 precedent), reusing the ':' glyph's lower-dot position and design tokens — no new token needed.
- **Files**:
  - `juce/app/src/VfdSegmentRenderer.hpp` — `.` added to `OVERRIDDEN_CHARACTERS`
  - `juce/app/src/VfdSegmentRenderer.cpp` — `case '.':` in `buildOverride()`, reuses `dotAt(vfdDotX, vfdDotLowerY, vfdDotSize)`
  - Vendor table's '.' row (line 42) is untouched (harmless dead data): the override bypasses it entirely, same precedent as ':' and the (now removed) 'x'.

---

### TASK-GUI-063: The app never displays lowercase; remove the now-dead 'x' override
- **Tier**: M
- **Status**: Done
- **Description**: Owner decision mid-session (2026-09-02): the app SHALL match the physical Xpander VFD, which has no lowercase capability, rather than keep a distinct lowercase glyph style. `DisplayPanel::setLines` now uppercases every line before storing/rendering it — the single choke point all VFD content passes through (tone names from `.syx` files included). This makes the `'x'` half-height override (added solely to keep lowercase 'x' distinct from 'X' per the old RQ-GUI-049 wording) unreachable dead code, so it was removed.
- **Requirement change**: RQ-GUI-049 amended — the old "'x' differs from 'X'" acceptance clause is struck; new acceptance requires lowercase text to display in uppercase, and narrows the pairwise-distinctness rule to exempt the four now-intentional collisions (`0`/`O`, `(`/`<`, `)`/`>`, `X`/`x`).
- **Files**:
  - `juce/app/src/DisplayPanel.hpp` / `.cpp` — `setLines` uppercases (new behaviour, tested)
  - `juce/app/src/VfdSegmentRenderer.hpp` / `.cpp` — `'x'` removed from `OVERRIDDEN_CHARACTERS` and `buildOverride()`
  - `juce/tests/app/DisplayPanelTests.cpp` — **new file**, pins the uppercase behaviour against the real component
  - `juce/tests/CMakeLists.txt` — registers the new test file and `DisplayPanel.cpp` in the `xpl_tests_app_juce` target
  - `process/1.requirements/RQ-GUI-user-interface.md` — RQ-GUI-049 amended
- **Acceptance (Gherkin)**: *Given* text containing a lowercase letter, *When* it reaches the display, *Then* it shows in uppercase. *Given* the renderer called directly with lowercase 'x' (bypassing the app), *When* it is rendered, *Then* it is pixel-identical to 'X' (dead code removed, not a distinguishing feature any more).

---

## Test updates (all in `juce/tests/app/`)
- `SegmentFontTests.cpp`: '1' pin updated to `{C,D}` (plain vertical, after the two rejected serif attempts above); the ring/diagonal-isolation scenario now anchors on `'O'` (not `'0'`, which no longer differs from the ring) plus a new direct pin of `{A,B,N,T}` via the corrected `'7'`; the known-collisions scenario now expects 6 pairs, not 2 (`"!1"`, `"(<"`, `":|"`, `")>"`, `"Xx"`, `"0O"`).
- `VfdSegmentRendererTests.cpp`: override-list scenario expects `{':','_','.'}`, not `{':','_','x'}`; the old "'x' differs from 'X'" scenario is replaced with "'x' renders the same as 'X'"; the full-range distinctness scenario now allows exactly 5 identical pairs (`0/O`, `(/<`, `)/>`, `X/x`, `1/!`) instead of requiring strict pairwise distinctness.
- `DisplayPanelTests.cpp` (new): pins that `DisplayPanel::setLines` renders lowercase and mixed-case input identically to their uppercase form.

All suites green: `xpl_tests_app` (headless, `SegmentFontTests`) and `xpl_tests_app_juce` (`VfdSegmentRendererTests`, `DisplayPanelTests`) pass in full (session.unit_tests = true).

---

## Acceptance Criteria (Gherkin)

*Given* the corrected glyph masks and the hardware photos:
- *When* each corrected character is rendered at 1x canvas scale,
- *Then* it visually matches the corresponding character in `digits_and_symbols.jpg` or `letters.jpg`.

*Given* the rebuilt application:
- *When* the VFD displays text containing any of the corrected characters,
- *Then* the display matches the Xpander hardware appearance — owner-confirmed for '7'; '0','1','5','9','S','Y','.','<','>' pending a final side-by-side re-screenshot (TASK-GUI-064).

---

## Dependencies
- TASK-GUI-063 depends on TASK-GUI-055..062 being in place first (it changes the reachable-character-set / distinctness contract those tasks' tests were re-derived against).

---

## Open follow-up

### TASK-GUI-064: Final visual sign-off
- **Tier**: S
- **Status**: Done — owner-confirmed against the running app ("good")
- **Description**: '1', '5', '7', '9', 'S', 'Y', '0' confirmed correct against the running app across this session's iterations (including two rejected '1' attempts, corrected live). '.', '<', '>' were not separately re-confirmed after the final rebuild — carried over as unverified if a future session touches this area again.
- **Assignee**: Human (owner) + AI review, both performed in-session.

---

## Notes
- The vendored source (`16-Segment-ASCII_HEX-NDP.txt`) comes from dmadison/LED-Segment-ASCII (MIT, GitHub). A header note was added there recording that 9 rows now diverge from upstream for hardware accuracy, naming the source photos.
- Five character pairs now deliberately render identically: `0`/`O`, `(`/`<`, `)`/`>`, `1`/`!` (Xpander hardware collisions, or — for `1`/`!` — an accepted side effect since `!` never reaches this display) and `X`/`x` (lowercase is unreachable through the app after TASK-GUI-063). This is a real, tested contract change from the pre-session "all 95 code points pairwise distinct" rule.
