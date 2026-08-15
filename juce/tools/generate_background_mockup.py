#!/usr/bin/env python3
"""Vector background mockup for Xplorer (prototype for BackgroundRenderer.cpp).

Geometry measured from the reference bitmap (line detection) + control-table
knob anchors; every SVG primitive maps 1:1 to a juce::Graphics call. This is the
owner-validated prototyping tool kept in lock-step with the C++ painter.

Colours, font sizes and line width are NOT hard-coded here: they are read from
the SAME single source of truth as the C++ tokens (design-tokens.yaml, via
generate_design_tokens.resolve()), so the mockup and the shipped app can never
diverge on a token value. Only the diagram GEOMETRY (coordinates) lives in this
script. [RQ-DSN-060..063, ADR-JUC-013, ADR-JUC-015]

Prerequisite: PyYAML (`pip install pyyaml`).

Usage:
    python3 generate_background_mockup.py        # writes background-mockup.svg next to this script
"""
import pathlib
import random

from generate_design_tokens import resolve

HERE = pathlib.Path(__file__).resolve().parent
OUT_PATH = HERE / "background-mockup.svg"

# ---- tokens (single source of truth) --------------------------------------
_T = resolve()


def _col(name: str) -> str:
    """'#RRGGBB' for a semantic colour token."""
    return "#" + _T["semantic"][name]["value"].upper()


def _num(name: str):
    """Numeric value of a semantic size/geometry token; integral -> int (so the
    SVG reads '2'/'15' rather than '2.0'/'15.0', matching the reference)."""
    v = _T["semantic"][name]["value"]
    return int(v) if float(v).is_integer() else v


# colours
FRAME = _col("diagramFrame")     # block frame / line colour
TITLE = _col("diagramTitle")     # bold titles
CAPTION = _col("diagramCaption")  # parameter captions
GRAIN = _col("panelWoodGrain")
PLATE = [_col("panelPlateTop"), _col("panelPlateHi"), _col("panelPlateMid"), _col("panelPlateBot")]
WOOD = [_col("panelWood0"), _col("panelWood1"), _col("panelWood2"), _col("panelWood3"), _col("panelWood4")]
BAR = [_col("sectionBarTop"), _col("sectionBarMid"), _col("sectionBarBot")]

# Functional-block identity hues: frames, section labels and section bars are
# drawn in their block's colour so the panel reads as grouped areas. Anything
# outside an identified block keeps FRAME. [RQ-DSN-092/093, RQ-GUI-044]
BLOCK = {n: _col("block" + n.capitalize())
         for n in ("vco", "lag", "track", "vcf", "env", "lfo", "ramp", "matrix")}
BAR_FADE = _T["component"]["sectionBarFadeEnd"]["value"]
FILL_A = _T["component"]["blockFillAlpha"]["value"]
RELIEF = _T["component"]["blockFrameRelief"]["value"]


def _darker(hex_col: str, amount: float) -> str:
    """JUCE Colour::darker(amount) equivalent: HSB brightness x 1/(1+amount)."""
    import colorsys
    r, g, b = (int(hex_col[i:i + 2], 16) / 255 for i in (1, 3, 5))
    h, sat, v = colorsys.rgb_to_hsv(r, g, b)
    r, g, b = colorsys.hsv_to_rgb(h, sat, v / (1.0 + amount))
    return "#%02X%02X%02X" % (round(r * 255), round(g * 255), round(b * 255))

# line width + font scale (names mirror BackgroundRenderer.cpp FS_*)
# ONE width for every diagram stroke -- block frames, signal lines and neutral
# sub-panel frames alike -- so the diagram reads as a single drawing rather than
# a set of frames with lines of a different weight between them. [RQ-GUI-051]
LW = _num("strokeDiagram")
FS_SECTION = _num("textTitle")     # 15 — section titles
FS_VCO = _num("textDisplay")       # 16 — VCO1 / VCO2
FS_MIX = _num("textSubtitle")      # 14 — MIX / LAG / LFO / RAMP
FS_BLOCK = _num("textLabel")       # 13.5 — wide block labels
FS_VCA = _num("textLabelAlt")      # 13 — VCA / VCA1
FS_PWM = _num("textBody")          # 12.5 — PWM
FS_CAPTION = _num("textCaption")   # 12 — parameter captions
FS_WAVE = _num("textWave")         # 11.5 — TRIANGLE / SAWTOOTH / PULSE
FS_OUT = _num("textSmall")         # 11 — IN/OUT labels
FS_SMALL = _num("textDense")       # 9 — DESTINATION / TRIGGER IN / NOISE

