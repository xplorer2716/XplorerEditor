# RQ-FMW — MidiApp Framework Port (C++)

Scope: C++ port of `MidiApp.MidiController` Model/Controller abstractions. UI-side framework classes (`AbstractControllerMainForm`, UIControls) are covered by RQ-GUI.

## Parameter model

- **RQ-FMW-001** — The framework shall provide an abstract parameter with: unique name, optional label, min value, max value, step (≠ 0), current value, changed flag, and an associated SysEx message.
- **RQ-FMW-002** — When a parameter value is set, the parameter shall quantize it to the step (integer division), clamp it to [min, max], and only when the resulting value differs from the current one: store it, set the changed flag, and regenerate its SysEx message.
- **RQ-FMW-003** — The parameter shall be cloneable (deep copy including its SysEx message) and thread-safe for concurrent get/set.

## Tone model

- **RQ-FMW-010** — The framework shall provide an abstract tone owning an insertion-ordered parameter map keyed by parameter name, a tone name, and a MIDI channel validated to 0–15.
- **RQ-FMW-011** — When randomization is requested with an exclusion set and an optional humanize ratio, the tone shall randomize every non-excluded parameter within its bounds; with humanize, the new value shall stay within ±ratio of the current value (clamped); two-state parameters shall pick min or max with equal probability.
- **RQ-FMW-012** — When morphing two tones of the same type with factor f ∈ [0,1], the framework shall set each eligible parameter of the result tone to (1−f)·A + f·B (integer-truncated), using the tone-defined eligible-parameter set.

## Tone I/O

- **RQ-FMW-020** — The framework shall define reader/writer interfaces: read one tone from file into an existing tone, read all tones of a bank file as (name, tone) pairs, and write one tone to file.

## Controller core

- **RQ-FMW-030** — The controller shall own the edited tone (created via a factory method implemented by subclasses) and expose Start/Stop lifecycle controlling input reception and the transmit worker.
- **RQ-FMW-031** — When `SetParameter(name, value)` is called with a known parameter name and parameter setting is enabled, the controller shall update that parameter; unknown names shall be rejected without side effect.
- **RQ-FMW-032** — The controller shall expose an automation table as a bidirectional map between parameter names and CC numbers, where one CC number may map to several parameter names; lookups shall be thread-safe.
- **RQ-FMW-033** — The controller shall allow temporarily disabling one CC number (the one being edited from the UI) so automation input for it is ignored.
- **RQ-FMW-034** — The controller shall expose a configurable parameter transmit delay in ms (≥ 0, reference default 20 ms).

## Transmit worker

- **RQ-FMW-040** — While running, the transmit worker shall, every transmit-delay tick: scan the parameter map, enqueue a clone of each parameter whose changed flag is set (then clear the flag), dequeue at most one queued parameter, and send its SysEx message to the synth output.
- **RQ-FMW-041** — When the controller stops, the worker shall terminate cooperatively within 2 s and the queue shall be cleared; starting again shall begin with an empty queue.
- **RQ-FMW-042** — The queue shall preserve FIFO order and be safe against concurrent enqueue/dequeue.

## Automation input behavior

- **RQ-FMW-050** — When a CC message arrives on the automation input and its CC number is mapped (and not disabled), the controller shall, for each mapped parameter, scale the 0–127 CC value to the parameter range as in the reference (ratio × (max+|min|) − |min|; for two-state parameters values > 63 map to max), set the parameter, and raise a parameter-change notification.
- **RQ-FMW-051** — When a CC message arrives whose number is not mapped, or any non-CC channel message arrives on the automation input, the controller shall forward it to the synth output re-stamped on the tone's MIDI channel.
- **RQ-FMW-052** — When SysEx, System Common or System Realtime messages arrive on the automation input, the controller shall forward them unchanged to the synth output.

## Events

- **RQ-FMW-060** — The controller shall raise a parameter-change event (name, value) consumable by the UI layer, for automation-driven and programmatic changes.
- **RQ-FMW-061** — Controller event callbacks shall be deliverable to the UI thread via an injectable dispatcher, so the core stays UI-framework-agnostic.

## Services

- **RQ-FMW-070** — The framework shall provide diagnostic logging with severity levels, configurable at runtime, writing to a per-user application log file (reference: `applog.txt`).
- **RQ-FMW-071** — The framework shall provide a bug-report facility that captures exception context including MIDI device state, equivalent in content to the reference `BugReportFactory`.
- **RQ-FMW-072** — The framework shall enforce single-instance execution per user session (reference: `FileMutex`).
