# RQ-GOV — Community Health & Contribution Standards

Scope: the repository's public-facing governance surface — Code of Conduct,
contribution workflow, issue intake, and pull-request intake — so that
external contributors (human or AI-agent-assisted) have one unambiguous,
non-negotiable path into the project, and maintainers (human or AI) receive
structured, machine-parseable input instead of free-text issues. Distinct
functional area from the JUCE application requirements (RQ-GUI-*, RQ-MOD-*,
etc.) and from build/release tooling (RQ-BLD-*), hence the RQ-GOV-* series.

---

## Functional Requirements

### RQ-GOV-001: Code of Conduct

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The repository SHALL publish a `CODE_OF_CONDUCT.md` at its
  root, based on the Contributor Covenant v2.1, stating expected behaviour,
  unacceptable behaviour, and an enforcement contact.
- **Rationale**: a published, standard Code of Conduct is a prerequisite for
  predictable, low-friction community interaction and is one of GitHub's
  community-standards checklist items.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* the repository root, *When* `CODE_OF_CONDUCT.md` is opened,
    *Then* it states the standards of behaviour and how to report a
    violation.
  - *Given* GitHub's "Community Standards" checklist for this repository,
    *When* it is viewed, *Then* "Code of conduct" is satisfied.
- **Dependencies**: None

---

### RQ-GOV-002: Contribution workflow

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The repository SHALL publish a `CONTRIBUTING.md` at its
  root describing the standard fork → branch → commit → pull-request
  workflow, how to report bugs and request features (via RQ-GOV-004/005),
  and how pull requests are reviewed (via RQ-GOV-006).
- **Rationale**: a single, discoverable document removes ambiguity for
  first-time contributors and reduces maintainer triage effort.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* the repository root, *When* `CONTRIBUTING.md` is opened, *Then*
    it describes the fork/branch/PR workflow and links the issue and PR
    templates.
- **Dependencies**: RQ-GOV-004, RQ-GOV-005, RQ-GOV-006

---

### RQ-GOV-003: Mandatory AGNOS process for AI-agent-assisted contributions

- **Category**: Functional
- **EARS Type**: Event-driven
- **Statement**: WHEN a contributor uses an AI coding agent (e.g. GitHub
  Copilot, Claude Code, or an equivalent tool) to prepare a change to this
  repository, `CONTRIBUTING.md` SHALL state — as a non-negotiable
  requirement, not a suggestion — that the agent SHALL follow the AGNOS
  process defined in
  `.github/instructions/agnos-sw-eng.v2.instructions.md` (Claude Code entry
  point: `CLAUDE.md`), and SHALL NOT duplicate or paraphrase that process
  inline; it SHALL link to it as the single source of truth.
- **Rationale**: the project's stated purpose for the JUCE port includes
  demonstrating disciplined AI-agent SDLC integration (README "What's new in
  the JUCE port"); an external contribution prepared by an AI agent without
  the same traceability discipline (requirement/ADR/task IDs) would break
  that guarantee and reintroduce the ambiguity AGNOS exists to remove.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* `CONTRIBUTING.md`, *When* it is read, *Then* it states plainly
    that using an AI coding agent makes following AGNOS mandatory, and links
    to `.github/instructions/agnos-sw-eng.v2.instructions.md` rather than
    restating its content.
  - *Given* a pull request whose description or commits show evidence of
    AI-agent assistance (e.g. AGNOS traceability IDs absent where the change
    touches `process/`-managed areas), *When* it is reviewed against the PR
    template (RQ-GOV-006), *Then* the missing traceability is flagged before
    merge.
- **Dependencies**: RQ-GOV-002, RQ-GOV-006

---

### RQ-GOV-004: Structured bug-report issue template

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The repository SHALL provide a structured bug-report issue
  form (`.github/ISSUE_TEMPLATE/bug_report.yml`) with required fields
  covering: Xplorer version/build, platform, affected synthesizer, a
  summary, reproduction steps, expected behaviour, and actual behaviour.
  Free-form ("blank") issues SHALL be disabled for bug reports.
- **Rationale**: a fixed, required-field structure is the only way to make
  "what information must be provided" non-negotiable — a prose template can
  be skipped section by section; a GitHub Issue Form with required fields
  cannot be submitted incomplete. No "implementation" field is needed: the
  archived .NET codebase (`XplorerEditor-dotnet-archive`) is no longer
  supported once the JUCE port reaches its first official release (Status
  section, README), so the JUCE version is the only one the form needs to
  ask about; the free-text version/build field alone answers "which
  build".
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* a user opens a new issue, *When* they choose "Bug Report",
    *Then* they cannot submit without filling every field marked required.
  - *Given* a submitted bug report, *When* a maintainer reads it, *Then*
    every field required to reproduce the problem (version, platform,
    steps, expected vs. actual) is present without needing a follow-up
    question.
- **Dependencies**: RQ-GOV-008

---

### RQ-GOV-005: Structured feature-request issue template

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The repository SHALL provide a structured feature-request
  issue form (`.github/ISSUE_TEMPLATE/feature_request.yml`) with required
  fields covering: the problem/motivation, the proposed solution, and the
  impacted area (UI, MIDI, patch management, build/CI, other); an
  alternatives-considered field SHALL be present and optional.
- **Rationale**: separates "what problem are you having" from "what do you
  want built", which is the single most common source of ambiguous feature
  requests.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* a user opens a new issue, *When* they choose "Feature Request",
    *Then* they cannot submit without describing both the motivation and
    the proposed solution.
- **Dependencies**: RQ-GOV-008

---

### RQ-GOV-006: Pull Request template with traceability fields

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The repository SHALL provide a pull-request template
  (`.github/PULL_REQUEST_TEMPLATE.md`) requiring: a description of the
  change, the issue(s) it addresses, the AGNOS requirement/ADR/task IDs it
  implements (when applicable), a checklist including Code of Conduct
  acknowledgement and, WHEN the contribution was AI-agent-assisted,
  confirmation that AGNOS (RQ-GOV-003) was followed.
- **Rationale**: makes AGNOS non-compliance visible at review time instead
  of relying on trust, and gives every PR the same minimum reviewable shape.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* a new pull request, *When* its description is opened, *Then*
    the template's sections (description, related issues, traceability
    IDs, checklist) are pre-filled as placeholders.
