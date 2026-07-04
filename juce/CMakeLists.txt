# Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
# Copyright (C) 2012-2026 Pascal Schmitt
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Root build for the JUCE (C++) port. [RQ-BLD-001, RQ-BLD-003, RQ-BLD-005]
cmake_minimum_required(VERSION 3.22)

project(XplorerJuce
    VERSION 0.1.0
    DESCRIPTION "Xplorer JUCE port - real-time editor for Oberheim Xpander/Matrix-12"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.9
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

if(XPL_BUILD_TESTS)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.9.1
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
endif()
add_library(xpl::warnings ALIAS xpl_warnings)

# --- Layer libraries, bottom-up [RQ-BLD-005, ADR-002] ---
add_subdirectory(midi)
add_subdirectory(framework)
add_subdirectory(model)
add_subdirectory(controller)
add_subdirectory(settings)

if(XPL_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(XPL_BUILD_APP)
    add_subdirectory(app)
endif()
