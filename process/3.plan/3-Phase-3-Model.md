## Phase 3 — Model (`xpl_model`)

| Task | Deliverable | Tier | Requirements |
|---|---|---|---|
| TASK-JUC-040 | Constants & enums (`XpanderConstants` port, page/subpage tables) | M | RQ-MOD-001..003 |
| TASK-JUC-041 | Parameter types (base, signed, mod-matrix, full-tone) with byte-exact SysEx generation tests | L | RQ-MOD-010..013, RQ-TST-003 |
| TASK-JUC-042 | Nibble packetization reader/writer | M | RQ-MOD-040 |
| TASK-JUC-043 | `XpanderSinglePatch` binary mapping (399-byte layout) | M | RQ-MOD-022 (layout) |
| TASK-JUC-044 | `XpanderTone`: full 226-parameter map + program numbers + name handling | L | RQ-MOD-020, 021, 023 |
| TASK-JUC-045 | Tone ⇄ bytes round-trip (`FromByteArray`/`ToByteArray`) against fixtures | L | RQ-MOD-022, RQ-MOD-050, RQ-TST-002 |
| TASK-JUC-046 | Modulation matrix (entries, rules, edit ops, randomize) | L | RQ-MOD-030..033 |
| TASK-JUC-047 | Tone reader/writer + sysex file type detection + bank enumeration | L | RQ-MOD-041..043 |
