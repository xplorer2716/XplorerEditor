## Phase 1 — MIDI layer (`xpl_midi`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-020 | `MidiMessage` value type + backend interfaces (`MidiBackend`, `MidiInputPort`, `MidiOutputPort`) | M | RQ-MID-040, RQ-MID-010..012 (types) |
| TASK-JUC-021 | `MockMidiBackend`: scriptable input, captured output, loopback wiring | M | RQ-MID-041 |
| TASK-JUC-022 | `SysexStreamIterator` (byte-stream splitter, reference-identical) | M | RQ-MID-030 |
| TASK-JUC-023 | `JuceMidiBackend` adapters (enumeration, open-by-name, hot-swap, send/receive incl. large SysEx, error callback) | L | RQ-MID-001..006, 010..012, 020..024 |
| TASK-JUC-024 | Optional loopback integration test (skipped without virtual MIDI cable) | M | RQ-TST-005 |
