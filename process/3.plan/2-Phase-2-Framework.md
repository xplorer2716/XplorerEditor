## Phase 2 — Framework (`xpl_framework`, port of MidiApp.MidiController)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-030 | `AbstractParameter` (quantize/clamp/changed/clone, thread-safe) | M | RQ-FMW-001..003 |
| TASK-JUC-031 | `AbstractTone` (ordered parameter map, channel, randomize, morph) | M | RQ-FMW-010..012 |
| TASK-JUC-032 | `DualDictionary` (name⇄CC), `ToneReader/Writer` interfaces | M | RQ-FMW-032, RQ-FMW-020 |
| TASK-JUC-033 | `AbstractController` core: tone ownership, Start/Stop, SetParameter, devices by name via backend, transmit delay | L | RQ-FMW-030..034 |
| TASK-JUC-034 | Transmit worker (jthread + cv per ADR-JUC-005) with mock-backend pacing tests | L | RQ-FMW-040..042, RQ-NFR-001 |
| TASK-JUC-035 | Automation input behavior (CC scaling, disable-one-CC, forwarding) + events via `EventDispatcher` | L | RQ-FMW-050..052, 060..061, RQ-FMW-033 |
| TASK-JUC-036 | Services: logger, bug-report payload, single-instance guard | M | RQ-FMW-070..072, RQ-NFR-008 |
