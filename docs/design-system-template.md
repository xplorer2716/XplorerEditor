# Desktop Application Design System — Reusable Template

*Framework-agnostic. Applies to JUCE, .NET (WinForms/WPF), Qt, or any native*
*desktop UI stack. Copy this file into a new project and fill in every*
*`[...]`. This template is written to be followed literally by an agentic*
*coding AI, not just read by a human — see §0 before anything else.*

---

## 0. Agent Operating Rules (read this first)

*This section exists because scattered, inconsistent visual literals are*
*usually not introduced by one bad decision — they accumulate one small,*
*locally-reasonable AI or human edit at a time, each too small to trigger a*
*review. §0 is the standing contract that prevents that. An agentic AI*
*making ANY change that touches colour, size, spacing, radius, duration, or*
*component state SHALL follow this procedure before writing code.*

**Before writing a visual literal, resolve it through the token chain — never
skip a tier:**

```
Need a colour/size/duration/radius for new or changed UI code
  │
  ├─ 1. Does an existing COMPONENT token already name this exact thing?
  │     → yes: use it. Done.
  │
  ├─ 2. Does an existing SEMANTIC token fit the role (surface, text, border,
  │     accent, status)?
  │     → yes: reference it directly, or add a component token that aliases
  │       it if this component will need to override it independently later.
  │
  ├─ 3. No existing token fits.
  │     → Is this value genuinely reusable (will another component need the
  │       same role)?
  │         → yes: propose a new SEMANTIC token in §2 of the instantiated
  │           design-system document (add the row to the token table), THEN
  │           use it. Do this in the same change — never add the raw literal
  │           "temporarily" with a TODO to tokenise later.
  │         → no, truly one-off: still declare it as a named local constant
  │           at the top of the file/class (never inline) AND record it as a
  │           COMPONENT-scoped token in §4. A one-off is rare — if you find
  │           yourself adding a second one-off with the same value anywhere
  │           in the codebase, that is proof it was not one-off; promote it
  │           to a semantic token instead.
  │
  └─ 4. NEVER invent a new GLOBAL/raw palette or scale entry ad hoc while
        implementing a component. Global tokens (§2.0) are a closed set
        curated by design intent, not grown organically by feature work. If
        a component seems to need a wholly new base colour or size step,
        stop and ask (this is exactly the "two valid designs, irreconcilable
        trade-off" case that warrants a human decision, not a default).
```

**Hard rules (grep-checkable — treat a violation as a defect, not a style nit):**

1. No raw colour literal (`#RRGGBB`, `rgb(...)`, framework-native colour
   constructors with numeric arguments) outside the token module.
2. No raw numeric size/duration literal for a tokenised category (font size,
   corner radius, stroke width, spacing, animation duration) outside the
   token module — including inside a `LookAndFeel`/`Theme`/`Painter` class.
   The one exception: the token module's own definitions.
