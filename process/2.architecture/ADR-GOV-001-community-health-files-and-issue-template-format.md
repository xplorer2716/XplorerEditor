# ADR-GOV-001: Community Health Files & Issue Template Format

## Status
Accepted

<!-- Motivated by RQ-GOV-001..008. New functional area (repository
governance / contribution intake) distinct from the JUCE UI/model
architecture (ADR-JUC-*) and build/release tooling (ADR-BLD-*), hence the
ADR-GOV-* series. -->

## Context

The repository has no standard GitHub community-health files
(`CODE_OF_CONDUCT.md`, `CONTRIBUTING.md`, issue templates, PR template).
`README.md`'s current "Contributing" section is a five-line generic
fork/branch/PR list with no traceability to the AGNOS process the project
otherwise uses for every change (RQ-NFR-007, `CLAUDE.md`), and no
structured intake for bug reports or feature requests.

Two forces shape the decision:
1. **Issue intake must be non-negotiable and unambiguous** (owner
   requirement): a submitter must know exactly what to provide, a
   maintainer must receive unambiguous, directly actionable information,
   and — because this project explicitly uses AI coding agents in its own
   SDLC (README "What's new in the JUCE port") — an AI assistant must be
   able to read a raw submission and correctly reformulate it in plain
   language for a non-technical user, with no invented information.
2. **AI-agent-assisted contributions must follow AGNOS, non-negotiably**
   (owner requirement): the project's own commits already carry AGNOS
   traceability (requirement/ADR/task IDs); an external contribution
   prepared by an AI agent without that discipline would silently
   reintroduce the ambiguity AGNOS exists to remove.

## Decision

