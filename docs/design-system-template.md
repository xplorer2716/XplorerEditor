# Desktop Application Design System — Reusable Template

## Purpose

A generic, framework-agnostic structure for defining a **design system** for a
desktop application. It applies equally to a JUCE app, a .NET/WinForms or WPF
app, a Qt app, or any other native desktop UI stack. It is meant to be copied
into a new project and filled in — it defines *sections and questions to
answer*, not answers themselves.

A design system produced from this template has three properties, regardless
of the target framework:

1. **Single source of truth** — every visual value (colour, size, spacing,
   timing) is declared exactly once, referenced everywhere it is used.
2. **Framework-bound, framework-independent** — the tokens and rules are
   described in terms that make sense before any framework code exists; §7
   is the *only* section that says how they map onto a specific toolkit.
3. **Governed** — the document states how new tokens are added, how drift
   (implementation vs. spec) is prevented, and how compliance is checked.

## How to use this template

1. Copy this file into the target project (e.g. `docs/design-system.md` or,
   in a process that authors formal requirements, as the body of a
   requirements artifact).
2. Fill every `[...]` placeholder. Delete guidance text (in *italics*) once
   the section is filled in — the shipped document should read as a spec,
   not as an annotated template.
3. Do not skip §7 (Framework Binding) or §9 (Governance) — a design system
   without a stated binding strategy and a stated drift-prevention rule
   degenerates back into scattered literals, which is the problem this
   template exists to prevent.
4. Keep the document versioned with the codebase (same repo, same review
   process as code) — a design system that lives outside version control
   drifts from the implementation immediately.

---

## 1. Purpose & Principles

