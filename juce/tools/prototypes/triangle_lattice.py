#!/usr/bin/env python3
"""PROTOTYPE — rejected alternative, kept on purpose. Not part of any build.

ADR-JUC-023 evaluated a "triangle lattice" VFD against an owner-supplied mockup
and rejected it in favour of plain vector 16-segment rendering. Its Alternatives
section states that the skeleton stays reusable if the option is ever
revisited — this file is what makes that statement true. Nothing imports it,
no CMake target references it, and it is not exercised by CI.

The model
--------
The glyph body is covered by an Nx x Ny lattice; each cell is split by BOTH
diagonals into four triangles (N/E/S/W). A glyph is the set of lit triangles,
obtained by rasterising the SAME 16-segment skeleton the shipped renderer uses
and lighting every triangle covered past a threshold. That yields, for all 95
glyphs and without authoring any artwork:

  - strokes tiled along their length      (chained square tiles)
  - triangular stroke terminals           (a half-covered end cell)
  - 45-degree chamfers at corners
  - a faintly visible unlit lattice

The one non-obvious part is `cell_polygons`: contiguous lit quadrants of a cell
must be merged into a single polygon before drawing. Without that merge every
triangle reads individually and the result is a diamond texture, not the tiled
strokes of the mockup. With it, a straight stroke is a chain of squares and
only the ends and corners stay triangular.

Why it was rejected (recorded here so the next reader does not re-derive it):
the tiling is illegible below roughly three times the current cell size, it is
a reinterpretation rather than a restoration of the Xpander display, and unlike
option A its fidelity cannot be measured — the mockup is a generated image with
no ground truth to fit against.

Geometry, photometry and the segment table are imported from the shipped tools,
never copied, so this prototype follows any change made there.

Usage:
    python3 triangle_lattice.py [--out PATH] [--text "..."] [--scale N]

[ADR-JUC-023 (Alternatives), TASK-VFD-009]
"""
from __future__ import annotations

import argparse
import pathlib
import sys

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent))

from fit_vfd_tokens import (  # noqa: E402
    CELL_H, CELL_W, SEED, VENDOR_PATH, baseline_cells, blur, drawn_characters,
    measured_unlit, parse_vendor, photometry, segments,
)
from generate_segment_font import FIRST_GLYPH  # noqa: E402

# Lattice. ODD counts so the mid rails (xc, ym) land on a cell CENTRE, and the
# rails sit on cell centres rather than boundaries: a stroke is then exactly one
# cell wide and fills a clean chain of tiles instead of straddling two columns.
LAT_NX, LAT_NY = 7, 11
COVER_THRESHOLD = 0.40   # a triangle lights above this area coverage
TILE_INSET = 0.24        # tile shrink toward its centroid -> the dark grout
GHOST_LEVEL = 0.10       # unlit lattice, relative to the measured unlit level
COVERAGE_SUPERSAMPLE = 8
QUADRANTS = 4

SEGMENT_ORDER = ["A", "B", "C", "D", "E", "F", "G", "H",
                 "K", "M", "N", "P", "R", "S", "T", "U"]


def lattice_metrics(p):
    """Cell size and the origin of cell (0,0), with rails on cell centres."""
    cw = (p["xr"] - p["xl"]) / (LAT_NX - 1)
    ch = (p["yb"] - p["yt"]) / (LAT_NY - 1)
    return cw, ch, p["xl"] - cw / 2.0, p["yt"] - ch / 2.0


def stroke_mask(mask_bits, p, supersample):
    """Coverage of the raw 16-segment skeleton, unsheared (shear is applied to
    the tiles at draw time instead, so coverage stays computable on the grid)."""
    image = Image.new("L", (CELL_W * supersample, CELL_H * supersample), 0)
    draw = ImageDraw.Draw(image)
    geometry = segments(p)
    for bit, name in enumerate(SEGMENT_ORDER):
        if not (mask_bits >> bit) & 1:
            continue
        (x0, y0), (x1, y1) = geometry[name]
        dx, dy = x1 - x0, y1 - y0
        length = (dx * dx + dy * dy) ** 0.5
        ux, uy = dx / length, dy / length
        gap = min(p["gap"], length / 2.2)
        draw.line([((x0 + ux * gap) * supersample, (y0 + uy * gap) * supersample),
                   ((x1 - ux * gap) * supersample, (y1 - uy * gap) * supersample)],
                  fill=255, width=max(1, int(round(p["stroke"] * supersample))))
    return np.asarray(image, np.float64) / 255.0


def triangle_index_map(p, supersample):
    """Per-pixel triangle id over the body box. Glyph-independent: built once."""
    cw, ch, ox, oy = lattice_metrics(p)
    px = (np.arange(CELL_W * supersample) + 0.5) / supersample
    py = (np.arange(CELL_H * supersample) + 0.5) / supersample
    X, Y = np.meshgrid(px, py)
    i = np.floor((X - ox) / cw).astype(int)
    j = np.floor((Y - oy) / ch).astype(int)
    inside = (i >= 0) & (i < LAT_NX) & (j >= 0) & (j < LAT_NY)
    ic, jc = np.clip(i, 0, LAT_NX - 1), np.clip(j, 0, LAT_NY - 1)
    # normalised offset from the cell centre -> quadrant across both diagonals
    u = (X - (ox + (ic + 0.5) * cw)) / (cw / 2.0)
    v = (Y - (oy + (jc + 0.5) * ch)) / (ch / 2.0)
    quad = np.where(np.abs(u) <= -v, 0,
                    np.where(np.abs(v) <= u, 1,
                             np.where(np.abs(u) <= v, 2, 3)))
    return np.where(inside, (jc * LAT_NX + ic) * QUADRANTS + quad, -1)


