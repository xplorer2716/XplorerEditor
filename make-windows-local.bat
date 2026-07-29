@echo off
rem Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
rem Copyright (C) 2012-2026 Pascal Schmitt
rem SPDX-License-Identifier: GPL-3.0-or-later
rem Local Windows x64 GUI build. [RQ-BLD-002, RQ-BLD-008, TASK-JUC-070]
rem
rem Deliberately Release with XPL_BUILD_TESTS=OFF: this script exists to
rem produce a binary quickly, so it fetches and compiles neither Catch2 nor the
rem test suites. Two consequences are worth stating out loud to whoever runs it
rem - hence the banner below - because both are silent otherwise:
rem   * no test is built or run here;
rem   * development-only checks guarded by JUCE_DEBUG are compiled out of a
rem     Release build, which includes the combo-box label-fit assertion
rem     RQ-GUI-048.
rem The CI covers both: the juce-windows workflow builds Release AND Debug with
rem tests enabled and runs every test, including the real-metrics combo-box fit
rem check that only a JUCE-linked build can perform. [RQ-GUI-048, ADR-JUC-022]

setlocal

echo.
echo ============================================================
echo  Release build, tests DISABLED.
echo.
echo  This script only produces Xplorer.exe. It does not build or
echo  run any test, and Release compiles out the development-only
echo  checks - including the combo-box label-fit assertion.
echo.
echo  For the full suite, push the branch or run the "juce-windows"
echo  workflow from the Actions tab: its Release and Debug jobs
echo  build with tests ON and run all of them.
echo ============================================================
echo.

set "SOURCE_DIR=%~dp0juce"
set "BUILD_DIR=%SOURCE_DIR%\build-win-local"
set "ARCHITECTURE=x64"
set "CONFIGURATION=Release"
set "CMAKE_EXE="
set "VSWHERE_EXE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

where cmake.exe >nul 2>&1
if not errorlevel 1 set "CMAKE_EXE=cmake.exe"

if not defined CMAKE_EXE if exist "%ProgramFiles%\CMake\bin\cmake.exe" (
    set "CMAKE_EXE=%ProgramFiles%\CMake\bin\cmake.exe"
)

if not defined CMAKE_EXE (
    if exist "%VSWHERE_EXE%" (
        for /f "usebackq delims=" %%I in (`"%VSWHERE_EXE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.CMake.Project -find Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`) do set "CMAKE_EXE=%%I"
    )
)

if not defined CMAKE_EXE (
    echo Error: CMake was not found.
    echo Install CMake 3.22 or later, or add the Visual Studio CMake component.
    exit /b 1
)

echo Configuring Xplorer for %ARCHITECTURE% %CONFIGURATION%...
"%CMAKE_EXE%" -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -A %ARCHITECTURE% -DXPL_BUILD_APP=ON -DXPL_BUILD_TESTS=OFF
if errorlevel 1 exit /b %errorlevel%

echo Building Xplorer...
"%CMAKE_EXE%" --build "%BUILD_DIR%" --config %CONFIGURATION% --parallel
if errorlevel 1 exit /b %errorlevel%

echo Build completed. Xplorer.exe is under "%BUILD_DIR%\app\XplorerApp_artefacts\%CONFIGURATION%".
exit /b 0