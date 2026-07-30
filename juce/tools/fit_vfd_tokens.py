#!/usr/bin/env python3
"""Derive the VFD rendering parameters by fitting them against the baseline
sprite sheet, so the design tokens are measured rather than eyeballed.

`juce/app/assets/vfd-matrix.png` is the artwork the .NET editor shipped. It is
no longer a runtime asset (ADR-JUC-023 replaced the blit with vector segments)
but it is kept as the **reference of record**: the .NET tree that produced it
has moved to XplorerEditor-dotnet-archive, so this file is the only surviving
witness of the original look. This tool measures it and produces the token
values the renderer consumes (RQ-DSN-097, DEC-JUC-054, DEC-JUC-056).

Units
-----
Every length is emitted as a fraction of the glyph cell **width**. The cell is
12x16, so normalised y runs 0..4/3 rather than 0..1. That is deliberate: it
keeps the unit cell isotropic, letting the renderer apply one uniform scale
without turning round glow into ellipses. Normalising x and y independently
would have been tidier to read and wrong to draw.

Modes
-----
    --fit      run the optimisation and print the parameter set (slow, and a
               developer action, not a CI gate)
    --emit     print the YAML token block ready for design-tokens.yaml
    --check    evaluate the values COMMITTED in design-tokens.yaml against the
               baseline and fail if the error regressed past the budget

`--check` deliberately does NOT re-run the fit. A coordinate descent is not
reproducible to the last digit across versions of numpy or of this file, so
gating on "does the fit still land here" would be flaky. Gating on "do the
committed values still render this close to the reference" is deterministic and
is what actually matters.

Prerequisites: numpy, Pillow, PyYAML.

[RQ-GUI-033, RQ-DSN-097, ADR-JUC-023 (DEC-JUC-054, DEC-JUC-056)]
"""
from __future__ import annotations

import argparse
import pathlib
import sys

try:
    import numpy as np
    from PIL import Image, ImageDraw
except ImportError:  # pragma: no cover
    sys.exit("numpy and Pillow are required: pip install numpy pillow")

try:
    import yaml
except ImportError:  # pragma: no cover
    sys.exit("PyYAML is required: pip install pyyaml")

from generate_segment_font import FIRST_GLYPH, LAST_GLYPH, VENDOR_PATH, parse_vendor

HERE = pathlib.Path(__file__).resolve().parent
BASELINE_PATH = HERE.parent / "app" / "assets" / "vfd-matrix.png"
YAML_PATH = HERE / "design-tokens.yaml"

CELL_W, CELL_H = 12, 16          # baseline sheet cell, in sheet pixels
SHEET_COLUMNS = 32               # glyphs per sheet row
BLANK_EPSILON = 0.5              # mean-level delta below which a cell is blank
CHANNEL_MAX = 255.0

# Segment order of the vendored table: bit 0 = A .. bit 15 = U.
SEGMENT_ORDER = ["A", "B", "C", "D", "E", "F", "G", "H",
                 "K", "M", "N", "P", "R", "S", "T", "U"]

# Free parameters, with the bounds the search may explore. Lengths are in sheet
# pixels here and normalised on output; keeping the search in pixel space lets
# the bounds be reasoned about against the 12x16 reference directly.
SEED = dict(xl=3.5, xr=9.5, yt=1.0, yb=11.0, slant=0.10, stroke=1.15, gap=0.55,
            glow=0.60, glowAmount=0.40, lift=0.55, blue=184.0)
BOUNDS = dict(xl=(1.5, 5.0), xr=(7.5, 11.0), yt=(0.0, 3.0), yb=(9.0, 14.0),
              slant=(0.0, 0.30), stroke=(0.6, 2.2), gap=(0.0, 1.4),
              glow=(0.20, 4.0), glowAmount=(0.0, 2.0),
              lift=(0.0, 1.5), blue=(140.0, 220.0))

FIT_PASSES = 6
FIT_SUPERSAMPLE = 8
VERIFY_SUPERSAMPLE = 16
TOKEN_PRECISION = 5    # decimals kept when emitting normalised token values
# Error budget for --check, in 0..255 units. Set from the value the fit reaches
# with a little headroom: the residual is dominated by glyphs where the vendored
# table legitimately differs from the hand-drawn sheet, not by the filters.
RMSE_BUDGET = 26.0


# --------------------------------------------------------------------------
# geometry
# --------------------------------------------------------------------------

