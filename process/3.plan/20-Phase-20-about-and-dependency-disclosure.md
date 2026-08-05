# PLAN-ABT-001: About Dialog Typography & SBOM-Driven Dependency Disclosure

## Overview

Fix the two owner-reported defects of the About dialog and give the product a
real third-party licence disclosure, sourced from a Software Bill of Materials
the build ships beside the executable rather than from a hard-coded list.

Owner report (2026-08-05, session ABT):
1. *"La police des liens/url est vraiment petite. Mettre la même police que les
   autres libellés, au besoin rallonger la fenêtre. idem pour la police de la
   license."*
2. *"Pour la license, ajouter également les licences des composants tiers."*

Then, after a first inline-rows implementation was reviewed and rejected:
*"on peut peut-être prévoir juste un texte et un lien pour la license de Xplorer
lui même, puis un bouton pour les dépendances qui affichera toutes les
dépendances par ordre alphabétique et leur licences et site dans une fenêtre
dédiée [...] on va assumer que le build fournit la sbom avec l'exécutable [...]
on va partir sur une sbom au format SPDX car c'est le format natif fourni par
Github."*

**Deferred by owner decision, recorded so it is not read as an oversight:** the
About dialog's **version string** (`Xplorer 0.1.0`, hard-coded, matching neither
the CMake project version nor any published release tag) is *not* fixed here —
it gets its own requirement covering end-to-end alignment with the GitHub
Actions release tagging.

**Finding recorded during specification, with a consequence outside this plan:**
JUCE 8.0.9's open-source option is **AGPLv3**, not GPLv3 (vendored `LICENSE.md`
of the pinned tag). RQ-BLD-006 states "JUCE shall be used under its GPL option",
which was true of JUCE 6/7 and is not of 8. The disclosure built here states
AGPL v3, because it must be accurate; **RQ-BLD-006's wording is left untouched
and raised to the owner separately** — it is a licensing-position question, not
a UI one.

## References

- **Requirements**: RQ-GUI-057 *(new — this plan implements it)*, RQ-BLD-014
  *(new — this plan implements it)*, RQ-GUI-025 *(amended — this plan implements
  the amendment)*, RQ-BLD-006, RQ-DSN-021, RQ-DSN-031, RQ-DSN-061
- **ADRs**: ADR-ABT-001 *(new — this plan implements DEC-ABT-001…007)*,
  ADR-JUC-014 *(design-token compliance)*, ADR-JUC-002 *(layering)*

Session state: `unit_tests = true`, `platform = windows`, `chat_mode = normal`.
Branch: `feature/GUI` *(owner decision — this session does **not** open a
`feature/ABT` branch; recorded as a deliberate deviation from the AGNOS
per-session branch rule, agreed at session start).*

## What must NOT change

- **The About dialog's reference-derived identity** — the `About.jpg` image
  docked left and stretched, the white background, the title/version/copyright
  column at `TEXT_X`, and click-anywhere-to-dismiss (RQ-GUI-025). The dialog may
  grow vertically; it does not get redesigned.
- **The version string** — deferred, see Overview. `MainComponent.cpp`'s
  `showAboutDialog("Xplorer 0.1.0")` call site stays exactly as it is.
- **Every other dialog**, the panel, the controller, the design-system token set.
- **`oberheim.syx`'s own POST_BUILD copy** — the new SBOM copy is a sibling
  step, added beside it, not a modification of it.

## Tasks

| Task | Description | Tier | RQ / ADR |
|------|-------------|:-:|---|
| TASK-ABT-001 | Unify About body typography; licence section = project licence + Dependencies button | M | RQ-GUI-025; ADR-JUC-014 |
| TASK-ABT-002 | `SbomReader`: SPDX 2.3 parse, field mapping, sentinel/describes filtering, sort, error results + tests | M | RQ-GUI-057; DEC-ABT-003/004/005/006 |
| TASK-ABT-003 | Dependencies window rendering the reader's output; wire the About button | M | RQ-GUI-057; DEC-ABT-005 |
| TASK-ABT-004 | Ship `xplorer.sbom.spdx.json` beside the executable | M | RQ-BLD-014; DEC-ABT-002/007 |

