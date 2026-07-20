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
        inline constexpr float stroke20 = 2.0F;
        inline constexpr float stroke24 = 2.4F;
        inline constexpr float hoverBrightenFactor = 0.4F;  // knob ring hover brighten
        inline constexpr float tickBoxBorderAlpha = 0.6F;  // tick-box border vs accent
        inline constexpr int indicatorHoldMs = 100;  // MIDI LED retriggerable hold
        inline constexpr float knobTrackAlpha = 0.0196F;  // reference KnobControl.cs DEFAULT_KNOB_LED_BACKGROUND_COLOR ARGB(5,255,255,255), Standard style = 5/255; a near-invisible white wash over the panel, not a solid ring
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
        inline constexpr float strokeLine = global::stroke20;  // frames + signal lines
        inline constexpr float strokeKnobRing = global::stroke24;  // knob track + value arc
        inline constexpr float hoverBrighten = global::hoverBrightenFactor;
        inline constexpr int indicatorHoldMs = global::indicatorHoldMs;
    }

    // -----------------------------------------------------------------
    // TIER 3 — COMPONENT : usages/transforms aliasing semantics.
    // -----------------------------------------------------------------
    namespace component
    {
        inline constexpr float knobRingHoverBrighten = semantic::hoverBrighten;  // accent.brighter(f) on hover
        inline constexpr float tickBoxBorderAlpha = global::tickBoxBorderAlpha;  // accent.withAlpha(a) border
        inline constexpr float knobTrackAlpha = global::knobTrackAlpha;  // controlTrack.withAlpha(a)
    }

}
