# RQ-DSN — XplorerEditor (JUCE) Design System

**Instantiates**: `docs/design-system-template.md` (generic, framework-agnostic
template — read it first; this document follows its section numbering 0–10 +
Appendix and fills it in for this project).

Scope: cross-cutting presentation layer for the JUCE reimplementation
(`juce/app/src/`) — colour, typography, spacing, geometry, motion and
interaction-state tokens shared by every visual component. Presentation only:
defines *how things look and react*, never *what they do* (functional
behaviour stays owned by RQ-GUI/RQ-CTL).

**Status**: Draft — token *structure* and *proposed* values below; final
palette/scale sign-off and the component migration itself belong to a
follow-up ADR-JUC-* and implementation plan (RQ-DSN-090).

---

## 0. Agent Operating Rules

Adopted as-is from `docs/design-system-template.md` §0: the token-resolution
order (component → semantic → global → propose new → ask), the four hard
grep-checkable rules, and the "stop and ask" escalation for a genuinely new
global value. No project-specific change to this section — it is the
standing contract for **every** future JUCE UI change in this repository,
not just the initial migration.

- **RQ-DSN-001** — Any code change to `juce/app/src/*` that introduces or
  modifies a colour, font size, corner radius, stroke width, spacing value or
  animation duration shall resolve the value through an existing token
  (component → semantic → global, in that order) before writing a literal,
  per the template §0 procedure; a genuinely new value shall be proposed as a
  token addition to this document in the same change, never inlined with a
  follow-up-later intent.
- **RQ-DSN-002** — IF a change would introduce a raw colour/size/duration
  literal outside the token module (§7.1) for a tokenised category, THEN the
  change shall be rejected/reworked before merge — this is a Definition-of-
  Done gate for every Tier M/L task touching visual presentation code
  (extends the existing "no duplicated literals" rule to non-literal-but-
  untokenised single-use values too).

## 1. Purpose & Principles

- **Hardware-faithful reproduction.** The UI reproduces the Oberheim
  Xpander/Matrix-12 front-panel block-diagram layout and control feel
  (RQ-GUI-001, RQ-GUI-037) — visual choices are checked against the
  reference screenshot/bitmap first, invented only where the reference is
  silent (e.g. hover feedback, which hardware has no equivalent of).
- **Vector crispness at any scale.** The window is freely resizable
  (RQ-GUI-005); every visual token and rendering path must stay crisp under
  non-integer scale factors — this is *why* the background moved from a
  bitmap to vector primitives (ADR-JUC-013) and why raster icon assets are
  flagged as a scaling risk (§2.5).
- **Information density for an expert user.** 226 parameters, 49 knobs and a
  20-row modulation matrix fit on one screen (RQ-GUI-001); tokens favour
  compact, legible defaults over generous whitespace.
- **No added latency for real-time feedback.** Parameter changes reach the
  synth in real time (RQ-NFR-001); no animation/transition may sit between a
  user's input and its visible or transmitted effect (§2.6 codifies this as
  an explicit non-animation zone).

## 2. Foundations — Design Tokens

### 2.0 Global tier (raw values — curated, closed set)

| Token | Value | Source (as found, pre-tokenisation) |
|---|---|---|
| `colour.slate.900` | `#181C22` (24,28,34) | `XplorerLookAndFeel.cpp:48`, `SettingsDialog.cpp:599,674`, `ProgressWindow.cpp:32` |
| `colour.slate.800` | `#1E242C` (30,36,44) | `XplorerLookAndFeel.cpp:8,10`, `ModMatrixPanel.cpp:22`, `SettingsDialog.cpp:44` |
| `colour.slate.700` | `#282E36` (40,46,54) | `XplorerLookAndFeel.cpp:32` (knob track) |
| `colour.slate.600` | `#323A44` (50,58,68) | `SettingsDialog.cpp:44` (selected row) |
| `colour.slate.500` | `#3C424A` (60,66,74) | `SettingsDialog.cpp:56` (border) |
| `colour.slate.400` | `#36363E` (54,54,62) | `MainComponent.cpp:472` (LED off-fill) |
| `colour.slate.350` | `#2C2C34` (44,44,52) | `MainComponent.cpp:473` (LED border) |
| `colour.metal.top` | `#45464F` | `ADR-JUC-013` plate gradient, top |
| `colour.metal.mid` | `#37373F` | `ADR-JUC-013` plate gradient, mid |
| `colour.blue.underline` | `#243876` | `ADR-JUC-013` section-title bar |
| `colour.diagram.frame` | `#B7BDD0` | `ADR-JUC-013` block frames |
| `colour.indicator.green` | `#90FF90` (144,255,144) | `MainComponent.cpp:470` (automation LED) |
| `colour.indicator.blue` | `#5CABFF` (92,171,255) | `MainComponent.cpp:470` (synth-in LED) |
| `colour.indicator.red` | `#FF4020` (255,64,32) | `MainComponent.cpp:471` (synth-out LED) |
| `colour.white` | `#FFFFFF` | `juce::Colours::white`, all text today |

