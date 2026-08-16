# PLAN-GUI-005: Control Tick Junctions

## Overview
Make every tick of the background diagram end on the control it feeds — touching
it, never crossing into it, never stopping short — by deriving its end from the
target's own geometry instead of carrying a hand-authored length.

## References
- **Requirements**: RQ-GUI-071 (depends on RQ-GUI-031, RQ-GUI-037, RQ-GUI-051, RQ-CLR-001, RQ-DSN-099)
- **ADRs**: ADR-JUC-027 (amended — DEC-JUC-112); ADR-JUC-009, ADR-JUC-013, ADR-JUC-014 (referenced)
- **Design system**: no new token — the rule is derived from `semantic.strokeKnobRing` and `semantic.strokeDiagram`

---

## Tasks

### TASK-GUI-027: Derive every tick's end from the control it runs into
- **Tier**: L
- **Status**: Done (2026-08-15)
- **Description**: Replace `BackgroundRenderer`'s `stub(cx, y, len)` with
  `knobTick`/`comboTick`, which take the target's canvas-frame top edge; add the
  junction rule and the target rows to `SectionLayout.hpp`; move the knob's two
  ring insets there so `XplorerLookAndFeel::drawRotarySlider` and the painter read
  one owner; recentre the two combo ticks and their captions.
- **Requirement refs**: RQ-GUI-071
- **ADR refs**: ADR-JUC-027 (DEC-JUC-112)
- **Acceptance Criteria** (Gherkin):
  - **Given** any knob a tick runs into, **When** the background is rendered,
    **Then** the tick is still painted 1 px above the outer edge of the ring and
    is not painted 1 px inside it
  - **Given** the VCO1 and VCO2 VCA VOLUME knobs, **When** their ticks are
    inspected, **Then** neither stops short of its ring (they were −1 and −3 px)
  - **Given** the LAG, TRACK, LFO, ENV, VCO, FM and RAMP ticks, **When** they are
    inspected, **Then** none crosses into its ring (they were +1 to +3 px)
  - **Given** the VCF `MODE (15)` and LFO `WAVESHAPE` combos, **When** the tick
    above and the caption below are measured, **Then** both sit on the combo's own
    centre (they were 6.5 px and 4 px left of it)
  - **Given** every row the painter states, **When** it is compared with
    `controlTable()`, **Then** it matches the control it names
  - **Given** the sources, **When** the knob ring insets are searched for, **Then**
    they are declared once and read by both the painter and the `LookAndFeel`
- **Dependencies**: None
- **Assignee**: AI

### TASK-GUI-028: Correct the TASK-GUI-026 badge-colour assertion
- **Tier**: S
- **Status**: Done (2026-08-15)
- **Description**: `DialogIconTests`' badge-colour scenario sampled pixel (2, 2)
  — a CORNER of the square image — and asserted the accent colour there. The badge
  is a circle inscribed in that square, so the corner is the one place the accent
  can never be. Re-sampled in the annulus between the glyph radius and the badge
  radius, on the 22.5° bearing (a gap between two gear teeth).
- **Requirement refs**: RQ-GUI-070
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** either dialog icon, **When** the annulus between glyph and badge
    edge is sampled, **Then** the pixel is exactly `component.dialogIconAccent`
  - **Given** `xpl_tests_app_juce`, **When** it is run, **Then** every test passes
- **Dependencies**: None
- **Assignee**: AI

### TASK-GUI-029: Bring the mockup generator back into parity
- **Tier**: M
- **Status**: Done (2026-08-15)
- **Description**: `generate_background_mockup.py` gets the same tick rule
  (derived from the same tokens, rows mirrored) — and, because it was one refactor
  behind, the TASK-CLR-001/002 group displacements it never received: LAG +13,
  TRACK +21, LFO +8, RAMP +12, the six moved section anchors, RQ-CLR-006's shared
  centre-column x (527 → 526) and RQ-CLR-007's matrix bar (958/260 → 960/258).
  `background-mockup.svg` regenerated.
- **Requirement refs**: RQ-GUI-071, RQ-CLR-005, RQ-CLR-006, RQ-CLR-007
- **ADR refs**: ADR-JUC-013, ADR-JUC-027 (DEC-JUC-076, DEC-JUC-112)
- **Acceptance Criteria** (Gherkin):
  - **Given** the generator and the painter, **When** their tick calls are
    compared, **Then** both take the target's top edge and derive the end from
    `strokeKnobRing`/`strokeDiagram`, neither carries a length
  - **Given** `background-mockup.svg` and a screenshot of the running app,
    **When** the two are overlaid at the same scale, **Then** every block frame,
    tick, section bar and trigger frame coincides
  - **Given** the generator, **When** it is searched for `stub(`, **Then** there
    is no match
- **Dependencies**: TASK-GUI-027
- **Assignee**: AI

---

## Measurements taken before the change
Ticks, canvas frame, measured against `GeneratedControlTable.inc`. Negative =
stops short of the ring, positive = crosses into it.

| Tick | Was | Now |
|---|---|---|
| VCO2 VOLUME (VCA) | −3.05 | 0 |
| VCO1 VOLUME (VCA) | −1.05 | 0 |
| LAG RATE, TRACK PT1–5, LFO SPEED/RETRIG/AMPLITUDE | +2.95 | 0 |
| VCO1 FREQ/DETUNE, VCO2 FREQ/DETUNE, FM AMPLITUDE, ENV ×6, RAMP RATE | +0.95 to +1.95 | 0 |
| VCO1 PW, VCO2 PW, VCF FREQ/RES/VOLUME ×2 | 0 (by coincidence) | 0 |
| VCF `MODE (15)`, LFO `WAVESHAPE` | 6.5 px / 4 px left of centre | centred |

## Verification note
`xpl_tests_app_juce` (668 assertions, 41 cases) and `xpl_tests_app` (2049
assertions) both pass in full. To run them here, `app/BoundRadioGroupTests.cpp`
had to be excluded from the target for the duration of the run — the pre-existing
GCC 13 `-Wdangling-reference` failure recorded in PLAN-GUI-001, unrelated and not
touched; the exclusion was a local edit, reverted before commit (`git diff` on
`juce/tests/CMakeLists.txt` is empty). Excluding it is what let the RQ-GUI-070
tests of TASK-GUI-026 run for the first time, which is how TASK-GUI-028's defect
surfaced. The visual result was checked by rebuilding the app under Xvfb and
comparing magnified before/after crops of the four rows the owner named;
TASK-GUI-029 was checked by rendering `background-mockup.svg` to PNG and
50 %-blending it over the same screenshot, rescaled to the 1260×786 canvas — the
frames, ticks and bars land on each other, and only the text doubles, which is
expected (the SVG uses Arial metrics, JUCE its own default face).

## Out of scope
Equalising how far a control hangs below its block (9 to 26 px across the panel).
Within each block the knobs already share one `y`, so the panel is row-aligned
already; levelling the blocks would move control-table positions and cascade into
the ADR-CLR-001 rhythm. Owner decision, 2026-08-15: junctions only.

*(The generator-parity work first recorded here as a finding became TASK-GUI-029
below, on the owner's request the same day.)*