# ---- section header geometry (label inline with the separator bar) ---------
# Tokens, not local literals: the bar thickness doubles as the label's baseline
# offset (label and bar are bottom-aligned), so it drives appearance and not
# just this script's coordinates. [RQ-GUI-062, RQ-DSN-101, RQ-DSN-060..063]
SECTION_BAR_HEIGHT = _T["component"]["sectionBarHeight"]["value"]
SECTION_LABEL_GAP = _T["component"]["sectionLabelGap"]["value"]
SECTION_LEAD_STUB = _T["component"]["sectionLeadStub"]["value"]
# Helvetica/Arial Bold advance widths, in em. Needed because the bar now starts
# where the label ends, so the label has to be *measured* — the stacked layout
# never had to know its width. JUCE measures the real glyphs
# (juce::Font::getStringWidthFloat); these are the published metrics of the same
# typeface, so the mockup lands within a pixel of the painter.
# Cap height (published Helvetica/Arial Bold metric, 716/1000 em). The rule is
# as tall as the label's capitals, so it reads as a solid band level with the
# letters rather than as an underline. The painter measures the real 'H'
# outline instead of using this ratio -- juce::Font exposes no cap-height
# metric and its glyph bounding box reports the full line box (ascent +
# descent), which would make the rule visibly taller than the label.
_CAP_HEIGHT = 0.716
_CHAR_W = {"A": .722, "B": .722, "C": .722, "D": .722, "E": .667, "F": .611,
           "G": .778, "H": .722, "I": .278, "J": .556, "K": .722, "L": .611,
           "M": .833, "N": .722, "O": .778, "P": .667, "Q": .778, "R": .722,
           "S": .667, "T": .611, "U": .722, "V": .667, "W": .944, "X": .667,
           "Y": .667, "Z": .611, " ": .278, "/": .278}
_CHAR_W.update(dict.fromkeys("0123456789", .556))
_LETTER_SPACING = 0.5          # the ls default of T(), applied between glyphs


def _text_width(s: str, size: float) -> float:
    """Rendered width of a bold section label, in px."""
    return sum(_CHAR_W[c] for c in s) * size + _LETTER_SPACING * (len(s) - 1)

# Reference client area was 1260x813 with a 32 px band at the top reserved
# for the WinForms menustrip (14 px dark strip + 18 px empty plate; the first
# content row, the VCO1 frame, sits at reference y=32). The JUCE port hosts
# its menu bar outside the canvas, so the band is cropped — except a small
# top gap kept for cosmetics (PADDING, ~ the section-bar height), filled with
# panel material (metal + wood) for visual continuity, not black. Canvas
# height is 813 - 27 = 786 and all diagram geometry (kept in reference
# coordinates below) is translated up by CROP. Keep in sync with
# extract_control_table.py CANVAS_TOP_CROP and BackgroundRenderer.cpp
# CANVAS_PADDING. [ADR-JUC-013]
MENUSTRIP_BAND = 32
PADDING = 5                    # cosmetic top gap (panel material, not black)
CROP = MENUSTRIP_BAND - PADDING  # 27
W, H = 1260, 813 - CROP
random.seed(42)

# ---- control ticks: where a tick STOPS -------------------------------------
# Mirrors SectionLayout.hpp. A tick ends on the OUTER edge of the first pixel
# its target paints — touching it, never crossing into it, never stopping short.
# The RULE is derived from the same tokens the painter reads (strokeKnobRing,
# strokeDiagram), so the two cannot diverge on a value; only the ROWS are
# mirrored, exactly as every other coordinate in this file is.
# [RQ-GUI-071, ADR-JUC-013, ADR-JUC-027 (DEC-JUC-112, DEC-JUC-076)]
KNOB_BOUNDS_INSET = 2.0        # anti-aliasing margin inside the control bounds
KNOB_RING_INSET = 1.0          # ring path radius, inside that margin
KNOB_RING_TOP_CANVAS_INSET = (KNOB_BOUNDS_INSET + KNOB_RING_INSET
                              - _num("strokeKnobRing") / 2.0)
COMBO_TOP_CANVAS_INSET = 0.0   # a combo paints from its bounds; a Component clips to itself