3. Every component token resolves to a semantic token; every semantic token
   resolves to a global token. No component token may hold a raw value
   directly (see §2's three-tier model) — this is what makes re-theming a
   one-place edit instead of a search-and-replace.
4. When two components independently need "the same-looking thing", that is
   a signal they should share one token, not each define their own with the
   same value — duplicate *values* under different *names* are exactly as
   bad as duplicate raw literals, because the next person who changes one
   won't know to change the other.
5. If you are about to write a state (hover/pressed/disabled/focused/
   selected) visual rule that isn't already in §3's state table, you are
   either implementing an already-decided state (look it up, don't
   reinvent it) or introducing a new one (add it to §3, don't bury a
   one-off rule inside a single component).

**When genuinely blocked** (missing token AND not clearly one-off AND not
obviously a new semantic role either): stop and ask, per the same
"ask when blocking" discipline as functional work — do not guess a colour or
size and move on. A wrong guess here is cheap to make and expensive to find
later, because it looks intentional.

---

## 1. Purpose & Principles

*State the product's visual/interaction philosophy in 2–5 falsifiable*
*bullets — each should be specific enough that a design choice can be*
*checked against it ("does this respect principle N?"), not vague taste*
*statements. Every token/component decision in this document should trace*
*back to one of these.*

- [Principle 1]
- [Principle 2]
- ...

## 2. Foundations — Design Tokens

A token is a named constant standing in for a raw value. Tokens are declared
in **exactly one module** (§7.1) and organized in **three tiers**, each
referencing only the tier below it:

```
Component tokens   (e.g. knob.track.colour)        — what a specific control uses
      │  aliases
Semantic tokens     (e.g. colour.surface.recessed)  — what role a value plays
      │  aliases
Global tokens        (e.g. colour.slate.800)        — the raw curated value
```

- **Global tier**: the raw palette/scale, curated deliberately (§0 rule 4)
  — this is the *only* tier allowed to hold a literal.
- **Semantic tier**: names a *role* (surface, text, border, accent, status,
  indicator), independent of any one component — reusable by construction.
- **Component tier**: names a *usage* inside one control's anatomy, aliasing
  a semantic token. Only add a component-tier token when that usage might
  need to move independently of its semantic role later (e.g. theming one
  control differently); otherwise reference the semantic token directly.

### Naming grammar

Use a consistent, dot-delimited spec name for every tier; transliterate to
the target language's constant convention in the token module (state the
transliteration rule once in §7.1 — e.g. `colour.surface.recessed` →
`kColourSurfaceRecessed` in C++, `Colour_Surface_Recessed` in C#).

| Tier | Pattern | Example |
|---|---|---|
| Global | `<category>.<family>.<step>` | `colour.slate.800`, `size.space.4` |
| Semantic | `<category>.<role>.<variant?>` | `colour.surface.recessed`, `colour.text.disabled` |
| Component | `<component>.<part>.<state?>` | `knob.ring.track`, `button.toggle.checked` |

### 2.1 Colour

- **Global palette**: [family/step → hex value table — the only place hex
  values appear]
- **Semantic roles** (fill in the roles the product actually needs; typical
  minimum set): `surface.base`, `surface.recessed` (inputs/lists),
  `surface.elevated` (popovers/dialogs-over-content), `border.default`,
  `text.primary`, `text.secondary`, `text.disabled`, `accent.default`
  (brand/primary interactive colour), one entry per **status/indicator**
  meaning the app has (success/error/warning, or domain-specific indicators).
- **Theming**: [light/dark/multiple themes? If yes: each semantic token maps
  to one global value *per theme*, component tokens are theme-independent
  (they always alias the semantic tier). If no: state that explicitly.]
- **User-overridable colours**: [if an end user can change a colour via
  settings, it is still a semantic or component token — the setting supplies
  the *effective* value at runtime; a single accessor resolves "default
  token vs. user override", never two independent code paths.]

### 2.2 Typography

- **Font family/fallback**: [primary + fallback]
- **Type scale** (semantic, by *role* not by pixel count): [e.g.
  `text.title`, `text.label`, `text.caption`, `text.dense` → size table]
- **Dynamic-fit bounds**: any text programmatically shrunk to fit shares one
  minimum-legibility floor and one maximum, both scale entries — never a
  bespoke floor per call site.

### 2.3 Spacing & Layout Grid

- **Base unit**: [e.g. 4px/8px — pick one]
- **Spacing scale**: named multiples (`space.tight`, `space.default`,
  `space.loose`, ...) — components reference the name, not the multiple.
- **Canvas/DPI scaling model**: [if content renders on a logical canvas that
  scales with window/display, state the model precisely: uniform vs.
  independent-axis scale, where the factor is computed, what stays
  pixel-fixed if anything (e.g. hairline borders).]

### 2.4 Geometry

- **Corner radius scale**: [`radius.sharp` / `radius.soft` / `radius.round`
  → value table]
- **Stroke/border width scale**: [named weights → value table]
- **Elevation/shadow model**: [named levels, or state "flat — no elevation"
  explicitly so it isn't silently assumed later]

### 2.5 Iconography & Imagery

- **Style**: [outline/filled, stroke width, corner treatment]
- **Format & sizing**: [vector preferred; if raster, source resolution +
  scaling policy — raster assets are the first thing to blur under
  non-integer window scale, state the mitigation]
- **Sourcing policy**: [hand-authored/vector vs. photographic — state which,
  and why, since it drives crispness at scale]

### 2.6 Motion & Timing

- **Duration scale**: name every duration category the app actually uses
  (`motion.hover`, `motion.transientHold`, `motion.fade`, ...) — never a
  bespoke millisecond literal per component.
- **Easing**: [per duration category if they differ]
- **When not to animate**: [explicit exceptions — e.g. real-time/low-latency
  controls where any added delay between input and visible feedback is a
  regression, not a polish opportunity]

## 3. Component Interaction States

The canonical state machine every interactive control implements, and the
**shared visual rule** for each — decided once here, referenced everywhere.
A component's own spec (§4) states *which* states it implements, never
*how a state looks* — that would be a re-decision, which is the drift this
document exists to prevent.

| State | Trigger | Visual rule (token reference, not a literal) | Notes |
|---|---|---|---|
| Idle/default | no interaction | [base tokens] | |
| Hover | pointer over, not pressed | [rule, e.g. "brighten `accent.default` by `motion` -scale factor F"] | must not fire on touch-only input if the platform has none |
| Active/pressed | pointer down / dragging | [rule] | |
| Focused | keyboard focus | [rule] | must remain visible without a pointer; never rely on colour alone (§6) |
| Disabled | control inactive | [rule, e.g. shared opacity/desaturation] | |
| Selected | represents a chosen option | [rule] | |
| Error/invalid | failed validation | [rule] | only if the app validates input |

## 4. Component Catalogue

For each distinct control type, use this compact structure so the entry is
scannable and diffable — prefer tables over prose:

```markdown
### <Component name>
| Field | Value |
|---|---|
| Anatomy | [named parts, e.g. "ring track, value arc, crown"] |
| States implemented | [subset of §3] |
| Tokens used | [component-tier token names, one per anatomy part] |
| Variants | [only if >1 actually exists] |
| Behaviour owner | [link to the functional requirement — this catalogue
                     owns appearance only, never behaviour] |
```

*This is a structure to repeat per real component in the instantiated*
*document — this template does not enumerate components itself.*

## 5. Layout Patterns

*Name and describe each recurring macro-layout pattern once, so screens*
*reuse it instead of re-deriving layout rules ad hoc.*

- [Pattern 1]: [description, e.g. "fixed diagram with controls anchored to
  measured points — single source of truth for anchor coordinates"]
- ...

## 6. Accessibility & Ergonomics

- **Contrast**: [minimum ratio for each text-on-surface token pairing —
  check semantic pairs, not just the default theme]
- **Minimum hit target size**: [pointer, and touch if relevant]
- **Resizability**: [does content scale with the window? Minimum usable size?]
- **Never colour-alone**: [any state/meaning conveyed by colour must have a
  redundant cue — shape, label, position, icon. Audit §3's state table and
  §2.1's status colours against this.]

## 7. Framework Binding

*The only section whose content is inherently framework-specific.*

### 7.1 Token module

[Path/namespace of the single module holding all token constants. It SHALL
have no dependency on a live UI context (window, graphics context,
component tree) so it can be read/tested headlessly. State the
spec-name → code-identifier transliteration rule here (see §2 naming
grammar).]

### 7.2 Rendering integration

[How a component consumes tokens to draw itself — e.g. a custom
`LookAndFeel` subclass reading token accessors in its `draw*` overrides
(JUCE); a `Theme`/`Painter` consumed by `OnPaint` (WinForms); a stylesheet
generated from the token table (Qt).]

### 7.3 State-to-code mapping

[How §3's abstract states map onto the framework's own state queries — e.g.
`isMouseOverOrDragging()` / `isEnabled()` (JUCE), `MouseEnter` / `Enabled`
(WinForms).]

### 7.4 Prototyping / mockup sync

[If mockups are produced outside the framework (SVG/Figma/etc.), name the
mechanism keeping their palette/scale in sync with the token module —
manual mirroring with a stated review checkpoint is acceptable; "no
mechanism" is not, because it guarantees silent drift.]

## 8. Testability

- **Headless verification**: [what's testable without a display — token
  resolution, state→token mapping logic — vs. what needs visual/manual
  verification.]
- **Regression check**: [how "no raw literal outside the token module" is
  verified — e.g. a grep-based check listed as a Definition-of-Done item,
  run before any visual-code change is considered complete.]

## 9. Governance

- **Adding a token**: [process — propose in this document (which tier, which
  name per the grammar, which value), get sign-off if the project requires
  it, then add the constant and migrate call sites in the same change.]
- **Changing a token's value**: must never require a call-site edit — if it
  does, it was never actually a single source of truth; treat that as a bug
  in the token structure, not a one-off exception.
- **Deprecating a token**: [process for removing an unused token without
  orphaning call sites — grep for zero remaining references first.]
- **Drift prevention**: the standing rule (§0) that keeps future work
  compliant without re-litigating it per change.
- **Document ownership**: [who approves changes to this design system itself]

## 10. Non-Goals

*State explicitly what this document does not cover, so scope is bounded.*

- Does not define application/business behaviour — that's owned by
  functional requirements elsewhere; this document owns *appearance and
  interaction feedback* of that behaviour, never the behaviour itself.
- [Other non-goals specific to the project]

---

## Appendix: Good / Bad Examples

*Concrete right/wrong pairs an agentic AI can pattern-match against.*
*Replace with project-specific examples once instantiated — generic ones*
*shown here to illustrate the pattern itself.*

| ❌ Bad | ✅ Good | Why |
|---|---|---|
| `g.setColour(Colour::fromRGB(30,36,44));` | `g.setColour(Tokens::colour(SurfaceRecessed));` | Raw literal vs. token reference (§0 rule 1) |
| A second component defines its own `kDarkBg = 0x1E242C` matching an existing token's value | Reuse the existing semantic token | Same value, different name = untraceable duplicate (§0 rule 4) |
| `slider.brighter(0.4f)` hard-coded per component | `slider.brighter(Tokens::motion(HoverBrighten))` | Shared state rule vs. re-decided per component (§3) |
| A component token holds a raw hex directly | A component token aliases a semantic token, which aliases a global token | Breaks the three-tier chain (§2, §0 rule 3) |