Sequencing: 001 is independent. 002 before 003 (the window renders the reader's
result type). 004 before end-to-end verification (the window has nothing to read
until the build ships a file), but independent of 002/003 at compile time.

---

### TASK-ABT-001: Unify About-dialog body typography; add the Dependencies control
- **Tier**: M
- **Status**: **Done** — build clean, suite green (123/123, 0 test modified);
  **owner visual verification pending**. Body rows now share
  `tokens::semantic::textSubtitle` (14 px), replacing the two unrelated JUCE
  fallbacks (`Label` 15 px vs `HyperlinkButton` ~12.6 px). `HEIGHT` is now
  *derived* from the licence block rather than fixed — and evaluates to 261, the
  reference form's own height, so the dialog did not need to grow after all.
  **Four raw visual literals removed while here** (`Colours::white/black/grey/
  lightgrey/blue`), promoted to the new document-surface token roles rather than
  left inline, since the DoD forbids raw visual literals in UI code this task
  touches.
- **Description**: In `AboutContent` (`juce/app/src/Dialogs.cpp`), give every
  body row — version, copyright, licence notice and both `HyperlinkButton`s — one
  single explicit font derived from a design-system token, replacing the two
  different JUCE fallbacks in effect today (`Label`'s 15 px default vs
  `HyperlinkButton`'s 14 px underlined default auto-shrunk to 70 % of row height,
  ~12.6 px here). Pin the link font with
  `setFont(font, /*resizeToMatchComponentHeight*/ false, Justification::centredLeft)`
  so it cannot be re-shrunk by row geometry. Replace the inline third-party rows
  attempted earlier with a single **Dependencies** button opening the RQ-GUI-057
  window; the licence section keeps only the project's own GPL v3 notice and
  link. Grow `HEIGHT` if the unified size needs the room. The button must not
  trigger the dialog's click-to-dismiss.
- **Requirement refs**: RQ-GUI-025 *(2026-08-05 amendment)*
- **ADR refs**: ADR-JUC-014 *(token compliance)*, ADR-ABT-001 *(the window it opens)*
- **Acceptance Criteria** (Gherkin):
  - *Given* the About dialog, *When* its body rows are rendered, *Then* version,
    copyright, licence notice and both links all use the same text size.
  - *Given* the unified size, *When* the dialog is displayed, *Then* no row is
    clipped and no text overlaps the image column.
  - *Given* the licence section, *When* it is read, *Then* it names the project's
    GPL v3 licence with a working link and enumerates no third-party licence.
  - *Given* the Dependencies button, *When* it is clicked, *Then* the
    dependencies window opens and the About dialog stays open.
  - *Given* the dialog background, *When* it is clicked outside any control,
    *Then* the dialog still closes (RQ-GUI-025 click-to-dismiss preserved).
  - *Given* the sources, *When* the new text size is read, *Then* it resolves to
    a design-system token, not a raw literal.