- **RQ-DSN-003** — The global palette shall be the only tier in which a raw
  RGB/hex value appears; semantic and component tokens shall reference a
  global entry, never a literal (template §0 rule 3).

### 2.1 Colour — semantic tier

| Semantic token | Aliases | Role |
|---|---|---|
| `colour.surface.base` | `slate.900` | App/dialog background, deepest layer |
| `colour.surface.recessed` | `slate.800` | Inputs, lists, popups, dropdown backgrounds |
| `colour.surface.selected` | `slate.600` | Selected list/table row |
| `colour.control.track` | `slate.700` | Unlit track of a valued/progress-style control |
| `colour.border.default` | `slate.500` | Dialog/list borders |
| `colour.indicator.off.fill` | `slate.400` | Inactive indicator fill |
| `colour.indicator.off.border` | `slate.350` | Inactive indicator border |
| `colour.indicator.automation` | `indicator.green` | MIDI automation-source activity |
| `colour.indicator.synthIn` | `indicator.blue` | Synth-input activity |
| `colour.indicator.synthOut` | `indicator.red` | Synth-output activity |
| `colour.text.primary` | `white` | Default text on any surface above |
| `colour.accent.default` | *(user-configurable, RQ-SET-003; default TBD by owner)* | Knob LED ring, checked toggle, modulation-matrix highlight |

- **RQ-DSN-004** — `colour.accent.default` shall remain the single runtime
  source of truth already established by `XplorerLookAndFeel::ledColour()`
  (ADR-JUC-011): the settings-provided user override and the token default
  resolve through that one accessor; the modulation-matrix highlight and any
  future consumer of the accent colour shall read the accessor, never cache
  or hard-code a copy.
- **RQ-DSN-005** — Component-tier colour tokens (§2 three-tier model) may
  apply a documented, named transformation of a semantic token — brighten by
  the shared hover factor (§2.6), or a fixed alpha (e.g. the tick-box border
  today uses `ledColour.withAlpha(0.6F)`, `XplorerLookAndFeel.cpp:50`) — but
  shall not introduce a second independent colour value for what is
  conceptually the same semantic role at a different opacity/brightness.
- **RQ-DSN-006** — Theming: the current build ships one theme (dark). If a
  light/alternate theme is ever added, each semantic token gains one value
  per theme; component tokens are unaffected (they alias the semantic tier,
  never a raw value), so no component code changes.

### 2.2 Typography

Nine distinct, independently-chosen sizes exist today, all in
`BackgroundRenderer.cpp` (`FS_SECTION` 15, `FS_VCO` 16, `FS_MIX` 14,
`FS_BLOCK` 13.5, `FS_VCA` 13, `FS_PWM` 12.5, `FS_CAPTION` 12, `FS_WAVE` 11.5,
`FS_OUT` 11, `FS_SMALL` 9), plus two more decided independently in
`XplorerLookAndFeel.cpp` (combo box `BASE_SIZE` 16 / `MIN_SIZE` 9; toggle
caption `min(12, height-3)`). `FS_CAPTION` (12) and the toggle caption size
(12) coincide by accident, not by shared reference — exactly the drift
pattern RQ-DSN exists to close.

**Proposed semantic type scale** (consolidation candidates flagged, not
silently merged — owner confirms before migration):

