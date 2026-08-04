#pragma once

// =====================================================================
// GENERATED FILE — DO NOT EDIT BY HAND.
// Source of truth : juce/tools/design-tokens.yaml
// Regenerate with : python3 juce/tools/generate_design_tokens.py
// =====================================================================
//
// Single source of truth for JUCE UI presentation (colour, typography,
// geometry, motion). Three tiers, each referencing only the tier below:
// global (raw literals) -> semantic (roles) -> component (usages).
// Every value equals the literal it replaced, so the UI is pixel-identical
// by construction. [RQ-DSN-001..024, RQ-DSN-060..063; ADR-JUC-014, ADR-JUC-015]

#include <juce_graphics/juce_graphics.h>

namespace xplorer::app::tokens
{
    // -----------------------------------------------------------------
    // TIER 1 — GLOBAL : raw curated values (the only tier with literals).
    // -----------------------------------------------------------------
    namespace global
    {
        inline const juce::Colour slate900 = juce::Colour(0xFF181C22);  // fromRGB(24,28,34)
        inline const juce::Colour slate800 = juce::Colour(0xFF1E242C);  // fromRGB(30,36,44)
        inline const juce::Colour slate600 = juce::Colour(0xFF323A44);  // fromRGB(50,58,68)
        inline const juce::Colour slate500 = juce::Colour(0xFF3C424A);  // fromRGB(60,66,74)
        inline const juce::Colour slate400 = juce::Colour(0xFF36363E);  // fromRGB(54,54,62)
        inline const juce::Colour slate350 = juce::Colour(0xFF2C2C34);  // fromRGB(44,44,52)
        inline const juce::Colour white = juce::Colour(0xFFFFFFFF);  // juce::Colours::white
        inline const juce::Colour ledGreen = juce::Colour(0xFF90FF90);  // fromRGB(144,255,144) — automation in
        inline const juce::Colour ledBlue = juce::Colour(0xFF5CABFF);  // fromRGB(92,171,255) — synth in
        inline const juce::Colour ledRed = juce::Colour(0xFFFF4020);  // fromRGB(255,64,32) — synth out
        inline const juce::Colour frame = juce::Colour(0xFFB7BDD0);  // block frames / signal lines
        inline const juce::Colour title = juce::Colour(0xFFF2F2F6);  // bold block/section titles
        inline const juce::Colour caption = juce::Colour(0xFFC9CACE);  // parameter captions
        inline const juce::Colour plateTop = juce::Colour(0xFF45464F);  // brushed-metal gradient stops
        inline const juce::Colour plateHi = juce::Colour(0xFF3B3C44);
        inline const juce::Colour plateMid = juce::Colour(0xFF36363E);  // == slate400 today; kept independent
        inline const juce::Colour plateBot = juce::Colour(0xFF303138);
        inline const juce::Colour wood0 = juce::Colour(0xFF4A1D08);  // side-rail wood gradient stops
        inline const juce::Colour wood1 = juce::Colour(0xFF7C3615);
        inline const juce::Colour wood2 = juce::Colour(0xFF8A431C);
        inline const juce::Colour wood3 = juce::Colour(0xFF6B2C0F);
        inline const juce::Colour wood4 = juce::Colour(0xFF38160A);
        inline const juce::Colour woodGrain = juce::Colour(0xFF2E1206);  // wood grain hairlines
        inline const juce::Colour barTop = juce::Colour(0xFF3050B8);  // section underline gradient
        inline const juce::Colour barMid = juce::Colour(0xFF24388A);
        inline const juce::Colour barBot = juce::Colour(0xFF1A2A66);
        inline constexpr float fs16 = 16.0F;
        inline constexpr float fs15 = 15.0F;
        inline constexpr float fs14 = 14.0F;
        inline constexpr float fs135 = 13.5F;
        inline constexpr float fs13 = 13.0F;
        inline constexpr float fs125 = 12.5F;
        inline constexpr float fs12 = 12.0F;
        inline constexpr float fs115 = 11.5F;
        inline constexpr float fs11 = 11.0F;
        inline constexpr float fs9 = 9.0F;
        inline constexpr float radius20 = 2.0F;
        inline constexpr float radius15 = 1.5F;
        inline constexpr float stroke10 = 1.0F;
        inline constexpr float stroke15 = 1.5F;  // every stroke of the background diagram — block frames, signal lines, control sub-panel frames. Thicker than the button/control frame (stroke10), thinner than the former uniform diagram width (stroke20), RQ-GUI-051
        inline constexpr float stroke20 = 2.0F;
        inline constexpr float stroke24 = 2.4F;
        inline constexpr int indicatorSize = 7;  // MIDI LED lamp diameter. FIRST VERSION (owner, 2026-08-04, RQ-GUI-056): 7 px is provisional and expected to be retuned after visual review -- which is why it is a token and no longer a LED_SIZE constant. Three independent ceilings, all satisfied: <= 10 (twice the 5 px .NET reference, owner allowance), < 14 (the check-box indicator, so a read-only lamp can never be mistaken for a control), <= 8 (three lamps plus spacing must fit the extracted 32x8 _ledPanelControl bounds -- the binding limit). ADR-JUC-031 (DEC-JUC-096)
        inline constexpr float hoverBrightenFactor = 0.4F;  // knob ring hover brighten
        inline constexpr float tickBoxBorderAlpha = 0.6F;  // tick-box border vs accent
        inline constexpr int indicatorHoldMs = 100;  // MIDI LED retriggerable hold
        inline constexpr float indicatorGlowAlpha = 0.55F;  // peak opacity of the radial glow at a lit MIDI LED's centre, fading to fully transparent at its outer edge. Below the accent hover brighten (0.4 is a colour delta, this is an alpha) and deliberately under 1.0: a glow that reaches full opacity reads as a second, larger lamp rather than as light around the first. ADR-JUC-031 (DEC-JUC-095)
        inline constexpr float indicatorGlowRadius = 2.0F;  // glow outer radius as a MULTIPLE of the lamp's own radius (indicatorSize/2), not of its diameter -- at 2.0 the glow reaches exactly one lamp-radius past the visible dot's edge (3.5 px at the 7 px first version), so retuning indicatorSize scales the halo with it. Checked against the real clearance around _ledPanelControl (11 px to the VFD glass, 14+ px to the button row): a diameter-multiple reading would have reached 14 px and left only 0.5 px before touching the glass -- deliberately kept well inside instead. Also sets how far LedPanelComponent inflates its painting bounds past the extracted panel rect, since the 8 px-high panel has no room for a halo (DEC-JUC-097). ADR-JUC-031 (DEC-JUC-095)
        inline constexpr float knobTrackAlpha = 0.2F;  // owner review 2026-07: reference's literal 5/255 read as invisible once combined with our transparent knob body (ADR-JUC-009) -- at minimum value the knob had no visible shape at all. Raised to a clearly-visible-but-subtle wash.
        inline constexpr float disabledAlpha = 0.5F;  // owner-confirmed 2026-07: shared Disabled treatment for check boxes/radios/combos (RQ-DSN-032, TASK-JUC-109/110, ADR-JUC-017)
        inline constexpr int space2 = 2;  // hairline inset — row.reduced(0,2), swatch.reduced(2)
        inline constexpr int space4 = 4;  // xs gap — pre-automation-table gap
        inline constexpr int space6 = 6;  // sm gap — inter-section gaps, label/bar gap
        inline constexpr int space8 = 8;  // md gap — inter-button gaps
        inline constexpr int space12 = 12;  // lg gap — dialog outer margin
        inline constexpr int space24 = 24;  // xl gap — progress bar height
        inline constexpr int dialogRowHeight = 28;  // SettingsDialog/ProgressWindow row height
        inline constexpr int dialogLabelWidth = 150;  // SettingsDialog label column width, sized to the longest existing label
        inline constexpr int dialogSwatchWidth = 40;  // colour swatch cell width (value-preserving: the pre-existing knob-LED swatch literal)
        inline constexpr int dialogChooseWidth = 100;  // uniform 'Choose...' button width (mockup: all colour buttons identical)
        inline constexpr int dialogBlockLabelWidth = 90;  // block-name label width in the 2x4 colour grid (sized to 'MOD MATRIX')
        inline constexpr int dialogResetWidth = 140;  // 'Reset to defaults' button width
        inline constexpr int dialogGroupHeaderHeight = 16;  // vertical clearance under a GroupComponent title before the first row
        inline constexpr float comboTextSize = 12.0F;  // one fixed size for every combo box; measured to fit every label of every value list in the embedded face with >=9.9px to spare (RQ-GUI-047)
        inline constexpr int comboArrowZone = 20;  // arrow zone width, replacing LookAndFeel_V4's 30px (DEC-JUC-047)
        inline constexpr int comboLabelBorder = 2;  // combo Label left/right border, replacing juce::Label's default 5px (DEC-JUC-047)
        inline const juce::Colour blockVco = juce::Colour(0xFF549ECB);  // VCO1 / VCO2 / FM group (v2, H238)
        inline const juce::Colour blockLag = juce::Colour(0xFF8F969D);  // LAG (achromatic)
        inline const juce::Colour blockTrack = juce::Colour(0xFFB19047);  // TRACK / tracking generator (v2)
        inline const juce::Colour blockVcf = juce::Colour(0xFF7AA364);  // VCF / VCA (v2)
        inline const juce::Colour blockEnv = juce::Colour(0xFFC88160);  // envelope generator (v2)
        inline const juce::Colour blockLfo = juce::Colour(0xFFA087C9);  // LFO (v2)
        inline const juce::Colour blockRamp = juce::Colour(0xFF37A9A3);  // RAMP (v2)
        inline const juce::Colour blockMatrix = juce::Colour(0xFF6579EE);  // modulation matrix — Oberheim-signature blue (v2, L0.62/C0.175/H273)
        inline constexpr float sectionBarFadeEnd = 0.35F;  // section-bar opacity at the far end; 1.0 at the label end (RQ-GUI-037 'bright at the label end' preserved with the block hue)
        inline constexpr float blockFillAlpha = 0.3F;  // tinted block fill: block hue at this alpha over the panel plate. Originally 0.18, measured from the owner-supplied modernisation mockup (fill #443326 on plate #242528 for block #C27A52), RQ-DSN-094; raised to 0.30 (owner decision, after 0.24 was reviewed in the mockup and judged too subtle) so the block background reads less transparent and the block is easier to identify, RQ-GUI-051 — deviation from the measured mockup value, recorded here per the design-system deviation rule.
        inline constexpr float blockFrameRelief = 0.2F;  // block frame relief: JUCE Colour::darker() amount applied to the BOTTOM edge of the frame gradient, top edge stays the pure block hue (mockup ratio bottom/top = 0.84). RQ-DSN-094
        inline constexpr int controlRowHeight = 17;  // reference control-row height (check box / radio button); BoundRadioGroup rows use it so radios align with sibling check boxes. TASK-JUC-108, RQ-GUI-040
        inline const juce::Colour vfdPhosphor = juce::Colour(0xFF00FFA1);  // phosphor hue; red stays 0 through the halo and only rises once the core clips, so this is the whole colour of the display
        inline constexpr float vfdSegLeft = 0.28713F;  // left segment rail
        inline constexpr float vfdSegRight = 0.75083F;  // right segment rail
        inline constexpr float vfdSegTop = 0.11833F;  // top segment rail
        inline constexpr float vfdSegBottom = 0.96333F;  // bottom segment rail; also the pivot the italic shear rotates about
        inline constexpr float vfdSegStroke = 0.09583F;  // segment thickness
        inline constexpr float vfdSegGap = 0.03222F;  // shortening at each segment end, so adjacent segments read as separate
        inline constexpr float vfdSegSlant = 0.13267F;  // italic shear, x per unit y; dimensionless, so it does not scale
        inline constexpr float vfdGlowRadius = 0.17906F;  // glow sigma. MUST be multiplied by the render scale (DEC-JUC-053) or the halo shrinks visually as the window grows
        inline constexpr float vfdGlowAmount = 1.23067F;  // glow amplitude added over the core; one Gaussian, not two — see DEC-JUC-054 for why the second was refused
        inline constexpr float vfdUnlitLevel = 0.08627F;  // always-drawn unlit segment bed. MEASURED off the baseline's blank cell, not fitted: left free it trades against the glow and parks on its bound, over-brightening the bed
        inline constexpr float vfdWhiteLift = 0.43333F;  // how fast the core washes toward white once the phosphor hue clips past 1.0
        inline constexpr float vfdDotX = 0.31404F;  // colon dot centre, upright — the renderer applies the shear
        inline constexpr float vfdDotSize = 0.18634F;  // colon dot side
        inline constexpr float vfdDotUpperY = 0.4F;  // upper colon dot centre
        inline constexpr float vfdDotLowerY = 0.73333F;  // lower colon dot centre
        inline constexpr float vfdUnderscoreY = 1.0F;  // underscore bar centre; sits BELOW the bottom rail, which is precisely why no segment can express it
        inline constexpr int vfdBezelMarginV = 6;  // band thickness above and below the glass
        inline constexpr int vfdBezelMarginH = 4;  // band thickness left and right. DELIBERATELY thinner than the vertical one (DEC-JUC-060): at 6 the mockup crowded the modulation matrix on the right while having air above
        inline constexpr float vfdBezelInnerShadow = 0.75F;  // alpha of the shadow the band casts onto the glass edge — the cue that the glass sits BELOW the band
        inline constexpr float vfdBezelInnerWidth = 3.0F;  // width of that inner shadow
        inline constexpr float vfdBezelRimDark = 0.5F;  // alpha of the dark hairline on the bezel's TOP outer edge
        inline constexpr float vfdBezelRimLight = 0.18F;  // alpha of the light hairline on the BOTTOM outer edge. Dark-top/light-bottom is the inverse of the raised-plate relief (RQ-DSN-094) — under light from above that inversion IS the difference between a recess and a bump, so the two must never converge
    }

