---
name: agnos-git-workflow
description: "AGNOS process GIT workflow skill. Handles all git operations mandated by the AGNOS process: 'start-session' creates and checks out a feature branch; 'commit-task' stages and commits changes with the correct AGNOS message format. USE FOR: START SESSION step 7 (branch creation) and DoD commit step (task commit). Validates all artifact IDs before executing any git command."
argument-hint: "start-session <TRI> | commit-task <TASK-ID> [<ADR-ID>] <description>"
---

# AGNOS Git Workflow

Deterministic git operations for the AGNOS process. All artifact IDs are validated before any
git command is executed. This is the canonical procedure, shared by GitHub Copilot
(`.github/skills/agnos-git-workflow/`) and Claude Code (`.claude/skills/agnos-git-workflow/`,
which points here) — see ADR-PRT-001.

## Script Selection (RQ-PRT-006)

Every validation step below runs a `<validate-ids>` script. Select it from `session.platform`
(read `process/_sessionstate/session.yaml` first if not already in context):

| `session.platform` | Command |
|---|---|
| `windows` | `powershell -NoProfile -File .github/skills/agnos-git-workflow/scripts/validate-ids.ps1 -Type <T> -Value <V>` |
| `linux` / `macos` | `bash .github/skills/agnos-git-workflow/scripts/validate-ids.sh -Type <T> -Value <V>` |

Use `powershell` (Windows PowerShell 5.1), never `pwsh` — PowerShell 7 is not guaranteed to be
installed (RQ-PRT-007).

## Sub-Commands

### `start-session <TRI>` — Feature Branch Creation (RQ-GIT-001)

**When to use**: START SESSION step 7 — at the beginning of every new session.

**Procedure:**

1. Validate `<TRI>` by running `<validate-ids>` (see Script Selection) with `-Type TRI -Value <TRI>`.
   If exit code ≠ 0: report the validation error and STOP. Do NOT proceed.

2. Check whether the branch already exists:
   ```
   git show-ref --quiet refs/heads/feature/<TRI>
   ```
   - **Does NOT exist** (exit code ≠ 0): run `git checkout -b feature/<TRI>` then confirm to the user.
   - **Already exists** (exit code = 0): ask the user how to proceed. Options:
     - (a) Check it out: `git checkout feature/<TRI>`
     - (b) Reset to current HEAD: `git checkout feature/<TRI> && git reset --hard HEAD`
     - (c) Delete and recreate
     - (d) Use a different trigram
   - Wait for explicit confirmation before executing any option.

---

### `commit-task <TASK-ID> [<ADR-ID>] <description>` — Task Commit (RQ-GIT-002)

**When to use**: DoD commit step — when a task is marked Done.

**Procedure:**

1. Parse the arguments:
   - First token → TASK-ID
   - If second token matches `ADR-*` → ADR-ID; remaining tokens → description
   - Otherwise → no ADR-ID; all remaining tokens → description

2. Validate TASK-ID using `<validate-ids>` (see Script Selection) with `-Type TASK -Value <TASK-ID>`.
   If exit code ≠ 0: report the error and STOP.

3. If ADR-ID was provided, validate it using `<validate-ids>` with `-Type ADR -Value <ADR-ID>`.
   If exit code ≠ 0: report the error and STOP.

4. Build the commit message:
   - With ADR: `<ADR-ID>/<TASK-ID> <description>`
   - Without ADR: `<TASK-ID> <description>`

5. Confirm the computed commit message with the user:
   > "About to commit: `<message>`. Proceed?"
   Wait for confirmation.

6. Execute:
   ```
   git add .
   git commit -m "<message>"
   ```
   Confirm the commit hash to the user.
