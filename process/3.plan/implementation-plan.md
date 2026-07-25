# Implementation Plan — Xplorer JUCE Migration

> Bottom-up execution: MIDI base → MidiApp framework → Model → Controller → View.
> Every task = one commit series TASK-JUC-NNN: <summary> [RQ-…], tier per the project process (M = tests required, L = tests + ADR + full delivery checklist).
> Non-UI tasks are TDD: failing Catch2 scenario first (tags = RQ IDs), then implementation, then refactor.

## Plan Index

### Phases

- [Phase 0 — Bootstrap](0-Phase-0-Bootstrap.md)
- [Phase 1 — MIDI layer (xpl_midi)](1-Phase-1-MIDI-layer.md)
- [Phase 2 — Framework (xpl_framework)](2-Phase-2-Framework.md)
- [Phase 3 — Model (xpl_model)](3-Phase-3-Model.md)
- [Phase 4 — Controller (xpl_controller + xpl_settings)](4-Phase-4-Controller.md)
- [Phase 5 — View (JUCE app)](5-Phase-5-View.md)
- [Phase 6 — Integration & cut-over](6-Phase-6-Integration.md)

### Cross-cutting

- [Design System (RQ-DSN, ADR-JUC-014 / ADR-JUC-015)](7-Phase-7-Design%20System.md)
- [Traceability matrix & Sequencing rules](8-Phase-8-Traceability%20and%20sequencing%20rules.md)