# The rows a tick aims at, CANVAS frame — GeneratedControlTable.inc.
KNOB_ROW_TOP = 67          # VCO1 FREQ/DETUNE/PW/VOLUME + VCF FREQ/RES/VCA1/VCA2
KNOB_ROW_FM = 229          # FM AMPLITUDE
KNOB_ROW_VCO2_VCA = 245    # VCO2 VOLUME (on the FM row, not the VCO2 row)
KNOB_ROW_ENV = 251
KNOB_ROW_VCO2 = 347
KNOB_ROW_LFO = 483
KNOB_ROW_LAG = 532
KNOB_ROW_RAMP = 667
KNOB_ROW_TRACK = 718
COMBO_VCF_MODE_Y, COMBO_VCF_MODE_CX = 68, 675.5   # VCF_MODE.x 612 + 127/2
COMBO_LFO_WAVE_Y, COMBO_LFO_WAVE_CX = 486, 661.0  # LFO_X_WAVESHAPE.x 611 + 100/2

# ---- section separators, REFERENCE frame — mirrors SectionLayout.hpp --------
# A column's separators share one x and one width (RQ-CLR-006); the three bottom
# anchors share one baseline (RQ-CLR-003); MOD MATRIX's two ends are defined by
# the control grid it runs alongside (RQ-CLR-007). [ADR-CLR-001]
SECTION_X_LEFT, SECTION_X_CENTRE = 53, 526
SECTION_X_MATRIX, SECTION_MATRIX_BAR_WIDTH = 960, 258
SECTION_VCO_Y, SECTION_LAG_Y, SECTION_TRACK_Y = 465, 615, 799
SECTION_VCF_Y, SECTION_ENV_Y, SECTION_LFO_Y, SECTION_RAMP_Y = 182, 415, 599, 799
SECTION_MATRIX_Y = 799
# Section bottoms the painter DRAWS rather than reading off a control.
LAG_FRAME_TOP_CANVAS_Y = 487
LAG_RATE_CAPTION_BASELINE_CANVAS_Y = 576
TRACK_PT_CAPTION_BASELINE_CANVAS_Y = 760
RAMP_TRIGGER_FRAME_TOP_CANVAS_Y, RAMP_TRIGGER_FRAME_HEIGHT = 719, 41


def refY(canvas_y):
    """CANVAS frame -> REFERENCE frame, the frame this file's draw calls use."""
    return canvas_y + CROP

