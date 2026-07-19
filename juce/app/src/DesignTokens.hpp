#pragma once

// Design token module — the single source of truth for JUCE UI presentation
// (colour, typography, geometry, motion). Three tiers, each referencing only
// the tier below it:
//   global   — raw curated values; the ONLY tier that holds literals.
//   semantic — roles (surface, text, accent, indicator, …) aliasing globals.
//   component — per-control usages/transforms aliasing semantics (only where a
//               transform or independent movement exists; pure pass-throughs
//               reference the semantic token directly).
//
// This module is the value-preserving migration of the literals previously
// scattered across the app: every token below equals, exactly, the literal it
// replaced (ARGB / point size / factor), so the rendered UI is pixel-identical
// by construction. Values are NOT consolidated here (e.g. 13.5 vs 13 font
// sizes stay distinct) — that is deferred, owner-reasoned work.
// [RQ-DSN-001..006, RQ-DSN-010..011, RQ-DSN-020..024, RQ-DSN-060..063,
//  RQ-DSN-090..091; ADR-JUC-014 / DEC-JUC-014-A..D]

#include <juce_graphics/juce_graphics.h>

namespace xplorer::app::tokens
{
    // =====================================================================
    // TIER 1 — GLOBAL : raw curated values. The ONLY tier holding literals.
    // Each colour comment shows the equivalent ARGB for cross-referencing.
    // =====================================================================
    namespace global
    {
        // ---- neutral slate palette (LookAndFeel / dialogs / indicators) ----
        inline const juce::Colour slate900 = juce::Colour::fromRGB(24, 28, 34);   // 0xFF181C22
        inline const juce::Colour slate800 = juce::Colour::fromRGB(30, 36, 44);   // 0xFF1E242C
        inline const juce::Colour slate700 = juce::Colour::fromRGB(40, 46, 54);   // 0xFF282E36
        inline const juce::Colour slate600 = juce::Colour::fromRGB(50, 58, 68);   // 0xFF323A44
        inline const juce::Colour slate500 = juce::Colour::fromRGB(60, 66, 74);   // 0xFF3C424A
        inline const juce::Colour slate400 = juce::Colour::fromRGB(54, 54, 62);   // 0xFF36363E
        inline const juce::Colour slate350 = juce::Colour::fromRGB(44, 44, 52);   // 0xFF2C2C34
        inline const juce::Colour white    = juce::Colours::white;

        // ---- MIDI-activity LED hues (MainComponent LedPanelComponent) ----
        inline const juce::Colour ledGreen = juce::Colour::fromRGB(144, 255, 144); // 0xFF90FF90
        inline const juce::Colour ledBlue  = juce::Colour::fromRGB(92, 171, 255);  // 0xFF5CABFF
        inline const juce::Colour ledRed   = juce::Colour::fromRGB(255, 64, 32);   // 0xFFFF4020

        // ---- vector-background palette (BackgroundRenderer, ADR-JUC-013) ----
        inline const juce::Colour frame    {0xFFB7BDD0}; // block frames / signal lines
        inline const juce::Colour title    {0xFFF2F2F6}; // bold block/section titles
        inline const juce::Colour caption  {0xFFC9CACE}; // parameter captions
        inline const juce::Colour plateTop {0xFF45464F}; // brushed-metal gradient stops
        inline const juce::Colour plateHi  {0xFF3B3C44};
        inline const juce::Colour plateMid {0xFF36363E}; // == slate400 today; kept independent
        inline const juce::Colour plateBot {0xFF303138};
        inline const juce::Colour wood0    {0xFF4A1D08}; // side-rail wood gradient stops
        inline const juce::Colour wood1    {0xFF7C3615};
        inline const juce::Colour wood2    {0xFF8A431C};
        inline const juce::Colour wood3    {0xFF6B2C0F};
        inline const juce::Colour wood4    {0xFF38160A};
        inline const juce::Colour woodGrain{0xFF2E1206};
        inline const juce::Colour barTop   {0xFF3050B8}; // section underline gradient
        inline const juce::Colour barMid   {0xFF24388A};
        inline const juce::Colour barBot   {0xFF1A2A66};

        // ---- type scale (point sizes; one entry per distinct value in use) ----
        inline constexpr float fs16  = 16.0F;
        inline constexpr float fs15  = 15.0F;
        inline constexpr float fs14  = 14.0F;
        inline constexpr float fs135 = 13.5F;
        inline constexpr float fs13  = 13.0F;
        inline constexpr float fs125 = 12.5F;
        inline constexpr float fs12  = 12.0F;
        inline constexpr float fs115 = 11.5F;
        inline constexpr float fs11  = 11.0F;
        inline constexpr float fs9   = 9.0F;

        // ---- geometry (appearance: corner radius / stroke width, in px) ----
        inline constexpr float radius20 = 2.0F;
        inline constexpr float radius15 = 1.5F;
        inline constexpr float stroke10 = 1.0F;
        inline constexpr float stroke20 = 2.0F;
        inline constexpr float stroke24 = 2.4F;