    // -----------------------------------------------------------------
    // TIER 2 — SEMANTIC : roles aliasing globals. Components use these.
    // -----------------------------------------------------------------
    namespace semantic
    {
        inline const juce::Colour surfaceBase = global::slate900;  // dialogs, progress, tick-box fill
        inline const juce::Colour surfaceRecessed = global::slate800;  // combo/popup/tab/list-row/mod-matrix
        inline const juce::Colour surfaceSelected = global::slate600;  // selected list row
        inline const juce::Colour controlTrack = global::white;  // knob unlit ring track base (with component.knobTrackAlpha)
        inline const juce::Colour borderDefault = global::slate500;  // dialog list cell separator
        inline const juce::Colour indicatorOffFill = global::slate400;
        inline const juce::Colour indicatorOffBorder = global::slate350;
        inline const juce::Colour indicatorAutomation = global::ledGreen;
        inline const juce::Colour indicatorSynthIn = global::ledBlue;
        inline const juce::Colour indicatorSynthOut = global::ledRed;
        inline const juce::Colour textPrimary = global::white;
        inline const juce::Colour diagramFrame = global::frame;
        inline const juce::Colour diagramTitle = global::title;
        inline const juce::Colour diagramCaption = global::caption;
        inline const juce::Colour panelPlateTop = global::plateTop;
        inline const juce::Colour panelPlateHi = global::plateHi;
        inline const juce::Colour panelPlateMid = global::plateMid;
        inline const juce::Colour panelPlateBot = global::plateBot;
        inline const juce::Colour panelWood0 = global::wood0;
        inline const juce::Colour panelWood1 = global::wood1;
        inline const juce::Colour panelWood2 = global::wood2;
        inline const juce::Colour panelWood3 = global::wood3;
        inline const juce::Colour panelWood4 = global::wood4;
        inline const juce::Colour panelWoodGrain = global::woodGrain;
        inline const juce::Colour sectionBarTop = global::barTop;
        inline const juce::Colour sectionBarMid = global::barMid;
        inline const juce::Colour sectionBarBot = global::barBot;
        inline const juce::Colour blockVco = global::blockVco;
        inline const juce::Colour blockLag = global::blockLag;
        inline const juce::Colour blockTrack = global::blockTrack;
        inline const juce::Colour blockVcf = global::blockVcf;
        inline const juce::Colour blockEnv = global::blockEnv;
        inline const juce::Colour blockLfo = global::blockLfo;
        inline const juce::Colour blockRamp = global::blockRamp;
        inline const juce::Colour blockMatrix = global::blockMatrix;
        inline constexpr float textDisplay = global::fs16;  // VCO1/VCO2, combo base size
        inline constexpr float textTitle = global::fs15;  // section titles
        inline constexpr float textSubtitle = global::fs14;  // MIX/LAG/LFO/RAMP, settings table
        inline constexpr float textLabel = global::fs135;  // wide block labels
        inline constexpr float textLabelAlt = global::fs13;  // VCA / VCA1
        inline constexpr float textBody = global::fs125;  // PWM
        inline constexpr float textCaption = global::fs12;  // parameter captions, toggle caption
        inline constexpr float textWave = global::fs115;  // TRIANGLE/SAWTOOTH/PULSE
        inline constexpr float textSmall = global::fs11;  // IN/OUT labels
        inline constexpr float textDense = global::fs9;  // DESTINATION/TRIGGER/NOISE, combo floor
        inline constexpr float radiusControl = global::radius20;  // tick box outer, block corner
        inline constexpr float radiusControlInner = global::radius15;  // tick box checked fill
        inline constexpr float strokeBorder = global::stroke10;  // tick box border
        inline constexpr float strokeDiagram = global::stroke15;  // EVERY stroke of the background diagram: labelled block frames, signal lines and neutral control sub-panel frames, deliberately one single width so the diagram reads as one drawing (owner decision, RQ-GUI-051)
        inline constexpr float strokeLine = global::stroke20;  // control-widget strokes: combo-box arrow, page-family selector outline. No longer used by the background diagram — that moved to strokeDiagram (RQ-GUI-051). No longer used for a focus ring either — RQ-GUI-054 removed the keyboard-focus indicator (and the strokeFocusRing role it briefly had) on 2026-08-04
        inline constexpr float strokeKnobRing = global::stroke24;  // knob track + value arc
        inline constexpr float hoverBrighten = global::hoverBrightenFactor;
        inline constexpr int indicatorHoldMs = global::indicatorHoldMs;
        inline constexpr int indicatorSize = global::indicatorSize;  // MIDI LED lamp diameter, RQ-GUI-056
        inline constexpr float indicatorGlowAlpha = global::indicatorGlowAlpha;  // MIDI LED glow peak alpha, RQ-GUI-056
        inline constexpr float indicatorGlowRadius = global::indicatorGlowRadius;  // MIDI LED glow radius multiplier, RQ-GUI-056
        inline constexpr int layoutHairline = global::space2;  // SettingsDialog row/swatch fine inset
        inline constexpr int layoutFieldGap = global::space4;  // SettingsDialog pre-table gap
        inline constexpr int layoutSectionGap = global::space6;  // SettingsDialog section gaps, ProgressWindow label/bar gap
        inline constexpr int layoutButtonGap = global::space8;  // SettingsDialog/ProgressWindow inter-button gap
        inline constexpr int layoutMargin = global::space12;  // dialog outer margin
        inline constexpr int dialogRowHeight = global::dialogRowHeight;
        inline constexpr int dialogLabelWidth = global::dialogLabelWidth;
        inline constexpr int dialogSwatchWidth = global::dialogSwatchWidth;  // settings colour-swatch width, RQ-GUI-046
        inline constexpr int dialogChooseWidth = global::dialogChooseWidth;  // uniform Choose... button width, RQ-GUI-046
        inline constexpr int dialogBlockLabelWidth = global::dialogBlockLabelWidth;  // block label width in the colour grid, RQ-GUI-046
        inline constexpr int dialogResetWidth = global::dialogResetWidth;  // Reset to defaults button width, RQ-GUI-046
        inline constexpr int dialogGroupHeaderHeight = global::dialogGroupHeaderHeight;  // group-title clearance, RQ-GUI-046
        inline constexpr float comboTextSize = global::comboTextSize;  // fixed combo-box text size, RQ-GUI-047
        inline constexpr int comboArrowZone = global::comboArrowZone;  // combo arrow zone, RQ-GUI-047
        inline constexpr int comboLabelBorder = global::comboLabelBorder;  // combo label border, RQ-GUI-047
        inline const juce::Colour textHint = global::caption;  // muted settings hint lines (same muted grey as diagram captions), RQ-GUI-046
        inline constexpr int progressBarHeight = global::space24;  // ProgressWindow bar height
        inline constexpr int controlRowHeight = global::controlRowHeight;  // shared control-row height for stacked in-panel controls (BoundRadioGroup rows aligned with sibling check boxes)
    }

