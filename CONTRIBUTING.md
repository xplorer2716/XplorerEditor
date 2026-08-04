<!-- RQ-GOV-002, RQ-GOV-003 / ADR-GOV-001 (DEC-GOV-002) -->

# Contributing to Xplorer

Thanks for your interest in contributing. This document is the single
entry point for reporting bugs, requesting features, and submitting code.
Please also read the [Code of Conduct](CODE_OF_CONDUCT.md) — participation
in this project requires following it.

## Reporting a bug or requesting a feature

Do not open a blank issue — it is disabled on this repository. Instead,
pick the template that matches:

- **Bug** → [Bug Report](.github/ISSUE_TEMPLATE/bug_report.yml)
- **Feature / enhancement** → [Feature Request](.github/ISSUE_TEMPLATE/feature_request.yml)

Both are structured forms with required fields. This is deliberate and
non-negotiable, for three reasons:
1. it tells you exactly what information is needed, so you don't have to
   guess;
2. it gives the maintainer unambiguous, directly actionable information,
   with no back-and-forth to fill in gaps;
3. every field carries a plain-language description, so the raw submission
   can be read and accurately summarised — by the maintainer or by an AI
   assistant — even by someone with no technical background.

## Submitting code changes

1. Fork the repository.
2. Create a branch for your change (`git checkout -b my-fix`).
3. Make your change, following the build/test instructions in
   [README.md](README.md) for the [juce](juce) tree.
4. Open a Pull Request using the
   [PR template](.github/PULL_REQUEST_TEMPLATE.md) (pre-filled
   automatically) and link the issue it addresses, if any.
5. Respond to review feedback; a maintainer will merge once the checklist
   is satisfied.

All contributions are made under the project's license — see
[LICENSE](LICENSE) (GPL v3).

## Mandatory: using an AI coding agent

**If you use an AI coding agent — GitHub Copilot, Claude Code, or any
equivalent tool — to prepare a contribution to this repository, that agent
SHALL follow the project's AGNOS software engineering process. This is not
optional or a suggestion: it is a non-negotiable requirement for any
AI-agent-assisted contribution.**

This repository is developed using the **AGNOS lightweight agentic
development process** — requirements, architecture decisions, plans and
tasks, all cross-referenced with traceable IDs, so that every change stays
explainable and auditable. The full, authoritative procedure is defined
here and is **not duplicated in this document**, to avoid the two copies
drifting apart:

- **Process definition**: [.github/instructions/agnos-sw-eng.v2.instructions.md](.github/instructions/agnos-sw-eng.v2.instructions.md)
- **Claude Code entry point**: [CLAUDE.md](CLAUDE.md) (tool-specific mapping onto the process above)

In short, an AI agent preparing a change here is expected to author or
reference the relevant requirement (`RQ-*`), architecture decision
(`ADR-*`) and task (`TASK-*`) artifacts under [process](process), and to
carry those IDs into commit messages and code comments — read the linked
instructions for the exact rules rather than relying on this summary.

Pull requests that show signs of AI-agent assistance (e.g. an
AI-authored-looking description, or changes to `process/`-managed areas)
without this traceability will be sent back for revision before review —
see the [Pull Request template](.github/PULL_REQUEST_TEMPLATE.md)
checklist.

Contributions prepared without an AI agent are not required to run the
full AGNOS session ceremony (session state, feature branches, ADRs) —
follow the plain workflow above.
