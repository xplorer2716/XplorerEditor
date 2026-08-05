# RQ-NFR — Non-Functional Requirements

- **RQ-NFR-001** — Real-time feel: a UI parameter change shall reach the synth within one transmit-delay tick plus queue latency, matching the reference (default 20 ms pacing), with no added buffering.
- **RQ-NFR-002** — The UI thread shall never block on MIDI I/O, file I/O of dumps, or worker synchronization (no UI freezes during any operation).
- **RQ-NFR-003** — File-format and wire-format compatibility (RQ-MOD-050, RQ-MOD-022) shall be preserved so users can mix the .NET and JUCE versions on the same patch library and hardware.
- **RQ-NFR-004** — The application shall remain a bidirectional editor: simultaneous edits from mouse, MIDI controller/DAW and synth panel shall converge without requiring a patch reload.
- **RQ-NFR-005** *(amended 2026-08-05, session ABT — architecture: ADR-ABT-002)* — Licensing: the port shall remain **AGPL v3**; all third-party dependencies shall be AGPL-compatible. *Amended 2026-08-05: was GPL v3 — JUCE 8's open-source option is AGPLv3, not GPLv3 (RQ-BLD-006, ADR-ABT-002); GPL-3.0/MIT/Apache-2.0/BSL-1.0 dependencies already in use remain compatible under AGPLv3 (all are one-way compatible with the GPL family, and GPLv3 itself permits combination with AGPLv3 code under its §13).* **Dependencies:** RQ-BLD-006; ADR-ABT-002.
- **RQ-NFR-006** — All documents, code comments, identifiers and commit messages shall be in English.
- **RQ-NFR-007** — Traceability: every code/test/doc artifact shall reference its requirement IDs; commits follow `TASK-JUCE-NNN: <summary> [RQ-...]`.
- **RQ-NFR-008** — The port shall not regress the reference's robustness features: single-instance guard, top-level exception reporting, diagnostic logging with configurable verbosity.
- **RQ-NFR-009** — Where the reference has known defects flagged for improvement (DoEvents pumping, busy-sleep polling), the port shall implement the modern equivalent while preserving observable behavior; each such deviation shall be recorded in an ADR.