def segments(p):
    """Endpoints of the 16 segments, in sheet pixels, before shear."""
    xl, xr, yt, yb = p["xl"], p["xr"], p["yt"], p["yb"]
    xc, ym = (xl + xr) / 2.0, (yt + yb) / 2.0
    return {
        "A": ((xl, yt), (xc, yt)), "B": ((xc, yt), (xr, yt)),
        "C": ((xr, yt), (xr, ym)), "D": ((xr, ym), (xr, yb)),
        "E": ((xc, yb), (xr, yb)), "F": ((xl, yb), (xc, yb)),
        "G": ((xl, ym), (xl, yb)), "H": ((xl, yt), (xl, ym)),
        "K": ((xl, yt), (xc, ym)), "M": ((xc, yt), (xc, ym)),
        "N": ((xr, yt), (xc, ym)), "P": ((xc, ym), (xr, ym)),
        "R": ((xr, yb), (xc, ym)), "S": ((xc, ym), (xc, yb)),
        "T": ((xl, yb), (xc, ym)), "U": ((xl, ym), (xc, ym)),
    }


def core_mask(mask_bits, p, supersample):
    """Crisp 0..1 coverage of the lit segments, supersampled then area-averaged."""
    width, height = CELL_W * supersample, CELL_H * supersample
    image = Image.new("L", (width, height), 0)
    draw = ImageDraw.Draw(image)
    geometry = segments(p)
    for bit, name in enumerate(SEGMENT_ORDER):
        if not (mask_bits >> bit) & 1:
            continue
        (x0, y0), (x1, y1) = geometry[name]
        dx, dy = x1 - x0, y1 - y0
        length = (dx * dx + dy * dy) ** 0.5
        ux, uy = dx / length, dy / length
        gap = min(p["gap"], length / 2.2)   # never eat a whole short segment
        ax, ay = x0 + ux * gap, y0 + uy * gap
        bx, by = x1 - ux * gap, y1 - uy * gap
        ax += p["slant"] * (p["yb"] - ay)   # italic shear, pivoting on the baseline
        bx += p["slant"] * (p["yb"] - by)
        draw.line([(ax * supersample, ay * supersample),
                   (bx * supersample, by * supersample)],
                  fill=255, width=max(1, int(round(p["stroke"] * supersample))))
    small = image.resize((CELL_W, CELL_H), Image.BOX)
    return np.asarray(small, np.float64) / CHANNEL_MAX


# --------------------------------------------------------------------------
# photometry
# --------------------------------------------------------------------------

def gaussian_kernel(sigma):
    radius = max(1, int(np.ceil(3 * sigma)))
    x = np.arange(-radius, radius + 1, dtype=np.float64)
    kernel = np.exp(-(x ** 2) / (2 * sigma ** 2))
    return kernel / kernel.sum()


def blur(a, sigma):
    """Separable Gaussian, zero-padded.

    Not np.convolve(mode='same'): at the fitted radius the kernel is wider than
    the 12-pixel cell, and that mode then silently returns the wrong length
    instead of failing.
    """
    kernel = gaussian_kernel(sigma)
    radius = len(kernel) // 2
    out = a
    for axis in (0, 1):
        out = np.moveaxis(out, axis, 0)
        padded = np.zeros((out.shape[0] + 2 * radius,) + out.shape[1:])
        padded[radius:radius + out.shape[0]] = out
        acc = np.zeros_like(out)
        for index, weight in enumerate(kernel):
            acc += weight * padded[index:index + out.shape[0]]
        out = np.moveaxis(acc, 0, axis)
    return out


def photometry(core, ghost, p, unlit, scale=1.0):
    """Lit-coverage mask -> RGB 0..255, the reference's light behaviour.

    Split out of render() so an alternative geometry can reuse the photometry
    unchanged (see tools/prototypes/); the renderer's own behaviour is defined
    by DEC-JUC-054 and this is its executable statement.

    The reference's red channel is 0 through the whole halo and only rises at
    the saturated core: that is one hue accumulating past 1.0 and washing out
    to white, not two colours. So intensity is accumulated as a scalar field
    and turned into colour exactly once.

    One glow component, not two. A two-component glow was tried — the falloff
    measured on an isolated glyph looked too wide-tailed for one Gaussian — but
    fitted over the whole drawn set the two radii converge (2.10 vs 2.59 px),
    which is what an over-parameterised model looks like. It bought 0.53 RMSE
    for twice the blur cost, with no visible difference. Dropped.

    `scale` multiplies the glow radius: the radius is a fraction of the cell,
    so a caller rendering at N times the reference resolution must pass N or
    the halo shrinks visually as the window grows (DEC-JUC-053).
    """
    field = core + p["glowAmount"] * blur(core, p["glow"] * scale)
    field = np.maximum(field, ghost * unlit)
    base = np.array([0.0, CHANNEL_MAX, p["blue"]]) / CHANNEL_MAX
    below = np.clip(field, 0, 1)[..., None] * base
    above = np.clip(field - 1.0, 0, None)[..., None]
    return np.clip(below + p["lift"] * above * (1.0 - base), 0, 1) * CHANNEL_MAX


