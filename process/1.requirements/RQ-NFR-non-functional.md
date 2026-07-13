# RQ-NFR — Non-Functional Requirements

- **RQ-NFR-001** — Real-time feel: a UI parameter change shall reach the synth within one transmit-delay tick plus queue latency, matching the reference (default 20 ms pacing), with no added buffering.
- **RQ-NFR-002** — The UI thread shall never block on MIDI I/O, file I/O of dumps, or worker synchronization (no UI freezes during any operation).
- **RQ-NFR-003** — File-format and wire-format compatibility (RQ-MOD-050, RQ-MOD-022) shall be preserved so users can mix the .NET and JUCE versions on the same patch library and hardware.
- **RQ-NFR-004** — The application shall remain a bidirectional editor: simultaneous edits from mouse, MIDI controller/DAW and synth panel shall converge without requiring a patch reload.
- **RQ-NFR-005** — Licensing: the port shall remain GPL v3; all third-party dependencies shall be GPL-compatible.
- **RQ-NFR-006** — All documents, code comments, identifiers and commit messages shall be in English.
- **RQ-NFR-007** — Traceability: every code/test/doc artifact shall reference its requirement IDs; commits follow `TASK-JUCE-NNN: <summary> [RQ-...]`.
- **RQ-NFR-008** — The port shall not regress the reference's robustness features: single-instance guard, top-level exception reporting, diagnostic logging with configurable verbosity.
- **RQ-NFR-009** — Where the reference has known defects flagged for improvement (DoEvents pumping, busy-sleep polling), the port shall implement the modern equivalent while preserving observable behavior; each such deviation shall be recorded in an ADR.
