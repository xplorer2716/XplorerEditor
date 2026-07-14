# ADR-JUC-008: MIDI LED Panel — Per-Device LEDs, Event-Driven Hold Instead of a UI Poll Timer

## Status
Accepted (owner, 2026-07-13)

## Requirements
RQ-GUI-022, RQ-CTL-027

## Context
The reference shows MIDI traffic on a `LedPanelControl` (MidiApp.UIControls)
below the VFD: **three** 5 px square LEDs in a 32×8 transparent panel
(extracted bounds 1190,150), evenly spaced (`hSpace = (w − n·size)/(n+1)` →
x = 4/13/22, y = 1), 1 px border RGB(44,44,52), off fill RGB(54,54,62)
(designer values); the on colours are set at startup in
`MainForm.Overrides.cs`: **automation/controller input = green
(144,255,144), synth input = blue (92,171,255), synth output = red
(255,64,32)**.

Timing: the controller's `MidiDataSendReceive` event (any thread) maps its
device to a LED index and stamps `remaining[i] = 100 ms`; a **30 ms WinForms
UI timer** (shared with the VFD refresh) decrements the stamps and switches
each LED on while its remaining time is positive — so a LED stays lit
100–130 ms after the last event of its source and continuous traffic keeps
it lit.

The current JUCE port has a single orange LED that flashes on *any* device's
event (the `EnumMidiDevice` payload — already correctly emitted by the
controller for all three sources — is ignored by the view). Gap is
view-only.

## Decision
1. **`LedPanelComponent`** (app) replaces the inner `MidiActivityLed` class:
   three LEDs at the reference geometry/colours above, painted as filled
   squares + border; non-opaque component so the background artwork shows
   through (reference transparent background). Colours stay hard-coded as in
   the reference (they are code-side there too, not designer/user settings).
2. **Device → LED mapping** in the existing `setMidiActivityHandler` wiring:
   `AutomationInputDevice` → 0, `SynthInputDevice` → 1,
   `SynthOutputDevice` → 2 (port of `OnMidiDataSendReceive`).
3. **Event-driven retriggerable hold, no polling timer.** Each event stamps
   the LED's expiry (`now + 100 ms`) and lights it; a single `juce::Timer`
   runs at 30 ms **only while at least one LED is lit**, switching off the
   expired ones and stopping itself when all are dark. Observable behaviour
   is the reference's (lit 100–130 ms past the last event, held under
   sustained traffic), but the timer no longer ticks for the application's
   whole lifetime — the reference's always-on 30 ms tick also drove the VFD,
   whose JUCE port is already event-driven (ADR-JUC-006/007), so nothing else
   needs the poll.
4. Events arrive marshalled to the message thread by `JuceEventDispatcher`
   (RQ-GUI-006), so no cross-thread stamp race as in the reference (noted
   there as tolerated).

## Consequences
- Three-colour traffic indication identical to the reference; the green LED
  gives back automation-input visibility, the blue/red pair separates synth
  in/out (today's single LED conflates all three).
- No permanent 30 ms UI timer; idle application does zero periodic work.
- The LED geometry derives from the already-extracted control-table bounds;
  no new asset.
