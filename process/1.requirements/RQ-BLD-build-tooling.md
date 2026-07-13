# RQ-BLD — Build System & Tooling

- **RQ-BLD-001** — The C++ implementation shall build with CMake (≥ 3.22) fetching JUCE 8.x as a pinned dependency, with no manually-copied binaries.
- **RQ-BLD-002** — The primary target platform shall be Windows (x64); the non-UI layers (MID, FMW, MOD, CTL, SET) shall also compile and run their tests headless on Linux.
- **RQ-BLD-003** — The code shall use C++20, compile warning-clean at high warning levels (/W4, -Wall -Wextra) with warnings-as-errors for project code.
- **RQ-BLD-004** — The C++ tree shall live in this repository under a dedicated top-level directory, leaving the .NET solution untouched and buildable during the whole migration.
- **RQ-BLD-005** — A single CMake invocation shall build libraries per layer (midi, framework, model, controller, settings), the application, and the test executables.
- **RQ-BLD-006** — All source files shall carry the GPL v3 header of the reference project; JUCE shall be used under its GPL option (no commercial JUCE license assumed).
- **RQ-BLD-007** — CI shall build and run the non-UI test suite on every push (GitHub Actions, Linux at minimum).
- **RQ-BLD-008** — The Windows (x64) GUI binary shall be produced with the MSVC toolchain. JUCE does not support MinGW (`juce_TargetPlatform.h` `#error`s on `__MINGW32__`), so cross-compiling the GUI app from Linux with MinGW-w64 is not viable; the amd64 `Xplorer.exe` is built by a native `windows-latest` CI job (`.github/workflows/juce-windows.yml`) that also runs the unit suite and uploads the binary as an artifact.
