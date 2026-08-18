# PLAN-GUI-010: VCF/VCA Block — Second VCA Mislabelled

## Overview
Fix a defect reported by the owner: in the VCF/VCA functional block, the
second VCA (paired with VCA1) is labelled "VCA" instead of "VCA2". The model
has always named the pair `VCA1_VOL`/`VCA2_VOL` (`XpanderConstants.hpp`); the
label text just never matched.

## References
- **Requirements**: RQ-GUI-001 (corrected — its own wording carried the same
  "VCA1/VCA" typo)
- **ADRs**: None. A literal string fix, no design decision. Tier S.

---

## Tasks

### TASK-GUI-037: Correct the second VCA label to "VCA2"
- **Tier**: S
- **Status**: Done (2026-08-18)
- **Description**: Change the block-title text for the VCF/VCA chain's second
  VCA box from "VCA" to "VCA2" in both renderers that must stay in lock-step
  (`ADR-JUC-013`): `generate_background_mockup.py` (and its regenerated
  `background-mockup.svg`) and `BackgroundRenderer.cpp`. No geometry, token,
  or other label changed — same box, same font-size token (`FS_VCA`).
- **Requirement refs**: RQ-GUI-001
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** the VCF/VCA block, **When** the main window is rendered,
    **Then** the box next to "VCA1" reads "VCA2"
  - **Given** `generate_background_mockup.py`, **When** it is run,
    **Then** `background-mockup.svg` contains "VCA2" at the same position
  - **Given** the ENV X and LFO X blocks' own (unrelated, unpaired) VCA
    boxes, **When** rendered, **Then** they are untouched and still read
    "VCA" — RQ-GUI-001 only ever named a VCA1/VCA2 pair inside VCF/VCA
- **Dependencies**: None
- **Assignee**: AI

---

## Note on testing
Tier S, string-literal fix to an already-exercised rendering path (both
renderers already build and are covered by `BackgroundRendererTests.cpp`'s
existing geometry assertions) — no new unit test added. "VCA2" is the same
length as "VCA1", already rendered at the same font-size token in the
adjacent box, so no new width/overlap risk is introduced.

## Out of scope
The ENV X and LFO X blocks' own single, unpaired VCA output boxes (correctly
"VCA", not part of a numbered pair) — unchanged.