| Semantic token | Size | Maps from |
|---|---|---|
| `text.display` | 16 | `FS_VCO`, combo `BASE_SIZE` |
| `text.title` | 15 | `FS_SECTION` |
| `text.subtitle` | 14 | `FS_MIX` |
| `text.label` | 13 | `FS_BLOCK` (13.5→13, −0.5) *and* `FS_VCA` (13) — **consolidation candidate** |
| `text.body` | 12.5 | `FS_PWM` |
| `text.caption` | 12 | `FS_CAPTION`, toggle caption |
| `text.small` | 11 | `FS_WAVE` (11.5→11, −0.5 — **consolidation candidate**), `FS_OUT` |
| `text.dense` (floor) | 9 | `FS_SMALL`, combo `MIN_SIZE` |

- **RQ-DSN-010** — The typography module shall expose the scale above by
  semantic role; the two consolidation candidates (`FS_BLOCK`/`FS_VCA` →
  `text.label`; `FS_WAVE`/`FS_OUT` → distinct sizes kept separate, `text.
  small`=11 and a new `text.smaller`=11.5 only if the owner rejects merging
  them) shall be confirmed one way or the other by the owner before the
  BackgroundRenderer migration task, not silently decided by this document.
- **RQ-DSN-011** — Dynamic-fit sizing (combo-box shrink-to-fit, RQ-GUI-032)
  shall clamp between `text.display` (ceiling) and `text.dense` (floor)
  instead of the locally-declared `BASE_SIZE`/`MIN_SIZE`; the fitting
  algorithm itself (RQ-GUI-032) is unchanged.

### 2.3 Spacing & Layout Grid

- **RQ-DSN-020** — The logical-canvas uniform-scale model already specified
  by RQ-GUI-005 is the project's canvas/DPI scaling model; RQ-DSN does not
  redefine it, only requires every new screen/dialog to use the same model
  rather than an independent scaling scheme.
- Base spacing unit and scale: **not yet derived from evidence** — dialogs
  (`SettingsDialog.cpp`) use ad hoc per-control positioning, not a grid.
  Deferred: proposing a spacing scale requires a pass over dialog layout code
  not yet done in this session; tracked as an open item for the follow-up
  ADR rather than guessed here (template §0's "stop and ask" / "don't invent"
  rule applies to spacing exactly as it does to colour).

### 2.4 Geometry

| Semantic token | Value | Source |
|---|---|---|
| `radius.control` | 2.0 | `XplorerLookAndFeel.cpp` tick box outer, `BackgroundRenderer.cpp CORNER` |
| `radius.controlInner` | 1.5 | `XplorerLookAndFeel.cpp:55` tick box fill |
| `stroke.default` | 2.0 | `BackgroundRenderer.cpp LINE_WIDTH` |
| `stroke.knobRing` | 2.4 | `XplorerLookAndFeel.cpp` knob track/value arc |
| `stroke.border` | 1.0 | tick box border, dialog list border |