        // ---- motion / timing ----
        inline constexpr float hoverBrightenFactor = 0.4F; // knob ring hover
        inline constexpr float tickBoxBorderAlpha  = 0.6F; // tick-box border vs accent
        inline constexpr int   indicatorHoldMs     = 100;  // MIDI LED retriggerable hold
    }

    // =====================================================================
    // TIER 2 — SEMANTIC : roles aliasing globals. Components reference these.
    // (Colour tokens are value copies of a global — a Colour is a uint32, so a
    //  copy stays a single source of truth: change the global, rebuild, done.)
    // =====================================================================
    namespace semantic
    {
        // ---- colour roles: surfaces / borders ----
        inline const juce::Colour surfaceBase     = global::slate900; // dialogs, progress, tick-box fill
        inline const juce::Colour surfaceRecessed = global::slate800; // combo/popup/tab/list-row/mod-matrix
        inline const juce::Colour surfaceSelected = global::slate600; // selected list row
        inline const juce::Colour controlTrack    = global::slate700; // knob unlit ring track
        inline const juce::Colour borderDefault   = global::slate500; // dialog list cell separator

        // ---- colour roles: indicators (RQ-GUI-022) ----
        inline const juce::Colour indicatorOffFill    = global::slate400;
        inline const juce::Colour indicatorOffBorder  = global::slate350;
        inline const juce::Colour indicatorAutomation = global::ledGreen;
        inline const juce::Colour indicatorSynthIn    = global::ledBlue;
        inline const juce::Colour indicatorSynthOut   = global::ledRed;

        // ---- colour roles: text ----
        inline const juce::Colour textPrimary = global::white;

        // ---- colour roles: vector panel/diagram (ADR-JUC-013) ----
        inline const juce::Colour diagramFrame   = global::frame;
        inline const juce::Colour diagramTitle   = global::title;
        inline const juce::Colour diagramCaption = global::caption;
        inline const juce::Colour panelPlateTop  = global::plateTop;
        inline const juce::Colour panelPlateHi   = global::plateHi;
        inline const juce::Colour panelPlateMid  = global::plateMid;
        inline const juce::Colour panelPlateBot  = global::plateBot;
        inline const juce::Colour panelWood0     = global::wood0;
        inline const juce::Colour panelWood1     = global::wood1;
        inline const juce::Colour panelWood2     = global::wood2;
        inline const juce::Colour panelWood3     = global::wood3;
        inline const juce::Colour panelWood4     = global::wood4;
        inline const juce::Colour panelWoodGrain = global::woodGrain;
        inline const juce::Colour sectionBarTop  = global::barTop;
        inline const juce::Colour sectionBarMid  = global::barMid;
        inline const juce::Colour sectionBarBot  = global::barBot;

        // ---- type roles: every distinct size kept (consolidation deferred,
        //      RQ-DSN-010). Named by the role each size plays today. ----
        inline constexpr float textDisplay  = global::fs16;  // VCO1/VCO2, combo base size
        inline constexpr float textTitle    = global::fs15;  // section titles
        inline constexpr float textSubtitle = global::fs14;  // MIX/LAG/LFO/RAMP, settings table
        inline constexpr float textLabel    = global::fs135; // wide block labels
        inline constexpr float textLabelAlt = global::fs13;  // VCA / VCA1
        inline constexpr float textBody     = global::fs125; // PWM
        inline constexpr float textCaption  = global::fs12;  // parameter captions, toggle caption
        inline constexpr float textWave     = global::fs115; // TRIANGLE/SAWTOOTH/PULSE
        inline constexpr float textSmall    = global::fs11;  // IN/OUT labels
        inline constexpr float textDense    = global::fs9;   // DESTINATION/TRIGGER/NOISE, combo floor

        // ---- geometry roles ----
        inline constexpr float radiusControl      = global::radius20; // tick box outer, block corner
        inline constexpr float radiusControlInner = global::radius15; // tick box checked fill
        inline constexpr float strokeBorder       = global::stroke10; // tick box border
        inline constexpr float strokeLine         = global::stroke20; // frames + signal lines
        inline constexpr float strokeKnobRing     = global::stroke24; // knob track + value arc

        // ---- motion roles ----
        inline constexpr float hoverBrighten   = global::hoverBrightenFactor;
        inline constexpr int   indicatorHoldMs = global::indicatorHoldMs;
    }

    // =====================================================================
    // TIER 3 — COMPONENT : usages/transforms aliasing semantics. Only where a
    // transform or independent-movement potential exists; the effective accent
    // colour itself stays runtime-owned by XplorerLookAndFeel::ledColour()
    // (DEC-JUC-011, unchanged) — these are the transform *parameters* applied
    // to that runtime colour, not a cached copy of it.
    // =====================================================================
    namespace component
    {
        inline constexpr float knobRingHoverBrighten = semantic::hoverBrighten;    // accent.brighter(f) on hover
        inline constexpr float tickBoxBorderAlpha    = global::tickBoxBorderAlpha; // accent.withAlpha(a) border
    }
}
