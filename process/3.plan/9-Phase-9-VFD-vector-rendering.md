# PLAN-VFD-001: VFD Vector 16-Segment Rendering

## Overview

Replace the VFD's inherited 12×16 sprite-sheet blit with 16-segment glyphs drawn
vectorially at the physical pixel scale, so the display stays crisp on HiDPI and
at any window size, covers the whole printable ASCII range, and stops depending
on an asset that can no longer be regenerated in this repository.

Scope is the *rendering* of `DisplayPanel` only. The content pipeline
(`VfdDisplayHelper`), the grid formula, the centering and the caching strategy
are carried over unchanged from ADR-JUC-007 (DEC-JUC-055).

## References

- **Requirements**: RQ-GUI-020, RQ-GUI-031, RQ-GUI-033 *(amended, session VFD)*,
  RQ-GUI-049 *(new)*, RQ-DSN-097 *(new)*, RQ-GUI-005
- **ADRs**: ADR-JUC-023 *(this plan implements it)*, superseding ADR-JUC-007;
  ADR-JUC-014 / ADR-JUC-015 (tokens and their codegen), ADR-JUC-013 (the rest of
  the panel is already vector)

Session state: `unit_tests = true`, `platform = linux`, `chat_mode = normal`.
Tier M and L tasks therefore carry unit tests, and functional verification runs
through the existing Xvfb/screenshot pipeline.

*Python tooling exception, following repo precedent:* this repository has no
Python test harness, and the comparable Tier L task TASK-JUC-095
(`generate_design_tokens.py`) was verified by its `--check` mode plus the C++
suite rather than by Python unit tests. The generators and the fitting tool
added here follow that same convention — their `--check` mode **is** the
executable verification. Standing up a pytest harness for three developer
scripts would be infrastructure beyond this plan's scope; if the owner wants it,
it belongs in its own task.

**DoR — this plan is presented for owner approval before any task starts.**

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-VFD-001 | Vendor the 16-segment ASCII table | M | RQ-GUI-033, RQ-GUI-049, DEC-JUC-051 |
| TASK-VFD-002 | Fitting tool deriving the VFD parameters from the baseline sheet | M | RQ-DSN-097, DEC-JUC-054, DEC-JUC-056 |
| TASK-VFD-003 | VFD token group in `design-tokens.yaml` | M | RQ-DSN-097, DEC-JUC-056 |
| TASK-VFD-004 | Vector segment renderer | L | RQ-GUI-033, DEC-JUC-053, DEC-JUC-054 |
| TASK-VFD-005 | Off-model primitives and their override table | M | RQ-GUI-033, DEC-JUC-052 |
| TASK-VFD-006 | Full ASCII 32–126 coverage | M | RQ-GUI-049 |
| TASK-VFD-007 | Wire into `DisplayPanel`, retire the sprite blit | M | RQ-GUI-033, DEC-JUC-055 |
| TASK-VFD-008 | Verification at scale 1.0 / 2.0 / 3.0 | S | RQ-GUI-005, RQ-GUI-033, RQ-GUI-049 |
| TASK-VFD-009 | Preserve the rejected triangle-lattice prototype | M | ADR-JUC-023 (Alternatives) |

---

### TASK-VFD-001: Vendor the 16-segment ASCII table
- **Tier**: M
- **Status**: **Done** — `juce/tools/vendor/16-Segment-ASCII_HEX-NDP.txt` vendored
  verbatim (its MIT notice travels with the data), `generate_segment_font.py`
  emits `GeneratedSegmentFont.inc` (95 masks, `--check` idempotent), public API
  in `SegmentFont.hpp`. Suite green, 92/92 (+7 scenarios, 432 assertions).
  *Finding:* the "every lowercase differs from its uppercase" expectation was
  **wrong** and its test was corrected, not forced — `'x'` and `'X'` share mask
  `0x5500` because a 16-segment cell has no distinct lowercase `x`. It is the
  single collision in the table; RQ-GUI-049 asks for legibility, not case
  distinctness, so the requirement stands unchanged.