def cell_polygons(p, lit):
    """Union the lit quadrants of each cell into as few polygons as possible.

    This is what separates the mockup's look from a diamond texture: a fully lit
    cell becomes ONE square tile, a half-lit cell one big triangle, and only
    genuinely partial cells keep a quadrant triangle.
    """
    cw, ch, ox, oy = lattice_metrics(p)
    polys = []
    for cell in range(LAT_NX * LAT_NY):
        quads = [q for q in range(QUADRANTS) if lit[cell * QUADRANTS + q]]
        if not quads:
            continue
        i, j = cell % LAT_NX, cell // LAT_NX
        x0, y0 = ox + i * cw, oy + j * ch
        x1, y1 = x0 + cw, y0 + ch
        corner = [(x0, y0), (x1, y0), (x1, y1), (x0, y1)]
        centre = ((x0 + x1) / 2.0, (y0 + y1) / 2.0)
        if len(quads) == QUADRANTS:
            polys.append(corner)
            continue
        runs, current = [], [quads[0]]
        for q in quads[1:]:
            if q == current[-1] + 1:
                current.append(q)
            else:
                runs.append(current)
                current = [q]
        runs.append(current)
        if len(runs) > 1 and runs[0][0] == 0 and runs[-1][-1] == QUADRANTS - 1:
            runs[0] = runs.pop() + runs[0]          # wrap 3 -> 0
        for run in runs:
            points = [corner[(run[0] + k) % QUADRANTS] for k in range(len(run) + 1)]
            polys.append(points + [centre])
    return polys


def draw_polys(polys, p, scale):
    """Inset each polygon toward its centroid, shear it, fill it."""
    image = Image.new("L", (int(CELL_W * scale), int(CELL_H * scale)), 0)
    draw = ImageDraw.Draw(image)
    for points in polys:
        gx = sum(x for x, _ in points) / len(points)
        gy = sum(y for _, y in points) / len(points)
        sheared = []
        for x, y in points:
            x = gx + (x - gx) * (1.0 - TILE_INSET)
            y = gy + (y - gy) * (1.0 - TILE_INSET)
            sheared.append(((x + p["slant"] * (p["yb"] - y)) * scale, y * scale))
        draw.polygon(sheared, fill=255)
    return np.asarray(image, np.float64) / 255.0


class TriangleFont:
    def __init__(self, masks, p, unlit):
        p = dict(p)
        cw, _, _, _ = lattice_metrics(p)
        # one lattice cell wide, so a stroke fills a clean chain of tiles; the
        # inset already separates them, so the segment end-gap is not needed
        p["stroke"], p["gap"] = cw, 0.10
        self.masks, self.p, self.unlit = masks, p, unlit
        ids = triangle_index_map(p, COVERAGE_SUPERSAMPLE).ravel()
        self.valid = ids >= 0
        self.ids = ids[self.valid]
        self.counts = np.bincount(self.ids,
                                  minlength=LAT_NX * LAT_NY * QUADRANTS)

    def lit_triangles(self, mask_bits):
        mask = stroke_mask(mask_bits, self.p, COVERAGE_SUPERSAMPLE)
        summed = np.bincount(self.ids, weights=mask.ravel()[self.valid],
                             minlength=len(self.counts))
        return (summed / np.maximum(self.counts, 1)) >= COVER_THRESHOLD

    def render(self, character, scale):
        """`scale` = output pixels per baseline sheet pixel (1 -> 12x16)."""
        lit = self.lit_triangles(self.masks[ord(character) - FIRST_GLYPH])
        core = draw_polys(cell_polygons(self.p, lit), self.p, scale)
        every = np.ones(LAT_NX * LAT_NY * QUADRANTS, bool)
        ghost = draw_polys(cell_polygons(self.p, every), self.p, scale)
        ghost = np.minimum(ghost * GHOST_LEVEL / self.unlit, 1.0)
        return photometry(core, ghost, self.p, self.unlit, scale)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--text", default="599 RANDOM VCF:127")
    parser.add_argument("--scale", type=int, default=8,
                        help="output pixels per baseline sheet pixel")
    parser.add_argument("--out", default="triangle_lattice_sample.png")
    args = parser.parse_args()

    masks = [mask for mask, _ in parse_vendor(VENDOR_PATH)]
    unlit = measured_unlit(baseline_cells())
    font = TriangleFont(masks, SEED, unlit)

    scale = args.scale
    strip = Image.new("RGB", (CELL_W * scale * len(args.text), CELL_H * scale),
                      (0, 0, 0))
    for index, character in enumerate(args.text):
        cell = font.render(character, scale).astype(np.uint8)
        strip.paste(Image.fromarray(cell, "RGB"), (index * CELL_W * scale, 0))
    strip.save(args.out)
    print(f"wrote {args.out} ({strip.width}x{strip.height}, scale x{scale})")
    print(f"lattice {LAT_NX}x{LAT_NY} cells, {LAT_NX * LAT_NY * QUADRANTS} triangles")
    return 0


if __name__ == "__main__":
    sys.exit(main())