def render(mask_bits, p, ghost, unlit, supersample):
    """One glyph cell as RGB 0..255, at the baseline sheet's own resolution."""
    return photometry(core_mask(mask_bits, p, supersample), ghost, p, unlit)


# --------------------------------------------------------------------------
# the baseline
# --------------------------------------------------------------------------

def baseline_cells():
    """Every printable cell of the sheet, keyed by character."""
    sheet = np.asarray(Image.open(BASELINE_PATH).convert("RGB"), np.float64)
    cells = {}
    for code_point in range(FIRST_GLYPH, LAST_GLYPH + 1):
        index = code_point - FIRST_GLYPH
        x = (index % SHEET_COLUMNS) * CELL_W
        y = (index // SHEET_COLUMNS) * CELL_H
        cells[chr(code_point)] = sheet[y:y + CELL_H, x:x + CELL_W]
    return cells


def drawn_characters(cells):
    """The characters the sheet actually draws.

    Detected, never hard-coded: a blank cell is byte-identical to the space
    cell, so any list written down here would rot the moment the baseline did.
    """
    blank_level = cells[" "][:, :, 1].mean()
    return [ch for ch, cell in cells.items()
            if ch != " " and abs(cell[:, :, 1].mean() - blank_level) >= BLANK_EPSILON]


def measured_unlit(cells):
    """Unlit-segment level, read off the sheet's own blank cell.

    Measured, not fitted. Left free, the search trades it against the glow and
    parks it at its bound, which visibly over-brightens the unlit bed.
    """
    return cells[" "][:, :, 1].max() / CHANNEL_MAX


# --------------------------------------------------------------------------
# fitting
# --------------------------------------------------------------------------

def rmse(p, masks, cells, characters, unlit, supersample):
    ghost = core_mask(0xFFFF, p, supersample)
    total, count = 0.0, 0
    for ch in characters:
        got = render(masks[ord(ch) - FIRST_GLYPH], p, ghost, unlit, supersample)
        target = cells[ch]
        total += ((got - target) ** 2).sum()
        count += target.size
    return (total / count) ** 0.5


def fit(masks, cells, characters, unlit, verbose=True):
    p = dict(SEED)
    best = rmse(p, masks, cells, characters, unlit, FIT_SUPERSAMPLE)
    if verbose:
        print(f"  seed RMSE = {best:6.2f}")
    for iteration in range(FIT_PASSES):
        improved = False
        for key in SEED:
            low, high = BOUNDS[key]
            span = (high - low) * (0.28 / (iteration + 1))
            for delta in (+span, -span, +span / 3, -span / 3):
                trial = dict(p)
                trial[key] = float(np.clip(p[key] + delta, low, high))
                if trial[key] == p[key]:
                    continue
                score = rmse(trial, masks, cells, characters, unlit, FIT_SUPERSAMPLE)
                if score < best - 1e-4:
                    p, best, improved = trial, score, True
        if verbose:
            print(f"  pass {iteration + 1}: RMSE = {best:6.2f}")
        if not improved:
            break
    return p, best


def report_bounds(p):
    """Parameters sitting on a bound — the fit wanted to go further."""
    stuck = []
    for key, value in p.items():
        low, high = BOUNDS[key]
        if abs(value - low) < 1e-6 or abs(value - high) < 1e-6:
            stuck.append(key)
    return stuck


# --------------------------------------------------------------------------
# token emission
# --------------------------------------------------------------------------

def normalise(p, unlit):
    """Fitted pixel-space parameters -> cell-width fractions (RQ-DSN-097)."""
    def frac(value):
        return round(value / CELL_W, TOKEN_PRECISION)

    def plain(value):
        return round(value, TOKEN_PRECISION)

    return {
        "vfdSegLeft": (frac(p["xl"]), "left rail, fraction of cell width"),
        "vfdSegRight": (frac(p["xr"]), "right rail"),
        "vfdSegTop": (frac(p["yt"]), "top rail (y is also over cell WIDTH: the unit cell keeps the 12:16 aspect so one uniform scale stays isotropic)"),
        "vfdSegBottom": (frac(p["yb"]), "bottom rail / shear pivot"),
        "vfdSegStroke": (frac(p["stroke"]), "segment thickness"),
        "vfdSegGap": (frac(p["gap"]), "shortening at each segment end"),
        "vfdSegSlant": (plain(p["slant"]), "italic shear, x per unit y (dimensionless)"),
        "vfdGlowRadius": (frac(p["glow"]), "glow sigma; scale it with the render scale or the halo shrinks as the window grows"),
        "vfdGlowAmount": (plain(p["glowAmount"]), "glow amplitude added over the core"),
        "vfdUnlitLevel": (plain(unlit), "unlit segment bed; MEASURED off the baseline blank cell, not fitted"),
        "vfdWhiteLift": (plain(p["lift"]), "how fast the core washes to white once the hue clips"),
    }


def emit_yaml(p, unlit, error):
    tokens = normalise(p, unlit)
    blue = int(round(p["blue"]))
    lines = [
        "    # ---- VFD 16-segment display (RQ-GUI-033, RQ-DSN-097, ADR-JUC-023) ----",
        "    # Derived by juce/tools/fit_vfd_tokens.py against the baseline sheet",
        f"    # juce/app/assets/vfd-matrix.png; RMSE {error:.2f}/255 over the glyphs it draws.",
        "    # Lengths are fractions of the glyph cell WIDTH; see the tool's docstring.",
        f'    vfdPhosphor: {{ kind: colour, hex: "00FF{blue:02X}", note: "phosphor hue; red stays 0 until the core clips" }}',
    ]
    for name, (value, note) in tokens.items():
        lines.append(f'    {name}: {{ kind: scalar, value: {value}, note: "{note}" }}')
    return "\n".join(lines)


def committed_tokens():
    """The VFD token values currently in design-tokens.yaml, or None."""
    data = yaml.safe_load(YAML_PATH.read_text(encoding="utf-8"))
    global_tier = data.get("tiers", {}).get("global", {})
    if "vfdSegLeft" not in global_tier:
        return None
    values = {}
    for key, entry in global_tier.items():
        if key.startswith("vfd") and isinstance(entry, dict) and "value" in entry:
            values[key] = float(entry["value"])
        elif key == "vfdPhosphor" and isinstance(entry, dict):
            values[key] = entry["hex"]
    return values


def denormalise(tokens):
    """Committed cell-fraction tokens -> the pixel-space dict the model uses."""
    def px(name):
        return tokens[name] * CELL_W
    phosphor = tokens["vfdPhosphor"]
    p = dict(
        xl=px("vfdSegLeft"), xr=px("vfdSegRight"),
        yt=px("vfdSegTop"), yb=px("vfdSegBottom"),
        stroke=px("vfdSegStroke"), gap=px("vfdSegGap"),
        slant=tokens["vfdSegSlant"],
        glow=px("vfdGlowRadius"), glowAmount=tokens["vfdGlowAmount"],
        lift=tokens["vfdWhiteLift"],
        blue=float(int(phosphor[4:6], 16)),
    )
    return p, tokens["vfdUnlitLevel"]


# --------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--fit", action="store_true", help="run the optimisation (slow)")
    mode.add_argument("--emit", action="store_true", help="fit, then print the YAML block")
    mode.add_argument("--check", action="store_true",
                      help="score the committed tokens against the baseline")
    args = parser.parse_args()

    masks = [mask for mask, _ in parse_vendor(VENDOR_PATH)]
    cells = baseline_cells()
    characters = drawn_characters(cells)
    unlit = measured_unlit(cells)

    print(f"baseline   : {BASELINE_PATH.relative_to(HERE.parent.parent)}")
    print(f"vendored   : {VENDOR_PATH.name} ({len(masks)} masks)")
    print(f"drawn cells: {len(characters)} of {LAST_GLYPH - FIRST_GLYPH + 1} "
          f"(the rest are blank in the baseline)")
    print(f"unlit level: {unlit:.4f}  (measured off the blank cell)")

    if args.check:
        tokens = committed_tokens()
        if tokens is None:
            print("\nNo VFD tokens in design-tokens.yaml yet (TASK-VFD-003 adds them).")
            return 0
        p, committed_unlit = denormalise(tokens)
        error = rmse(p, masks, cells, characters, committed_unlit, VERIFY_SUPERSAMPLE)
        print(f"\ncommitted tokens RMSE = {error:.2f}/255 (budget {RMSE_BUDGET:.2f})")
        if error > RMSE_BUDGET:
            print("REGRESSED past the budget", file=sys.stderr)
            return 1
        print("within budget")
        return 0

    if not (args.fit or args.emit):
        parser.print_help()
        return 0

    print("\nfitting:")
    p, error = fit(masks, cells, characters, unlit)
    verify = rmse(p, masks, cells, characters, unlit, VERIFY_SUPERSAMPLE)
    print(f"\nRMSE {error:.2f} (fit) / {verify:.2f} (verify at higher supersample)")

    stuck = report_bounds(p)
    if stuck:
        print(f"WARNING: parameters resting on a bound: {', '.join(sorted(stuck))}")
        print("         the fit wanted to go further — widen BOUNDS and rerun")

    if args.emit:
        print("\n" + emit_yaml(p, unlit, verify))
    else:
        print()
        for key in sorted(p):
            print(f"  {key:8s} = {p[key]:9.4f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
