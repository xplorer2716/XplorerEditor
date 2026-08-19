<!-- RQ-GOV-009 / ADR-GOV-001 (DEC-GOV-006) -->

# Security Policy

## Supported Versions

Xplorer follows a single rolling release line — only the latest published release receives
security fixes. There are no long-term-support branches.

| Version | Supported |
|---|---|
| Latest release | :white_check_mark: |
| Anything older | :x: |
| [.NET reference implementation](https://github.com/xplorer2716/XplorerEditor-dotnet-archive) (archived) | :x: — no longer maintained |

## Reporting a Vulnerability

**Do not open a public GitHub issue for a security vulnerability.** Use GitHub's private
vulnerability reporting instead:

👉 **[Report a vulnerability](https://github.com/xplorer2716/XplorerEditor/security/advisories/new)**

This opens a draft security advisory visible only to the maintainer — nobody else sees it until a
fix is ready and you both agree to publish.

If that link doesn't work for you, contact the maintainer directly through their GitHub profile:
[@xplorer2716](https://github.com/xplorer2716).

### What to include

- The affected component (application, MIDI I/O, patch file parsing, build/release pipeline, ...)
- The version/build affected (Help → About, or the release tag)
- Platform and OS version
- Steps to reproduce, or a proof of concept
- The potential impact, as you understand it

### What to expect

Xplorer is maintained by a single volunteer, not a security team, so there is no formal SLA. As a
target:

- Acknowledgement within 7 days.
- A first assessment (confirmed / not applicable / needs more information) within 14 days.
- A fix or mitigation timeline once the report is confirmed, communicated in the advisory thread.

Coordinated disclosure is preferred: please give the maintainer a reasonable window to ship a fix
before any public disclosure. There is no bug bounty program.

## Scope

**In scope:**
- The JUCE-based Xplorer application (`juce/`) and its build/release pipeline (`.github/`) —
  patch-file (`.syx`) parsing, MIDI I/O, settings storage, the update/release process.

**Out of scope:**
- The archived .NET reference implementation
  ([`XplorerEditor-dotnet-archive`](https://github.com/xplorer2716/XplorerEditor-dotnet-archive))
  — no longer maintained; please don't report issues against it here.
- Vulnerabilities in third-party dependencies themselves (JUCE, Catch2, ...) — please report those
  upstream. If you believe *how Xplorer uses* a dependency is the actual problem, that's in scope.
- Denial-of-service reports that require physical or local access to the machine running Xplorer.

## What Xplorer already does

For transparency, so a report doesn't have to rediscover this:

- **Dependencies are pinned**, not floating — JUCE, Catch2, and the third-party GitHub Actions
  used in CI.
- **Every release publishes a Software Bill of Materials** (`xplorer.sbom.spdx.json`), also shown
  in-app under Help → About → Dependencies.
- **Production and pre-production release archives carry a signed build-provenance attestation**
  (Sigstore, via GitHub Artifact Attestations). Verify one with:
  ```
  gh attestation verify <downloaded-archive> --owner xplorer2716
  ```
  This proves the archive was built by this repository's own CI, from a specific commit — it is
  **not** code signing.
- **Binaries are not code-signed.** Xplorer is an AGPLv3 project with no paid Apple Developer ID
  or Windows code-signing certificate, so Windows SmartScreen and macOS Gatekeeper will warn on
  first launch — every release's notes explain how to proceed. That's a cost decision, not a
  suggestion to bypass those warnings without checking the attestation and the SBOM above first.
