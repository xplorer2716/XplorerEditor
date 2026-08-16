#!/usr/bin/env bash
#
# Tests for resolve-version.sh. [RQ-BLD-020, ADR-BLD-003 (DEC-BLD-014)]
#
# This script is the one piece of logic every deployment workflow agrees
# through: up to six of them build one commit concurrently and must produce the
# identical version without communicating. That property cannot be observed by
# looking at a green run — a wrong-but-consistent answer looks exactly like a
# right one — so it is asserted here against throwaway repositories with pinned
# committer dates.
#
# Run:  bash .github/actions/resolve-version/tests/resolve-version-tests.sh
set -uo pipefail

SCRIPT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/resolve-version.sh"
PASS=0
FAIL=0

fail() { printf '  FAIL %s\n       expected: %s\n       actual:   %s\n' "$1" "$2" "$3"; FAIL=$((FAIL + 1)); }
ok()   { printf '  ok   %s\n' "$1"; PASS=$((PASS + 1)); }

assert_eq() { # <label> <expected> <actual>
    if [ "$2" = "$3" ]; then ok "$1"; else fail "$1" "$2" "$3"; fi
}

# A repository whose single commit has an exactly-known committer date. The
# AUTHOR date is deliberately set to something else: the derivation must read
# the COMMITTER date, which is the one a rebase or a merge updates and therefore
# the one that describes when this artefact came to exist.
make_repo() { # <committer date, git-parseable>
    local dir; dir="$(mktemp -d)"
    git -C "$dir" init -q
    git -C "$dir" config user.email t@example.invalid
    git -C "$dir" config user.name Test
    : > "$dir/f"
    git -C "$dir" add f
    GIT_AUTHOR_DATE='2001-01-01T00:00:00+00:00' GIT_COMMITTER_DATE="$1" \
        git -C "$dir" commit -qm c
    printf '%s' "$dir"
}

run() { # <repo> <ref> -> "numeric|display|full|stage"
    local out
    out="$(cd "$1" && XPL_REF="$2" GITHUB_OUTPUT= bash "$SCRIPT" 2>/dev/null)"
    printf '%s|%s|%s|%s' \
        "$(sed -n 's/^numeric=//p' <<<"$out")" \
        "$(sed -n 's/^display=//p' <<<"$out")" \
        "$(sed -n 's/^full=//p'    <<<"$out")" \
        "$(sed -n 's/^stage=//p'   <<<"$out")"
}

echo "resolve-version.sh"

# --- the three forms, and the stage per branch -----------------------------
REPO="$(make_repo '2026-08-19T17:40:07+00:00')"

echo "GIVEN a commit committed 2026-08-19 17:40 UTC"
assert_eq "WHEN built on dev  THEN preprod, three forms" \
    "2026.8.19.1740|2026.08.19-1740|2026.08.19-1740-preprod|preprod" \
    "$(run "$REPO" refs/heads/dev)"

assert_eq "WHEN built on main THEN no suffix" \
    "2026.8.19.1740|2026.08.19-1740|2026.08.19-1740|" \
    "$(run "$REPO" refs/heads/main)"

assert_eq "WHEN built on a feature branch THEN canary" \
    "2026.8.19.1740|2026.08.19-1740|2026.08.19-1740-canary|canary" \
    "$(run "$REPO" refs/heads/feature/BLD)"

# A production build is triggered by the tag cut-deployment pushes, so its ref
# is refs/tags/*, never refs/heads/main. Reading that as canary would have
# stamped every production binary "-canary" — the defect this case exists for.
assert_eq "WHEN built from the production tag THEN production, not canary" \
    "2026.8.19.1740|2026.08.19-1740|2026.08.19-1740|" \
    "$(run "$REPO" refs/tags/2026.08.19-1740)"

# A pull_request build arrives as refs/pull/N/merge. Canary is correct: it
# publishes nothing.
assert_eq "WHEN built from a pull request THEN canary" \
    "2026.8.19.1740|2026.08.19-1740|2026.08.19-1740-canary|canary" \
    "$(run "$REPO" refs/pull/48/merge)"

echo "GIVEN the production tag equals the derived display form"
assert_eq "THEN a workflow can check the tag against the build" \
    "2026.08.19-1740" \
    "$(cut -d'|' -f2 <<<"$(run "$REPO" refs/tags/2026.08.19-1740)")"
rm -rf "$REPO"

# --- the same instant, committed in three timezones ------------------------
# The runners are Windows, macOS and Linux and each has its own idea of local
# time. The version must describe the commit, not the machine that read it.
echo "GIVEN one instant recorded in three timezones"
for spec in '2026-08-19T17:40:07+00:00' '2026-08-19T19:40:07+02:00' '2026-08-19T10:40:07-07:00'; do
    R="$(make_repo "$spec")"
    assert_eq "WHEN committed as $spec THEN the UTC form is identical" \
        "2026.08.19-1740-preprod" \
        "$(cut -d'|' -f3 <<<"$(run "$R" refs/heads/dev)")"
    rm -rf "$R"
done

# --- leading zeros ---------------------------------------------------------
# 08 is not a valid octal literal, so a naive $((month)) fails outright in
# August, September and every month whose day or minute field starts with 0 —
# a bug that would lie dormant until the calendar reached it.
echo "GIVEN a date whose fields all carry leading zeros"
R="$(make_repo '2026-08-09T07:05:00+00:00')"
assert_eq "WHEN the numeric form is built THEN base ten, zeros stripped" \
    "2026.8.9.705" \
    "$(cut -d'|' -f1 <<<"$(run "$R" refs/heads/main)")"
assert_eq "WHEN the display form is built THEN zeros kept" \
    "2026.08.09-0705" \
    "$(cut -d'|' -f2 <<<"$(run "$R" refs/heads/main)")"
rm -rf "$R"

echo "GIVEN a commit at midnight"
R="$(make_repo '2026-01-01T00:00:00+00:00')"
assert_eq "WHEN the numeric form is built THEN the minute field is 0, not empty" \
    "2026.1.1.0" \
    "$(cut -d'|' -f1 <<<"$(run "$R" refs/heads/main)")"
rm -rf "$R"

# --- determinism -----------------------------------------------------------
echo "GIVEN the same commit resolved twice"
R="$(make_repo '2026-08-19T17:40:07+00:00')"
assert_eq "WHEN a failed workflow is re-run THEN the version is unchanged" \
    "$(run "$R" refs/heads/dev)" "$(run "$R" refs/heads/dev)"
rm -rf "$R"

# --- what the derivation must never read -----------------------------------
# Comments are stripped first, and deliberately so: the header explains at
# length why a counter and a wall clock are the wrong sources, and a check that
# scanned the prose would fail on the very paragraph justifying it. What must
# hold is that the executable lines never reach for them.
echo "GIVEN the executable lines of the derivation, comments stripped"
CODE="$(grep -vE '^[[:space:]]*#' "$SCRIPT")"
for forbidden in 'run_number' 'run_id' 'vars.' '$(date' '`date'; do
    if grep -qF -- "$forbidden" <<<"$CODE"; then
        fail "THEN no mutable or wall-clock source is read ($forbidden)" "absent" "present"
    else
        ok "THEN it does not read $forbidden"
    fi
done

printf '\n%d passed, %d failed\n' "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
