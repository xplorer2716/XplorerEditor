# Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
# Copyright (C) 2012-2026 Pascal Schmitt
# SPDX-License-Identifier: AGPL-3.0-or-later
#
# Root build for the JUCE (C++) port. [RQ-BLD-001, RQ-BLD-003, RQ-BLD-005]
cmake_minimum_required(VERSION 3.22)

# --- Product version: DERIVED, never declared [RQ-BLD-015, RQ-BLD-020, ADR-BLD-003] ---
#
# CI passes these on the configure line; .github/actions/resolve-version is the
# one place either value is computed, from the commit being built. There is
# deliberately no version literal anywhere in this repository to keep in step
# with them -- that was the defect (four copies, in four languages, nothing
# holding them equal) these variables exist to remove.
#
#   XPL_VERSION_NUMERIC  2026.8.19.1740          four integers: CMake, JUCE target,
#                                                Windows FILEVERSION, CFBundleVersion
#   XPL_VERSION_FULL     2026.08.19-1740-preprod carries the stage: About box,
#                                                ProductVersion string, SBOM
#
# The default is what a developer's own build reports, and it is chosen to be
# unmistakably not a deployment: 0.0.0.0-local sorts below everything, states
# its origin, and cannot be confused with a published build in a bug report.
set(XPL_VERSION_NUMERIC "0.0.0.0" CACHE STRING
    "Numeric product version (YYYY.M.D.HHMM). CI supplies it; a local build gets 0.0.0.0.")
set(XPL_VERSION_FULL "" CACHE STRING
    "Full product version with stage suffix. Defaults to <numeric>-local.")
if(XPL_VERSION_FULL STREQUAL "")
    set(XPL_VERSION_FULL "${XPL_VERSION_NUMERIC}-local")
endif()
# The SBOM's mandatory SPDX `created` field. Supplied by resolve-version from the
# commit rather than read off the build clock, so rebuilding a commit produces a
# byte-identical document instead of one differing only here. The local default
# is the epoch: unmistakably not a deployment, like 0.0.0.0 above. [RQ-BLD-022]
set(XPL_VERSION_TIMESTAMP "1970-01-01T00:00:00Z" CACHE STRING
    "Commit timestamp (ISO 8601 UTC) stamped into the generated SBOM.")
message(STATUS "Xplorer product version: ${XPL_VERSION_FULL} (numeric ${XPL_VERSION_NUMERIC})")

project(XplorerJuce
    VERSION ${XPL_VERSION_NUMERIC}
    DESCRIPTION "Xplorer JUCE port - real-time editor for Oberheim Xpander/Matrix-12"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- MSVC C runtime: STATIC, in both configurations [RQ-BLD-024, ADR-BLD-004 (DEC-BLD-020)] ---
#
# The dynamic DEBUG runtime (MSVCP140D.dll, VCRUNTIME140D.dll) is not
# redistributable and ships only with Visual Studio, so a Debug binary linked
# against it does not start on a tester's machine. Debug binaries ARE published
# -- that is the point of the pre-production stream (RQ-BLD-019), and they were
# already being attached to every alpha pre-release before this policy existed.
#
# Position matters as much as the value: MSVC_RUNTIME_LIBRARY is a PER-TARGET
# property, and the variable read here seeds it for targets created afterwards.
# Setting it on XplorerApp alone would leave JUCE on the dynamic runtime and the
# link would fail with LNK2038 -- so it is set here, above
# FetchContent_MakeAvailable(JUCE), where every target still inherits it.
#
# Release keeps the same treatment for symmetry: one fewer thing to install on
# the user's machine, at the cost of a larger binary and of needing a rebuild
# rather than a system update to pick up a CRT fix. Accepted knowingly.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# The GUI application target only builds when explicitly enabled (Phase 5+);
# non-UI layers and their tests always build, headless. [RQ-BLD-002]
option(XPL_BUILD_APP "Build the Xplorer JUCE GUI application" OFF)
option(XPL_BUILD_TESTS "Build the test suites" ON)

# --- Third-party dependencies (pinned) [RQ-BLD-001] ---
include(FetchContent)

if(NOT XPL_BUILD_APP)
    # Skip juceaide and extras: they require GUI system libraries not needed
    # for the headless layer/test builds. [RQ-BLD-002]
    set(JUCE_MODULES_ONLY ON CACHE BOOL "" FORCE)
endif()
# Pinned dependency versions, named ONCE. The generated SBOM (RQ-BLD-022) states
# these same values, and a pin that moved without the disclosure following it is
# the kind of drift a hand-maintained SBOM cannot announce. Unlike the PRODUCT
# version (derived, RQ-BLD-020), a dependency pin is a declaration we want
# literal -- it is the thing being declared.
set(XPL_JUCE_VERSION 8.0.9)
set(XPL_CATCH2_VERSION 3.9.1)

FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG ${XPL_JUCE_VERSION}
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

if(XPL_BUILD_TESTS)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v${XPL_CATCH2_VERSION}
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    include(CTest)
    include(Catch)
endif()

# --- Strict warnings for project code only (not JUCE module sources) [RQ-BLD-003] ---
add_library(xpl_warnings INTERFACE)
if(MSVC)
    target_compile_options(xpl_warnings INTERFACE /W4 /WX /permissive-)
else()
    target_compile_options(xpl_warnings INTERFACE -Wall -Wextra -Wpedantic -Werror)
    # A target that links a juce::* module directly (xpl_tests_app_juce) compiles
    # JUCE's own module sources as ITS OWN sources -- that is how JUCE's CMake
    # support works -- so they inherit this INTERFACE library's flags too,
    # despite the heading above. First seen on the pinned Linux GCC 11 image:
    # GCC's -Wmaybe-uninitialized false-positives on JUCE's vendored
    # SheenBidi.c, three levels of inlining deep, a documented GCC heuristic
    # limitation rather than a real bug in mature, widely-used third-party C.
    # Scoped to C only: the project itself has no .c file, so this cannot hide
    # a genuine diagnostic in project code. [RQ-BLD-003]
    target_compile_options(xpl_warnings INTERFACE
        $<$<COMPILE_LANGUAGE:C>:-Wno-error=maybe-uninitialized>)
endif()
add_library(xpl::warnings ALIAS xpl_warnings)

# --- Layer libraries, bottom-up [RQ-BLD-005, ADR-JUC-002] ---
add_subdirectory(midi)
add_subdirectory(framework)
add_subdirectory(model)
add_subdirectory(controller)
add_subdirectory(settings)
add_subdirectory(app/core)

if(XPL_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(XPL_BUILD_APP)
    add_subdirectory(app)
endif()
