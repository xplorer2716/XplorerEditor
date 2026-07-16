#!/usr/bin/env python3
"""Vector background mockup for Xplorer (proposal for replacing
main-background.jpg). Geometry measured from the reference bitmap
(line detection) + control-table knob anchors. Produces an SVG whose
primitives map 1:1 to juce::Graphics calls."""
import random

W, H = 1260, 813
random.seed(42)

FRAME = "#B7BDD0"      # block frame / line colour
TITLE = "#F2F2F6"      # bold titles
CAPTION = "#C9CACE"    # parameter captions
LW = 2                 # line width

svg = []
svg.append(f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}">')

# ---------------------------------------------------------------- defs
svg.append('''<defs>
  <linearGradient id="metal" x1="0" y1="0" x2="0" y2="1">
    <stop offset="0" stop-color="#45464F"/>
    <stop offset="0.25" stop-color="#3B3C44"/>
    <stop offset="0.6" stop-color="#36363E"/>
    <stop offset="1" stop-color="#303138"/>
  </linearGradient>
  <linearGradient id="wood" x1="0" y1="0" x2="1" y2="0">
    <stop offset="0" stop-color="#4A1D08"/>
    <stop offset="0.25" stop-color="#7C3615"/>
    <stop offset="0.55" stop-color="#8A431C"/>
    <stop offset="0.85" stop-color="#6B2C0F"/>
    <stop offset="1" stop-color="#38160A"/>
  </linearGradient>
  <linearGradient id="bluebar" x1="0" y1="0" x2="0" y2="1">
    <stop offset="0" stop-color="#3050B8"/>
    <stop offset="0.5" stop-color="#24388A"/>
    <stop offset="1" stop-color="#1A2A66"/>
  </linearGradient>
</defs>''')

# ---------------------------------------------------------------- metal + brushed streaks
svg.append(f'<rect x="0" y="0" width="{W}" height="{H}" fill="url(#metal)"/>')
streaks = []
for _ in range(220):
    y = random.uniform(0, H)
    x0 = random.uniform(-100, W)
    ln = random.uniform(120, 700)
    op = random.uniform(0.015, 0.05)
    col = "#FFFFFF" if random.random() < 0.5 else "#000000"
    streaks.append(f'<line x1="{x0:.0f}" y1="{y:.1f}" x2="{x0+ln:.0f}" y2="{y:.1f}" stroke="{col}" stroke-opacity="{op:.3f}" stroke-width="1"/>')
svg.append("".join(streaks))
# top dark strip (menu shadow)
svg.append(f'<rect x="0" y="0" width="{W}" height="14" fill="#17181C"/>')
svg.append(f'<rect x="0" y="14" width="{W}" height="2" fill="#000000" fill-opacity="0.25"/>')

# ---------------------------------------------------------------- wood rails
def wood(x):
    parts = [f'<rect x="{x}" y="0" width="28" height="{H}" fill="url(#wood)"/>']
    for _ in range(90):  # grain
        gx = x + random.uniform(2, 26)
        gy = random.uniform(0, H)
        ln = random.uniform(30, 160)
        parts.append(f'<path d="M{gx:.1f} {gy:.0f} q {random.uniform(-2,2):.1f} {ln/2:.0f} 0 {ln:.0f}" stroke="#2E1206" stroke-opacity="{random.uniform(0.15,0.4):.2f}" stroke-width="{random.uniform(0.5,1.6):.1f}" fill="none"/>')
    parts.append(f'<rect x="{x}" y="0" width="2" height="{H}" fill="#000000" fill-opacity="0.45"/>')
    parts.append(f'<rect x="{x+26}" y="0" width="2" height="{H}" fill="#000000" fill-opacity="0.45"/>')
    return "".join(parts)
svg.append(wood(0))
svg.append(wood(W - 28))

