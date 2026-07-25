## Phase 0 — Bootstrap

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-010 | `juce/` CMake skeleton: FetchContent JUCE 8 + Catch2, layer targets, CTest wiring, warning-clean empty libs, GPL headers template | L | RQ-BLD-001..006 |
| TASK-JUC-011 | CI workflow: Linux configure/build/ctest on push | M | RQ-BLD-007 |
| TASK-JUC-012 | Test fixtures: reference `.syx` files (1 single patch min., 1 all-data dump) + expected byte vectors extracted from the C# reference | M | RQ-TST-002, RQ-TST-003 |
