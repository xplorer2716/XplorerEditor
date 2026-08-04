# PLAN-GOV-001: Community Health & Contribution Standards

## Overview

Create the repository's standard GitHub community-health surface — Code of
Conduct, contribution guide, structured issue templates, pull-request
template — and point `README.md` at them, so external contribution and
issue intake follow one non-negotiable, unambiguous, AI-parseable standard
instead of the current five-line generic list in `README.md`.

## References

- **Requirements**: RQ-GOV-001 … RQ-GOV-008 (`RQ-GOV-community-and-contribution.md`)
- **ADRs**: ADR-GOV-001 (DEC-GOV-001 … DEC-GOV-005)

Session state: `unit_tests = false`, `platform = windows`, `chat_mode = normal`.
Branch: `feature/GOV`.

This plan implements the tasks in the format specified by the AGNOS process.

---

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-GOV-001 | Author `CODE_OF_CONDUCT.md` (Contributor Covenant v2.1) | M | RQ-GOV-001, ADR-GOV-001 (DEC-GOV-003) |
| TASK-GOV-002 | Author `CONTRIBUTING.md` incl. mandatory-AGNOS clause | M | RQ-GOV-002, RQ-GOV-003, ADR-GOV-001 (DEC-GOV-002) |
| TASK-GOV-003 | Author structured issue templates (bug report, feature request, config) | M | RQ-GOV-004, RQ-GOV-005, RQ-GOV-008, ADR-GOV-001 (DEC-GOV-001, DEC-GOV-005) |
| TASK-GOV-004 | Author `PULL_REQUEST_TEMPLATE.md` | M | RQ-GOV-006, ADR-GOV-001 (DEC-GOV-004) |
| TASK-GOV-005 | Update `README.md` Contributing/Community section | S | RQ-GOV-007 |
| TASK-GOV-006 | Correct "What's new in the JUCE port" README section (stale focus-indicator claim; missing shipped GFX improvements) | S | RQ-GUI-054/055/056, RQ-SCL-001..004, ADR-JUC-023/024/025/026/029/030/031/032 |

No cross-task code dependency — each produces an independent file — but
TASK-GOV-005 (README) references the artifacts produced by 001-004, so it
runs last.

---

### TASK-GOV-001: Author Code of Conduct
- **Tier**: M
- **Status**: Done
- **Description**: Add `CODE_OF_CONDUCT.md` at the repository root, the
  Contributor Covenant v2.1 text (DEC-GOV-003), enforcement contact set to
  the maintainer's GitHub handle.
- **Requirement refs**: RQ-GOV-001
- **ADR refs**: ADR-GOV-001 (DEC-GOV-003)
- **Acceptance Criteria** (Gherkin):
  - *Given* the repository root, *When* `CODE_OF_CONDUCT.md` is opened,
    *Then* it states expected/unacceptable behaviour and an enforcement
    contact.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GOV-002: Author Contributing Guide
- **Tier**: M
- **Status**: Done
- **Description**: Add `CONTRIBUTING.md` describing the fork/branch/PR
  workflow, links to the issue templates (TASK-GOV-003) and PR template
  (TASK-GOV-004), and the non-negotiable clause: any contribution prepared
  with an AI coding agent SHALL follow AGNOS
  (`.github/instructions/agnos-sw-eng.v2.instructions.md`, entry point
  `CLAUDE.md`), linked rather than restated (DEC-GOV-002).
- **Requirement refs**: RQ-GOV-002, RQ-GOV-003
- **ADR refs**: ADR-GOV-001 (DEC-GOV-002)
- **Acceptance Criteria** (Gherkin):
  - *Given* `CONTRIBUTING.md`, *When* read, *Then* it links (not
    duplicates) the AGNOS instructions and states the AI-agent clause as
    mandatory.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GOV-003: Author structured issue templates
- **Tier**: M
- **Status**: Done
- **Description**: Add `.github/ISSUE_TEMPLATE/bug_report.yml`,
  `feature_request.yml` (GitHub Issue Forms, required fields, DEC-GOV-001)
  and `config.yml` (`blank_issues_enabled: false`, DEC-GOV-005). Every
  field carries a plain-language label/description satisfying RQ-GOV-008.
