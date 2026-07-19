#!/usr/bin/env python3
"""Generate juce/app/src/DesignTokens.hpp from design-tokens.yaml.

The YAML file is THE single source of truth for the JUCE UI presentation
tokens (colour, typography, geometry, motion). This script turns it into the
C++ header; tools/generate_background_mockup.py consumes the SAME source (via
`resolve()` below) for its SVG prototype, so the shipped app and the mockup can
never diverge on a token value. [RQ-DSN-060..063, ADR-JUC-015]

Prerequisite: PyYAML (`pip install pyyaml`).

Usage:
    python3 generate_design_tokens.py            # writes ../app/src/DesignTokens.hpp
    python3 generate_design_tokens.py --check     # exit 1 if the file is stale
"""
from __future__ import annotations

import argparse
import pathlib
import sys

try:
    import yaml
except ImportError:  # pragma: no cover
    sys.exit("PyYAML is required: pip install pyyaml")

HERE = pathlib.Path(__file__).resolve().parent
YAML_PATH = HERE / "design-tokens.yaml"
HPP_PATH = HERE.parent / "app" / "src" / "DesignTokens.hpp"

TIER_ORDER = ("global", "semantic", "component")


def _load(path: pathlib.Path) -> dict:
    with open(path, "r", encoding="utf-8") as handle:
        return yaml.safe_load(handle)["tiers"]


def _find(tiers: dict, dotted: str) -> dict:
    tier, name = dotted.split(".", 1)
    return tiers[tier][name]


def _base(tiers: dict, entry: dict) -> dict:
    """Follow the alias chain down to the entry that holds the literal value."""
    while "alias" in entry:
        entry = _find(tiers, entry["alias"])
    return entry


def resolve(path: pathlib.Path = YAML_PATH) -> dict:
    """Return {tier: {name: {'kind', 'value', 'note'}}} with aliases resolved to
    concrete values (hex string for colours, number for float/int). Shared with
    the mockup generator so both derive from one source."""
    tiers = _load(path)
    out: dict = {}
    for tier in TIER_ORDER:
        out[tier] = {}
        for name, entry in tiers[tier].items():
            base = _base(tiers, entry)
            kind = base["kind"]
            value = base["hex"] if kind == "colour" else base["value"]
            out[tier][name] = {"kind": kind, "value": value, "note": entry.get("note")}
    return out


def _cpp_float(value: float) -> str:
    text = repr(float(value))
    if text.endswith(".0"):
        text = text  # keep the .0, e.g. 16.0 -> "16.0"
    return text + "F"


def _emit_hpp(tiers: dict) -> str:
    resolved = {t: {} for t in TIER_ORDER}

    def kind_of(entry: dict) -> str:
        return _base(tiers, entry)["kind"]

    lines: list[str] = []
    a = lines.append
    a("#pragma once")
    a("")
    a("// =====================================================================")
    a("// GENERATED FILE — DO NOT EDIT BY HAND.")
    a("// Source of truth : juce/tools/design-tokens.yaml")
    a("// Regenerate with : python3 juce/tools/generate_design_tokens.py")
    a("// =====================================================================")
    a("//")
    a("// Single source of truth for JUCE UI presentation (colour, typography,")
    a("// geometry, motion). Three tiers, each referencing only the tier below:")
    a("// global (raw literals) -> semantic (roles) -> component (usages).")
    a("// Every value equals the literal it replaced, so the UI is pixel-identical")
    a("// by construction. [RQ-DSN-001..024, RQ-DSN-060..063; ADR-JUC-014, ADR-JUC-015]")
    a("")
    a("#include <juce_graphics/juce_graphics.h>")
    a("")
    a("namespace xplorer::app::tokens")
    a("{")

    tier_banner = {
        "global": "TIER 1 — GLOBAL : raw curated values (the only tier with literals).",
        "semantic": "TIER 2 — SEMANTIC : roles aliasing globals. Components use these.",
        "component": "TIER 3 — COMPONENT : usages/transforms aliasing semantics.",
    }

    for tier in TIER_ORDER:
        a(f"    // -----------------------------------------------------------------")
        a(f"    // {tier_banner[tier]}")
        a(f"    // -----------------------------------------------------------------")
        a(f"    namespace {tier}")
        a("    {")
        for name, entry in tiers[tier].items():
            kind = kind_of(entry)
            if "alias" in entry:
                expr = entry["alias"].replace(".", "::")
            elif kind == "colour":
                expr = f"juce::Colour(0xFF{entry['hex'].upper()})"
            elif kind == "float":
                expr = _cpp_float(entry["value"])
            elif kind == "int":
                expr = str(int(entry["value"]))
            else:  # pragma: no cover
                raise ValueError(f"unknown kind {kind!r} for {name}")

            if kind == "colour":
                decl = f"        inline const juce::Colour {name} = {expr};"
            elif kind == "float":
                decl = f"        inline constexpr float {name} = {expr};"
            else:
                decl = f"        inline constexpr int {name} = {expr};"

            note = entry.get("note")
            if note:
                decl = f"{decl}  // {note}"
            a(decl)
        a("    }")
        a("")

    a("}")
    a("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true",
                        help="exit 1 if DesignTokens.hpp is out of date, without writing")
    args = parser.parse_args()

    tiers = _load(YAML_PATH)
    generated = _emit_hpp(tiers)

    if args.check:
        current = HPP_PATH.read_text(encoding="utf-8") if HPP_PATH.exists() else ""
        if current != generated:
            print(f"STALE: {HPP_PATH} differs from design-tokens.yaml — regenerate.",
                  file=sys.stderr)
            return 1
        print("OK: DesignTokens.hpp is in sync with design-tokens.yaml.")
        return 0

    HPP_PATH.write_text(generated, encoding="utf-8")
    print(f"Wrote {HPP_PATH} ({len(generated.splitlines())} lines) from {YAML_PATH.name}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
