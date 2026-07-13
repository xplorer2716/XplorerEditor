# Mission Statement — Xplorer Migration to JUCE

> **Status**: Draft — awaiting owner validation before any work starts
> **Branch**: `claude/xplorer-editor-juce-wl25q7` (the "JUCE" working branch, based on `improvements`)
> **Language policy**: all documents, code comments and commit messages in **English**

## 1. What I understood

### 1.1 Goal

Migrate **Xplorer** — a real-time, bi-directional patch editor for the Oberheim Xpander / Matrix-12 (.NET 10, WinForms, 3 repos / 5 projects) — to a **C++ / JUCE** implementation (https://docs.juce.com/master/index.html), following the 3-part roadmap already stated in `README.md`:

1. Replace the Sanford C# MIDI toolkit with JUCE MIDI (ports config, send/receive), limited to what Xplorer actually uses.
2. Port the **MidiApp** framework (Model + Controller abstractions) to C++.
3. Re-implement the **UI** in JUCE (largest effort: ~226 parameters, custom controls — knobs, VFD display, LED panels — transparency, in-house controller binding).

Reference architecture is documented in `docs/architecture-analysis.md` (3-layer MVC, parameter queue + worker thread, 3 simultaneous MIDI devices, settings service, tone model & SysEx I/O).

### 1.2 Mandated work sequence

1. **Define migration requirements** for the JUCE implementation.
2. **Establish a step-by-step implementation plan** derived from those requirements.
3. **Implement bottom-up**: base MIDI functions → MidiApp framework → Xplorer Model → Controller → View.
4. **Unit + functional tests** for every non-UI deliverable (test-first where the TDD agents apply).
5. **Full traceability** on every action (requirements → ADRs → code → tests → commits).

### 1.3 Process to follow

The project process is `spashx/flutins/.github/copilot-instructions.md`:

- **Phases**: Session kickoff → Planning → Implementation → Error recovery (one targeted fix attempt, then stop & report) → Delivery checklist.
- **Requirements**: EARS format (ubiquitous / event-driven / state-driven), IDs `RQ-<FTR>-<NNN>` (e.g. `RQ-MID-001`), stored under `1_requirements/`.
- **ADRs**: `ADR-NNN-<short-title>.md` under `2_architecture/decisions/`, template *Status / Context / Decision / Consequences / Diagram (Mermaid mandatory)*, must reference the motivating requirement IDs.
- **Task tiers**: S (≤5 lines) / M (new class, tests required) / L (cross-cutting: tests + ADR + full delivery checklist). Most migration steps will be **L**.
- **Delivery checklist (L)**: requirement IDs in all artifacts, Gherkin-style tests passing, ADRs with diagrams, no duplicated literals, no modified-to-pass tests, clean build + static analysis.
- **Traceability**: every artifact carries the requirement ID it fulfils. I will continue the existing commit convention of this repo (`TASK-UI-00x` on `improvements`) as **`TASK-JUCE-NNN: <summary> [RQ-...]`**.

Process folders will live inside the repo as `docs/1_requirements/` and `docs/2_architecture/decisions/` (adaptation: the process mandates the folder names, the repo keeps them under `docs/`).

### 1.4 Local competency assets (`.github/`)

To be loaded per task as relevant:

- **Agents**: `repo-architect`, `se-system-architecture-reviewer`, `tdd-red` / `tdd-green` / `tdd-refactor`, `expert-dotnet-software-engineer`, `csharp-dotnet-janitor`, `se-security-reviewer`, `swe-subagent`.
- **Prompts**: `create-specification`, `create-architectural-decision-record`, `breakdown-feature-implementation`, `dotnet-best-practices`, `dotnet-design-pattern-review`, `review-and-refactor`, `refactor-method-complexity-reduce`, `csharp-*` (async, docs, xunit).
- **Instructions**: `csharp.instructions.md`, `dotnet-architecture-good-practices.instructions.md`.

Note: these assets are .NET/C#-oriented; the *principles* (TDD cycle, spec/ADR templates, architecture review checklists) transfer to the C++/JUCE target and will be applied there; the C#-specific rules keep applying to any residual C# work on the existing code.

### 1.5 Token-economy directives

- **rtk** (https://github.com/rtk-ai/rtk): CLI proxy to compress tool-call output (git/build/test noise). Not pre-installed; `cargo` is available — I will install it and route repetitive shell calls through it when it pays off.
- **caveman-code** (https://github.com/JuliusBrussee/caveman-code): terse reporting style — short factual statements when exposing work, no ceremony.
- **repomix** (https://github.com/yamadashy/repomix): pack only the source subset relevant to the current feature (available via `npx repomix`) instead of reading files one by one when scoping a layer.

### 1.6 Environment & constraints I identified

- Work happens in a **Linux container**: g++ 13, cmake 3.28 available. JUCE will be pulled via CMake (`FetchContent` or submodule). Non-UI layers (MIDI wrappers, MidiApp core, Model, Controller) build and **test headless on Linux**; that matches the "tests for non-UI parts" requirement.
- **Primary target remains Windows** (the user base), JUCE giving macOS/Linux for free; final hardware validation (real Xpander/Matrix-12, MIDI interfaces) can only be done by the owner — the plan must define which functional tests are automatable (e.g. SysEx encode/decode round-trips, virtual/loopback MIDI) vs. manual hardware test procedures.
- The C# codebase is the **behavioral reference**: SysEx formats, parameter maps (226 params), page/subpage logic, mod-matrix rules must be ported bit-exact; existing `.syx` files and the Oberheim MIDI spec (https://github.com/xplorer2716/OberheimXpanderMidiSpec) are the ground truth for functional tests.

## 2. Planned first deliverables (upon your GO)

| # | Deliverable | Location |
|---|---|---|
| 1 | Migration requirements (EARS, `RQ-*`) covering: MIDI layer, MidiApp port, Model, Controller, View, tooling/build, testing, non-regression | `docs/1_requirements/` |
| 2 | Founding ADRs (JUCE version & integration mode, build system, project layout, test framework, threading model) | `docs/2_architecture/decisions/` |
| 3 | Step-by-step implementation plan (bottom-up, task IDs, tier, RQ traceability matrix) | `docs/3_plan/` (proposed) |
| 4 | Then implementation, task by task, test-first for non-UI code | new C++ tree (layout fixed by ADR) |

## 3. Open points for you

1. Confirm `docs/1_requirements/`, `docs/2_architecture/decisions/`, `docs/3_plan/` as process folders.
2. Confirm JUCE integration via CMake + JUCE 8.x (latest stable) as the default assumption.
3. Confirm the C++ code lives in this repo (e.g. under a new top-level folder) rather than in new repos, at least during migration.

---
*Awaiting your feedback before starting step 1 (requirements definition).*
