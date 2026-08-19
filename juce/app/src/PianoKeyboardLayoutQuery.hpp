#pragma once

// Factory for the platform KeyboardLayoutQuery. Exactly one of
// PianoKeyboardLayoutQuery_windows.cpp / _linux.cpp / _mac.cpp provides the
// definition, selected in juce/app/CMakeLists.txt by the target OS. This
// header (and the factory) live in the XplorerApp target, never in
// xpl_app_core — the concrete queries call real OS APIs the headless
// configuration must not depend on. [RQ-GUI-074, ADR-JUC-035 (DEC-JUC-116)]

#include "xplorer/app/PianoKeyMapping.hpp"

#include <memory>

namespace xplorer::app
{
    /// Builds the KeyboardLayoutQuery for the running platform, or nullptr if
    /// none could be started (e.g. no display server) — the caller's cue,
    /// via buildPianoKeyMapping(), to leave JUCE's built-in default alone.
    [[nodiscard]] std::unique_ptr<KeyboardLayoutQuery> makeNativeKeyboardLayoutQuery();
}
