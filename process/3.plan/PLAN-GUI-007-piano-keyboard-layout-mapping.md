# PLAN-GUI-007: Layout-Independent Piano Keyboard Mapping and Key Labels

## Overview
Replace the piano window's inherited US-QWERTY key mapping with one resolved at
window open from physical key positions, so it draws a piano on any keyboard
layout — and print the resolved character on each mapped key so the mapping is
self-documenting.

## References
- **Requirements**: RQ-GUI-074, RQ-GUI-075; carries RQ-GUI-028, RQ-MID-010
- **ADRs**: ADR-JUC-035 (DEC-JUC-114..121); ADR-JUC-004 (seam pattern,
  referenced not amended); ADR-JUC-014, ADR-JUC-015 (tokens)
- **Design system**: RQ-DSN-102 (`pianoKeyLabel`, `pianoKeyLabelSize`)

---

## Tasks

### TASK-GUI-032: Design tokens for the key-mapping labels
- **Tier**: S
- **Status**: Done (2026-08-18)
- **Description**: Add `pianoKeyLabel` (mid grey) and `pianoKeyLabelSize` to
  `juce/tools/design-tokens.yaml` in the semantic tier, then regenerate
  `DesignTokens.hpp` with `generate_design_tokens.py`. No hand edit of the
  generated header.
- **Requirement refs**: RQ-DSN-102, RQ-GUI-075
- **ADR refs**: ADR-JUC-014, ADR-JUC-015
- **Acceptance Criteria** (Gherkin):
  - **Given** `design-tokens.yaml`, **When** the generator is run, **Then**
    `DesignTokens.hpp` exposes both tokens and the build succeeds
  - **Given** the generated header, **When** it is compared to a fresh
    regeneration, **Then** they are identical (no hand edit)
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GUI-033: Resolve the mapping from physical key positions
- **Tier**: L
- **Status**: Done (2026-08-18) — Windows and macOS query implementations are
  unverified by compilation in this session (no toolchain available); CI is
  their first real build. Linux compiled, linked and ran under the headless
  suite.
- **Description**: Introduce the physical-position table — the same 17
  positions as JUCE's default (`a w s e d f t g y h u j k o l p ;`, 10 white +
  7 black), named after those reference letters rather than a row/column
  index — and the `KeyboardLayoutQuery` seam that answers "which character
  does position P produce on this layout?", with Windows (`MapVirtualKey`),
  Linux/X11 (`XkbKeycodeToKeysym`) and macOS
  (`TISGetInputSourceProperty` + `UCKeyTranslate`) implementations. On piano
  window open, build the mapping and install it via `clearKeyMappings()` +
  `setKeyPressForNote()`. Unresolvable position → note left unmapped;
  unavailable query → keep JUCE's default.
- **Requirement refs**: RQ-GUI-074, RQ-GUI-028, RQ-MID-010
- **ADR refs**: ADR-JUC-035 (DEC-JUC-114..118), ADR-JUC-004
- **Acceptance Criteria** (Gherkin):
  - **Given** a fake query returning the US QWERTY layout, **When** the mapping
    is built, **Then** it equals `a w s e d f t g y h u j k o l p ;` — today's
    behaviour, proving no regression for US users
  - **Given** a fake query returning the French AZERTY layout, **When** the
    mapping is built, **Then** the home-row leftmost letter key plays C and the
    upper row carries the seven black keys
  - **Given** a fake query returning a layout never named in the sources,
    **When** the mapping is built, **Then** every position resolves to that
    layout's character with no layout name appearing anywhere in the code
  - **Given** a fake query failing on one position, **When** the mapping is
    built, **Then** that note is unmapped and all others are bound
  - **Given** a fake query unavailable entirely, **When** the mapping is built,
    **Then** JUCE's default mapping is left in place
  - **Given** the sources, **When** searched, **Then** no hard-coded list of
    mapping characters exists
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GUI-034: Print the resolved character on each mapped key
- **Tier**: M
- **Status**: Done (2026-08-18) — headless pixel-render check confirmed white
  AND black key labels, correct stacking over JUCE's octave marker on both
  mapped Cs, and no label on unmapped notes.
- **Amended same day, owner report ("les lettres sont vraiment petites")**:
  upper-cased display text (matched character unchanged), split
  `pianoKeyLabel` into `pianoKeyLabelOnWhite`/`pianoKeyLabelOnBlack`
  (RQ-DSN-102's reserved escape hatch), and scaled the piano window + key
  width 1.5× JUCE's defaults. No new task ID: same feature, same-session
  refinement, Tier S in isolation.
- **Description**: Override `drawWhiteNote` and `drawBlackNote`, each calling
  the base implementation first and then drawing the binding character from the
  RQ-DSN-102 tokens. On the two Cs inside the mapped span, stack the character
  above JUCE's octave marker. Drop black-key labels when the key is narrower
  than the stated minimum legible size, keeping the white-key labels.
- **Requirement refs**: RQ-GUI-075, RQ-DSN-102
- **ADR refs**: ADR-JUC-035 (DEC-JUC-119..121), ADR-JUC-014
- **Acceptance Criteria** (Gherkin):
  - **Given** the piano keyboard rendered headlessly, **When** a mapped white
    key is inspected, **Then** it carries its binding character
  - **Given** a black key at or above the minimum size, **When** it is
    inspected, **Then** it carries its character within its own width
  - **Given** the seven black keys below the minimum size, **When** the
    keyboard is rendered, **Then** none carries a label and every mapped white
    key still does
  - **Given** the two Cs carrying both a marker and a binding, **When** they are
    inspected, **Then** both are present and do not overlap
  - **Given** every C of the keyboard, **When** compared, **Then** all still
    show their octave marker
  - **Given** `PianoWindow`, **When** searched, **Then** the label colour and
    size come from tokens with no visual literal
- **Dependencies**: TASK-GUI-032, TASK-GUI-033
- **Assignee**: AI

---

## Verification note

The Windows and Linux query paths are exercisable here; **the macOS path is
not** — this container has no Mac, and CI cannot prove it either. RQ-GUI-074 is
therefore **not** met on macOS until the owner confirms it on a real machine.
That gap is stated here rather than discovered after delivery.

## Out of scope
Tracking a keyboard-layout change while the piano window is open (DEC-JUC-117,
owner decision); the notes sent (RQ-MID-010, unchanged); the window's geometry,
mouse play and octave range; the settings dialog (no user-facing layout option
— the OS is the source of truth).
