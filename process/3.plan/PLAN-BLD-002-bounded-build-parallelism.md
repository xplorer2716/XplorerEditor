# PLAN-BLD-002: Bounded Build Parallelism — CI OOM Fix

## Overview
Fix a CI defect reported by the owner: the `linux-x64-debug` build on PR #56
failed with `cc1plus` killed by the kernel, not a compile error. Root cause:
`.github/actions/build-app/action.yml`'s `Build` step ran
`cmake --build ... -j` with a bare `-j` — unlimited parallelism on the Unix
Makefiles generator — which OOM-killed the compiler on the 2-core/7GB Linux
runner once the tree had enough targets to launch dozens of heavy Debug JUCE
compiles within the same couple of seconds.

## References
- **Requirements**: RQ-BLD-031 (new)
- **ADRs**: None. A one-line build-command fix with a directly observed root
  cause (the failed run's own log); no design decision to record. Tier S.

---

## Tasks

### TASK-BLD-014: Bound `-j` to the runner's own core count
- **Tier**: S
- **Status**: Done (2026-08-18)
- **Description**: Compute the job count from a portable core-count query
  (`getconf _NPROCESSORS_ONLN`, falling back to `nproc` then
  `sysctl -n hw.ncpu` then a fixed default) and pass it explicitly to
  `cmake --build ... -j "$jobs"`, instead of a bare `-j`. Applied
  unconditionally across all three operating systems the composite action
  covers — Release and the other two OSes happened not to trip the defect,
  but nothing about it was principled to Linux or Debug specifically.
- **Requirement refs**: RQ-BLD-031
- **ADR refs**: None
- **Acceptance Criteria** (Gherkin):
  - **Given** the `Build` step's script, **When** it is read, **Then** the
    `-j` argument is a computed job count, never bare
  - **Given** a `linux-x64-debug` run after this fix, **When** it completes,
    **Then** it does not fail with a `cc1plus` kill
- **Dependencies**: None
- **Assignee**: AI

---

## Note on testing
Tier S, and the actual defect only reproduces on a real GitHub-hosted
runner under real memory pressure — nothing this session's sandbox can
exercise. Verified by: reading the failed run's own job log (confirms the
`cc1plus` kill and the unbounded-`-j` compile burst), running the portable
core-count one-liner locally to confirm it resolves to a sane number, and
validating the edited `action.yml`'s YAML syntax. The actual fix is proven
by the next CI run on this PR going green, not by a local test.

## Out of scope
Reducing the number of translation units compiled into `xpl_tests_app_juce`
(this session's own additions contributed to crossing the threshold, but
removing test coverage to fit an under-provisioned runner would be the wrong
fix — the runner's job count, not the test suite's size, was never bounded
to begin with).