    // -----------------------------------------------------------------
    // TIER 3 — COMPONENT : usages/transforms aliasing semantics.
    // -----------------------------------------------------------------
    namespace component
    {
        inline constexpr float knobRingHoverBrighten = semantic::hoverBrighten;  // accent.brighter(f) on hover
        inline constexpr float tickBoxBorderAlpha = global::tickBoxBorderAlpha;  // accent.withAlpha(a) border
        inline constexpr float knobTrackAlpha = global::knobTrackAlpha;  // controlTrack.withAlpha(a)
        inline constexpr float disabledAlpha = global::disabledAlpha;  // shared control disabled treatment, ADR-JUC-017
        inline constexpr float sectionBarFadeEnd = global::sectionBarFadeEnd;  // block-colour section bar: opacity at the far end (bright at the label end), RQ-DSN-092
        inline constexpr float blockFillAlpha = global::blockFillAlpha;  // blockColour.withAlpha(a) filled behind a labelled block, RQ-DSN-094
        inline constexpr float blockFrameRelief = global::blockFrameRelief;  // blockColour.darker(a) for the frame's bottom edge, RQ-DSN-094
        inline const juce::Colour vfdPhosphor = global::vfdPhosphor;  // lit segment / halo colour
        inline constexpr float vfdSegLeft = global::vfdSegLeft;  // segment rails, fractions of cell width
        inline constexpr float vfdSegRight = global::vfdSegRight;
        inline constexpr float vfdSegTop = global::vfdSegTop;
        inline constexpr float vfdSegBottom = global::vfdSegBottom;  // also the italic shear pivot
        inline constexpr float vfdSegStroke = global::vfdSegStroke;
        inline constexpr float vfdSegGap = global::vfdSegGap;
        inline constexpr float vfdSegSlant = global::vfdSegSlant;
        inline constexpr float vfdGlowRadius = global::vfdGlowRadius;  // multiply by the render scale — DEC-JUC-053
        inline constexpr float vfdGlowAmount = global::vfdGlowAmount;
        inline constexpr float vfdUnlitLevel = global::vfdUnlitLevel;
        inline constexpr float vfdWhiteLift = global::vfdWhiteLift;
        inline constexpr float vfdDotX = global::vfdDotX;  // off-model primitives, DEC-JUC-052
        inline constexpr float vfdDotSize = global::vfdDotSize;
        inline constexpr float vfdDotUpperY = global::vfdDotUpperY;
        inline constexpr float vfdDotLowerY = global::vfdDotLowerY;
        inline constexpr float vfdUnderscoreY = global::vfdUnderscoreY;
        inline const juce::Colour vfdBezelBandTop = global::plateBot;  // band gradient start (top) — the plate's DARK stop
        inline const juce::Colour vfdBezelBandBottom = global::plateTop;  // band gradient end (bottom) — the plate's LIGHT stop
        inline constexpr int vfdBezelMarginV = global::vfdBezelMarginV;
        inline constexpr int vfdBezelMarginH = global::vfdBezelMarginH;  // thinner than MarginV on purpose, DEC-JUC-060
        inline constexpr float vfdBezelInnerShadow = global::vfdBezelInnerShadow;
        inline constexpr float vfdBezelInnerWidth = global::vfdBezelInnerWidth;
        inline constexpr float vfdBezelRimDark = global::vfdBezelRimDark;
        inline constexpr float vfdBezelRimLight = global::vfdBezelRimLight;
    }

}