- **Dependencies**: None *(the button's target is stubbed until TASK-ABT-003)*
- **Assignee**: AI

---

### TASK-ABT-002: `SbomReader` — SPDX 2.3 parsing with explicit failure results
- **Tier**: M
- **Status**: **Done** — build clean, suite green (123/123, 0 test modified);
  4 new scenarios / 14 fixtures, all written to temp files by the tests
  themselves, none committed. **One implementation finding recorded at the
  site:** `juce::var::getProperty` returns a *reference to its own default
  argument* when the key is missing, which dangles if a caller passes a
  temporary and holds the result past the full expression — every read goes
  through a `propertyOf()` helper returning by value instead.
- **Description**: New `juce/app/src/SbomReader.hpp` / `.cpp` exposing a pure
  data-in/data-out surface: a `SbomEntry` (name, version, licence, website), a
  `SbomResult` discriminating **Loaded / FileNotFound / Unreadable / InvalidJson
  / NotSpdxOrEmpty** (DEC-ABT-005), and a `readSbom(const juce::File&)` free
  function. Parse with `juce::JSON` (DEC-ABT-006 — no new dependency). Apply the
  field mapping of DEC-ABT-003 (`licenseConcluded` → `licenseDeclared`;
  `homepage` → `downloadLocation`), treating the SPDX sentinels `NOASSERTION`
  and `NONE` as **absent** at the parse boundary so they can never reach the
  screen. Exclude packages named by `documentDescribes` (DEC-ABT-004). Sort
  entries alphabetically by name, case-insensitively. No `juce::Component`
  anywhere in this unit. Register the new source in `juce/app/CMakeLists.txt`
  and in `xpl_tests_app_juce` (`juce/tests/CMakeLists.txt`), following the
  existing `Dialogs.cpp` precedent.
- **Requirement refs**: RQ-GUI-057
- **ADR refs**: ADR-ABT-001 (DEC-ABT-003, DEC-ABT-004, DEC-ABT-005, DEC-ABT-006)
- **Acceptance Criteria** (Gherkin):
  - *Given* a valid SPDX document with packages out of alphabetical order,
    *When* it is read, *Then* the entries come back alphabetically by name.
  - *Given* a package whose `licenseConcluded` is `NOASSERTION` and whose
    `licenseDeclared` is `MIT`, *When* it is read, *Then* its licence is `MIT`.
  - *Given* a package whose licence fields are all sentinels, *When* it is read,
    *Then* its licence is empty — never the literal `NOASSERTION`.
  - *Given* a package with `homepage` absent and a usable `downloadLocation`,
    *When* it is read, *Then* its website is that download location.
  - *Given* a document whose `documentDescribes` names a package, *When* it is
    read, *Then* that package is absent from the entries.
  - *Given* a path that does not exist, *When* it is read, *Then* the result is
    `FileNotFound` and no entry is returned.
  - *Given* a file containing malformed JSON, *When* it is read, *Then* the
    result is `InvalidJson`.
  - *Given* valid JSON that is not an SPDX document, or an SPDX document whose
    package list is empty or fully filtered, *When* it is read, *Then* the
    result is `NotSpdxOrEmpty`.
  - *Given* the build, *When* it runs, *Then* it compiles warning-clean and the
    suite passes with no test modified.
- **Dependencies**: None
- **Assignee**: AI

---

### TASK-ABT-003: Dependencies window; wire the About dialog's button
- **Tier**: M
- **Status**: **Done** — build clean, suite green; **owner visual verification
  pending** (the four failure messages are reproducible by renaming, truncating
  or corrupting `xplorer.sbom.spdx.json` beside the executable). Four columns
  (Component / Version / Licence / Website) in a scrolling viewport, sized to
  content between 4 and 14 visible rows, resizable because a generated SBOM can
  carry names and URLs no fixed width fits. **A new design-system need surfaced
  and was routed through the tokens rather than inlined:** this is the first
  *light* window in the application besides the reference-ported About dialog,
  so a `surfaceDocument` / `textOnDocument` / `textOnDocumentMuted` /
  `documentSeparator` / `textLink` role family was added to
  `design-tokens.yaml` and regenerated — value-preserving, since the four hues
  are exactly the literals the About dialog already carried inline.
- **Description**: Add a dedicated, resizable window listing the reader's
  entries — one row per dependency showing name, version, licence and website,
  the website activatable and opening in the system browser. On any non-`Loaded`
  result, render the corresponding human-readable explanation in place of the
  list (DEC-ABT-005): not found / unreadable / not parseable / no dependency
  information. Resolve the SBOM path at open time from
  `juce::File::currentExecutableFile.getSiblingFile(...)` (DEC-ABT-002). Every
  visual value comes from design-system tokens (ADR-JUC-014). Wire
  TASK-ABT-001's button to it; declare the entry point in `Dialogs.hpp`
  alongside `showAboutDialog`.
- **Requirement refs**: RQ-GUI-057
- **ADR refs**: ADR-ABT-001 (DEC-ABT-002, DEC-ABT-005), ADR-JUC-014
- **Acceptance Criteria** (Gherkin):
  - *Given* an executable with a valid SBOM beside it, *When* the window is
    opened, *Then* every non-describing package appears exactly once,
    alphabetically, with its version, licence and website.
  - *Given* an entry's website, *When* the user activates it, *Then* that URL
    opens in the system browser.
  - *Given* no SBOM beside the executable, *When* the window is opened, *Then*
    it opens and states that the SBOM was not found — no crash, no empty frame.
  - *Given* a malformed SBOM, *When* the window is opened, *Then* it opens and
    states that the file could not be parsed.
  - *Given* the window sources, *When* they are read, *Then* every visual value
    resolves to a design-system token and no dependency name, version or licence
    identifier appears anywhere in them.
  - *Given* the build, *When* it runs, *Then* it compiles warning-clean and the
    suite passes with no test modified.
- **Dependencies**: TASK-ABT-002 *(renders its result type)*, TASK-ABT-001
  *(provides the button)*
- **Assignee**: AI

---

### TASK-ABT-004: Ship the SBOM beside the executable
- **Tier**: M
- **Status**: **Done** — verified: `xplorer.sbom.spdx.json` (3 207 B) sits beside
  `Xplorer.exe` after a build, alongside `oberheim.syx`. **A licensing finding
  came out of writing it:** JUCE 8.0.9's open-source option is **AGPL-3.0**, not
  GPL-3.0 (checked against the pinned tag's own `LICENSE.md`, which reads
  "dual-licensed under the AGPLv3 and the commercial JUCE licence"). The SBOM
  declares AGPL-3.0 accordingly. **RQ-BLD-006 still says "JUCE shall be used
  under its GPL option" and has been left untouched — it needs an owner
  decision, not a silent edit** (see this plan's Overview).
- **Description**: Add `juce/app/sbom/xplorer.sbom.spdx.json` — a valid SPDX 2.3
  JSON document declaring the product as its `documentDescribes` subject and
  listing the three real third-party components with their **verified** licences:
  JUCE 8.0.9 (**AGPL-3.0-only**, per the pinned tag's own `LICENSE.md`),
  Catch2 3.9.1 (BSL-1.0), Roboto Condensed (Apache-2.0). Copy it beside the
  executable with a `POST_BUILD` step mirroring `oberheim.syx`'s
  (DEC-ABT-002/007). This file is explicitly interim: RQ-BLD-014 requires that
  GitHub's generated SBOM can replace it verbatim. **Test SBOMs stay out of
  version control** — TASK-ABT-002's tests write their own fixtures to temporary
  files (owner instruction), so nothing generated needs ignoring.
- **Requirement refs**: RQ-BLD-014, RQ-BLD-006
- **ADR refs**: ADR-ABT-001 (DEC-ABT-002, DEC-ABT-007)
- **Acceptance Criteria** (Gherkin):
  - *Given* a successful build, *When* the output directory is listed, *Then*
    `xplorer.sbom.spdx.json` sits beside the executable.
  - *Given* that file, *When* it is parsed, *Then* it is valid JSON, declares an
    SPDX version, and covers every third-party component bundled in the binary.
  - *Given* each package's licence field, *When* it is compared with that
    component's actual licence, *Then* they match — in particular JUCE is
    declared AGPL-3.0, not GPL-3.0.
  - *Given* the repository, *When* it is searched for a committed test SBOM,
    *Then* there is none.
- **Dependencies**: None
- **Assignee**: AI

---

## Verification

- **Headless-ish unit tests** (`session.unit_tests = true`): TASK-ABT-002 is the
  mechanically checkable core of this plan — parsing, sentinel handling,
  `documentDescribes` filtering, alphabetical ordering and all four failure
  results are pure data transformations, tested in `xpl_tests_app_juce` against
  fixtures the tests write themselves to temp files. Per DEC-ABT-006 these run
  in app-enabled builds (Windows/macOS CI), not the Linux headless job — the
  same scope as the existing real-metrics and painter tests.
- **Visual/interactive verification by the owner**: About dialog typography and
  layout, the Dependencies button, the window's list rendering and its four
  error messages (reproducible by renaming/corrupting the shipped file). There
  is no pixel baseline for dialogs, matching every other dialog in this codebase.
- **Existing suite stays green**, unmodified, at every task.

## Definition of Ready

- [x] Each task has a description, Gherkin acceptance criteria and a tier.
- [x] Each task references its requirement and ADR IDs.
- [x] No UI literal introduced without a token (typography, window metrics).
- [x] **Owner approval** — granted 2026-08-05 (*"mets à jour REQ/ADR/PLAN et
      continue"*).

## Outcome

*(to be completed at plan close)*