- **Description**: Bring `dmadison/LED-Segment-ASCII`'s 16-segment character map
  into the tree as generated data (95 masks, ASCII 32–126, one `uint16_t` each),
  preserving the upstream MIT notice, and pin the bit→geometry mapping with
  tests rather than assuming it.
- **Requirement refs**: RQ-GUI-033, RQ-GUI-049
- **ADR refs**: ADR-JUC-023 (DEC-JUC-051)
- **Acceptance Criteria**:
  - *Given* the generated table, *When* `'0'`, `'1'`, `'8'`, `'A'`, `'E'`, `'H'`,
    `'M'`, `'W'`, `'X'`, `'+'`, `'-'` and `'T'` are decoded, *Then* each lights
    exactly the segment set its shape requires, pinning the outer ring `A`–`H`
    clockwise from top-left and the inner `K,M,N,P,R,S,T,U`.
  - *Given* the table, *When* its length is checked, *Then* it holds 95 entries
    covering ASCII 32–126 with no gap.
  - *Given* the repository, *When* the generated header is read, *Then* the
    upstream MIT copyright notice is present and the dependency is declared as a
    third party.
- **Dependencies**: None
- **Assignee**: AI

### TASK-VFD-002: Fitting tool deriving the VFD parameters from the baseline sheet
- **Tier**: M
- **Status**: **Done** — `juce/tools/fit_vfd_tokens.py`, RMSE **25.81/255** over
  the 51 glyphs the baseline draws (seed 35.79). Emits 11 cell-normalised tokens.
  Two findings changed the design rather than being worked around:
  - **DEC-JUC-054 corrected: the glow is one Gaussian, not two.** The
    two-component claim rested on the falloff of a single isolated glyph. Fitted
    over the whole drawn set the two radii converge (2.10 / 2.59 px) and the
    second component buys 0.53 RMSE — 2 % — for twice the blur cost per repaint,
    with no visible difference side by side. Refuted before it reached the code.
  - **The `--check` criterion was changed.** The DoR wording ("exits non-zero if
    the values no longer match what it derives") implies re-running the descent
    as a gate, which is not reproducible across numpy versions and would be
    flaky. `--check` instead scores the *committed* tokens against the baseline
    and fails past an RMSE budget: deterministic, fast, and it gates the thing
    that actually matters.
  Also: the drawn-glyph set is detected from the sheet (51/95) rather than
  hard-coded, and the unlit level is measured off the blank cell (0.0863) rather
  than fitted — left free it parks on its bound and over-brightens the bed.
- **Description**: Add `juce/tools/fit_vfd_tokens.py`, which measures
  `vfd-matrix.png` and fits the geometry and photometry parameters against it,
  so the token values are *derived and reproducible* rather than eyeballed. The
  tool reports the RMSE it reaches, and offers a `--check` mode consistent with
  `generate_design_tokens.py`.
- **Requirement refs**: RQ-DSN-097
- **ADR refs**: ADR-JUC-023 (DEC-JUC-054, DEC-JUC-056)
- **Acceptance Criteria**:
  - *Given* the baseline sheet, *When* the tool runs, *Then* it emits a parameter
    set and its RMSE over the 51 glyphs the sheet actually draws.
  - *Given* the committed token values, *When* the tool runs with `--check`,
    *Then* it scores them against the baseline and exits non-zero if the error
    regressed past the recorded budget. *(Reworded during implementation: the
    original "no longer match what it derives" would have re-run the descent as
    a gate — not reproducible across numpy versions, so flaky by construction.)*
  - *Given* the measured unlit level, *When* it is reported, *Then* it is taken
    from the sheet's own blank cell rather than from a free fit parameter.
- **Dependencies**: None
- **Assignee**: AI

### TASK-VFD-003: VFD token group in `design-tokens.yaml`
- **Tier**: M
- **Status**: **Done** — 12 tokens in `global` (raw fitted values) aliased in
  `component`, `DesignTokens.hpp` regenerated, generator `--check` idempotent,
  build clean, suite 93/93.
  - **Round-trip verified, which is the real check here:** the committed tokens
    read back by `fit_vfd_tokens.py --check` re-score at **exactly 25.81/255**,
    the value the fit produced. That rules out a normalisation bug in either
    direction — the failure mode this task was most exposed to, since the fit
    works in sheet pixels and the tokens are stored as cell-width fractions.
  - **No `semantic` tier entry.** These are one component's geometry and
    photometry, not roles anything else could share; a role invented to be
    aliased exactly once is tier ceremony, not design system. Noted in the YAML
    so the omission reads as deliberate.
  - **`kind: float`, not a new `scalar` kind.** The emitter first produced
    `scalar`, which `generate_design_tokens.py` rejects — it knows `colour`,
    `float`, `int`. Aligned with the existing vocabulary rather than widening it.
  - The pre-existing `-Wdeprecated-declarations` warning at
    `XplorerLookAndFeel.cpp:189` is unrelated: verified present with these
    changes stashed.
- **Description**: Add the VFD token group — segment geometry, italic shear,
  stroke weight, segment gap, the two glow radii and amplitudes, the unlit level
  and the phosphor hue — in cell-normalised units, each carrying a `note`
  recording its provenance, and regenerate `DesignTokens.hpp`.
- **Requirement refs**: RQ-DSN-097
- **ADR refs**: ADR-JUC-023 (DEC-JUC-056), ADR-JUC-014, ADR-JUC-015
- **Acceptance Criteria**:
  - *Given* the YAML, *When* a VFD geometry token is read, *Then* its value is a
    fraction of the glyph cell and never a pixel count.
  - *Given* the generator, *When* it runs with `--check`, *Then* it is idempotent
    against the committed `DesignTokens.hpp`.
  - *Given* a token fitted against the reference, *When* it is read, *Then* a
    `note` records that provenance.
- **Dependencies**: TASK-VFD-002
- **Assignee**: AI

### TASK-VFD-004: Vector segment renderer
- **Tier**: L
- **Status**: **Done** — `VfdSegmentRenderer` (`juce/app/src/`), 5 scenarios in
  `xpl_tests_app_juce`, suite 98/98, build clean. Output visually verified at
  scale 1 and 3 against the Python model the tokens were fitted with: same halo,
  same unlit bed, crisp at 3.
  - **The renderer returns an `Image`, it does not paint into a `Graphics`.**
    The glow is a blur and a blur needs a raster. The image is built at the
    caller's device scale and drawn 1:1, so nothing is ever magnified —
    `DisplayPanel` keeps owning bounds and centering (DEC-JUC-055, TASK-VFD-007).
  - **The separable Gaussian is hand-written, deliberately.** JUCE offers
    `ImageConvolutionKernel` (a 2D convolution — for this radius roughly 20x the
    work for an identical result) and `applySingleChannelBoxBlurEffect` (a box
    blur — a *different* kernel, which would have silently diverged from the
    Gaussian the tokens were fitted against). Neither fits; two 1D passes do.
  - **Testing the scale behaviour, not the glyph shapes.** Shapes are the
    table's job (pinned by `SegmentFontTests`) and the look is the tokens' job
    (pinned by `fit_vfd_tokens.py --check`). What only this layer can get wrong
    is scale, so that is what the tests attack: proportional image size, detail
    a magnification could not carry, and mean luminance held constant across
    scales 1/2/3 — which is precisely how a glow radius that forgot to follow
    the scale would betray itself (DEC-JUC-053).
  - **A source grep for "no visual literal" was rejected as a test.** A regex
    cannot tell a visual literal from an algorithmic constant, so it would have
    produced false positives on `0.5F` (a midpoint) or `3.0F` (the Gaussian's
    support). Replaced by a test that samples a below-saturation pixel and
    checks its hue equals the token's — evidence the token is consumed.
- **Description**: Build the 16 segment outlines once as `juce::Path` in a unit
  cell; accumulate a scalar radiance field (lit core + two-component glow over an
  always-drawn unlit bed) and tone-map it once, preserving the phosphor hue below
  1.0 and lifting toward white above it. Rasterise at the physical pixel scale
  read from the graphics context, scaling the glow radii by that same factor.
- **Requirement refs**: RQ-GUI-033, RQ-GUI-005
- **ADR refs**: ADR-JUC-023 (DEC-JUC-053, DEC-JUC-054)
- **Acceptance Criteria**:
  - *Given* a render scale of 1.0 and one of 2.0, *When* a glyph is produced,
    *Then* the second is rasterised at twice the resolution and is not a
    magnification of the first.
  - *Given* the render scale is doubled, *When* the glow is applied, *Then* its
    radius in device pixels doubles with it.
  - *Given* a glyph cell, *When* it is painted, *Then* the unlit segments are
    present at the token's low intensity.
  - *Given* the renderer source, *When* it is read, *Then* it contains no numeric
    visual literal — every value resolves to a token (RQ-DSN-097).
- **Dependencies**: TASK-VFD-001, TASK-VFD-003
- **Assignee**: AI

### TASK-VFD-005: Off-model primitives and their override table
- **Tier**: M
- **Status**: **Done** — three primitives, one table (`OVERRIDDEN_CHARACTERS`),
  5 new measured tokens, 4 scenarios, suite 102/102.
  - **Their geometry is measured, not typed in.** `fit_vfd_tokens.py --extras`
    reads the dot and bar positions off the baseline's own `:` and `_` cells as
    bright-pixel centroids — no optimisation involved, but the same principle as
    the fit: a number nobody can re-derive is a number nobody can check.
  - **A finding on the shear.** Both colon dots sit at the *same* x in the
    baseline. Un-sheared that leaves 0.53 px between them — exactly the shear a
    hand-drawn 12-px-wide cell could not express. So this is quantisation, not
    evidence that the reference colon is upright, and the renderer shears the
    dots like every other shape now that it can. The tool's first wording
    claimed the agreement *confirmed* the shear model; that was backwards and
    was corrected.
  - **`x` needed no new geometry**: a lower-half crossing built from the
    existing rails. `_` needed only its y.
  - **Two test expectations of mine were wrong and were corrected, not forced.**
    `|` does not render as one continuous bar: on a 16-segment cell a vertical
    bar is two stacked segments with the standard hairline between them, so both
    `:` and `|` give two runs of lit rows. Counting runs cannot separate them.
    What does is how much of the vertical extent is empty — a colon is mostly
    gap (>20%), a bar mostly bar (<10%).
  - The unlit bed deliberately shows **segments only**, never the override
    primitives: a real display's unlit state is its segments, and ghosting a
    colon into every cell would show marks the hardware does not have.
- **Description**: A pure 16-segment renderer cannot draw the reference's `:`
  (two separated dots) or `_` (a bar below the glyph body), and cannot tell
  lowercase `x` from `X`. Add explicit primitives for the three, selected
  through one small, data-driven per-character override table so the divergence
  from the vendored table stays auditable in a single place. `:` is the
  priority: it appears in every `NAME:VALUE` and every `MIDI CC:` line.
  *Scope extended 2026-07-30 (owner decision):* lowercase `x` joins the table.
  Measured on the vendored data, the two collisions across all 95 glyphs are
  `':'`/`'|'` (`0x2200`) and `'x'`/`'X'` (`0x5500`); the colon override was
  already required for fidelity, so these two entries make every glyph render
  distinctly (RQ-GUI-049). The vendored table is **not** edited — upstream's
  `x = X` is correct for a pure 16-segment device.
- **Requirement refs**: RQ-GUI-033, RQ-GUI-049
- **ADR refs**: ADR-JUC-023 (DEC-JUC-052)
- **Acceptance Criteria**:
  - *Given* `':'`, *When* it is rendered, *Then* it produces two vertically
    separated dots and not a continuous vertical bar.
  - *Given* `'x'` and `'X'`, *When* both are rendered, *Then* they differ, the
    lowercase form sitting in the lower half of the cell.
  - *Given* `':'` and `'|'`, *When* both are rendered, *Then* they differ, `'|'`
    keeping the centre verticals as a bar.
  - *Given* the override table, *When* it is read, *Then* every character that
    diverges from the vendored table is listed there and nowhere else, and the
    vendored data file is unmodified.
  - *Given* a character with no override, *When* it is rendered, *Then* the
    vendored segment mask is used unchanged.
- **Dependencies**: TASK-VFD-004
- **Assignee**: AI

### TASK-VFD-006: Full ASCII 32–126 coverage
- **Tier**: M
- **Status**: **Done** — 3 scenarios, suite 105/105.
  - **The proof is at the pixel, not at the table.** RQ-GUI-049 is about what
    the display *shows*, so the test renders all 95 cells and compares their
    appearance. Anything less would only have re-checked the mask table, which
    `SegmentFontTests` already covers — and would have missed a collision
    introduced by rasterisation rather than by the data.
  - **Verified at scale 1 as well as at scale 4, deliberately.** Scale 1 is the
    reference's own 12×16 cell and the *hardest* case, not the easiest: two
    shapes have the fewest pixels available to differ in. The risk was real —
    the lowercase `x` primitive is a half-height crossing, which could plausibly
    have rasterised into the same cell as `X` at that size. **Result: 95 distinct
    appearances at scale 1, 2 and 4.** If it holds at 1 it holds above.
  - Out-of-range code points (31, 127, `€`, `é`) render byte-identically to the
    space cell, so the previous behaviour is preserved rather than merely
    "close".
- **Description**: Close the inherited gap where 44 of 95 cells — every lowercase
  letter among them — rendered blank, silently dropping characters that reach the
  display from a synthesizer patch name or a `.syx` file.
- **Requirement refs**: RQ-GUI-049
- **ADR refs**: ADR-JUC-023 (DEC-JUC-051)
- **Acceptance Criteria**:
  - *Given* each code point in 32–126, *When* it is rendered, *Then* its cell
    differs from the cell produced for a space — except for the space itself.
  - *Given* all 95 code points in 32–126, *When* each is rendered, *Then* the 95
    results are pairwise distinct (the two vendored-table collisions being
    resolved by the TASK-VFD-005 overrides, not by editing the table).
  - *Given* a tone name containing lowercase letters, *When* it is shown, *Then*
    every letter is legible.
  - *Given* a code point outside 32–126, *When* it is rendered, *Then* it renders
    as a space.
- **Dependencies**: TASK-VFD-004, TASK-VFD-005
- **Assignee**: AI

### TASK-VFD-007: Wire into `DisplayPanel`, retire the sprite blit
- **Tier**: M
- **Status**: **Done** — verified live under Xvfb at the launch geometry
  (22×5 cells, unlit bed in every cell, glow present). Build clean, 105/105.
  - The block is drawn into its **logical-size** rectangle from an image built
    at the *device* scale, which lands one image pixel per device pixel. Drawing
    it at its own pixel size, or building it at the logical size, would each
    have reintroduced a resample — the exact degradation this plan removes.
  - `GLYPH_WIDTH`/`GLYPH_HEIGHT` now name the renderer's cell rather than
    repeating 12 and 16, so the grid formula and the glyph geometry cannot
    drift apart.
  - `vfd-matrix.png` removed from the binary data, **kept in the repository**,
    with the reason recorded beside the CMake target rather than left implicit.
    Verified all three ways: absent from `BinaryData.h`, still tracked by git,
    and `fit_vfd_tokens.py --check` still finds it (25.81/26.00).
- **Description**: Replace `DisplayPanel::paint`'s per-cell `drawImage` with the
  vector renderer, keeping the grid formula, the centering, the `setLines`
  early-out and `setBufferedToImage(true)` exactly as they are (DEC-JUC-055).
  Drop the sprite from the binary-data target; **keep `vfd-matrix.png` in the
  repository** as the fitting reference and regression baseline.
- **Requirement refs**: RQ-GUI-033, RQ-GUI-020
- **ADR refs**: ADR-JUC-023 (DEC-JUC-055)
- **Acceptance Criteria**:
  - *Given* the panel, *When* it is painted, *Then* no sprite sheet is read for
    glyph artwork.
  - *Given* the panel bounds, *When* the grid is computed, *Then* it is still
    `cols = floor(w/12)`, `lines = floor(h/16)` from the logical bounds, and the
    block is still centered — 22×5 at the launch geometry.
  - *Given* text set twice with identical content, *When* `setLines` is called
    the second time, *Then* no repaint is triggered.
  - *Given* the repository, *When* it is inspected, *Then* `vfd-matrix.png` is
    still present and referenced by the fitting tool.
- **Dependencies**: TASK-VFD-004, TASK-VFD-005, TASK-VFD-006
- **Assignee**: AI

### TASK-VFD-008: Verification at scale 1.0 / 2.0 / 3.0
- **Tier**: S
- **Status**: **Done** — captured from the running application under Xvfb by
  actually resizing the window to 1260 / 2520 / 3780 px wide, giving canvas
  scales of exactly 1.0, 2.0 and 3.0. The glyphs get *sharper* as the scale
  rises rather than blockier, which is the whole point of the change.
  - **A parameter edit shows all five content lines** (`* S99 XPLORER *`,
    `ENV1 ATTACK:10`, blank, blank, `MIDI CC: 073`), with `:` rendering as two
    dots in both places — the off-model primitive working where it matters most.
  - Build warning-clean: after touching all five changed sources, the rebuild
    emits **zero** warnings. The one at `XplorerLookAndFeel.cpp:189` is
    pre-existing and unrelated (verified earlier by stashing).
  - Suite 105/105.
  - *Method note:* the first capture attempt cropped the wrong region at scales
    2 and 3. `ScaledCanvasComponent` centres the canvas vertically inside the
    area below the menu bar, so the VFD's screen y is
    `24 + (h - 24 - 786·s)/2 + 40·s`, not `64·s`. Worth recording because any
    future screenshot check of a canvas control needs the same formula.
- **Description**: Build, run the suite, and verify the rendered panel under Xvfb
  at the three physical pixel scales that occur in practice — launch at 100 % DPI
  (scale exactly 1.0), launch on HiDPI (2.0), enlarged window on HiDPI (3.0) —
  against the baseline sheet.
- **Requirement refs**: RQ-GUI-005, RQ-GUI-033, RQ-GUI-049
- **ADR refs**: ADR-JUC-023
- **Acceptance Criteria**:
  - *Given* the app under Xvfb, *When* it is launched and a parameter is edited,
    *Then* the display shows the same five content lines as before the change.
  - *Given* the window enlarged, *When* the display is captured, *Then* the
    glyphs are crisp at the new size, with no block magnification.
  - *Given* the build, *When* it completes, *Then* it is warning-clean at the
    project's warning level and the test suite is green.
- **Dependencies**: TASK-VFD-007
- **Assignee**: AI

### TASK-VFD-009: Preserve the rejected triangle-lattice prototype
- **Tier**: M
- **Status**: **Done** — `juce/tools/prototypes/triangle_lattice.py`.
- **Description**: ADR-JUC-023's Alternatives section states that the
  triangle-lattice skeleton "stays reusable if it is ever revisited". That was
  **false as written**: the exploration lived only in a scratch directory and
  would have been lost with the session container, even though the owner had
  explicitly asked for the option to be explored. Commit the prototype so the
  ADR's claim is true. Added mid-plan on owner decision.
- **Requirement refs**: None — this traces to an ADR section, not a requirement;
  it ships no behaviour.
- **ADR refs**: ADR-JUC-023 (Alternatives)
- **Acceptance Criteria**:
  - *Given* the repository, *When* ADR-JUC-023's reusability claim is checked,
    *Then* the referenced skeleton is present under version control.
  - *Given* the prototype, *When* it is read, *Then* it imports the segment
    table, geometry and photometry from the shipped tools rather than copying
    them, so it cannot drift from them.
  - *Given* the prototype, *When* the project is built, *Then* nothing
    references it — no CMake target, no CI step, no runtime import.
  - *Given* the prototype, *When* it is run, *Then* it renders a sample strip
    showing the tiled-stroke look that motivated the option.
- **Dependencies**: TASK-VFD-001, TASK-VFD-002
- **Assignee**: AI