- **RQ-DSN-021** — `radius.control`/`radius.controlInner` and
  `stroke.default`/`stroke.knobRing` are already near-identical across the
  vector background and the native controls drawn over it (2.0 vs. 2.0;
  distinct 2.4 for the knob ring only because a thicker stroke reads better
  at the knob's arc radius) — the geometry module shall expose both as named
  tokens so this alignment is asserted, not coincidental, and any future
  change to one is a deliberate decision about the other.

### 2.5 Iconography & Imagery

- **RQ-DSN-022** — Shortcut buttons use existing GIF raster assets
  (RQ-GUI-031); per this project's crispness principle (§1), raster icon
  assets are a scaling risk at non-integer window scale, the same problem
  the background bitmap had before ADR-JUC-013. Not a blocker for this
  document, but flagged as a candidate for a future vector re-authoring pass
  — recorded here so it is not silently forgotten, not to prescribe a
  decision on its own.

### 2.6 Motion & Timing

| Token | Value | Source |
|---|---|---|
| `motion.hoverBrighten` | 0.4 | `XplorerLookAndFeel.cpp:40` `_ledColour.brighter(0.4F)` |
| `motion.indicatorHold` | ≈100 ms | `ADR-JUC-008` MIDI-activity LED retriggerable hold |
| `motion.popupFade` | *(JUCE default, not yet an explicit token)* | `setPopupDisplayEnabled` (RQ-GUI-034) |

- **RQ-DSN-023** — `motion.hoverBrighten` shall be the shared factor for
  *every* hover-brighten rule (§3), not only the knob's — when toggle
  buttons/combo boxes/list rows gain a hover state (RQ-DSN-031), they use
  this same token, so "how much brighter on hover" reads as one product
  decision everywhere.
- **RQ-DSN-024** — Per the real-time principle (§1), no token in this
  section shall be applied between a user's control edit and that edit
  reaching the model/controller/synth (RQ-NFR-001) — motion tokens style
  *feedback* (hover, transient popups, LED holds), never gate or delay the
  underlying parameter change.

## 3. Component Interaction States

| State | Trigger | Visual rule | Currently implemented by |
|---|---|---|---|
| Idle/default | no interaction | base component tokens | all |
| Hover | pointer over, not pressed | brighten `colour.accent.default` (or the relevant token) by `motion.hoverBrighten` | **knob only** (ADR-JUC-009) |
| Active/pressed | pointer down/dragging | same rule as Hover (JUCE's `isMouseOverOrDragging(true)` already covers both) | knob only |
| Focused | keyboard focus | **not yet defined** — no component shows a distinct focus ring today | none |
| Disabled | control inactive | **not yet defined** — no shared opacity/desaturation rule exists | none |
| Selected | represents chosen option | background = `colour.surface.selected` | `SettingsDialog` list rows only |

- **RQ-DSN-030** — Every interactive control type in the catalogue (§4)
  shall declare which of the states above it implements; a control with no
  entry for Hover/Disabled/Focused is not assumed exempt — it is an open gap
  to close, tracked per-component in §4's table.
- **RQ-DSN-031** — WHEN toggle buttons, combo boxes and `SettingsDialog` list
  rows gain a hover state (currently absent — only the rotary knob has one),
  they shall use the same `colour.accent.default` + `motion.hoverBrighten`
  rule as the knob, so hover feedback reads as one consistent affordance
  across every control type, not a per-component invention.
- **RQ-DSN-032** — A shared Disabled rule shall be defined (proposed:
  `colour.text.disabled` = `text.primary` at a fixed reduced opacity, e.g.
  50% — value to be confirmed by the owner, not asserted here) and applied
  to every control that can be disabled; no control-specific disabled
  treatment shall be introduced independently.
- **RQ-DSN-033** — A keyboard-focus visual rule shall be defined before any
  keyboard-navigable dialog is built or extended — this is a genuine
  accessibility gap in the current build (no component shows focus), not
  merely a consistency nice-to-have (§6).

## 4. Component Catalogue

### RotaryKnob (`BoundKnob` + `XplorerLookAndFeel::drawRotarySlider`)
| Field | Value |
|---|---|
| Anatomy | ring track, value arc (no centre pointer, no filled body — ADR-JUC-009) |
| States implemented | Idle, Hover/Active (brighten) |
| States missing | Disabled, Focused |
| Tokens used | `colour.control.track` (track), `colour.accent.default` (arc), `stroke.knobRing`, `motion.hoverBrighten` |
| Behaviour owner | RQ-GUI-030, RQ-GUI-031, RQ-GUI-034; ADR-JUC-009 |

### ToggleButton / TickBox (`XplorerLookAndFeel::drawTickBox`/`drawToggleButton`)
| Field | Value |
|---|---|
| Anatomy | square tick box (fill + border), caption label |
| States implemented | Idle (checked/unchecked) |
| States missing | Hover, Disabled, Focused |
| Tokens used | `colour.surface.base` (fill), `colour.accent.default` @ 60% alpha (border), `colour.accent.default` (checked fill), `radius.control`/`radius.controlInner`, `text.caption` |
| Behaviour owner | RQ-GUI-032 |

### ComboBox
| Field | Value |
|---|---|
| Anatomy | background, popup list, per-item label (dynamically sized) |
| States implemented | Idle |
| States missing | Hover, Disabled, Focused |
| Tokens used | `colour.surface.recessed` (background + popup), `colour.text.primary`, `text.display`…`text.dense` (dynamic fit) |
| Behaviour owner | RQ-GUI-032 |

### SettingsDialog list row
| Field | Value |
|---|---|
| Anatomy | row background, border |
| States implemented | Idle, Selected |
| States missing | Hover, Focused |
| Tokens used | `colour.surface.recessed` (default), `colour.surface.selected`, `colour.border.default` |
| Behaviour owner | RQ-GUI-025 |

### MIDI Activity LED panel (`MainComponent`)
| Field | Value |
|---|---|
| Anatomy | 3 square LEDs (fill + border), off/lit states per source |
| States implemented | Idle/off, Lit (event-triggered, `motion.indicatorHold`) |
| Tokens used | `colour.indicator.{automation,synthIn,synthOut}`, `colour.indicator.off.fill`, `colour.indicator.off.border` |
| Behaviour owner | RQ-GUI-022; ADR-JUC-008 |

### Modulation-matrix hover highlight (`ModMatrixPanel`)
| Field | Value |
|---|---|
| Anatomy | row background highlight |
| States implemented | Hover (of a related knob/selector, not of the row itself) |
| Tokens used | `colour.accent.default` (single source, ADR-JUC-011), `colour.surface.recessed` (base) |
| Behaviour owner | RQ-GUI-018; ADR-JUC-010, ADR-JUC-011 |

### Vector background (`BackgroundRenderer`)
| Field | Value |
|---|---|
| Anatomy | metal plate + gradient, wood rails, block diagram (frames, lines, stubs, captions), section-title bars |
| States implemented | static (no interaction) |
| Tokens used | `colour.metal.{top,mid}`, `colour.diagram.frame`, `colour.blue.underline`, `stroke.default`, `radius.control`, full typography scale (§2.2) |
| Behaviour owner | RQ-GUI-037; ADR-JUC-013 |

- **RQ-DSN-040** — Every component listed above with a "States missing"
  row is an open, tracked gap; closing it is a Tier M task per component
  (new visual behaviour, contained to one file) once this requirements
  document and its follow-up ADR are accepted — not implied or silently
  batched into this document.

## 5. Layout Patterns

- **Fixed-diagram-anchor pattern**: the vector background's stub ticks are
  positioned from the same `ControlTable` knob-centre coordinates the
  controls themselves use (ADR-JUC-013) — single source of truth for
  alignment. Any future panel that overlays graphics on live controls shall
  follow this pattern (derive positions from one shared table) rather than
  hand-measuring two independent coordinate sets.
- **Shared page-family block pattern**: ENV/LFO/RAMP/TRACK areas are one
  control block rebound to the selected instance (RQ-GUI-010/011) — a
  layout pattern to reuse for any future "one block, many instances"
  screen, instead of duplicating the block per instance.
- **Uniform logical-canvas scaling**: the whole window scales as one unit
  preserving aspect ratio (RQ-GUI-005) — any new dialog/window shall use the
  same `AffineTransform`-based approach (ADR-JUC-013's diagram), not an
  independent per-dialog scaling scheme.

## 6. Accessibility & Ergonomics

- **RQ-DSN-050** — `colour.text.primary` (white) on `colour.surface.
  {base,recessed}` (both near-black) shall be verified against a minimum
  4.5:1 contrast ratio as part of the follow-up ADR — both are very dark so
  this is expected to pass, but is asserted here as a check to run, not
  assumed.
- **RQ-DSN-051** — The three MIDI-activity LEDs and the modulation-matrix
  highlight currently convey meaning by **colour alone** (green/blue/red
  source identity; accent-colour highlight vs. none). This is a genuine gap
  against the "never colour-alone" rule (template §6): a colour-vision-
  deficient user cannot distinguish the automation/synth-in/synth-out LEDs
  by colour alone. Flagged for owner decision (e.g. distinct LED shapes or
  position labels already exist per RQ-GUI-022's fixed layout — whether that
  positional redundancy is sufficient, or a stronger cue is wanted, is an
  owner call, not decided here).
- **RQ-DSN-052** — Minimum hit target: the 49 rotary knobs render at their
  full extracted control-table bounds (RQ-GUI-034); no token-driven size
  reduction (e.g. a future "compact" variant) shall shrink a knob below its
  current reference-derived bounds without an explicit owner decision, since
  that bound is also the minimum comfortable pointer target today.

## 7. Framework Binding (JUCE)

- **RQ-DSN-060** — A single header, e.g. `juce/app/src/DesignTokens.hpp`
  (or, if headless testing is wanted per RQ-DSN-070, `xpl_app_core`'s
  headless tree, mirroring the `BackgroundSpec`-in-core pattern ADR-JUC-013
  considered and deferred) shall declare every token in §2 as a `constexpr`
  value or accessor, with no `juce::Component`/`Graphics` dependency.
  Spec-name → identifier transliteration: `colour.surface.recessed` →
  `kColourSurfaceRecessed` (dot → nothing, PascalCase segments, `k` prefix
  matching this codebase's existing constant style, e.g. `CANVAS_PADDING`
  uses `SCREAMING_SNAKE_CASE` today — **owner to confirm** which convention
  the new module follows; not silently decided here).
  *(As built: `DesignTokens.hpp` is **generated** from `design-tokens.yaml`
  (ADR-JUC-015), not hand-declared; token names are camelCase grouped in
  `tokens::{global,semantic,component}` namespaces, so the dotted spec name
  `colour.surface.recessed` maps to `tokens::semantic::surfaceRecessed`.)*
- **RQ-DSN-061** — `XplorerLookAndFeel`, `BackgroundRenderer`,
  `ModMatrixPanel`, `SettingsDialog`, `ProgressWindow` and `MainComponent`
  shall all read from `DesignTokens.hpp` instead of their current
  independent literals; migrating each is a Tier M task per file (contained,
  mechanical, testable by the grep-based check of RQ-DSN-071).
- **RQ-DSN-062** — State-to-code mapping: Hover/Active uses
  `juce::Component::isMouseOverOrDragging()`; Disabled uses `isEnabled()`;
  Focused uses `hasKeyboardFocus()`; Selected is component-local state
  (e.g. `SettingsDialog`'s row index) — each `draw*` override reads the
  live JUCE state and resolves the matching §3 rule, no cached per-instance
  state duplicating what JUCE already tracks (same principle as
  ADR-JUC-011's single-source-of-truth LED colour).
- **RQ-DSN-063** — The C++ token module and the SVG mockup prototype shall
  derive from ONE language-neutral source of truth (`juce/tools/design-tokens.yaml`),
  so they cannot diverge on a token value: `DesignTokens.hpp` is *generated*
  from it (`tools/generate_design_tokens.py`) and the mockup *consumes* it
  (`tools/generate_background_mockup.py`, via the generator's shared
  `resolve()`), closing the sync gap ADR-JUC-013 flagged. Mechanism decided in
  ADR-JUC-015. *(Supersedes the earlier "import or mirror … hand-synced"
  wording — the sync is now structural, not a review obligation.)*

## 8. Testability

- **RQ-DSN-070** — `DesignTokens.hpp` shall be consumable and unit-testable
  headlessly (no display required), consistent with the `xpl_app_core`
  pattern; under `session.unit_tests = true`, at minimum a compile-time/
  static-assert-level check that component tokens only ever alias semantic
  tokens (never a raw literal) is in scope for the follow-up implementation
  task.
- **RQ-DSN-071** — A grep-based check (e.g. `rg` for
  `Colour::fromRGB|Colour\(0x` outside `DesignTokens.hpp`, and for
  `juce::Font\(` with a numeric literal outside it) shall be run and reported
  clean as part of the Definition-of-Done for any task closing an item in
  §4's "States missing" / token-migration backlog.

## 9. Governance

- **RQ-DSN-080** — Adding a token: propose the row (tier, name per the §2
  grammar, value, source) in this document, get owner sign-off on the
  *value* if it's new (not just a rename of an existing literal), then add
  the constant and migrate call sites in the same change.
- **RQ-DSN-081** — Changing a token's value shall never require a call-site
  edit; if a change does require one, the token was not actually load-bearing
  as a single source of truth and that is itself a defect to fix, not an
  accepted cost of the change.
- **RQ-DSN-082** — Deprecating a token requires a zero-result grep for its
  identifier across `juce/` before removal.
- **RQ-DSN-083** — Any future ADR-JUC-* deciding a visual-presentation detail
  shall reference the RQ-DSN token(s) it uses or adds, per template §9 —
  extends, never bypasses, this token set (mirrors how ADR-JUC-011 is
  already referenced by RQ-GUI-018).

## 10. Non-Goals

- Does not alter any RQ-GUI/RQ-CTL functional behaviour — migrating a
  component onto tokens is a pure refactor (RQ-DSN-090).
- Does not fix the flagged accessibility gaps (§6) or missing states (§3) by
  itself — it documents them as tracked, owner-decidable items; fixing them
  is follow-up implementation work.
- Does not redesign the visual language (§1's principles are descriptive of
  the current, owner-validated direction, not a proposal to change it).

---

## Non-Functional Requirements

- **RQ-DSN-090** — Adopting this design system for an existing component is
  a pure refactor: no RQ-GUI/RQ-CTL requirement it fulfils changes as a
  result — only whether a literal is inline or token-referenced.
- **RQ-DSN-091** — This document defines token *structure* and *proposed*
  values (explicitly flagged where a value is a proposal pending owner
  confirmation, e.g. RQ-DSN-010's consolidation candidates, RQ-DSN-032's
  disabled opacity, RQ-DSN-060's identifier casing); final sign-off and the
  component-by-component migration are recorded in a follow-up ADR-JUC-*
  and implementation plan, per the mandated requirements → ADR → plan →
  implementation sequence (`process/1.requirements/prompt.md` §1.2).

---

## Appendix: Good / Bad Examples (this codebase)

| ❌ Bad (current) | ✅ Good (target) | Why |
|---|---|---|
| `g.setColour(juce::Colour::fromRGB(30, 36, 44));` (`XplorerLookAndFeel.cpp:8`, `ModMatrixPanel.cpp:22`, `SettingsDialog.cpp:44`) | `g.setColour(DesignTokens::colourSurfaceRecessed());` | Same value, four independent literals — a colour change today means four hand-edits, easy to miss one |
| `FS_CAPTION = 12.0F` in `BackgroundRenderer.cpp` and an unrelated `12.0F` toggle-caption size in `XplorerLookAndFeel.cpp:75` | Both reference `text.caption` | Values coincide today by accident; a designer changing one would not know to change the other |
| Only `drawRotarySlider` checks `isMouseOverOrDragging` for a hover brighten | Every interactive `draw*` override checks its matching JUCE state and applies the shared `motion.hoverBrighten` rule | Hover is a control-type feature today, not a design-system rule — RQ-DSN-031 |
| `ledColour.withAlpha(0.6F)` (`XplorerLookAndFeel.cpp:50`) — a bespoke alpha, undocumented as a rule | A named, documented component-tier transform of `colour.accent.default` | Same pattern (alpha-modified accent) could silently diverge in a new component if not named |

---

## Traceability

| RQ-DSN | Depends on / relates to |
|---|---|
| 001–006 | RQ-SET-003, ADR-JUC-011 |
| 010–011 | RQ-GUI-032, RQ-GUI-033, ADR-JUC-013 |
| 020–024 | RQ-GUI-005, RQ-NFR-001, ADR-JUC-008, ADR-JUC-013 |
| 030–033 | RQ-GUI-018, RQ-GUI-022, RQ-GUI-031, RQ-GUI-034, ADR-JUC-008, ADR-JUC-009, ADR-JUC-010, ADR-JUC-011 |
| 040 | RQ-GUI-010, RQ-GUI-011, RQ-GUI-018, RQ-GUI-022, RQ-GUI-025, RQ-GUI-030–037, ADR-JUC-008–013 |
| 050–052 | RQ-GUI-005, RQ-GUI-022, RQ-GUI-034 |
| 060–063 | ADR-JUC-013 |
| 070–071 | RQ-NFR-007, RQ-TST (process) |
| 080–083 | ADR-JUC-011, ADR-JUC-013 |
| 090–091 | `process/1.requirements/prompt.md` §1.2 |
