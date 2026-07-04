# RQ-TST — Testing & Verification

- **RQ-TST-001** — Every non-UI layer (MID, FMW, MOD, CTL, SET) shall ship with unit tests written before or with the code (TDD red/green/refactor cycle), named/structured in Given/When/Then style, runnable headless via CTest.
- **RQ-TST-002** — Model serialization shall be verified by round-trip tests (RQ-MOD-022/040/042) using reference `.syx` fixtures checked into the repository, including at least one real single patch and one all-data dump.
- **RQ-TST-003** — Bit-exactness of generated SysEx (parameter edit, page select, mod edit, display control) shall be verified against expected byte vectors captured from the reference implementation.
- **RQ-TST-004** — Controller behavior (worker pacing, queue semantics, automation scaling, bidirectional handlers, dump state machine) shall be tested functionally against the mock MIDI backend (RQ-MID-041), without hardware.
- **RQ-TST-005** — When a JUCE-backend integration test is run on a machine with a virtual MIDI loopback, the suite shall verify end-to-end send/receive through real JUCE devices; this suite shall be optional (skipped when no loopback exists).
- **RQ-TST-006** — A manual hardware validation checklist shall be maintained for behaviors only verifiable on a real Xpander/Matrix-12 (dump timing, VFD rendering, panel edit follows), one item per RQ it validates.
- **RQ-TST-007** — Tests shall reference the requirement IDs they verify (in test names or tags), and the traceability matrix in the plan shall map RQ → tests.
- **RQ-TST-008** — Existing failing behavior shall never be made to pass by weakening a test; fixture or expected-value changes require an ADR note.