# ---------------------------------------------------------------- helpers
def box(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="2" fill="none" stroke="{FRAME}" stroke-width="{LW}"/>'
def line(x1, y1, x2, y2):
    return f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="{FRAME}" stroke-width="{LW}" stroke-linecap="square"/>'
def stub(cx, y, ln=12):
    return line(cx, y, cx, y + ln)
def T(x, y, s, size=15, w="bold", fill=TITLE, anchor="start", ls="0.5"):
    return (f'<text x="{x}" y="{y}" font-family="Arial, Helvetica, sans-serif" font-size="{size}" '
            f'font-weight="{w}" fill="{fill}" text-anchor="{anchor}" letter-spacing="{ls}">{s}</text>')
def caption(x, y, s):
    return T(x, y, s, 12, "normal", CAPTION, "middle", "0.5")
def section(x, y, s, barw):
    return T(x, y - 7, s, 15, "bold") + f'<rect x="{x}" y="{y}" width="{barw}" height="4.5" fill="url(#bluebar)"/>'
def outlab(x, y, s1, s2):
    return (line(x, y, x + 14, y) +
            T(x + 19, y - 2, s1, 11, "bold") + T(x + 19, y + 10, s2, 11, "bold"))
def smalllab(x, y, s, anchor="end"):
    return T(x, y, s, 9, "bold", CAPTION, anchor, "0.3")

# ================================================================ LEFT COLUMN
# --- VCO1 group
svg.append(box(51, 32, 147, 52) + T(64, 63, "VCO1", 16))
for i, (wave, y) in enumerate([("TRIANGLE", 45), ("SAWTOOTH", 57), ("PULSE", 70)]):
    svg.append(T(193, y + 4, wave, 11.5, "bold", TITLE, "end"))
svg.append(line(198, 45, 330, 45) + line(198, 57, 330, 57))          # tri, saw
svg.append(line(198, 70, 234, 70) + line(286, 70, 330, 70))          # pulse via PWM
svg.append(box(234, 60, 52, 23) + T(260, 76, "PWM", 12.5, "bold", TITLE, "middle") + stub(259, 83))
svg.append(box(330, 32, 53, 52) + T(356, 63, "MIX", 14, "bold", TITLE, "middle"))
svg.append(line(383, 58, 405, 58))
svg.append(box(405, 45, 53, 26) + T(431, 63, "VCA", 13, "bold", TITLE, "middle") + stub(431, 71, 23))
svg.append(stub(82, 84) + stub(170, 84))
svg.append(caption(82, 137, "FREQUENCY") + caption(170, 137, "DETUNE") + caption(259, 137, "PULSE WIDTH") + caption(431, 137, "VOLUME"))
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
svg.append(f'<path d="M492 180 A 7 7 0 0 1 506 180" fill="none" stroke="{FRAME}" stroke-width="{LW}"/>')
svg.append(line(506, 180, 513, 180) + line(513, 180, 513, 82) + line(513, 82, 541, 82))

# --- FM / VCO2 group
svg.append(box(51, 210, 102, 36) + T(102, 233, "FM VCA", 13.5, "bold", TITLE, "middle"))
svg.append(box(184, 210, 90, 52))
svg.append(T(229, 274, "DESTINATION", 9, "bold", CAPTION, "middle", "0.3"))
svg.append(line(153, 228, 184, 228))
svg.append(stub(106, 246) + caption(106, 300, "FM AMPLITUDE"))
svg.append(box(329, 210, 52, 52) + T(355, 241, "MIX", 14, "bold", TITLE, "middle"))
svg.append(line(381, 232, 405, 232))
svg.append(box(405, 220, 53, 26) + T(431, 238, "VCA", 13, "bold", TITLE, "middle") + stub(431, 246, 24))
# VCO2-row VCA out -> right, then up at x=499 into the VCF  [owner point 2]
svg.append(line(458, 232, 499, 232) + line(499, 232, 499, 70))
svg.append(f'<path d="M499 70 Q499 58 509 58" fill="none" stroke="{FRAME}" stroke-width="{LW}"/>')
svg.append(caption(431, 314, "VOLUME"))
svg.append(box(51, 310, 147, 52) + T(64, 341, "VCO2", 16))
for wave, y in [("TRIANGLE", 320), ("SAWTOOTH", 334), ("PULSE", 348)]:
    svg.append(T(193, y + 4, wave, 11.5, "bold", TITLE, "end"))
# vco2 waves route up into MIX
svg.append(line(198, 320, 297, 320) + line(297, 320, 297, 228) + line(297, 228, 329, 228))
svg.append(line(198, 334, 303, 334) + line(303, 334, 303, 237) + line(303, 237, 329, 237))
svg.append(line(198, 348, 233, 348) + box(233, 340, 52, 23) + T(259, 356, "PWM", 12.5, "bold", TITLE, "middle") + stub(259, 363))
svg.append(line(285, 348, 309, 348) + line(309, 348, 309, 246) + line(309, 246, 329, 246))
svg.append(f'<text x="318" y="300" font-family="Arial" font-size="9" font-weight="bold" fill="{CAPTION}" transform="rotate(-90 318 300)" letter-spacing="0.3">NOISE</text>')
svg.append(line(317, 255, 329, 255) + line(317, 255, 317, 270))
svg.append(stub(82, 362) + stub(170, 362))
svg.append(caption(82, 418, "FREQUENCY") + caption(170, 418, "DETUNE") + caption(259, 418, "PULSE WIDTH"))
# FM carrier path: VCO2 TRIANGLE line taps up at x=204 to the y=305 run,
# which feeds the left bus into the FM VCA input  [owner point 3]
svg.append(line(40, 229, 40, 305) + line(40, 229, 51, 229) + line(40, 305, 204, 305))
svg.append(line(204, 305, 204, 320))
svg.append(section(53, 487, "VCO1/VCO2/FM", 370))

# --- LAG
svg.append(box(81, 501, 268, 36) + T(215, 524, "LAG", 14, "bold", TITLE, "middle"))
svg.append(outlab(349, 518, "LAG", "OUT"))
svg.append(line(52, 518, 81, 518) + line(52, 518, 52, 563) + smalllab(62, 576, "LAG IN"))
svg.append(stub(215, 537) + caption(215, 590, "RATE"))
svg.append(section(53, 629, "LAG", 370))

# --- TRACKING GENERATOR
svg.append(box(81, 679, 268, 36) + T(215, 702, "TRACKING GENERATOR", 13.5, "bold", TITLE, "middle"))
svg.append(outlab(349, 696, "TRACK", "OUT"))
svg.append(line(52, 696, 81, 696) + line(52, 696, 52, 741) + smalllab(66, 766, "TRACK IN"))
for i, cx in enumerate([123, 166, 209, 252, 295]):   # PT knob centres
    svg.append(stub(cx, 715))
    svg.append(caption(cx, 766, f"PT {i+1}"))
svg.append(section(53, 799, "TRACK X", 370))

# ================================================================ CENTER COLUMN
# --- VCF/VCA chain
svg.append(box(525, 45, 186, 26) + T(618, 63, "MULTIMODE VCF", 13.5, "bold", TITLE, "middle"))
svg.append(box(729, 45, 62, 26) + T(760, 63, "VCA1", 13, "bold", TITLE, "middle"))
svg.append(box(804, 45, 62, 26) + T(835, 63, "VCA", 13, "bold", TITLE, "middle"))
svg.append(line(711, 58, 729, 58) + line(791, 58, 804, 58))
svg.append(outlab(866, 58, "VOICE", "OUT"))
for cx, ln in [(541, 24), (591, 24), (669, 24), (759, 24), (834, 24)]:
    svg.append(stub(cx, 71, ln))
svg.append(caption(541, 137, "FREQ") + caption(591, 137, "RES") + caption(669, 137, "MODE (15)") + caption(759, 137, "VOLUME") + caption(834, 137, "VOLUME"))
svg.append(section(526, 194, "VCF/VCA", 370))

# --- ENV
svg.append(box(525, 242, 267, 26) + T(658, 260, "ENVELOPE GENERATOR", 13.5, "bold", TITLE, "middle"))
svg.append(box(804, 242, 63, 26) + T(835, 260, "VCA", 13, "bold", TITLE, "middle"))
svg.append(line(792, 255, 804, 255))
svg.append(outlab(867, 255, "ENV", "OUT"))
# TRIGGER IN: enters the ENVELOPE block and connects down into the trigger
# frame below, as the reference  [owner v3 point 3]
svg.append(line(514, 255, 525, 255) + line(514, 255, 514, 351) + line(514, 351, 524, 351)
           + smalllab(508, 363, "TRIGGER", "end") + smalllab(508, 373, "IN", "end"))
for cx in [541, 591, 640, 690, 749, 834]:
    svg.append(stub(cx, 268))
svg.append(caption(541, 320, "DELAY") + caption(591, 320, "ATTACK") + caption(640, 320, "DECAY") + caption(690, 320, "SUSTAIN") + caption(749, 320, "RELEASE") + caption(834, 320, "VOLUME"))
svg.append(box(524, 329, 373, 42))
svg.append(section(526, 416, "ENV X", 370))

# --- LFO
svg.append(box(524, 467, 269, 26) + T(658, 485, "LFO", 14, "bold", TITLE, "middle"))
svg.append(box(804, 467, 63, 26) + T(835, 485, "VCA", 13, "bold", TITLE, "middle"))
svg.append(line(793, 480, 804, 480))
svg.append(outlab(867, 480, "LFO", "OUT"))
for cx in [545, 657, 758, 834]:
    svg.append(stub(cx, 493))
svg.append(caption(545, 546, "SPEED") + caption(657, 546, "WAVESHAPE") + caption(758, 546, "RETRIG") + caption(834, 546, "AMPLITUDE"))
svg.append(section(527, 597, "LFO X", 370))

# --- RAMP
svg.append(box(524, 646, 266, 26) + T(656, 664, "RAMP", 14, "bold", TITLE, "middle"))
svg.append(outlab(790, 659, "RAMP", "OUT"))
svg.append(line(514, 659, 524, 659) + line(514, 659, 514, 758) + line(514, 758, 524, 758)
           + smalllab(508, 767, "TRIGGER", "end") + smalllab(508, 777, "IN", "end"))
svg.append(stub(657, 672))
svg.append(caption(657, 726, "RATE"))
svg.append(box(524, 734, 374, 41))
svg.append(section(527, 799, "RAMP X", 370))

# ================================================================ RIGHT
svg.append(section(958, 799, "MODULATION MATRIX", 268))

svg.append('</svg>')
open('/tmp/claude-0/-home-user-XplorerEditor/4ca34552-3b89-5447-9e4f-4b96a4375123/scratchpad/vector-bg.svg', 'w').write("\n".join(svg))
print("SVG written")
