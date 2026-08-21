# FTR-BUG-001: Defect Corrections

## Overview
Corrections of defects found in the shipped JUCE build. Each requirement here
states the behaviour the application SHALL have after the fix, so the defect and
its resolution stay greppable by ID like any other requirement.

Requirements in this file may deliberately depart from the reference .NET
implementation. That is not the default for this port (RQ-MOD-050 and the
"bit-exact versus the reference" scope note of RQ-MOD apply everywhere else) and
each departure SHALL be justified by an ADR — see ADR-BUG-001.

## Stakeholders
- **Owner**: project owner (defect reports)
- **Consumers**: `xpl_model` (`XpanderTone` randomizer helpers), `xpl_controller`
  (RQ-CTL-050), the Randomizer settings page, the user manual

---

## Functional Requirements

### RQ-BUG-001: Randomizer "Octave" VCO frequency strategy
- **Category**: Functional
- **EARS Type**: Event-driven
- **Statement**: WHEN the VCO frequency strategy is applied with the value
  `Octave`, the model SHALL set `VCO1_FREQ` to 0 and `VCO2_FREQ` to 12, so that
  VCO2 sounds exactly one octave above VCO1.
- **Rationale**: The reference implementation has no `case` for `Octave`, so the
  option falls through to `Free` and leaves both oscillator pitches random —
  the one strategy value in the list that is indistinguishable from doing
  nothing. The port carried the omission over faithfully
  (`XpanderTone.cpp`, "reference has no case: falls through unchanged"), which
  means a user selecting "Octave" on the Randomizer settings page gets no
  octave and no indication that the setting was ignored.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - **Given** a tone, **When** the VCO frequency strategy `Octave` is applied,
    **Then** `VCO1_FREQ` is 0 **And** `VCO2_FREQ` is 12
  - **Given** a tone, **When** the strategy `Free` is applied, **Then** neither
    `VCO1_FREQ` nor `VCO2_FREQ` is modified — `Octave` no longer shares its
    behaviour
  - **Given** the interval strategies as a family, **When** each is applied,
    **Then** `Octave` sits between `Seventh` (11) and `Ninth` (14) in the
    semitone distance it produces, matching its position in the option list
- **Dependencies**: RQ-CTL-050, RQ-MOD-033, RQ-SET-003; ADR-BUG-001

---

## Traceability note
The persisted settings value is unaffected: `Octave` already round-trips through
`XmlSettingsService` under the name `"Octave"` and keeps its enumerator position
(`EnumRandomVCOFreq`), so settings files written before this correction select
the corrected behaviour without migration. Only what the value *does* changes.
