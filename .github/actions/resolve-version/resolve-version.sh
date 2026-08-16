#!/usr/bin/env bash
#
# Derives the product version from the commit being built — the ONLY place a
# version is computed. [RQ-BLD-015, RQ-BLD-020, ADR-BLD-003 (DEC-BLD-014, DEC-BLD-015)]
#
# Three forms, because no single string can serve every consumer:
#
#   numeric  2026.8.19.1740   CMake, JUCE target, Windows FILEVERSION, CFBundleVersion
#   display  2026.08.19-1740  tag, release title, deployment archive names
#   full     …-1740-preprod   About box, ProductVersion string, SBOM
#
# The numeric form is dictated by Windows, not chosen: FILEVERSION is four
# 16-bit fields, so leading zeros must go and a stage suffix cannot be carried
# there at all.
#
# PURE FUNCTION OF THE COMMIT. No counter, no github.run_number, no repository
# variable, no wall-clock "now". This is load-bearing rather than tidy: up to
# six workflows build one commit concurrently and must compute the identical
# string without communicating, and a re-run of a failed workflow must produce
# the version it produced the first time.
set -euo pipefail

ref="${XPL_REF:-${GITHUB_REF:-}}"

# Production is reached through a TAG, not through refs/heads/main: DEC-BLD-017
# has cut-deployment push the tag, and every production workflow triggers on
# it. Tags are minted by that action alone, so a tag ref IS the production
# stream. A pull_request build arrives as refs/pull/N/merge and is canary, which
# is what it is.
case "$ref" in
    refs/heads/main | refs/tags/*) stage="" ;;
    refs/heads/dev)                stage="-preprod" ;;
    *)                             stage="-canary" ;;
esac

# Formatted by git, not by date(1): `date -d @epoch` is GNU-only and the macOS
# runners carry the BSD one. TZ=UTC makes three operating systems agree on which
# day and which minute a commit belongs to — the same technique, for the same
# reason, as the action this one replaces (DEC-BLD-011).
stamp="$(TZ=UTC git log -1 --format=%cd --date=format-local:'%Y %m %d %H%M' HEAD)"
read -r year month day hhmm <<<"$stamp"

# ISO 8601, for the SBOM's mandatory `created` field. Taken from the same commit
# as everything else rather than from the clock, so rebuilding a commit produces
# a byte-identical document instead of one that differs only in its timestamp.
timestamp="$(TZ=UTC git log -1 --format=%cd --date=format-local:'%Y-%m-%dT%H:%M:%SZ' HEAD)"

# 10# forces base ten: 08 is not a valid octal literal, and a version derived in
# August would otherwise fail arithmetic expansion outright.
numeric="${year}.$((10#$month)).$((10#$day)).$((10#$hhmm))"
display="${year}.${month}.${day}-${hhmm}"
full="${display}${stage}"

{
    echo "numeric=${numeric}"
    echo "display=${display}"
    echo "full=${full}"
    echo "stage=${stage#-}"
    echo "timestamp=${timestamp}"
} >> "${GITHUB_OUTPUT:-/dev/stdout}"

echo "Resolved version: ${full} (numeric ${numeric})" >&2