- **Dependencies**: RQ-GOV-003

---

### RQ-GOV-007: README references the governance artifacts

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: `README.md` SHALL reference `CODE_OF_CONDUCT.md`,
  `CONTRIBUTING.md`, and the issue/PR templates from a "Contributing" or
  "Community" section, replacing any contribution instructions duplicated
  inline.
- **Rationale**: `README.md` is the repository's entry point; contribution
  and conduct expectations SHALL be discoverable from there without
  duplicating content that would drift out of sync with `CONTRIBUTING.md`.
- **Priority**: Should
- **Acceptance Criteria** (Gherkin):
  - *Given* `README.md`, *When* its Contributing/Community section is read,
    *Then* it links to `CONTRIBUTING.md` and `CODE_OF_CONDUCT.md` and does
    not restate the fork/branch/PR steps inline.
- **Dependencies**: RQ-GOV-001, RQ-GOV-002

---

## Non-Functional Requirements

### RQ-GOV-008: Issue templates are unambiguous and AI-reformulation-friendly

- **Category**: Non-Functional
- **NFR Type**: Usability
- **EARS Type**: Ubiquitous
- **Statement**: Every issue-form field defined under RQ-GOV-004/RQ-GOV-005
  SHALL have a plain-language label and description such that (a) a
  non-technical user knows exactly what to enter, (b) a maintainer receives
  unambiguous, directly actionable information with no implicit fields, and
  (c) an AI assistant reading the raw submitted form (field label + value)
  can accurately reformulate the issue in plain language for a
  non-technical user without additional clarification.
- **Metric**: 100% of required fields have a non-empty `description` (or
  form-field placeholder) explaining, in plain language, what is expected.
- **Measurement Method**: manual review of each `.yml` issue-form field
  against the three audiences (submitter, maintainer, AI reader) at
  authoring time; re-checked whenever a field is added or changed.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* a submitted bug report or feature request, *When* its raw field
    labels and values are given to an AI assistant with no other context,
    *Then* the assistant can produce an accurate plain-language summary of
    the issue without inventing information not present in the submission.
- **Dependencies**: RQ-GOV-004, RQ-GOV-005

---

### RQ-GOV-009: Security policy and private vulnerability reporting

- **Category**: Functional
- **EARS Type**: Ubiquitous
- **Statement**: The repository SHALL publish a `SECURITY.md` at its root
  stating: which version line receives security fixes, how to report a
  vulnerability **privately** (not as a public issue), what a report should
  contain, a best-effort response target, disclosure expectations, and the
  scope (in/out) of what a report may cover. It SHALL point reporters at
  GitHub's private vulnerability reporting flow rather than an email
  address.
- **Rationale**: ADR-GOV-001 originally deferred a security policy for
  exactly this reason — no monitored contact email existed, and inventing
  one would have been misleading (same reasoning as DEC-GOV-003's Code of
  Conduct contact). GitHub's private vulnerability reporting (draft security
  advisories) removes that blocker without fabricating a contact channel: it
  is free, needs no separate inbox, and ties every report to the
  maintainer's existing GitHub identity. Revisited 2026-08-17 on owner
  request.
- **Priority**: Must
- **Acceptance Criteria** (Gherkin):
  - *Given* the repository root, *When* `SECURITY.md` is opened, *Then* it
    names the supported version line, links GitHub's private
    vulnerability-reporting form, and states scope, response targets and
    disclosure expectations.
  - *Given* `SECURITY.md`'s reporting section, *When* read, *Then* it tells
    the reader explicitly not to open a public issue for a vulnerability.
  - *Given* GitHub's "Community Standards" checklist for this repository,
    *When* viewed, *Then* "Security policy" is satisfied.
- **Dependencies**: ADR-GOV-001 (DEC-GOV-006)
