#!/usr/bin/env bash
#
# Fails if a product-version literal has crept back into the build or the
# application sources. [RQ-BLD-015, RQ-BLD-016, ADR-BLD-003 (DEC-BLD-015)]
#
# The defect this guards against is not hypothetical: the version was written
# out four times, in four languages -- juce/CMakeLists.txt, juce/app/CMakeLists.txt,
# Main.cpp and MainComponent.cpp -- and the About box advertised 0.1.0 while every
# published artefact carried something else. Nothing held the copies equal, so
# nothing announced when they stopped being equal.
#
# Scope is deliberately narrow: DEPENDENCY versions (the pinned JUCE tag, Catch2,
# cmake_minimum_required, the SPDX schema) are declarations we WANT literal.
# Only the product's own version must be derived.
#
# Run:  bash juce/tools/check_no_version_literals.sh
set -uo pipefail

# juce/tools/ -> repository root.
cd "$(dirname "${BASH_SOURCE[0]}")/../.." || exit 1
FAIL=0
report() { printf '  FAIL %s\n%s\n' "$1" "$2"; FAIL=$((FAIL + 1)); }

echo "no-version-literals"

# 1. The literal that actually shipped. Any reappearance in DECLARING code is
#    the regression.
#
#    Comment lines and juce/tests/ are excluded, and both exclusions were earned
#    rather than assumed: the first run of this check failed on the comment in
#    Main.cpp that explains why the literal was removed, and on an SPDX fixture
#    in SbomReaderTests.cpp whose whole job is to carry an arbitrary version
#    string. A check that cannot tell a declaration from a description reports
#    its own documentation as a defect.
if hits=$(grep -rn --include='*.cpp' --include='*.hpp' --include='CMakeLists.txt' \
              -e '0\.1\.0' juce/ 2>/dev/null \
          | grep -v '^juce/tests/' \
          | grep -vE '^[^:]+:[0-9]+:[[:space:]]*(//|\*|/\*|#)'); then
    report "the retired 0.1.0 literal is back" "$hits"
else
    echo "  ok   0.1.0 is declared nowhere under juce/"
fi

# 2. Every VERSION keyword in the build must take a variable, never a literal.
if hits=$(grep -rn --include='CMakeLists.txt' -E '^\s*VERSION\s+[0-9]' juce/ 2>/dev/null); then
    report "a CMake VERSION takes a literal instead of \${XPL_VERSION_NUMERIC}" "$hits"
else
    echo "  ok   every CMake VERSION reads a variable"
fi

# 3. No quoted version-shaped string in the application sources. Catches the
#    MainComponent.cpp form -- showAboutDialog("Xplorer 0.1.0") -- whatever
#    numbers a future author picks.
if hits=$(grep -rn --include='*.cpp' --include='*.hpp' \
              -E '"[^"]*[0-9]+\.[0-9]+\.[0-9]+[^"]*"' juce/app/src/ 2>/dev/null); then
    report "an application source carries a quoted version-shaped literal" "$hits"
else
    echo "  ok   no quoted version literal in juce/app/src/"
fi

# 4. The two definitions the application reads must actually be supplied.
for def in XPL_VERSION_FULL_STRING JUCE_APPLICATION_NAME_STRING; do
    if grep -q "$def=" juce/app/CMakeLists.txt; then
        echo "  ok   $def is defined by the build"
    else
        report "$def is read by the sources but never defined" "juce/app/CMakeLists.txt"
    fi
done

if [ "$FAIL" -eq 0 ]; then echo; echo "no version literals"; else echo; echo "$FAIL check(s) failed"; fi
[ "$FAIL" -eq 0 ]
