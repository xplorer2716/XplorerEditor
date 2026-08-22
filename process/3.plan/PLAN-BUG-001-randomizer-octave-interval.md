# PLAN-BUG-001: Randomizer — "Octave" Strategy Tunes Nothing

## Overview
Fix a defect reported by the owner while reviewing the user manual: the
randomizer's `Octave` VCO frequency strategy shares the `Free` branch of
`XpanderTone::defineVCOFrequenciesTuning`, so selecting it leaves both
oscillator pitches random. The option is offered on the Randomizer settings
page and persists to `settings.xml`, but has never had an effect.

The defect was ported faithfully from the reference .NET implementation, which
has the same missing `case`. Correcting it is therefore a deliberate divergence
from this port's reference-exactness principle, which is what ADR-BUG-001
decides and bounds.

## References
- **Requirements**: RQ-BUG-001. Related: RQ-CTL-050, RQ-MOD-033, RQ-MOD-050,
  RQ-SET-003
- **ADRs**: ADR-BUG-001 (DEC-BUG-001 … DEC-BUG-004)

## Baseline (before any change)
Established on this toolchain, 2026-08-21, branch `feature/BUG` from `dev`:

- `juce/model/src/XpanderTone.cpp:577` — `case EnumRandomVCOFreq::Octave:`
  falls through to `case EnumRandomVCOFreq::Free: break;`
- `juce/tests/model/XpanderToneTests.cpp` — "Randomizer helpers force the
  documented values" `[RQ-MOD-033]` covers `detune`, `Fifth`, and the forced
  ENV2→VCA2 envelope. No coverage of `Octave`, `Free`, or the interval family
  as a whole.
- No change required in `SettingsDialog.cpp` or `XmlSettingsService.cpp`: both
  already list and persist `Octave` in its correct enumerator position.

---

## Tasks

### TASK-BUG-001: Give `Octave` its own interval
- **Tier**: L
- **Status**: Done (2026-08-21)
- **Description**: Give `EnumRandomVCOFreq::Octave` its own `case` in
  `defineVCOFrequenciesTuning`, setting `VCO1_FREQ` to 0 and `VCO2_FREQ` to 12;
  replace the `// reference has no case: falls through unchanged` comment — which
  recorded the defect as intended behaviour — with the IDs that justify the
  divergence (DEC-BUG-004); and cover the whole interval family with unit tests
  so the sequence stays monotonic and `Free` stays a no-op.
- **Requirement refs**: RQ-BUG-001
- **ADR refs**: ADR-BUG-001 (DEC-BUG-001, DEC-BUG-003, DEC-BUG-004)
- **Acceptance Criteria** (Gherkin):
  - **Given** a tone, **When** `defineVCOFrequenciesTuning(Octave)` is called,
    **Then** `VCO1_FREQ` is 0 **And** `VCO2_FREQ` is 12
  - **Given** a tone whose oscillator frequencies hold known values,
    **When** `defineVCOFrequenciesTuning(Free)` is called, **Then** both values
    are unchanged
  - **Given** the interval strategies, **When** each is applied, **Then** the
    semitone distance between the two oscillators increases strictly from
    `SameNote` through `Thirteenth` in list order
  - **Given** `defineVCOFrequenciesTuning`, **When** it is read, **Then** the
    divergence from the reference is stated with the IDs that justify it
    **And** `juce/model/` no longer describes the fall-through as intended
  - **Given** the model test suite, **When** it is run, **Then** every
    pre-existing test still passes unmodified
- **Dependencies**: None
- **Assignee**: AI

---

## Verification
Run on this toolchain, 2026-08-21, `-DXPL_BUILD_APP=OFF`:

1. Every headless target builds with warnings-as-errors on
   (`-Wall -Wextra -Wpedantic -Werror`) — **no new diagnostic**.
2. `ctest` reports **117/117 pass** (115 baseline + the two scenarios added by
   TASK-BUG-001), 1 skipped as always (virtual MIDI cable absent). No
   pre-existing test modified (DoD).

## Out of scope
- `SettingsDialog.cpp` and `XmlSettingsService.cpp` — already correct, see
  Baseline.
- `SameNote`'s 12/12 base, which is not an interval origin (DEC-BUG-003).
- The user manual. Its "Octave" bullet is corrected on branch `feature/QLT`
  (manual editorial pass) and states the behaviour this plan delivers; the two
  branches converge on `dev` with no further edit needed.