*State the product's visual/interaction philosophy in 2–5 bullet points —*
*the "why" that later token choices should trace back to (e.g. "reproduces*
*a specific piece of hardware", "flat/modern SaaS look", "high information*
*density for expert users"). Every subsequent section should be traceable to*
*a principle stated here.*

- [Principle 1]
- [Principle 2]
- ...

## 2. Foundations — Design Tokens

A **token** is a named constant standing in for a raw value (colour, size,
duration). Tokens are declared once, in one module (§7.1), and referenced by
name everywhere else. No raw literal of a tokenised kind should appear outside
that module.

### 2.1 Colour

- **Semantic roles, not raw names.** Define colours by the *role* they play
  (e.g. `surface`, `surfaceElevated`, `border`, `textPrimary`,
  `textSecondary`, `textDisabled`, `accent`, plus one entry per
  status/semantic meaning the app needs — e.g. per-source activity
  indicators). Never name a token after its RGB value.
- **Palette table**: [role → value table, filled in per project]
- **Theming**: [does the app support light/dark or multiple themes? If yes,
  each role maps to one value per theme; if no, state that explicitly so it
  isn't silently assumed later]
- **User-overridable colours**: [if any token's *effective* value can be
  changed by the end user via settings, state which ones and how the
  override composes with the token default — the token remains the default,
  a single runtime accessor resolves the effective value]

### 2.2 Typography

- **Font family/fallback**: [name, plus fallback if the primary font may be
  unavailable]
- **Type scale**: a named set of sizes, one per text *role* (e.g. section
  title, block/group label, body/caption, small/dense label), not one raw
  size per screen. [role → size table]
- **Dynamic sizing bounds**: [for any text that is programmatically shrunk
  to fit its container, state the shared minimum legibility floor and
  maximum size as scale entries, not ad hoc per call site]

### 2.3 Spacing & Layout Grid

- **Base unit**: [e.g. 4px/8px grid — pick one, state it]
- **Spacing scale**: named multiples of the base unit (e.g. tight/default/
  loose) used for padding and gaps, instead of arbitrary per-component
  numbers.
- **Canvas/DPI scaling strategy**: [if the app renders onto a logical canvas
  that scales with the window/display DPI, state the scaling model here —
  uniform scale preserving aspect ratio, independent X/Y scale, fixed size,
  etc. — and where the scale factor is computed]

### 2.4 Geometry

- **Corner radius scale**: [named sizes, e.g. sharp/soft/round]
- **Stroke/border width scale**: [named weights]
- **Elevation/shadow model** (if the visual language uses depth):
  [named levels, or state "flat, no elevation" explicitly]

### 2.5 Iconography & Imagery

- **Icon style**: [outline/filled, stroke width, corner treatment]
- **Asset format & sizing**: [vector preferred; if raster, state the source
  resolution and scaling policy]
- **Sourcing**: [hand-drawn/vector-authored vs. photographic/bitmap assets —
  state the policy and the reason, since it drives crispness at scale]

### 2.6 Motion & Timing

- **Duration scale**: named durations for the interaction feedback the app
  actually uses (e.g. hover transition, transient indicator hold, popup
  fade) — not one bespoke millisecond value per component.
- **Easing**: [linear / ease-in-out / etc., per duration category if they
  differ]
- **When *not* to animate**: [state any deliberate exceptions — e.g.
  real-time/low-latency controls that must never add animated delay between
  input and visible feedback]

## 3. Component Interaction States

Define the **canonical state machine** every interactive control implements,
and the **shared visual rule** for each state transition — stated once here,
not re-decided per component.

| State | When it applies | Visual rule (token-referenced) |
|---|---|---|
| Idle/default | no interaction | [base tokens] |
| Hover | pointer over, not pressed | [e.g. "brighten accent by scale factor X"] |
| Active/pressed | pointer down / being dragged | [rule] |
| Focused | keyboard focus | [rule — must remain visible without a pointer] |
| Disabled | control inactive | [rule — e.g. shared desaturation/opacity] |
| Selected | control represents a chosen/active option | [rule] |
| Error/invalid | input failed validation | [rule, if the app has validated input] |

*Every component's own spec (§4) states which of these states it implements —*
*it does not redefine what "hover" looks like, it only says whether it has one.*

## 4. Component Catalogue — Per-Component Spec Structure

For **each** distinct control type in the app (buttons, knobs/sliders, list/
table rows, dialogs, indicators, etc.), document:

- **Anatomy**: the named parts of the control (e.g. "ring track", "value
  arc", "LED core") and which tokens each part uses.
- **States implemented**: subset of §3's state table that applies, with any
  component-specific nuance called out (rare — most nuance belongs in §3).
- **Variants**: [e.g. size variants, style variants — only if the project
  actually has more than one]
- **Behavioural notes**: interaction quirks that affect appearance (e.g.
  "value shown as a transient popup while dragging, not a persistent
  label") — link to the functional requirement that owns the *behaviour*;
  this catalogue owns only the *appearance* of that behaviour.
- **Do / Don't**: [optional — call out common mistakes if the project has
  a history of them]

*This section is a structure to repeat per component, not content to fill*
*in generically — the actual per-component entries belong in the project's*
*instantiated design system, not in this template.*

## 5. Layout Patterns

*If the app has recurring macro-layout patterns (e.g. a fixed diagram with*
*controls anchored to measured points, a form with label/field rows, a*
*toolbar), name and describe each pattern once here so screens reuse the*
*pattern instead of re-deriving layout rules per screen.*

- [Pattern 1]: [description]
- ...

## 6. Accessibility & Ergonomics

- **Contrast**: [minimum contrast ratio for text-on-surface token pairs]
- **Minimum hit target size**: [for pointer and, if relevant, touch]
- **Resizability**: [does the window/content scale? What's the minimum
  usable size?]
- **Colour-independence**: [is any state/meaning conveyed by colour alone?
  If so, state the redundant cue — shape, label, position]

## 7. Framework Binding

*This is the only section whose content is inherently framework-specific.*
*State how the abstract tokens/rules above become code in the actual stack.*

### 7.1 Token module location & shape

[Where do the token constants live — a dedicated header/class/module,
independent of any UI-framework base class, so it can be referenced or
tested without constructing a live UI. State the module's path/namespace.]

### 7.2 Rendering integration

[How does a component consume the tokens to draw itself — e.g. "a custom
`LookAndFeel` subclass reads token accessors in its `draw*` overrides"
(JUCE), "a `Theme`/`Painter` class consumed by `OnPaint` overrides"
(WinForms), "a stylesheet/QSS generated from the token table" (Qt), etc.]

### 7.3 State-to-code mapping

[How does §3's abstract state machine map onto the framework's own notion
of component state — e.g. JUCE's `isMouseOverOrDragging()`/`isEnabled()`,
WinForms' `MouseEnter`/`Enabled`, etc.]

### 7.4 Prototyping / mockup sync

[If design mockups are produced outside the framework (e.g. an SVG/Figma
prototype), state how their palette/scale stay in sync with the token
module — manual mirroring with a review checklist is acceptable, but the
sync mechanism must be named, or mockups and shipped code will silently
diverge.]

## 8. Testability

- **Headless verification**: [can the token module and any pure-logic state
  resolution (e.g. "which colour for this state") be exercised without a
  display/window? State the boundary between what's headlessly testable
  and what requires visual/manual verification.]
- **Regression check**: [how is "no raw literal outside the token module"
  verified — code review checklist, a grep-based lint step, a static
  analysis rule?]

## 9. Governance

- **Adding a token**: [process — e.g. "propose in the design-system doc,
  get owner sign-off, then add the constant and migrate call sites in the
  same change"]
- **Changing a token's value**: [must never require touching call sites —
  if it does, the token is not actually a single source of truth; state
  this as an explicit acceptance rule]
- **Deprecating a token**: [process for removing an unused token without
  leaving orphaned call sites]
- **Drift prevention**: [the standing rule that keeps future work compliant
  — e.g. "any change to visual presentation code must reference an existing
  token or introduce a new one in this document in the same change, never a
  bare literal"]
- **Document ownership**: [who approves changes to this design system
  itself]

## 10. Non-Goals

*State explicitly what this design system does **not** cover, so scope*
*creep and scope confusion are both prevented (e.g. "does not define*
*application behaviour/functional requirements — those are owned*
*elsewhere"; "does not cover a specific screen's business logic").*

- [Non-goal 1]
- ...
