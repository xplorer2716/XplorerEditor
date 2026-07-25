## Traceability matrix

Generated mechanically: Catch2 tags `[RQ-…]` ↔ requirement files; per-task RQ column above; commit messages carry task + RQ IDs. A snapshot table will be added to this file at each phase end (phase, tasks done, RQ covered by passing tests, RQ remaining).

## Sequencing rules

1. A phase starts only when the previous phase's tests are green in CI (Linux) — except Phase 5+ (GUI), validated on Windows by the owner.
2. Behavior questions are always answered by reading the C# reference; ambiguities that the reference cannot settle are logged and asked to the owner, never guessed.
3. Deviations from reference behavior (allowed only for RQ-NFR-009 defects) each get an ADR.