svg = []
svg.append(f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">')

# ---------------------------------------------------------------- defs
svg.append(f'''<defs>
  <linearGradient id="metal" x1="0" y1="0" x2="0" y2="1">
    <stop offset="0" stop-color="{PLATE[0]}"/>
    <stop offset="0.25" stop-color="{PLATE[1]}"/>
    <stop offset="0.6" stop-color="{PLATE[2]}"/>
    <stop offset="1" stop-color="{PLATE[3]}"/>
  </linearGradient>
  <linearGradient id="wood" x1="0" y1="0" x2="1" y2="0">
    <stop offset="0" stop-color="{WOOD[0]}"/>
    <stop offset="0.25" stop-color="{WOOD[1]}"/>
    <stop offset="0.55" stop-color="{WOOD[2]}"/>
    <stop offset="0.85" stop-color="{WOOD[3]}"/>
    <stop offset="1" stop-color="{WOOD[4]}"/>
  </linearGradient>
  <linearGradient id="bluebar" x1="0" y1="0" x2="1" y2="0">
    <stop offset="0" stop-color="{BAR[0]}"/>
    <stop offset="0.5" stop-color="{BAR[1]}"/>
    <stop offset="1" stop-color="{BAR[2]}"/>
  </linearGradient>''' +
  # one section-bar gradient per block: full block hue at the label end fading
  # to BAR_FADE at the far end (keeps RQ-GUI-037's "bright at the label end"
  # treatment, now carrying the block identity). [RQ-DSN-092, RQ-GUI-044]
  "".join(f'''
  <linearGradient id="bar-{n}" x1="0" y1="0" x2="1" y2="0">
    <stop offset="0" stop-color="{c}" stop-opacity="1"/>
    <stop offset="1" stop-color="{c}" stop-opacity="{BAR_FADE}"/>
  </linearGradient>
  <linearGradient id="frame-{n}" x1="0" y1="0" x2="0" y2="1">
    <stop offset="0" stop-color="{c}"/>
    <stop offset="1" stop-color="{_darker(c, RELIEF)}"/>
  </linearGradient>''' for n, c in BLOCK.items()) + '''
</defs>''')

# ---------------------------------------------------------------- plate
# Plate + wood rails fill the whole canvas (top/bottom margins included) so the
# diagram's PADDING gaps read as continuous panel material, not a black band
# against the menu bar. Gentle vertical luminance gradient only, no streaks.
svg.append(f'<rect x="0" y="0" width="{W}" height="{H}" fill="url(#metal)"/>')

# ---------------------------------------------------------------- wood rails
def wood(x):
    parts = [f'<rect x="{x}" y="0" width="28" height="{H}" fill="url(#wood)"/>']
    for _ in range(90):  # grain
        gx = x + random.uniform(2, 26)
        gy = random.uniform(0, H)
        ln = random.uniform(30, 160)
        parts.append(f'<path d="M{gx:.1f} {gy:.0f} q {random.uniform(-2,2):.1f} {ln/2:.0f} 0 {ln:.0f}" stroke="{GRAIN}" stroke-opacity="{random.uniform(0.15,0.4):.2f}" stroke-width="{random.uniform(0.5,1.6):.1f}" fill="none"/>')
    parts.append(f'<rect x="{x}" y="0" width="2" height="{H}" fill="#000000" fill-opacity="0.45"/>')
    parts.append(f'<rect x="{x+26}" y="0" width="2" height="{H}" fill="#000000" fill-opacity="0.45"/>')
    return "".join(parts)
svg.append(wood(0))
svg.append(wood(W - 28))

# Diagram geometry below stays in reference coordinates; translate the whole
# group up over the cropped menustrip band (matches the JUCE painter).
svg.append(f'<g transform="translate(0,-{CROP})">')
DIAGRAM_START = len(svg)

# ---------------------------------------------------------------- layering
# Every diagram element carries a layer marker and the whole diagram is
# re-ordered into LINES -> BOXES -> TEXT at output time, instead of being
# painted in the reading order of the code below.
#
# Why: a signal line or knob stub that ends ON a block edge is written after
# that block here (it belongs to the block's paragraph), so in code order its
# rounded end-cap lands on top of the block -- a visible nub of neutral line
# colour inside the coloured fill and across the frame. Painting every line
# first and every block over them makes each block read as a solid object the
# lines run *into*, which is the physical reading the diagram wants.
# Text stays last so a label is never covered by the fill it sits on.
# [RQ-GUI-051]
_LINES, _BOXES, _TEXT = "1", "2", "3"
_MARK = "\x00"


def _tag(layer, markup):
    """Prefix an element with its layer marker. Elements are concatenated with
    `+` all over the script, so the marker travels with each element rather
    than with the appended string as a whole."""
    return f"{_MARK}{layer}{markup}"


# ---------------------------------------------------------------- helpers
def box(x, y, w, h, blk=None):
    """Block frame. `blk` = the owning block's key: the box is then filled with
    the block hue at FILL_A and framed with the relief gradient (pure hue on the
    top edge, darkened on the bottom edge). `None` = neutral, unfilled frame,
    used for control sub-panels which must not compete with the blocks. Both
    use the single diagram stroke width LW (RQ-GUI-051).
    [RQ-GUI-044, RQ-DSN-094, RQ-GUI-051]"""
    if blk is None:
        return _tag(_BOXES, f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="2" fill="none" stroke="{FRAME}" stroke-width="{LW}"/>')
    return _tag(_BOXES,
                f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="2" fill="{BLOCK[blk]}" '
                f'fill-opacity="{FILL_A}" stroke="url(#frame-{blk})" stroke-width="{LW}"/>')
def line(x1, y1, x2, y2):
    # round caps so perpendicular segments join with a soft rounded corner
    # matching the block frames (JUCE PathStrokeType curved/rounded)
    return _tag(_LINES, f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="{FRAME}" stroke-width="{LW}" stroke-linecap="round"/>')
def _tick_end(first_painted_canvas_y):
    """REFERENCE y a tick must END at. The tick is stroked at LW with ROUNDED end
    caps, so its painted end reaches half a stroke past the endpoint — taking
    that half off is what lands the cap ON the target instead of over it."""
    return round(refY(first_painted_canvas_y) - LW / 2.0, 2)


def knob_tick(cx, y, knob_canvas_top_y):
    return line(cx, y, cx, _tick_end(knob_canvas_top_y + KNOB_RING_TOP_CANVAS_INSET))


def combo_tick(cx, y, combo_canvas_top_y):
    return line(cx, y, cx, _tick_end(combo_canvas_top_y + COMBO_TOP_CANVAS_INSET))


def caption_centre_x(cx):
    """A caption's integer x for a control centre — round, do not truncate: an
    odd-width combo (VCF_MODE is 127) would otherwise sit half a pixel left of
    the tick above it."""
    return int(cx + 0.5)
def T(x, y, s, size=FS_SECTION, w="bold", fill=TITLE, anchor="start", ls="0.5"):
    return _tag(_TEXT,
                f'<text x="{x}" y="{y}" font-family="Arial, Helvetica, sans-serif" font-size="{size}" '
                f'font-weight="{w}" fill="{fill}" text-anchor="{anchor}" letter-spacing="{ls}">{s}</text>')
def caption(x, y, s):
    return T(x, y, s, FS_CAPTION, "normal", CAPTION, "middle", "0.5")
def section(x, y, s, barw, blk):
    """Section header: label text + separator bar, both in the block's identity
    colour (replaces the former shared blue gradient). [RQ-GUI-044]

    The label sits *on* the rule rather than above it: the rule keeps its
    position and its right end, and is INTERRUPTED by the label instead of
    running under it, so the ~11 px the stacked layout spent above the bar is
    freed without any other element moving.

    Left to right: a short lead-in stub, a gap, the label, a gap, then the rest
    of the rule out to its unchanged right end — the Xpander's own silkscreen
    treatment, where the rule runs into the section name rather than starting
    after it (owner request).

    Only the run AFTER the label carries the fade, exactly as before. The
    lead-in stub is flat block hue at full opacity — the same colour as the
    label and as the first pixel of the run after it, so the three read as one
    interrupted rule and not as three elements (owner decision).

    Label and rule are aligned on their BOTTOM edges — the text baseline (these
    labels are all caps, so nothing descends below it) is the rule's bottom
    edge, not its centre. Centring was tried first and rejected on review: the
    rule then reads as crossing the text, and the letters hang 3 px below the
    line they are supposed to rest on.
    [RQ-GUI-062, RQ-DSN-101, ADR-JUC-034 (DEC-JUC-107, DEC-JUC-108, DEC-JUC-109)]"""
    label_w = _text_width(s, FS_SECTION)
    label_x = x + SECTION_LEAD_STUB + SECTION_LABEL_GAP
    bar_x = label_x + label_w + SECTION_LABEL_GAP
    baseline_y = y + SECTION_BAR_HEIGHT
    bar_h = FS_SECTION * _CAP_HEIGHT
    bar_y = baseline_y - bar_h
    return (_tag(_TEXT, f'<rect x="{x}" y="{bar_y:.2f}" width="{SECTION_LEAD_STUB}" '
                        f'height="{bar_h:.2f}" fill="{BLOCK[blk]}"/>') +
            T(label_x, baseline_y, s, FS_SECTION, "bold", BLOCK[blk]) +
            _tag(_TEXT, f'<rect x="{bar_x:.1f}" y="{bar_y:.2f}" width="{x + barw - bar_x:.1f}" '
                        f'height="{bar_h:.2f}" fill="url(#bar-{blk})"/>'))
def outlab(x, y, s1, s2):
    return (line(x, y, x + 14, y) +
            T(x + 19, y - 2, s1, FS_OUT, "bold") + T(x + 19, y + 10, s2, FS_OUT, "bold"))
def smalllab(x, y, s, anchor="end"):
    return T(x, y, s, FS_SMALL, "bold", CAPTION, anchor, "0.3")

# ================================================================ LEFT COLUMN
# --- VCO1 group
svg.append(box(51, 32, 147, 52, "vco") + T(64, 63, "VCO1", FS_VCO))
for i, (wave, y) in enumerate([("TRIANGLE", 45), ("SAWTOOTH", 57), ("PULSE", 70)]):
    svg.append(T(193, y + 4, wave, FS_WAVE, "bold", TITLE, "end"))
svg.append(line(198, 45, 330, 45) + line(198, 57, 330, 57))          # tri, saw
svg.append(line(198, 70, 234, 70) + line(286, 70, 330, 70))          # pulse via PWM
svg.append(box(234, 60, 52, 23, "vco") + T(260, 76, "PWM", FS_PWM, "bold", TITLE, "middle") + knob_tick(259, 83, KNOB_ROW_TOP))
svg.append(box(330, 32, 53, 52, "vco") + T(356, 63, "MIX", FS_MIX, "bold", TITLE, "middle"))
svg.append(line(383, 58, 405, 58))
svg.append(box(405, 45, 53, 26, "vco") + T(431, 63, "VCA", FS_VCA, "bold", TITLE, "middle") + knob_tick(432, 71, KNOB_ROW_TOP))
svg.append(knob_tick(82, 84, KNOB_ROW_TOP) + knob_tick(170, 84, KNOB_ROW_TOP))
svg.append(caption(82, 137, "FREQUENCY") + caption(170, 137, "DETUNE") + caption(259, 137, "PULSE WIDTH") + caption(432, 137, "VOLUME"))
# VCO1 VCA out -> straight into the VCF left edge
svg.append(line(458, 58, 525, 58))
# FM modulation buses (both at y=180, same height): left branch ends on the
# VCO1 FREQUENCY stub, right branch ends on the VCF FREQ stub — the FM
# modulates the frequencies, it does not enter the VCF block itself.
svg.append(line(40, 88, 82, 88) + line(40, 88, 40, 180) + line(40, 180, 284, 180))
# DESTINATION -> up to the VCO1 bus  [owner v2 point 1a]
svg.append(line(276, 220, 284, 220) + line(284, 220, 284, 180))
# DESTINATION -> up to the VCF bus running right at the same y, hopping over
# the x=499 vertical, rising at x=513, then onto the VCF FREQ stub (y=82).
svg.append(line(276, 230, 289, 230) + line(289, 230, 289, 180))
svg.append(line(289, 180, 492, 180))
svg.append(_tag(_LINES, f'<path d="M492 180 A 7 7 0 0 1 506 180" fill="none" stroke="{FRAME}" stroke-width="{LW}"/>'))
svg.append(line(506, 180, 513, 180) + line(513, 180, 513, 82) + line(513, 82, 541, 82))

# --- FM / VCO2 group
svg.append(box(51, 210, 102, 36, "vco") + T(102, 233, "FM VCA", FS_BLOCK, "bold", TITLE, "middle"))
svg.append(box(184, 210, 90, 52))
svg.append(T(229, 274, "DESTINATION", FS_SMALL, "bold", CAPTION, "middle", "0.3"))
svg.append(line(153, 228, 184, 228))
svg.append(knob_tick(106, 246, KNOB_ROW_FM) + caption(106, 300, "FM AMPLITUDE"))
svg.append(box(329, 210, 52, 52, "vco") + T(355, 241, "MIX", FS_MIX, "bold", TITLE, "middle"))
svg.append(line(381, 232, 405, 232))
svg.append(box(405, 220, 53, 26, "vco") + T(431, 238, "VCA", FS_VCA, "bold", TITLE, "middle") + knob_tick(430, 246, KNOB_ROW_VCO2_VCA))
# VCO2-row VCA out -> right, then up at x=499 into the VCF  [owner point 2]
svg.append(line(458, 232, 499, 232) + line(499, 232, 499, 70))
svg.append(_tag(_LINES, f'<path d="M499 70 Q499 58 509 58" fill="none" stroke="{FRAME}" stroke-width="{LW}"/>'))
svg.append(caption(430, 314, "VOLUME"))
svg.append(box(51, 310, 147, 52, "vco") + T(64, 341, "VCO2", FS_VCO))
for wave, y in [("TRIANGLE", 320), ("SAWTOOTH", 334), ("PULSE", 348)]:
    svg.append(T(193, y + 4, wave, FS_WAVE, "bold", TITLE, "end"))
# vco2 waves route up into MIX
svg.append(line(198, 320, 297, 320) + line(297, 320, 297, 228) + line(297, 228, 329, 228))
svg.append(line(198, 334, 303, 334) + line(303, 334, 303, 237) + line(303, 237, 329, 237))
svg.append(line(198, 348, 233, 348) + box(233, 340, 52, 23, "vco") + T(259, 356, "PWM", FS_PWM, "bold", TITLE, "middle") + knob_tick(260, 363, KNOB_ROW_VCO2))
svg.append(line(285, 348, 309, 348) + line(309, 348, 309, 246) + line(309, 246, 329, 246))
svg.append(_tag(_TEXT, f'<text x="318" y="300" font-family="Arial" font-size="{FS_SMALL}" font-weight="bold" fill="{CAPTION}" transform="rotate(-90 318 300)" letter-spacing="0.3">NOISE</text>'))
svg.append(line(317, 255, 329, 255) + line(317, 255, 317, 270))
svg.append(knob_tick(82, 362, KNOB_ROW_VCO2) + knob_tick(169, 362, KNOB_ROW_VCO2))
svg.append(caption(82, 418, "FREQUENCY") + caption(169, 418, "DETUNE") + caption(260, 418, "PULSE WIDTH"))
# FM carrier path: VCO2 TRIANGLE line taps up at x=204 to the y=305 run,
# which feeds the left bus into the FM VCA input  [owner point 3]
svg.append(line(40, 229, 40, 305) + line(40, 229, 51, 229) + line(40, 305, 204, 305))
svg.append(line(204, 305, 204, 320))
svg.append(section(SECTION_X_LEFT, SECTION_VCO_Y, "VCO1/VCO2/FM", 370, "vco"))

# --- LAG   (whole group +13 canvas px, RQ-CLR-004: equalises this column's
#            two below-separator gaps at 45 and 46 px)
svg.append(box(81, refY(LAG_FRAME_TOP_CANVAS_Y), 268, 36, "lag") + T(215, 537, "LAG", FS_MIX, "bold", TITLE, "middle"))
svg.append(outlab(349, 531, "LAG", "OUT"))
svg.append(line(52, 531, 81, 531) + line(52, 531, 52, 576) + smalllab(35, 589, "LAG IN", "start"))
svg.append(knob_tick(215, 550, KNOB_ROW_LAG)
           + caption(215, refY(LAG_RATE_CAPTION_BASELINE_CANVAS_Y), "RATE"))
svg.append(section(SECTION_X_LEFT, SECTION_LAG_Y, "LAG", 370, "lag"))

# --- TRACKING GENERATOR
# (whole group +21 canvas px: TRACK X is pinned by RQ-CLR-003, so the group
#  moves to meet it)
svg.append(box(81, 700, 268, 36, "track") + T(215, 723, "TRACKING GENERATOR", FS_BLOCK, "bold", TITLE, "middle"))
svg.append(outlab(349, 717, "TRACK", "OUT"))
svg.append(line(52, 717, 81, 717) + line(52, 717, 52, 762) + smalllab(35, 775, "TRACK IN", "start"))
for i, cx in enumerate([126, 170, 214, 258, 302]):   # PT knob centres (table)
    svg.append(knob_tick(cx, 736, KNOB_ROW_TRACK))
    svg.append(caption(cx, refY(TRACK_PT_CAPTION_BASELINE_CANVAS_Y), f"PT {i+1}"))
svg.append(section(SECTION_X_LEFT, SECTION_TRACK_Y, "TRACK X", 370, "track"))

# ================================================================ CENTER COLUMN
# --- VCF/VCA chain
svg.append(box(525, 45, 186, 26, "vcf") + T(618, 63, "MULTIMODE VCF", FS_BLOCK, "bold", TITLE, "middle"))
svg.append(box(729, 45, 62, 26, "vcf") + T(760, 63, "VCA1", FS_VCA, "bold", TITLE, "middle"))
svg.append(box(804, 45, 62, 26, "vcf") + T(835, 63, "VCA", FS_VCA, "bold", TITLE, "middle"))
svg.append(line(711, 58, 729, 58) + line(791, 58, 804, 58))
svg.append(outlab(866, 58, "VOICE", "OUT"))
for cx in (541, 591, 759, 834):   # FREQ / RES / VCA1 VOLUME / VCA VOLUME
    svg.append(knob_tick(cx, 71, KNOB_ROW_TOP))
svg.append(combo_tick(COMBO_VCF_MODE_CX, 71, COMBO_VCF_MODE_Y))
# The MODE caption moves with its tick: both used to sit at 669, 6.5 px left of
# the combo they belong to. [RQ-GUI-071]
svg.append(caption(541, 137, "FREQ") + caption(591, 137, "RES")
           + caption(caption_centre_x(COMBO_VCF_MODE_CX), 137, "MODE (15)")
           + caption(759, 137, "VOLUME") + caption(834, 137, "VOLUME"))
svg.append(section(SECTION_X_CENTRE, SECTION_VCF_Y, "VCF/VCA", 370, "vcf"))

# --- ENV
svg.append(box(525, 242, 267, 26, "env") + T(658, 260, "ENVELOPE GENERATOR", FS_BLOCK, "bold", TITLE, "middle"))
svg.append(box(804, 242, 63, 26, "env") + T(835, 260, "VCA", FS_VCA, "bold", TITLE, "middle"))
svg.append(line(792, 255, 804, 255))
svg.append(outlab(867, 255, "ENV", "OUT"))
# TRIGGER IN: enters the ENVELOPE block and connects down into the trigger
# frame below, as the reference  [owner v3 point 3]
svg.append(line(514, 255, 525, 255) + line(514, 255, 514, 351) + line(514, 351, 524, 351)
           + smalllab(508, 363, "TRIGGER", "end") + smalllab(508, 373, "IN", "end"))
for cx in [541, 591, 641, 691, 750, 835]:   # knob centres (table)
    svg.append(knob_tick(cx, 268, KNOB_ROW_ENV))
svg.append(caption(541, 320, "DELAY") + caption(591, 320, "ATTACK") + caption(641, 320, "DECAY") + caption(691, 320, "SUSTAIN") + caption(750, 320, "RELEASE") + caption(835, 320, "VOLUME"))
svg.append(box(524, 329, 373, 42))
svg.append(section(SECTION_X_CENTRE, SECTION_ENV_Y, "ENV X", 370, "env"))

# --- LFO
svg.append(box(524, 475, 269, 26, "lfo") + T(658, 493, "LFO", FS_MIX, "bold", TITLE, "middle"))
svg.append(box(804, 475, 63, 26, "lfo") + T(835, 493, "VCA", FS_VCA, "bold", TITLE, "middle"))
svg.append(line(793, 488, 804, 488))
svg.append(outlab(867, 488, "LFO", "OUT"))
for cx in [546, 759, 834]:   # SPEED/RETRIG/AMPLITUDE knob centres (table)
    svg.append(knob_tick(cx, 501, KNOB_ROW_LFO))
# WAVESHAPE is a combo, not a knob: different top inset, and its centre is 661 —
# the loop used to carry it at 657 and call that the combo centre. [RQ-GUI-071]
svg.append(combo_tick(COMBO_LFO_WAVE_CX, 501, COMBO_LFO_WAVE_Y))
svg.append(caption(546, 554, "SPEED")
           + caption(caption_centre_x(COMBO_LFO_WAVE_CX), 554, "WAVESHAPE")
           + caption(759, 554, "RETRIG") + caption(834, 554, "AMPLITUDE"))
svg.append(section(SECTION_X_CENTRE, SECTION_LFO_Y, "LFO X", 370, "lfo"))

# --- RAMP
svg.append(box(524, 658, 266, 26, "ramp") + T(656, 676, "RAMP", FS_MIX, "bold", TITLE, "middle"))
svg.append(outlab(790, 671, "RAMP", "OUT"))
svg.append(line(514, 671, 524, 671) + line(514, 671, 514, 770) + line(514, 770, 524, 770)
           + smalllab(508, 779, "TRIGGER", "end") + smalllab(508, 789, "IN", "end"))
svg.append(knob_tick(657, 684, KNOB_ROW_RAMP))
svg.append(caption(657, 738, "RATE"))
svg.append(box(524, refY(RAMP_TRIGGER_FRAME_TOP_CANVAS_Y), 374, RAMP_TRIGGER_FRAME_HEIGHT))
svg.append(section(SECTION_X_CENTRE, SECTION_RAMP_Y, "RAMP X", 370, "ramp"))

# ================================================================ RIGHT
# MOD MATRIX is the one section whose bar runs alongside a control grid, so BOTH
# its ends come from that grid: it starts on MOD_SRC_n's left edge (960) and
# stops on MOD_QUANTIZE_n's right edge (1206 + 12 = 1218), a width of 258.
# [RQ-GUI-062, RQ-CLR-007, ADR-JUC-034 (DEC-JUC-110)]
svg.append(section(SECTION_X_MATRIX, SECTION_MATRIX_Y, "MOD MATRIX", SECTION_MATRIX_BAR_WIDTH, "matrix"))

# ---------------------------------------------------------------- z-order
# Re-order the diagram out of code order into paint order (see "layering"
# above). Splitting on the marker rather than tracking elements per append
# keeps the geometry section untouched: every element carries its own layer,
# regardless of how many were concatenated into one appended string.
_diagram = "".join(svg[DIAGRAM_START:])
del svg[DIAGRAM_START:]
_layered = {_LINES: [], _BOXES: [], _TEXT: []}
for _element in _diagram.split(_MARK):
    if not _element:
        continue
    _layered[_element[0]].append(_element[1:])
_missing = [k for k, v in _layered.items() if not v]
assert not _missing, f"empty diagram layer(s): {_missing} — an element lost its marker"
for _layer in (_LINES, _BOXES, _TEXT):
    svg.append("".join(_layered[_layer]))

svg.append('</g>')
svg.append('</svg>')
OUT_PATH.write_text("\n".join(svg), encoding="utf-8", newline="\n")
print(f"SVG written to {OUT_PATH}: "
      f"{len(_layered[_LINES])} lines, {len(_layered[_BOXES])} boxes, {len(_layered[_TEXT])} text elements.")
