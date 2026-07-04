# RQ-MID — Base MIDI Layer (JUCE)

Scope: a thin C++ MIDI abstraction built on `juce::MidiInput` / `juce::MidiOutput` / `juce::MidiMessage`, covering exactly the Sanford API surface used by the reference implementation. No sequencing, clocks or MIDI-file support is required.

## Device management

- **RQ-MID-001** — The MIDI layer shall enumerate available MIDI input devices and MIDI output devices, exposing for each a stable display name.
- **RQ-MID-002** — When asked to open an input or output device by display name, the MIDI layer shall open the matching device and report failure (not throw) when no device with that name exists.
- **RQ-MID-003** — When a device is re-assigned by name at runtime, the MIDI layer shall stop, close and release the previously opened device before opening the new one (hot-swap).
- **RQ-MID-004** — The MIDI layer shall support three simultaneously open devices: one synth input, one synth output, one automation input.
- **RQ-MID-005** — The MIDI layer shall provide start/stop of message reception on an opened input device without closing it.
- **RQ-MID-006** — When the application terminates, the MIDI layer shall close all opened devices and release their resources.

## Message sending

- **RQ-MID-010** — The MIDI layer shall send channel voice messages: Control Change, Program Change, Note On, Note Off, on a configurable channel (0–15).
- **RQ-MID-011** — The MIDI layer shall send System Exclusive messages of arbitrary length, including all-data-dump-sized payloads (> 64 KB), as single complete SysEx frames (0xF0 … 0xF7).
- **RQ-MID-012** — The MIDI layer shall send System Common messages, including Tune Request (0xF6).

## Message reception

- **RQ-MID-020** — When a channel message arrives on an opened input, the MIDI layer shall deliver it to a registered per-device callback with command, channel, data1 and data2 decoded.
- **RQ-MID-021** — When a complete SysEx message arrives on an opened input, the MIDI layer shall deliver the full byte array (0xF0 … 0xF7 inclusive) to a registered per-device callback, reassembling driver-fragmented SysEx transparently.
- **RQ-MID-022** — The MIDI layer shall deliver System Common and System Realtime messages to registered per-device callbacks.
- **RQ-MID-023** — When a device error occurs, the MIDI layer shall deliver an error notification to a registered callback instead of crashing or silently losing the device.
- **RQ-MID-024** — While messages are being received, the MIDI layer shall dispatch callbacks off the UI thread in a serialized order per device (no reordering, no concurrent callbacks for one device).

## SysEx utilities

- **RQ-MID-030** — The MIDI layer shall provide an iterator that splits a byte stream (file content) into successive complete SysEx messages, tolerating leading/interleaved non-SysEx bytes, with behavior identical to the reference `SysexIterator`.

## Constraints

- **RQ-MID-040** — The MIDI layer shall not expose JUCE types in its public interface used by upper layers, so the framework above stays MIDI-backend-agnostic and unit-testable with a fake/loopback backend.
- **RQ-MID-041** — The MIDI layer shall provide an in-memory loopback/mock backend implementing the same interface, for automated tests without hardware.