- **DEC-GOV-001 — GitHub Issue Forms (YAML), not free-text Markdown
  templates.** Bug reports and feature requests SHALL be
  `.github/ISSUE_TEMPLATE/bug_report.yml` and `feature_request.yml`, using
  GitHub's structured Issue Forms schema (`type: input/textarea/dropdown/
  checkboxes`, `validations: { required: true }`). A required field cannot
  be submitted empty — this is what makes the standard *enforced*, not just
  documented. Every field carries a plain-language `label` and
  `description`, which is also what an AI assistant reads to reformulate
  the issue (RQ-GOV-004, RQ-GOV-005, RQ-GOV-008).

- **DEC-GOV-002 — `CONTRIBUTING.md` links to the AGNOS instructions; it
  does not restate them.** The mandatory-AGNOS clause (RQ-GOV-003) points
  to `.github/instructions/agnos-sw-eng.v2.instructions.md` (and
  `CLAUDE.md` as the Claude Code entry point) as the single source of
  truth, mirroring the pattern already established for the git-workflow
  skill (RQ-PRT-004: ".claude/skills/... points to .github/skills/... as
  canonical"). Duplicating the process text inline would create a second
  copy that drifts the first time either is updated.

- **DEC-GOV-003 — Code of Conduct = Contributor Covenant v2.1, verbatim.**
  Industry-standard text (also what GitHub's own community-profile
  checklist recognises), so no bespoke wording to maintain or justify.
  Enforcement contact is the maintainer's GitHub handle (`@xplorer2716`),
  not an invented email address — the repository does not currently
  publish a monitored contact email, and fabricating one would be
  misleading.

- **DEC-GOV-004 — PR template requires explicit traceability IDs and an
  AI-assistance checkbox.** `.github/PULL_REQUEST_TEMPLATE.md` has a
  dedicated "AGNOS traceability" field (requirement/ADR/task IDs, "N/A" if
  none apply) and a checklist item: "If this PR was prepared with the help
  of an AI coding agent, I confirm it followed the AGNOS process
  (CONTRIBUTING.md)." This makes non-compliance visible at review time
  instead of relying on trust (RQ-GOV-006).

- **DEC-GOV-005 — Blank issues disabled.** `.github/ISSUE_TEMPLATE/
  config.yml` sets `blank_issues_enabled: false`, so every bug/feature
  submission is forced through one of the two structured forms — this is
  the mechanism that makes the standard "non-negotiable" rather than one
  option among several (RQ-GOV-004/005).

- **DEC-GOV-006 — `SECURITY.md`, routed through GitHub private vulnerability
  reporting, not an email address.** Revisits the "deferred, out of scope"
  note this ADR originally carried under Alternatives Considered — the
  blocker was never "no policy wanted", it was "no monitored contact channel
  to publish", and GitHub's private security-advisory flow
  (`/security/advisories/new`) removes that blocker the same way
  DEC-GOV-003 solved it for the Code of Conduct: point at an existing GitHub
  identity (`@xplorer2716`) instead of inventing an inbox nobody watches.
  `SECURITY.md` states the supported-version line (latest release only, no
  LTS — ADR-BLD-003's rolling `dev`→`main` model has no branch that would
  make an older line meaningful), scope (the JUCE application and its
  `.github/` pipeline; the archived .NET codebase and third-party
  dependencies themselves are out of scope), a best-effort response target
  (acknowledge in 7 days, first assessment in 14 — explicitly not a
  contractual SLA, since this is a single-volunteer project), and restates
  in plain terms what the project already does for supply-chain trust
  (pinned dependencies RQ-BLD-001, the generated SBOM ADR-BLD-004
  (DEC-BLD-019), build-provenance attestations ADR-BLD-004 (DEC-BLD-026))
  so a reporter isn't left to rediscover it. **Enabling "Private
  vulnerability reporting" in the repository's Security settings is a
  GitHub UI toggle, unreachable from a commit** — same category as
  TASK-BLD-001's branch-protection settings — so `SECURITY.md`'s link only
  works once the owner turns it on.

## Consequences

- **Easier:** every issue arrives with the same required fields, cutting
  maintainer triage/clarification round-trips; an AI agent (used by the
  maintainer or by a contributor) can read a submission's field
  label/value pairs directly and produce a reliable plain-language summary
  or a first-pass fix without guessing missing context; PRs are reviewable
  against one fixed checklist.
- **Harder / constrained:** contributors must fill more structured fields
  before submitting an issue than a one-line free-text report; the two
  `.yml` forms need to be kept in sync (fields, required-ness) whenever the
  project's supported platforms or implementations change.
- **Neutral:** zero effect on application code or CI; `CONTRIBUTING.md`'s
  AGNOS clause applies only when an AI agent is used — a manual,
  human-only PR is not required to run the full AGNOS session ceremony
  (session state, feature branch, ADRs), only to follow the general
  workflow in `CONTRIBUTING.md`.

## Alternatives Considered

- **Plain Markdown issue template (`ISSUE_TEMPLATE.md`, single file):**
  rejected — no required-field enforcement, a submitter can delete or skip
  whole sections, and free text is harder for an AI reader to reliably map
  to "what was actually provided" versus "what the template suggested".
- **Restating the AGNOS process inline in `CONTRIBUTING.md`:** rejected —
  violates the single-source-of-truth pattern already established for the
  git-workflow skill (RQ-PRT-004); two descriptions of the same process
  will diverge.
- **A dedicated `SECURITY.md` with a monitored contact email:** originally
  deferred here as out of scope — not requested by the owner, and the Code
  of Conduct's enforcement contact already used the maintainer's GitHub
  handle rather than an invented address. Revisited 2026-08-17 on owner
  request; see DEC-GOV-006, which resolves the same objection (no
  fabricated inbox) through GitHub's private vulnerability reporting
  instead of email.

## Diagram

```mermaid
flowchart TD
    subgraph Issue Intake
        A["Contributor opens an issue"] --> B{"Bug or Feature?"}
        B -- "Bug" --> C["bug_report.yml<br/>(required fields, DEC-GOV-001)"]
        B -- "Feature" --> D["feature_request.yml<br/>(required fields, DEC-GOV-001)"]
        B -- "Blank" --> E["blocked — config.yml<br/>blank_issues_enabled: false (DEC-GOV-005)"]
        C --> F["Structured issue<br/>readable by maintainer AND AI (RQ-GOV-008)"]
        D --> F
    end
    subgraph Contribution Workflow
        G["Contributor forks / branches"] --> H{"AI coding agent used?<br/>(Copilot, Claude Code, ...)"}
        H -- "Yes" --> I["MUST follow AGNOS<br/>(.github/instructions/agnos-sw-eng.v2, DEC-GOV-002)"]
        H -- "No" --> J["Standard fork/branch/PR workflow<br/>(CONTRIBUTING.md)"]
        I --> K["Pull Request<br/>PULL_REQUEST_TEMPLATE.md (DEC-GOV-004)"]
        J --> K
        K --> L["Maintainer review:<br/>traceability IDs + AI checkbox verified"]
    end
    F -.->|"may become"| G
```
