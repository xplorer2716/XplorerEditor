---
applyTo: "**"
---

<!-- AGNOS-PROCESS-INSTANCE: 2026-07-22T23:04:50Z -->

# AGNOS SOFTWARE ENGINEERING PROCESS V2 - INSTRUCTIONS

These instructions describe a lightweight software engineering process for agentic AI development. They are designed to ensure high traceability, maintainability and quality code while enabling rapid iteration and delivery of features.

## MAIN PROCESS ACTIVITIES
The process is divided into 4 main activities:
1. **Requirements authoring**: defining the features and requirements of the project in a clear, concise, and testable way. see [REQUIREMENTS AUTHORING](#1-requirements-authoring) section for details.
2. **Architecture decisions records (ADRs)**: documenting the architectural choices and trade-offs made during the project, with clear traceability to the requirements they fulfill. see [ARCHITECTURE DECISIONS RECORDS (ADRs)](#2-architecture-decisions-records-adrs) section for details.
3. **Plan - Agentic Planning and Execution**: planning the implementation of the requirements and ADRs into tasks, and executing them with a clear definition of ready and done, and a strong emphasis on verification and error recovery. see [PLAN - Agentic Planning and Execution](#3-plan---planning-and-execution) section for details.
4. **Code implementation and quality**: implementing the code with high quality standards, including SOLID principles, testing, and anti-patterns to avoid. see [CODE IMPLEMENTATION AND QUALITY](#4-code-implementation-and-quality) section for details.

The process is iteratively based on sessions execution. see [START NEW SESSION ACTION](#start-session-action) and [END SESSION ACTIONS](#end-session-action) sections.

## PROCESS FOLDER STRUCTURE
The whole process artifacts are managed into the `process/` folder of the workspace:
- `process/1.requirements/` – Feature and Requirements files
- `process/2.architecture/` – Architecture Decision Records (ADRs) files
- `process/3.plan/` – Plan files with PLAN-* and TASK-* definitions
- `process/_sessionstate/` – Session state YAML file (`session.yaml`), version-controlled
 
## START SESSION ACTION

At the start of every session, run these steps in order before any task:

1. Read this file fully if it has not been loaded in context yet.
2. Read any domain- or stack-specific instruction files present in the current project.
3. **Resolve session state variables** :
   - a. **Detect platform** (system variable): run a terminal command to identify the OS
     (`$PSVersionTable` on Windows, `uname -s` on macOS/Linux). Set `platform` to `windows`,
     `macos`, or `linux`.
   - b. **Ask user for `unit_tests`** (user variable): use the built-in user-question tool
     (`askQuestion` on GitHub Copilot, `AskUserQuestion` on Claude Code) — "Should unit tests be
     generated and run in this session? (yes / no)". Set `unit_tests` to `true` or `false`.
   - c. **Write** `process/_sessionstate/session.yaml` with the resolved values.
   - d. **Display a summary table** of all variable names and values. Allow the user to override
     any value. If any value is overridden, rewrite `session.yaml` before proceeding.
   - e. **The values in `session.yaml` are the single source of truth for the entire session.**
     Before starting any Tier M or L task, if session variable values are not in active context,
     read `process/_sessionstate/session.yaml` and reload them before applying any conditional guard.
4. Scan `process/1.requirements/` for open or unimplemented requirement IDs.
5. Scan `process/2.architecture/` for existing ADRs to avoid creating duplicates.
6. Scan `process/3.plan/` for existing plans and tasks to avoid creating duplicates.
7. MANDATORY: Report ANY semantic CONFLICTS coming from your instructions files. If you find any, STOP and ask the user to clarify which rule to follow. Do NOT proceed until the conflict is resolved.
8. Create with GIT a new branch for the session by invoking the `agnos-git-workflow` skill's `start-session <TRI>` sub-command, where `<TRI>` is the trigram the user HAS to provide. This branch shall be used for ALL changes in the session, including edits to existing files and creation of new files. If the branch already exists, the skill will ask the user how to proceed. **NEVER work on or commit to the default/protected branch (`main` / `master` / the repository default): every change SHALL be made on the `feature/<TRI>` session branch.**

## END SESSION ACTION

- When the user indicates the end of a session or accept all the pending changes you made, run these steps in order:
1) Generate IN THE CHAT a CONCISE summary report of the session, including:
  - The requirement IDs addressed and their fulfillment status.
  - The ADRs created or updated, with their IDs and short titles.
  - The tasks completed, with their IDs, descriptions, and tiers.
  - Any blockers encountered and how they were resolved or escalated.


## MANDATORY RULES FOR ALL ARTIFACTS

### MANDATORY UNIQUE IDENTIFIERS
EVERY ARTIFACT SHALL have a unique identifier. Artifacts requiring IDs: features, requirements, ADRs and decisions, plans & tasks.
Features, requirements, ADRs and tasks SHALL be in the format: `<TYPE>-<TRI>-<NNN>` where:
  - `<TYPE>` is the type of the artifact (e.g. RQ for requirement, ADR for architecture decision, TASK for task, etc).
  - `<TRI>` is the feature or functional area trigram (3 letters uppercase) that the artifact belongs to (e.g. USR for user management, STM for state management, etc).
  - `<NNN>` is a zero-padded sequence number (e.g. 001, 002, etc) that is incremented for each new artifact of the same type and feature.

  
### MANDATORY TRACEABILITY
- EACH ARTIFACT SHALL reference all related artifacts by their unique identifier.
- Requirement IDs SHALL appear in ALL related artefacts: feature files, ADR files, Plan files, task descriptions, file headers, class/function doc comments, and test case names.
- The user SHALL be able to grep any requirement ID and find all related code and documentation.
- An artifact with no traceable requirement ID is considered incomplete and SHALL NOT be delivered.
- Always put traceability references into **commments** format , never as plain text.

### MANDATORY DESIGN SYSTEM (UI PROJECTS)
- Any project with a user interface SHALL define a design system BEFORE implementing the first
  UI requirement: a single machine-readable source of truth for all visual decisions (colors,
  spacing, typography, strokes, component metrics), consumable by code as tokens.
- The design system SHALL be introduced by its own requirement(s) and its own ADR, using the
  standard ID schema, so it is referenceable like any other artifact.
- End-to-end traceability: UI-related requirements, ADRs, plan tasks and code SHALL reference
  the design-system artifact IDs they depend on. A UI artifact that bypasses the design system
  is considered incomplete and SHALL NOT be delivered (same rule as missing requirement IDs).
- Deviations from a reference or mockup value SHALL be recorded in the design-system source of
  truth with a rationale note — never as an inline literal in code.

## CONTEXT MANAGEMENT

Long sessions risk filling the AI's context window, causing lost context and degraded output quality. Follow these rules:

1. **Checkpoint on context pressure.** If the session has produced more than 10 tasks or the context feels saturated, generate a checkpoint summary in `process/3.plan/` capturing: completed task IDs, open task IDs, current blockers, and key decisions made. Then suggest starting a new session.
2. **Summarize before continuing.** When resuming after a checkpoint, read the latest checkpoint summary before proceeding.
3. **Limit task batch size.** Do not plan more than 10 tasks in a single session. If a plan exceeds this, split it across sessions with explicit handoff notes.
4. **Prune intermediate context.** After a task is marked complete and verified, avoid re-reading its implementation details unless debugging a regression.

## SESSION STATE VARIABLES
Session state variables are named key/value pairs persisted in `process/_sessionstate/session.yaml`
and loaded at session start. They conditionally control process execution for the duration of the
session. The file is tracked in git .

### Schema (`process/_sessionstate/session.yaml`)
```yaml
unit_tests: true        # boolean — user variable: generate and run unit tests 
platform: windows       # string — system variable: "windows" | "macos" | "linux" 
```

### Variable Kinds
| Variable | Kind | Who resolves | Values |
|----------|------|-------------|--------|
| `unit_tests` | User | Agent asks via the user-question tool (`askQuestion` / `AskUserQuestion`) at every session start | `true` / `false` |
| `platform` | System | Agent auto-detects via terminal probe at session start | `windows` / `macos` / `linux` |

### Adding New Variables
Add a flat key/value entry to the schema above. Never rename or remove existing keys — add only. Document the kind (user/system), who resolves it, and its allowed values.

# PROCESS ACTIVITIES
The following chapters describe the 4 main activities of the process in detail, with specific rules and guidelines for each one.

## 1. REQUIREMENTS AUTHORING
**Format:** Write all requirements in **EARS format** (Easy Approach to Requirements Syntax): 
**Ubiquitous**: the <system name> SHALL <system response>
**State driven requirements**: WHILE <precondition(s)>, the <system name> SHALL <system response>
**Event driven requirements**: WHEN <trigger>, the <system name> SHALL <system response>
**Unwanted behavior requirements**: IF <trigger>, THEN the <system name> SHALL <system response>

Requirements SHALL be clear, testable, and meet SMART criteria (Specific, Measurable, Achievable, Relevant, Time-bound).

**Acceptance Criteria Format (Gherkin):** Use in all templates — Given/When/Then structure.

### Requirement File Template

Every feature file in `process/1.requirements/` SHALL follow this structure:

````markdown
# FTR-<TRI>-<NNN>: <Feature Title>

## Overview
Brief description of the feature's purpose and scope.

## Stakeholders
- **Owner**: [Name / Role]
- **Consumers**: [Systems or actors that depend on this feature]

---

## Functional Requirements


### RQ-<TRI>-<NNN>: <Short Title>
- **Category**: Functional
- **EARS Type**: [Ubiquitous | Event-driven | State-driven | Unwanted-behavior | Complex]
- **Statement**: [EARS-formatted requirement]
- **Rationale**: [Why this requirement exists — one sentence]
- **Priority**: [Must | Should | Could — MoSCoW]
- **Acceptance Criteria** (Gherkin; see [§1 format](#1-requirements-authoring)): Given/When/Then
- **Dependencies**: [List of RQ/ADR IDs this depends on, or "None"]

---

## Non-Functional Requirements

### RQ-<TRI>-<NNN>: <Short Title>
- **Category**: Non-Functional
- **NFR Type**: [Performance | Security | Reliability | Scalability | Usability | Accessibility | Maintainability | Portability | Compliance]
- **EARS Type**: [Ubiquitous | State-driven | Unwanted-behavior | Complex]
- **Statement**: [EARS-formatted requirement with measurable threshold]
- **Metric**: [Quantifiable measure — e.g. "response time < 200ms at P95"]
- **Measurement Method**: [How to verify — e.g. "Load test with k6, 1000 concurrent users"]
- **Priority**: [Must | Should | Could — MoSCoW]
- **Acceptance Criteria** (Gherkin; see [§1 format](#1-requirements-authoring))
- **Dependencies**: [List of RQ/ADR IDs this depends on, or "None"]
````

### Traceability in requirements
- The features and requirements filename, title, and body SHALL reference the requirement ID(s)
- GUI, UX and user-workflow requirements SHALL express visual values (colors, spacing, sizes,
  typography) as design-system token references, not raw literals, and SHALL list the
  design-system RQ/ADR IDs in their **Dependencies** field.
- Store features and requirements in `process/1.requirements/`.


## 2. ARCHITECTURE DECISIONS RECORDS (ADRs)

- Create an ADR for EVERY cross-cutting architectural choice related to a requirement (state management, DI framework, navigation, storage, platform channel strategy, etc.).
- Every ADR SHALL use the following structure exactly:
  ```
  # ADR-<TRI>-<NNN>: <Short Title>
  ## Status
  [Proposed | Accepted | Deprecated | Superseded by <ADR-<TRI>-<NNN>>]
  ## Context
  What problem or force requires a decision?
  ## Decision
  What was decided, and why?
  ## Consequences
  What becomes easier, harder, or constrained as a result?
  ## Alternatives Considered
  What other options were evaluated? Why were they rejected?
  ## Diagram
  [Mermaid diagram -- required]
  ```
- Every decision in ADR SHALL have a unique identifier in the format `DEC-<TRI>-<NNN>` (e.g., `DEC-USR-001`), consistent with the universal ID schema, that can be referenced in plans, tasks, and source code.
- The design system itself SHALL be captured as an ADR (token structure, tiers,
  generation/verification mechanism). Every subsequent UI-affecting ADR SHALL reference it.


### Traceability in ADRs
- The ADRs body SHALL reference the requirement ID(s) that motivated the decision.
- Store ADRs in `process/2.architecture/`.



## 3. PLAN - PLANNING AND EXECUTION

0. Determine the tasks tier of the workload - see [Task Tiers](#task-tiers) section for criteria. 

1. **Plan before acting.** For any Tier M or L task, list the steps before executing them. Tier S tasks may proceed directly. Use a todo list and mark each step in-progress then completed as you go.
2. **Read before editing.** NEVER modify a file you have not read in the current session. Always read the relevant file section first.
3. **Verify after each step.** After creating or editing files, check for compile/lint errors before proceeding to the next step.
4. **Ask when blocking.** Infer and proceed for anything that can be reasonably deduced. ALWAYS use the built-in user-question tool (`askQuestion` on GitHub Copilot, `AskUserQuestion` on Claude Code) when one of the following is true: 
- (a) an artifact ID is missing and cannot be inferred from context; 
- (b) an operation is destructive or irreversible and affects shared infrastructure; 
- (c) two valid designs have meaningfully different and irreconcilable long-term trade-offs. 
- (d) you failed to perform the action requested by the user after following the [Error Recovery Protocol](#error-recovery-protocol), WHATEVER THE REASON. In this case STOP EVERYTHING and WAIT for the user's help.
- Everything else: decide and proceed.

5. **No scope creep.** Implement ONLY what was explicitly requested. Do NOT add features, refactor surrounding code, or introduce new abstractions beyond the task scope.
6. **Self-check before declaring done.** Before ending a task, verify every item in the Delivery Checklist below.

### DEFINITION OF READY - DoR (Planning Checklist)
Before marking any task Ready, confirm ALL of the following:
- [ ] Task has clear description, acceptance criteria (Gherkin), and tier (S/M/L).
- [ ] Task references the requirement ID(s) and ADR ID(s) it implements.
- [ ] For UI tasks: the design-system RQ/ADR IDs are listed in the task's references.
- [ ] PRESENT task to the user and WAIT for explicit approval before starting.

### DEFINITION OF DONE - DoD (Delivery Checklist)

The DoD items that apply depend on the task tier. Use the matrix below:

| DoD Item | S | M | L |
|----------|:-:|:-:|:-:|
| Every new artifact references a requirement ID | ✓ | ✓ | ✓ |
| No string or numeric literal is duplicated inline — all are named constants | — | ✓ | ✓ |
| No failing test was modified to force it to pass | — | ✓ | ✓ |
| UI change consumes design-system tokens — no raw visual literal in code | ✓ | ✓ | ✓ |
| Code compiles and passes static analysis with no errors | ✓ | ✓ | ✓ |

WHEN a task is done, add and commit with GIT all changes into the feature branch by invoking the `agnos-git-workflow` skill's `commit-task <TASK> [<ADR>] <description>` sub-command, where:
- `<TASK>` is the unique id of the task (e.g. `TASK-USR-001`)
- `<ADR>` is the unique id of the related ADR, if applicable (e.g. `ADR-USR-001`)
- `<description>` is a brief summary of changes

The skill validates all artifact IDs before executing and produces the commit message `<ADR>/<TASK> <description>` when an ADR is provided, or `<TASK> <description>` otherwise.

### TASK TIERS

Tag every task with a tier before starting. The tier determines which steps are mandatory.

| Tier | Criteria | Tests required | ADR required | Plan required |
|------|----------|:-:|:-:|:-:|
| **S - Small** | Edits <= 5 lines, no new files, no new public API | No | No | No |
| **M - Medium** | New method or class, new file, contained to one module | Yes | No | No |
| **L - Large** | Cross-cutting change, new public API, or new dependency | Yes | Yes | Yes |

- Default to **M** when uncertain.
- A Tier S task that unexpectedly requires a new file SHALL be re-tiered to M or L before continuing.

### Plan/Task File Template

Every plan file in `process/3.plan/` SHALL follow this structure:

````markdown
# PLAN-<TRI>-<NNN>: <Plan Title>

## Overview
Brief description of the plan's purpose and scope.

## References
- **Requirements**: [List of RQ IDs this plan covers]
- **ADRs**: [List of ADR IDs relevant to this plan, or "None"]

This plan implements the tasks in the format specified below. 
---

## Tasks

### TASK-<TRI>-<NNN>: <Short Title>
- **Tier**: [S | M | L]
- **Status**: [Not Started | In Progress | Done | Blocked]
- **Description**: [What this task accomplishes — one to two sentences]
- **Requirement refs**: [RQ-<TRI>-<NNN>, ...]
- **ADR refs**: [ADR-<TRI>-<NNN>, ... or "None"]
- **Acceptance Criteria** (Gherkin; see [§1 format](#1-requirements-authoring)): Given/When/Then
- **Dependencies**: [List of TASK IDs that must be completed first, or "None"]
- **Assignee**: [Human | AI]
````


## 4. CODE IMPLEMENTATION AND QUALITY

- All generated code SHALL comply with **SOLID** principles (Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion).
- **No duplicated/magic literals (strings or numbers).** Every constant SHALL be declared as a named class-level or method-level constant. 
- **UI code**: every visual constant (color, spacing, size, alpha, font metric) SHALL be
  initialized from design-system tokens. Local named constants MAY alias a token for
  readability, but SHALL NOT hold a raw visual value.
- Code SHALL contains traceable references to the requirement ID(s) and ADR decisions ID(s) it implements in the form of comments or docstrings.


### Testing

> **WHILE `session.unit_tests = false`**: skip ALL steps in this section 
> If `session.unit_tests` is not in active context, read `process/_sessionstate/session.yaml` first.

- For Tier M and L tasks, every generated function or method SHALL have at least one unit test. Tier S tasks are exempt from this requirement.
- Write all tests and acceptance criteria in **Gherkin format** (Given / When / Then).
- A function is considered delivered ONLY when ALL of the following conditions are met:
  1. All associated unit tests pass without any modification to the test implementation.
  2. Traceability to the fulfilled requirement ID is explicit and unambiguous.
- Test modification rule: see [DoD matrix](#definition-of-done---dod-delivery-checklist) — "No failing test was modified to force it to pass." A test may only be changed if the change correctly reflects new or corrected expected behavior.

### Anti-Patterns (NEVER do these)

- Never recreate a file from scratch when a targeted edit on the existing file suffices.
- Never run a broad workspace search when the file path or symbol is already known.
- Never add error handling, logging, or fallbacks for scenarios that cannot occur in the current design.
- Never introduce a new abstraction, helper, or utility for a one-off operation.
- Never add documentation, comments, or type annotations to code you did not change.
- Never assume a constant or configuration value -- read the source file first.
- Never port a raw visual value from a reference implementation, mockup or screenshot directly
  into UI code. Route it through the design system first (add or alias a token, with a rationale
  note if it deviates from the reference), then consume the token.
- Never guess a fix and retry the same failing approach twice. If the first targeted fix fails, stop and report see [PLAN - Agentic Planning and Execution](#3-plan---planning-and-execution) "**Ask when blocking.** " step 4.d.
- **ALWAYS use `session.platform`** when generating shell commands:
  use PowerShell syntax on `windows`; use bash/zsh syntax on `macos` or `linux`. Never use
  bash-only commands (`ls`, `tail`, `grep`, `cat`) on Windows.
  If `session.platform` is not in active context, read `process/_sessionstate/session.yaml` first.


### Error Recovery Protocol

When a compile, lint, or test step fails:

1. **Read the full error message** before taking any action.
2. **Fix the root cause** -- do not suppress, comment out, or work around the failure.
3. **One attempt:** apply the targeted fix, then re-run the failing check.
4. **If still failing:** stop, report the exact error and what was attempted, and WAIT for guidance. Do NOT guess a second time.