- **Requirement refs**: RQ-GOV-004, RQ-GOV-005, RQ-GOV-008
- **ADR refs**: ADR-GOV-001 (DEC-GOV-001, DEC-GOV-005)
- **Acceptance Criteria** (Gherkin):
  - *Given* a new issue, *When* "Bug Report" or "Feature Request" is
    chosen, *Then* required fields block submission until filled.
  - *Given* the "New Issue" page, *When* opened, *Then* no blank/free-form
    option is offered.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GOV-004: Author Pull Request template
- **Tier**: M
- **Status**: Done
- **Description**: Add `.github/PULL_REQUEST_TEMPLATE.md` with description,
  related-issue, AGNOS-traceability-IDs fields and a checklist including
  Code of Conduct acknowledgement and the AI-assistance/AGNOS-compliance
  checkbox (DEC-GOV-004).
- **Requirement refs**: RQ-GOV-006
- **ADR refs**: ADR-GOV-001 (DEC-GOV-004)
- **Acceptance Criteria** (Gherkin):
  - *Given* a new pull request, *When* opened, *Then* the description box
    is pre-filled with the template's sections.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-GOV-005: Update README Contributing/Community section
- **Tier**: S
- **Status**: Done
- **Description**: Replace `README.md`'s inline fork/branch/PR list with a
  short Community/Contributing section linking `CONTRIBUTING.md` and
  `CODE_OF_CONDUCT.md`.
- **Requirement refs**: RQ-GOV-007
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - *Given* `README.md`, *When* its Contributing section is read, *Then* it
    links to `CONTRIBUTING.md` and `CODE_OF_CONDUCT.md` with no duplicated
    step-by-step instructions.
- **Dependencies**: TASK-GOV-001, TASK-GOV-002
- **Assignee**: AI

---

### TASK-GOV-006: Correct "What's new in the JUCE port" README section
- **Tier**: S
- **Status**: Done
- **Description**: The section claimed a keyboard-focus visual indicator,
  which ADR-JUC-029 removed (owner: "l'histoire du focus ce n'est plus
  vrai"). Corrected the claim and added the other shipped GFX-session
  improvements the section had not caught up with: window size presets /
  full screen and device-pixel-snapped VFD glyph grid (ADR-JUC-025/026),
  recessed VFD bezel and vector segment rendering (ADR-JUC-023/024), the
  MIDI-activity lamp redesign (ADR-JUC-031), the reference-aligned menu bar
  (ADR-JUC-032), and splash-screen removal (ADR-JUC-030). No new
  requirement or ADR — this traces to artifacts that already exist and are
  Accepted; the task only brings README's marketing copy back in sync with
  what actually shipped.
- **Requirement refs**: RQ-GUI-054, RQ-GUI-055, RQ-GUI-056, RQ-SCL-001,
  RQ-SCL-002, RQ-SCL-003, RQ-SCL-004
- **ADR refs**: ADR-JUC-023, ADR-JUC-024, ADR-JUC-025, ADR-JUC-026,
  ADR-JUC-029, ADR-JUC-030, ADR-JUC-031, ADR-JUC-032
- **Acceptance Criteria** (Gherkin):
  - *Given* `README.md`'s "What's new in the JUCE port" section, *When*
    read, *Then* it makes no claim about a keyboard-focus indicator.
  - *Given* that section, *When* read against the ADR-JUC-023/024/025/026/
    030/031/032 Consequences sections, *Then* every remaining claim is
    still true.
- **Dependencies**: None
- **Assignee**: AI

---

## Definition of Ready

- [x] Each task has a description, Gherkin acceptance criteria and a tier.
- [x] Each task references its requirement and ADR IDs.
- [x] **Owner approval** — granted 2026-08-05 (RQ-GOV-004 amended to drop the
      .NET dropdown per owner instruction before implementation started).
- [x] All six tasks Done.
