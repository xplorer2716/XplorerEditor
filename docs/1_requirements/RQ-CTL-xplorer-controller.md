# RQ-CTL — Xplorer Controller (Features & Bidirectional Sync)

Scope: C++ port of `Xplorer/Controller` (XpanderController and helpers). Extends the framework controller (RQ-FMW).

## Patch operations

- **RQ-CTL-001** — When loading a `.syx` file, the controller shall detect its type (RQ-MOD-043); a single patch shall replace the edited tone and be transmitted to the synth; an all-data dump shall be rejected with a user-reportable error suggesting extraction.
- **RQ-CTL-002** — When saving, the controller shall write the edited tone via the tone writer, defaulting the tone name to the file name when empty.
- **RQ-CTL-003** — The controller shall extract all single patches from an all-data-dump file into a chosen directory, one uniquely-named `.syx` per patch.
- **RQ-CTL-004** — The controller shall request and save all single patches from the synth into a chosen directory (iterating program numbers with per-patch dump requests).
- **RQ-CTL-005** — The controller shall back up the synth's full memory (all-data dump request) to a `.syx` file, and restore such a file to the synth with per-message pacing and a progression callback, without freezing the caller.
- **RQ-CTL-006** — The controller shall store the edited tone into a given synth program slot, and switch the synth to a given program (program change + single-patch request) keeping PC and synth synchronized.
- **RQ-CTL-007** — The controller shall support program increment/decrement wrapping within 0–99, updating the synth accordingly.
- **RQ-CTL-008** — The controller shall send the edited tone as a programmer-mode single patch (full-tone transmission without storing).

## Real-time editing & sync

- **RQ-CTL-020** — While the edited page/sub-page differs from the synth's current one, the controller shall send a page-select SysEx before a parameter-edit SysEx, as the reference worker does.
- **RQ-CTL-021** — When the synth sends a single-patch program dump, the controller shall stop the worker, reload the full tone from the dump, notify a full-tone-change event, and restart the worker.
- **RQ-CTL-022** — When the synth sends a panel edit (page-edit-follows SysEx, button or rotary), the controller shall update the matching parameter for the page/sub-page/ID and raise a parameter-change notification.
- **RQ-CTL-023** — When the synth sends a page/sub-page select SysEx, the controller shall raise a page-change event.
- **RQ-CTL-024** — When the synth sends program-up/program-down SysEx, the controller shall update the current program number accordingly.
- **RQ-CTL-025** — When the synth sends modulation-edit SysEx, the controller shall apply the command (change source, delete source, dial amount, set quantize, set sign, set unsigned value, toggle quantize) to the local matrix and raise a modulation-entry-change event.
- **RQ-CTL-026** — When a multi-patch dump is detected, the controller shall ignore it without corrupting the edited tone (multi mode is out of scope, tracked as future work).
- **RQ-CTL-027** — The controller shall raise a MIDI-activity event on message send/receive (drives the UI LED indicators).

## Modulation matrix operations

- **RQ-CTL-030** — The controller shall expose change-source, change-destination, change-amount and change-quantize operations for a matrix entry, delegating to the tone rules (RQ-MOD-031/032) and transmitting the resulting SysEx.

## Clipboard

- **RQ-CTL-040** — The controller shall copy all parameters of one page instance (page families TRACK, ENV, LFO, RAMP) and paste them onto another instance of the same family, transmitting updated parameters; paste eligibility shall be queryable.

## Randomizer & morphing

- **RQ-CTL-050** — The controller shall randomize the edited tone per RQ-FMW-011/RQ-MOD-033 options (excluded parameters, humanize, VCO tuning strategy, forced VCA envelope), rename it "RANDOM", and retransmit the full tone.
- **RQ-CTL-051** — The controller shall morph two tone files with a factor and make the result the edited tone, transmitted to the synth (RQ-FMW-012).

## Synth utilities

- **RQ-CTL-060** — The controller shall send: Tune Request; All Notes Off — "smart" variant per the reference when enabled in settings; and a Program Change on the tone's channel.
- **RQ-CTL-061** — The controller shall display text on the synth VFD (greetings at startup, typewriter mode), using display-control command 0x05 for Xpander and 0x06 for Matrix-12 per the synth-type setting.
- **RQ-CTL-062** — The controller shall support both Xpander and Matrix-12 protocol variants selected by configuration everywhere they differ.

## Events (to UI)

- **RQ-CTL-070** — The controller shall raise: full-tone-change, page-change, modulation-entry-change, all-data-dump progression, MIDI-activity, and parameter-change events, each carrying the same payload semantics as the reference event args.
